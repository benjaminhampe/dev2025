/// @author Benjamin Hampe <benjaminhampe@gmx.de>
#pragma once
#include <cstdint>
#include <de_glm.hpp>
#include <functional>

namespace de {

/// @brief A color format for images and textures mostly.
enum class ColorFormat
{
   RGBA8888 = 0,
   RGB888,
   R16,  // 16-bit short heightmap DTM geoTIFF (*.tif) for terrain meshes.
   R32F, // heightmap DTM geoTIFF (*.tif) for 3D engine terrain meshes.
   ARGB1555,
   RGB565,
   R8,
   R24,
   R32,
   BGRA8888,
   BGR888,
   Unknown
};

// ===================================================================
class ColorUtil
// ===================================================================
{
public:
   static uint32_t getBitsPerPixel( ColorFormat const colorFormat );
   static uint32_t getBytesPerPixel( ColorFormat const colorFormat );
   static std::string getString( ColorFormat const colorFormat );

   static uint32_t getMaxBitsPerChannel( ColorFormat const colorFormat );
   static uint32_t getChannelCount( ColorFormat const colorFormat );
   static uint32_t getBitsPerChannel( int const channel, ColorFormat const colorFormat );
};

uint8_t clampByte( int v );
void randomize();
uint8_t randomByte();
uint8_t randomByteInRange( uint8_t min8, uint8_t max8 );
float randomFloat();
float randomFloatInRange( float r_min, float r_max );
float randomFloat2();

uint32_t randomColor( uint8_t alpha = 255 );
glm::vec4 randomColorf( float alpha = 1.0f );
uint32_t randomColorInRange( uint8_t range_min, uint8_t range_max, uint8_t a );
uint32_t randomRGB( int minVal = -1 );

uint32_t RGBA( uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255 );
uint32_t RGBA( glm::vec4 const & color128 );
glm::vec4 RGBAf( uint32_t color32 );

uint8_t RGBA_R( uint32_t color );
uint8_t RGBA_G( uint32_t color );
uint8_t RGBA_B( uint32_t color );
uint8_t RGBA_A( uint32_t color );

void RGBA_setR( uint32_t & color, uint8_t r );
void RGBA_setG( uint32_t & color, uint8_t g );
void RGBA_setB( uint32_t & color, uint8_t b );
void RGBA_setA( uint32_t & color, uint8_t a );

//uint32_t RGBA_A( uint32_t color, uint8_t alpha );
//uint32_t RGBA_B( uint32_t color, uint8_t blue );
//uint32_t RGBA_G( uint32_t color, uint8_t green );
//uint32_t RGBA_R( uint32_t color, uint8_t red );
int RGBA_Ri( uint32_t color );
int RGBA_Gi( uint32_t color );
int RGBA_Bi( uint32_t color );
int RGBA_Ai( uint32_t color );

float RGBA_Rf( uint32_t color );
float RGBA_Gf( uint32_t color );
float RGBA_Bf( uint32_t color );
float RGBA_Af( uint32_t color );

uint32_t lerpColor( uint32_t bg, uint32_t fg, float t );
uint32_t blendColor( uint32_t bg, uint32_t fg );
uint32_t parseColor( std::string const & line );

//std::string color2string( uint32_t color );
//std::string rgba2string( uint32_t color );

std::string RGBA_toHexString( uint32_t color );
std::string RGBA_toString( uint32_t color );
std::string RGBA_toCSS( uint32_t color );


uint32_t lightenColor( uint32_t color, int offset );
uint32_t darkenColor( uint32_t color, int offset );
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
// ===================================================================
struct ColorConverter
// ===================================================================
{
   /// @brief Convert color 16 to 24, used in ImageWriterBMP, ImageWriterJPG
   /// @param[i] s Source color array
   /// @param[i] n Number of elements to convert, not a byte count.
   /// @param[o] d Destination color array
   typedef void (*Converter_t)( void const * /* src */, void * /* dst */, size_t /* n-color-reads */ );

   /// @brief Get color converter
   static Converter_t
   getConverter( ColorFormat const & src,
                 ColorFormat const & dst );

   /// @brief Convert color 24 to 24, used in ImageWriterJPG
   static void
   convert_RGB888toRGB888( void const * src, void* dst, size_t n );

   /// @brief Convert color 32 to 24, used in ImageWriterJPG
   static void
   convert_ARGB8888toRGB888( void const * src, void* dst, size_t n );

   /// @brief Convert color 16 to 24, used in ImageWriterBMP, ImageWriterJPG
   static void
   convert_RGB565toRGB888( void const * src, void* dst, size_t n );

   /// @brief Convert color 16 to 24, used in ImageWriterBMP
   static void
   convert_ARGB1555toRGB888( void const * src, void* dst, size_t n );

   /// @brief Convert color 32 to 32, used in ImageWriterBMP.
   static void
   convert_ARGB8888toARGB8888( void const * src, void* dst, size_t n );

   /// @brief Convert color 16 to 32, used in ImageWriterBMP.
   static uint32_t
   ARGB1555toARGB8888( uint16_t color );

   /// @brief Convert color 16 to 32
   static void
   convert_ARGB1555toARGB8888( void const * src, void* dst, size_t n );

   /// @brief Convert color 16 to 16, used in ImageWriterBMP.
   static void
   convert_ARGB1555toARGB1555( void const * src, void* dst, size_t n );

   /// @brief Convert color 24 to 32, used in ImageLoaderJPG.
   static void
   convert_RGB888toARGB8888( void const * src, void* dst, size_t n );

   /// @brief Convert color A1R5G5B5 Color from R5G6B5 color
   static uint16_t
   RGB565toARGB1555( uint16_t color );

   // Used in: ImageWriterTGA
   static void
   convert_RGB565toARGB1555( void const * src, void* dst, size_t n );

   // Used in: ImageWriterBMP
   /// copies R8G8B8 24bit data to 24bit data
   static void
   convert24BitTo24Bit( uint8_t const * in, uint8_t* out, int32_t width, int32_t height, int32_t linepad, bool flip, bool bgr );
   // Used in: ImageWriterBMP

   static void
   convert_ARGB8888toBGR888( void const * src, void* dst, size_t n );

   static void
   convert_RGB888toBGR888( void const * src, void* dst, size_t n );

   static void
   convert_BGR888toRGB888( void const * src, void* dst, size_t n );

   static void
   convert_ARGB1555toBGR888( void const * src, void* dst, size_t n );

   static void
   convert_RGB565toBGR888( void const * src, void* dst, size_t n );

   static void
   convert_ARGB8888toABGR8888( void const * src, void* dst, size_t n );

   static void
   convert_RGBA8888toBGRA8888( void const * src, void* dst, size_t n );

   static void
   convert_BGRA8888toRGBA8888( void const * src, void* dst, size_t n );

   static void
   convert_RGB888toABGR8888( void const * src, void* dst, size_t n );

   // used in ImageReaderBMP
   static void
   convert_BGR888toARGB8888( void const * src, void* dst, size_t n );

};

} // end namespace de.
