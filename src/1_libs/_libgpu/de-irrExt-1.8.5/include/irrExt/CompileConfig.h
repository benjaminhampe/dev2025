#pragma once
#include <irrlicht.h>
#include <de_freetype2.hpp>
#include <de_glm.hpp>
#include <tinyxml2/tinyxml2.h>


/*

inline bool
irr_equals( float32_t a, float32_t b, float32_t eps = 0.0001f ) { return irr::core::equals( a, b, eps ); }

inline float32_t
irr_curveValue( float_t a, float_t b, float_t speed = 1.5f )
{
   if ( speed < 1.0f ) // a = 1,[11,13.8],   <- this is the computed value from last iteration and fed into here, or the start value at first call
        speed = 1.0f;  // b = 15,[15,15]     <- destination value is const for all calls
   float32_t ba = a - b;      // ba = -14,[-4,-1.2] ba/s = -4,[-1.2,-0.3]
   return b + ( ba / speed ); // b = 11,[13.8,14.7]
}

inline glm::vec3
irr_curveVector( glm::vec3 a, glm::vec3 b, float_t speed = 1.5f )
{
   if ( speed < 1.0f ) // a = 1,[11,13.8],   <- this is the computed value from last iteration and fed into here, or the start value at first call
        speed = 1.0f;  // b = 15,[15,15]     <- destination value is const for all calls
   return b + ( ( a - b ) / speed ); // b = 11,[13.8,14.7] // ba = -14,[-4,-1.2] ba/s = -4,[-1.2,-0.3]
}

inline float32_t
irr_convertToNormalized( int16_t value )
{
    // ( make all symmetric around +0.5f )
    if ( value == 0 )
        return 0.5f;
    else if ( value > 0 )
        return 0.5f + static_cast< float32_t >(value)/65534.0f;
    else
        return 0.5f + static_cast< float32_t >(value)/65536.0f;
}

inline irr::core::dimension2du
irr_getTexSize( irr::video::ITexture* tex )
{
   return ( tex ? tex->getOriginalSize() : irr::core::dimension2du(0,0) );
}

inline float
irr_getTexAspectRatio( irr::video::ITexture* tex )
{
   if ( !tex ) return 0.0f;
   uint32_t h = tex->getOriginalSize().Height;
   if ( h < 1 ) return 0.0f;
   return float( tex->getOriginalSize().Width) / float( h );
}

inline std::string
irr_getTexNameA( irr::video::ITexture* tex )
{
   return ( tex ? std::string( irr::core::stringc( tex->getName() ).c_str() ) : std::string() );
}

inline std::wstring
irr_getTexNameW( irr::video::ITexture* tex )
{
   return ( tex ? std::wstring( irr::core::stringw( tex->getName() ).c_str() ) : std::wstring() );
}

inline int32_t
irr_getScreenWidth( irr::video::IVideoDriver* driver )
{
   return ( driver ? int32_t( driver->getScreenSize().Width ) : 0 );
}

inline int32_t
irr_getScreenHeight( irr::video::IVideoDriver* driver )
{
   return ( driver ? int32_t( driver->getScreenSize().Height ) : 0 );
}

inline void
irr_saveTexture( irr::video::IVideoDriver* driver, irr::video::ITexture* tex, std::string const & fileName )
{
    if ( !driver ) return;
    irr::video::IImage* img = driver->createImage( tex, irr::core::position2di(0,0), tex->getOriginalSize() );
    if ( img )
    {
        driver->writeImageToFile( img, fileName.c_str() );
        img->drop();
    }
}


inline irr::gui::CGUITTFont*
createTTFont( irr::gui::IGUIEnvironment* env, std::string const & fileName, uint32_t pxSize, bool aa = true, bool transparency = true )
{
    irr::gui::CGUITTFont* font = irr::gui::CGUITTFont::create( env, fileName.c_str(), pxSize, aa, transparency );
    return font;
}

inline void
addFont( irr::gui::IGUIEnvironment* env, std::string const & fileName, uint32_t pxSize, bool aa = true, bool transparency = true )
{
    irr::gui::CGUITTFont* font = irr::gui::CGUITTFont::create( env, fileName.c_str(), pxSize, aa, transparency );
    if ( !font )
    {
        std::cout << "[Error] " << __FUNCTION__ << " :: Cannot create and set font from file (" << fileName << ")\n";
        return;
    }
    env->getSkin()->setFont( font, irr::gui::EGDF_DEFAULT );
}




inline irr::gui::IGUIImage*
createGUIImage( irr::gui::IGUIEnvironment* env, irr::gui::IGUIElement* parent, irr::core::recti const & pos, irr::video::ITexture* tex  )
{
    std::cout << __FUNCTION__ << "(" << pos << ")\n";
    assert( env );
//    if ( !env )
//    {
//        std::cout << __FUNCTION__ << " :: Invalid pointer to IGUIEnvironment\n";
//        return nullptr;
//    }
    if ( !parent )
    {
        parent = env->getRootGUIElement();
    }

    //bool bPreserveAspect = true;
    int32_t x = pos.UpperLeftCorner.X;
    int32_t y = pos.UpperLeftCorner.Y;
    int32_t w = pos.getWidth();
    int32_t h = pos.getHeight();

    float32_t aspect = getTexAspectRatio( tex );
    if ( aspect > 0.0f )
    {
        irr::core::dimension2du size = getTexSize( tex );

        if ( aspect > 1.0f ) // w > h
        {
            float32_t fScale = float32_t(pos.getWidth()) / float32_t(size.Width);
            h = irr::core::round32( fScale * size.Height );
            y = pos.UpperLeftCorner.Y + (pos.getHeight() - h) / 2;
        }
        else
        {

        }

    }

    irr::gui::IGUIImage* element = env->addImage( mkRect( x, y, w, h ), parent, -1, getTexNameW(tex).c_str(), true );
    element->setScaleImage( true );
    element->setImage( tex );
    return element;
}




#ifdef GLM_COMPILER

inline irr::core::vector3df toIRR( glm::vec3 const & v )
{
    return irr::core::vector3df( v.x, v.y, v.z );
}

inline glm::vec3 toGLM( irr::core::vector3df const & v )
{
    return glm::vec3( v.X, v.Y, v.Z );
}

inline bool
equals( glm::vec2 a, glm::vec2 b, float32_t eps = 0.0001f )
{
    if ( equals( a.x, b.x, eps ) &&
         equals( a.y, b.y, eps ) ) return true;
    return false;
}

inline bool
equals( glm::vec3 a, glm::vec3 b, float32_t eps = 0.0001f )
{
    if ( equals( a.x, b.x, eps ) &&
         equals( a.y, b.y, eps ) &&
         equals( a.z, b.z, eps ) ) return true;
    return false;
}

inline glm::ivec2
getScreenSize( irr::video::IVideoDriver* driver )
{
    if ( !driver ) return glm::ivec2(0,0);
    return glm::ivec2( driver->getScreenSize().Width, driver->getScreenSize().Height );
}

inline glm::ivec3
getDesktopSize( irr::IrrlichtDevice * device )
{
    glm::ivec3 desktop( 0,0,0 );
    assert( device );
    if ( !device ) return desktop;
    desktop.x = device->getVideoModeList()->getDesktopResolution().Width;
    desktop.y = device->getVideoModeList()->getDesktopResolution().Height;
    desktop.z = device->getVideoModeList()->getDesktopDepth();
    return desktop;
}

inline glm::ivec3
getDesktopSize()
{
    glm::ivec3 desktop( 0,0,0 );
    irr::IrrlichtDevice * device = irr::createDevice( irr::video::EDT_NULL );
    desktop = getDesktopSize( device );
    device->drop();
    return desktop;
}

#endif
*/
