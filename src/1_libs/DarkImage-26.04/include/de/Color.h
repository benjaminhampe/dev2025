#pragma once
#include <de/Core.h>

// ===================================================================
inline uint8_t dbClampBytef( const float f )
// ===================================================================
{
    return static_cast<uint8_t>(
            std::clamp<int32_t>( std::lround( f ), 0, 255 ) );
}

// ===================================================================
constexpr uint32_t dbRGB( uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255 )
// ===================================================================
{
   return ( uint32_t( a ) << 24 ) | ( uint32_t( b ) << 16 )
        | ( uint32_t( g ) << 8 ) | uint32_t( r );
}
// ===================================================================
constexpr uint8_t dbRGB_R( uint32_t color ) { return color & 0xFF; }
constexpr uint8_t dbRGB_G( uint32_t color ) { return ( color >> 8 ) & 0xFF; }
constexpr uint8_t dbRGB_B( uint32_t color ) { return ( color >> 16 ) & 0xFF; }
constexpr uint8_t dbRGB_A( uint32_t color ) { return ( color >> 24 ) & 0xFF; }

// ===================================================================
constexpr float dbRGB_Rf( uint32_t color ) { return static_cast<float>(  color         & 0xFF); }
constexpr float dbRGB_Gf( uint32_t color ) { return static_cast<float>(( color >> 8 )  & 0xFF); }
constexpr float dbRGB_Bf( uint32_t color ) { return static_cast<float>(( color >> 16 ) & 0xFF); }
constexpr float dbRGB_Af( uint32_t color ) { return static_cast<float>(( color >> 24 ) & 0xFF); }

// ===================================================================
constexpr void dbRGB_setR( uint32_t & color, uint8_t r ) { color = (color & 0xFFFFFF00) | (uint32_t(r) & 0xFF); }
constexpr void dbRGB_setG( uint32_t & color, uint8_t g ) { color = (color & 0xFFFF00FF) | ((uint32_t(g) & 0xFF) << 8); }
constexpr void dbRGB_setB( uint32_t & color, uint8_t b ) { color = (color & 0xFF00FFFF) | ((uint32_t(b) & 0xFF) << 16); }
constexpr void dbRGB_setA( uint32_t & color, uint8_t a ) { color = (color & 0x00FFFFFF) | ((uint32_t(a) & 0xFF) << 24); }

inline glm::vec3
dbRGBfv3( const uint32_t color )
{
    const auto r = dbRGB_R( color );
    const auto g = dbRGB_G( color );
    const auto b = dbRGB_B( color );
    return glm::vec4(r,g,b,1) / 255.0f;
}

inline glm::vec4
dbRGBfv4( const uint32_t color )
{
    const auto r = dbRGB_R( color );
    const auto g = dbRGB_G( color );
    const auto b = dbRGB_B( color );
    const auto a = dbRGB_A( color );
    return glm::vec4(r,g,b,a) / 255.0f;
}

inline glm::vec4
dbRGBfv4( const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 255 )
{
    return glm::vec4(r,g,b,a) / 255.0f;
}

inline uint32_t
dbRGBv3( glm::vec3 const& color )
{
    uint8_t r = dbClampBytef(color.r * 255.0f);
    uint8_t g = dbClampBytef(color.g * 255.0f);
    uint8_t b = dbClampBytef(color.b * 255.0f);
    return dbRGB(r,g,b);
}

inline uint32_t
dbRGBv4( glm::vec4 const& color )
{
    uint8_t r = dbClampBytef(color.r * 255.0f);
    uint8_t g = dbClampBytef(color.g * 255.0f);
    uint8_t b = dbClampBytef(color.b * 255.0f);
    uint8_t a = dbClampBytef(color.a * 255.0f);
    return dbRGB(r,g,b,a);
}

namespace de {

// -> Non linear :: Resulting alpha is always 255.
// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
// glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);  // don't write alpha
uint32_t blendColor_NoAlphaWrite( const uint32_t src, const uint32_t dst );

// -> Linear blend :: (good for 100 lines at same px with alpha = 0.01 -> predictable brightness)
// glBlendFunc(GL_SRC_ALPHA, GL_ONE);
// glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);  // don't write alpha
uint32_t blendColorAdd_NoAlphaWrite( const uint32_t src, const uint32_t dst );

// -> Non linear :: Classic “SourceOver” Porter–Duff operator
// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
// glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
//                     GL_ONE,       GL_ONE_MINUS_SRC_ALPHA);
uint32_t blendColor( const uint32_t src, const uint32_t dst );

// More a lerpColor based on alpha values.
glm::vec4 blendColor_v4( const glm::vec4& src, const glm::vec4& dst );

uint32_t parseColor( const std::string & line );

uint32_t lerpColor( const uint32_t src, const uint32_t dst, const float t );

uint32_t lightenColor( uint32_t color, int offset );

uint32_t darkenColor( uint32_t color, int offset );

uint32_t randomColorRGB( uint8_t a = 255 );

uint32_t randomColorRGB( uint8_t minRGB, uint8_t maxRGB, uint8_t a = 255 );

uint32_t varyColor( uint32_t color, int variance );

// ===================================================================
struct RainbowColor
// ===================================================================
{
    /// Rainbow
    /// lambda in range 380 ( blue ) ... 780nm ( red )
    static glm::vec4
    computeFromWaveLength( double lambda, double gamma = 1.0f, float alpha = 1.0f );

    /// Rainbow
    /// lambda in range 380 ( blue ) ... 780nm ( red )
    static glm::vec4
    computeColor128( float t, float gamma = 1.0f, float alpha = 1.0f );

    /// Rainbow
    /// lambda in range 380 ( blue ) ... 780nm ( red )
    static uint32_t
    computeColor32( float t, float gamma = 1.0f, float alpha = 1.0f );
};


// =======================================================================
struct LineCap
// =======================================================================
{
   enum ECap { None=0, Round, Square, Max, Default=Round, Butt=None };
   LineCap() : Value( Default ) {}
   LineCap( ECap value ) : Value( value ) {}
   operator uint32_t() const { return Value; }
   ECap Value; ///< Member Field

   std::string toString() const
   {
       switch( Value )
       {
       case ::de::LineCap::Square: return "Square";
       case ::de::LineCap::Round: return "Round";
       default: return "None";
       }
   }

};

// =======================================================================
struct LineJoin
// =======================================================================
{
   enum EJoin { None=0, Bevel, Round, Miter, Max, Default=Bevel };
   LineJoin() : Value( Default ) {}
   LineJoin( EJoin value ) : Value( value ) {}
   operator uint32_t() const { return Value; }
   EJoin Value; ///< Member Field

   std::string toString() const
   {
       switch( Value )
       {
       case ::de::LineJoin::Bevel: return "Bevel";
       case ::de::LineJoin::Round: return "Round";
       case ::de::LineJoin::Miter: return "Miter";
       default: return "None"; break;
       }
   }
};

// =======================================================================
struct LineFilter
// =======================================================================
{
   enum EFilter { None = 0, Fxaa, Msaa, Max, Default = None };
   LineFilter() : value( Default ) {}
   LineFilter( EFilter filter ) : value( filter ) {}
   operator uint32_t() const { return value; }
   EFilter value; ///< Member Field
};

// =======================================================================
struct Pen
// =======================================================================
{
    uint32_t color;   ///< Line color
    float width;      ///< Line width in px
    LineCap cap;      ///< Line cap
    LineJoin join;    ///< Line join
    uint16_t stipple; ///< Line stipple pattern, emulates GL line stipple pattern here
    int32_t offset;   ///< Line offset in px, emulates GL line stipple offset here
    LineFilter filter;

    Pen()
        : color( 0xFF000000 ) // Default is = black pencil on white paper background.
        , width( 1.0f )       // Default is a simple
        , cap()
        , join()
        , stipple( 0xFFFF )
        , offset( 0 )
    {}

    Pen( uint32_t lineColor, float lineWidth = 1.0f, uint16_t lineStipple = 0xFFFF, int32_t lineOffset = 0 )
        : color( lineColor )
        , width( lineWidth )
        , cap()
        , join()
        , stipple( lineStipple )
        , offset( lineOffset )
    {}
};

typedef Pen LineStyle;


// Representing a hue, saturation, luminance color
// stored in 32bit floating point variables.
// Hue is in range [0,360],
// Luminance and Saturation are in percent [0,100]

struct ColorHSL
{
    float Hue;
    float Saturation;
    float Luminance;

    ColorHSL ( float h = 0.f, float s = 0.f, float l = 0.f )
        : Hue( h )
        , Saturation( s )
        , Luminance( l )
    {
        while (Hue < 0.0f) { Hue += 360.0f; }
        while (Hue > 360.0f) { Hue -= 360.0f; }
    }

    // algorithm from Foley/Van-Dam
    inline float toRGB1(float rm1, float rm2, float rh) const
    {
        if (rh<0) rh += 1;
        if (rh>1) rh -= 1;

        if (rh < 1.f/6.f)      rm1 = rm1 + (rm2 - rm1) * rh*6.f;
        else if (rh < 0.5f)    rm1 = rm2;
        else if (rh < 2.f/3.f) rm1 = rm1 + (rm2 - rm1) * ((2.f/3.f)-rh)*6.f;

        return rm1;
    }

    uint32_t toRGB( uint8_t alpha = 255 ) const
    {
        float const l = Luminance * 0.01f;
        if ( Saturation <= 0.000001f ) // is_zero = grey
        {
            uint8_t r = dbClampBytef(255.0f * l);
            return dbRGB( r, r, r, alpha );
        }

        float rm2;
        if ( Luminance <= 50.0f )
        {
            rm2 = l + l * (Saturation*0.01f);
        }
        else
        {
            rm2 = l + (1 - l) * (Saturation*0.01f);
        }

        const float rm1 = 2.0f * l - rm2;
        const float h = Hue / 360.0f;
        uint8_t r = dbClampBytef(255.f * toRGB1(rm1, rm2, h + 1.f/3.f));
        uint8_t g = dbClampBytef(255.f * toRGB1(rm1, rm2, h));
        uint8_t b = dbClampBytef(255.f * toRGB1(rm1, rm2, h - 1.f/3.f));

        return dbRGB(r,g,b,alpha);
    }

    /*
    void fromRGB( float r, float g, float b )
    {
        const float maxVal = core::max_(color.getRed(), color.getGreen(), color.getBlue());
        const float minVal = (float)core::min_(color.getRed(), color.getGreen(), color.getBlue());
        Luminance = (maxVal+minVal)*50;
        if (core::equals(maxVal, minVal))
        {
        Hue=0.f;
        Saturation=0.f;
        return;
        }

        const float delta = maxVal-minVal;
        if ( Luminance <= 50 )
        {
        Saturation = (delta)/(maxVal+minVal);
        }
        else
        {
        Saturation = (delta)/(2-maxVal-minVal);
        }
        Saturation *= 100;

        if (core::equals(maxVal, color.getRed()))
        Hue = (color.getGreen()-color.getBlue())/delta;
        else if (core::equals(maxVal, color.getGreen()))
        Hue = 2+((color.getBlue()-color.getRed())/delta);
        else // blue is max
        Hue = 4+((color.getRed()-color.getGreen())/delta);

        Hue *= 60.0f;
        while ( Hue < 0.f )
        Hue += 360;
        }
    }
    */

};


} // end namespace de.

/*
inline std::ostream &
operator<< ( std::ostream & o, de::LineCap const & lineCap )
{
   switch( lineCap )
   {
   case de::LineCap::Square: o << "Square"; break;
   case de::LineCap::Round: o << "Round"; break;
   default: o << "None"; break;
   }
   return o;
}

inline std::ostream &
operator<< ( std::ostream & o, de::LineJoin const & lineJoin )
{
   switch( lineJoin )
   {
   case de::LineJoin::Bevel: o << "Bevel"; break;
   case de::LineJoin::Round: o << "Round"; break;
   case de::LineJoin::Miter: o << "Miter"; break;
   default: o << "None"; break;
   }

   return o;
}
*/

#if 0

// ===================================================================
inline uint32_t RGBA( uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255 )
// ===================================================================
{
   return ( uint32_t( a ) << 24 ) | ( uint32_t( b ) << 16 )
        | ( uint32_t( g ) << 8 ) | uint32_t( r );
}
// ===================================================================
inline uint8_t RGBA_R( uint32_t color ) { return color & 0x000000ff; }
inline uint8_t RGBA_G( uint32_t color ) { return ( color >> 8 ) & 0x000000ff; }
inline uint8_t RGBA_B( uint32_t color ) { return ( color >> 16 ) & 0x000000ff; }
inline uint8_t RGBA_A( uint32_t color ) { return ( color >> 24 ) & 0x000000ff; }

// ===================================================================
inline void RGBA_setR( uint32_t & color, uint8_t r ) { color = (color & 0xFFFFFF00) | (uint32_t(r) & 0xFF); }
inline void RGBA_setG( uint32_t & color, uint8_t g ) { color = (color & 0xFFFF00FF) | ((uint32_t(g) & 0xFF) << 8); }
inline void RGBA_setB( uint32_t & color, uint8_t b ) { color = (color & 0xFF00FFFF) | ((uint32_t(b) & 0xFF) << 16); }
inline void RGBA_setA( uint32_t & color, uint8_t a ) { color = (color & 0x00FFFFFF) | ((uint32_t(a) & 0xFF) << 24); }

glm::vec4 RGBAf( uint32_t color32 );

float RGBA_Rf( uint32_t color );
float RGBA_Gf( uint32_t color );
float RGBA_Bf( uint32_t color );
float RGBA_Af( uint32_t color );



/*
//uint32_t RGBA_A( uint32_t color, uint8_t alpha );
//uint32_t RGBA_B( uint32_t color, uint8_t blue );
//uint32_t RGBA_G( uint32_t color, uint8_t green );
//uint32_t RGBA_R( uint32_t color, uint8_t red );

int RGBA_Ri( uint32_t color );
int RGBA_Gi( uint32_t color );
int RGBA_Bi( uint32_t color );
int RGBA_Ai( uint32_t color );

uint8_t clampByte( int v );
void randomize();
uint8_t randomByte();
uint8_t randomByteInRange( uint8_t min8, uint8_t max8 );
float randomFloat();
float randomFloatInRange( float r_min, float r_max );
float randomFloat2();

uint32_t randomColor( uint8_t alpha = 255 );
uint32_t randomColorInRange( uint8_t range_min, uint8_t range_max, uint8_t a );
uint32_t randomRGB( int minVal = -1 );

uint32_t lightenColor( uint32_t color, int offset );
uint32_t darkenColor( uint32_t color, int offset );
uint32_t varyColor( uint32_t color, int variance );

glm::vec4 randomColorf( float alpha = 1.0f );
uint32_t RGBA( glm::vec4 const & color128 );

//std::string color2string( uint32_t color );
//std::string rgba2string( uint32_t color );

std::string RGBA_toHexString( uint32_t color );
std::string RGBA_toString( uint32_t color );
std::string RGBA_toCSS( uint32_t color );
*/



#endif
