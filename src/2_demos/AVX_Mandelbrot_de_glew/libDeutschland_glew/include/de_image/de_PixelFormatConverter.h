/// @author Benjamin Hampe <benjaminhampe@gmx.de>
#pragma once
#include <de_image/de_PixelFormat.h>

namespace de {

// ===================================================================
struct PixelFormatConverter
// ===================================================================
{
   /// @brief Convert color 16 to 24, used in ImageWriterBMP, ImageWriterJPG
   /// @param[i] s Source color array
   /// @param[i] n Number of elements to convert, not a byte count.
   /// @param[o] d Destination color array
   typedef void (*Converter_t)( void const * /* src */, void * /* dst */, size_t /* n-color-reads */ );

   /// @brief Get color converter
   static Converter_t
   getConverter( PixelFormat const & src, PixelFormat const & dst );

   /// @brief Convert color 24 to 24, used in ImageWriterJPG
   static void
   convert_R8G8B8toR8G8B8( void const * src, void* dst, size_t n );

   /// @brief Convert color 32 to 24, used in ImageWriterJPG
   static void
   convert_R8G8B8A8toR8G8B8( void const * src, void* dst, size_t n );

   /// @brief Convert color 16 to 24, used in ImageWriterBMP, ImageWriterJPG
   static void
   convert_R5G6B5toR8G8B8( void const * src, void* dst, size_t n );

   /// @brief Convert color 16 to 32, used in ImageWriterPNG
   static void
   convert_R5G6B5toR8G8B8A8( void const * src, void* dst, size_t n );

   /// @brief Convert color 16 to 24, used in ImageWriterBMP
   static void
   convert_R5G5B5A1toR8G8B8( void const * src, void* dst, size_t n );

   /// @brief Convert color 32 to 32, used in ImageWriterBMP.
   static void
   convert_R8G8B8A8toR8G8B8A8( void const * src, void* dst, size_t n );

   /// @brief Convert color 16 to 32, used in ImageWriterBMP.
   static uint32_t
   R5G5B5A1toR8G8B8A8( uint16_t color );

   /// @brief Convert color 16 to 32
   static void
   convert_R5G5B5A1toR8G8B8A8( void const * src, void* dst, size_t n );

   /// @brief Convert color 16 to 16, used in ImageWriterBMP.
   static void
   convert_R5G5B5A1toR5G5B5A1( void const * src, void* dst, size_t n );

   /// @brief Convert color 24 to 32, used in ImageLoaderJPG.
   static void
   convert_R8G8B8toR8G8B8A8( void const * src, void* dst, size_t n );

   /// @brief Convert color A1R5G5B5 Color from R5G6B5 color
   static uint16_t
   R5G6B5toR5G5B5A1( uint16_t color );

   // Used in: ImageWriterTGA
   static void
   convert_R5G6B5toR5G5B5A1( void const * src, void* dst, size_t n );

   // Used in: ImageWriterBMP
   /// copies R8G8B8 24bit data to 24bit data
   static void
   convert24BitTo24Bit( uint8_t const * in, uint8_t* out, int32_t width, int32_t height, int32_t linepad, bool flip, bool bgr );
   // Used in: ImageWriterBMP

   static void
   convert_R8G8B8A8toB8G8R8( void const * src, void* dst, size_t n );

   static void
   convert_R8G8B8toB8G8R8( void const * src, void* dst, size_t n );

   static void
   convert_B8G8R8toR8G8B8( void const * src, void* dst, size_t n );

   static void
   convert_R5G5B5A1toB8G8R8( void const * src, void* dst, size_t n );

   static void
   convert_R5G6B5toB8G8R8( void const * src, void* dst, size_t n );

   static void
   convert_R8G8B8A8toB8G8R8A8( void const * src, void* dst, size_t n );

   static void
   convert_B8G8R8A8toR8G8B8A8( void const * src, void* dst, size_t n );

   static void
   convert_R8G8B8toB8G8R8A8( void const * src, void* dst, size_t n );

   // used in ImageReaderBMP
   static void
   convert_B8G8R8toR8G8B8A8( void const * src, void* dst, size_t n );

};

} // end namespace de.
