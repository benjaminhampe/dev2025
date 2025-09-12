#pragma once
#include <irrExt/gui/Align.h>

namespace irrExt {

// =======================================================================
/*
irr::core::dimension2du
Font_getPixelSize( irr::gui::IGUIFont* font, std::string txt );

irr::gui::CGUITTFont*
Font_create( irr::gui::IGUIEnvironment* env, std::string uri, int32_t pxSize, bool aa, bool transparent = true );

void
Font_draw( irr::gui::IGUIFont* font, std::string txt, int32_t x, int32_t y, uint32_t color );

void
Font_draw( irr::gui::IGUIFont* font, std::string txt, irr::core::recti const & pos, uint32_t color );

// =======================================================================
struct Text_t
// =======================================================================
{
   std::string Text;
   uint32_t Color;
   irr::gui::IGUIFont * Font;

   Text_t()
      : Text( "X" ), Color( 0xFFFFFFFF ), Font( nullptr )
   {}

   Text_t( std::string txt, uint32_t color, irr::gui::IGUIFont* font )
      : Text( txt ), Color( color ), Font( font )
   {}

   bool canDraw() const
   {
      if ( !Font ) return false;
      if ( Text.empty() ) return false;
      return true;
   }
};

void
Text_draw( Text_t * txt, int32_t x, int32_t y );

void
Text_draw( Text_t * txt, irr::core::recti const & pos );

inline void drawText( irr::gui::IGUIFont* font,
                      irr::IrrlichtDevice* device,
                      int x,
                      int y,
                      std::wstring const & msg,
                      irr::video::SColor const & color = 0xffffffff,
                      Align::EAlign align = Align::TopLeft,
                      int padding = 1,
                      irr::core::recti* clipRect = nullptr )
{
   if ( !font )
   {
      font = device->getGUIEnvironment()->getBuiltInFont();
   }

   auto ts = font->getDimension( msg.c_str() );

   x -= padding;
   y -= padding;

   if ( align & Align::Center )
   {
      x -= ts.Width / 2;
   }
   else if ( align & Align::Right )
   {
      x -= ts.Width;
   }

   if ( align & Align::Middle )
   {
      y -= ts.Height / 2;
   }
   else if ( align & Align::Bottom )
   {
      y -= ts.Height;
   }

   font->draw( msg.c_str(), mkRect(x,y,ts.getOptimalSize()),
               color, false, false, clipRect );
}

inline void
drawV3(
   irr::gui::IGUIFont* font,
   irr::IrrlichtDevice* device,
   int x,
   int y,
   std::wstring const & msg,
   irr::core::vector3df const & v )
{
   std::wostringstream o;
   o << msg << "  (  " << v.X << " ,  " << v.Y << " ,  " << v.Z << "  )";
   drawText( font, device, x,y, o.str() );
}

inline void
drawM4(
   irr::gui::IGUIFont* font,
   irr::IrrlichtDevice* device,
   int x,
   int y,
   std::wstring const & msg,
   irr::core::matrix4 const & m,
   int p = 1 )
{
   int h = font->getDimension( L"|" ).Height;

   std::wstring const s = L"    ";
   std::wostringstream o;
   o << msg << " |  " << m[0] << s << m[1] << s << m[2] << s << m[3] << "  |";
   drawText( font, device, x,y, o.str() ); y += h + p;
   o.str(L"");
   o << msg << " |  " << m[4] << s << m[5] << s << m[6] << s << m[7] << "  |";
   drawText( font, device, x,y, o.str() ); y += h + p;
   o.str(L"");
   o << msg << " |  " << m[8] << s << m[9] << s << m[10]<< s << m[11] << "  |";
   drawText( font, device, x,y, o.str() ); y += h + p;
   o.str(L"");
   o << msg << " |  " << m[12] << s << m[13]<<s << m[14]<< s << m[15] << "  |";
   drawText( font, device, x,y, o.str() ); y += h + p;
}

inline void draw3DLine( irr::IrrlichtDevice* device, irr::f32 x1, irr::f32 y1, irr::f32 z1,
                  irr::f32 x2, irr::f32 y2, irr::f32 z2, irr::video::SColor color )
{
   device->getVideoDriver()->draw3DLine( irr::core::vector3df(x1,y1,z1),
                                         irr::core::vector3df(x2,y2,z2), color );
}

*/

} // end namespace irrExt
