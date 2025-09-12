#include "de_ImagePNG_Reader.h"

#ifdef DE_IMAGE_READER_PNG_ENABLED

#include <de_image/de_PixelFormatConverter.h>

#include <cstdio>
#include <setjmp.h>
#include <png/png.h> // delib_png

namespace de {
namespace image {
namespace png {

struct PNGWriter
{
   static void
   cb_error(png_structp ctx, png_const_charp msg)
   {
      // os::Printer::log("PNG fatal error", msg, ELL_ERROR);
      longjmp( png_jmpbuf( ctx ), 1 );
   }

   static void
   cb_warn(png_structp ctx, png_const_charp msg)
   {
      // os::Printer::log("PNG warning", msg, ELL_WARNING);
   }

   // PNG function for file reading
   static void PNGAPI
   cb_read_user_data( png_structp ctx, png_bytep data, png_size_t length )
   {
      Binary* file = (Binary*)png_get_io_ptr( ctx );
      if ( !file )
      {
         png_error( ctx, "PNG read error invalid file handle" );
      }
      size_t const expectBytes = size_t( length );
      size_t const resultBytes = file->read( (uint8_t*)data, expectBytes );
      if ( resultBytes != expectBytes )
      {
         png_error( ctx, "PNG read error, unexpected bytes read" );
      }
   }
};

} // end namespace png

bool
ImageReaderPNG::load( Image & img, std::string const & uri )
{
   Binary file( uri );
   if ( !file.is_open() )
   {
      DE_ERROR( "Cant open file stream ",uri )
      return false;
   }

   return load( img, file );
}

bool
ImageReaderPNG::load( Image & img, Binary & file )
{
   std::string const & uri = file.getUri();
   if ( !file.is_open() )
   {
      DE_ERROR( "Cant open binary file(",uri,").")
      return false;
   }

   img.setUri( file.getUri() );

   png_byte buffer[8];
   // Read the first few bytes of the PNG file
   size_t resultCount = file.read( buffer, 8 );
   if( resultCount != 8 )
   {
      DE_ERROR( "Can't read file ", uri )
      return false;
   }

   // Check if it really is a PNG file
   if( png_sig_cmp( buffer, 0, 8 ) )
   {
      DE_ERROR( "Not really a png ", uri )
      return false;
   }

   // Allocate the png read struct
   png_structp ctx = png_create_read_struct( PNG_LIBPNG_VER_STRING, nullptr,
      (png_error_ptr)image::png::PNGWriter::cb_error,
      (png_error_ptr)image::png::PNGWriter::cb_warn );

   if ( !ctx )
   {
      DE_ERROR( "No png read struct ", uri )
      return false;
   }

   // Allocate the png info struct
   png_infop inf = png_create_info_struct( ctx );
   if ( !inf )
   {
      DE_ERROR( "No png info struct ", uri )
      png_destroy_read_struct( &ctx, nullptr, nullptr );
      return false;
   }

   // for proper error handling
   if ( setjmp( png_jmpbuf( ctx ) ) )
   {
      DE_ERROR( "No setjmp( png_jmpbuf( ctx ) ) ", uri )
      png_destroy_read_struct( &ctx, &inf, nullptr );
      return false;
   }

   // changed by zola so we don't need to have public FILE pointers
   png_set_read_fn( ctx, (void*)&file, image::png::PNGWriter::cb_read_user_data );

   // Tell png that we had read the signature
   png_set_sig_bytes( ctx, 8 );
   // Read the info section of the png file
   png_read_info( ctx, inf );

   uint32_t w = 0, h = 0;
   int32_t bpp = 0, srcFmt = 0;
   {
      // Extract info
      png_uint_32 tmp_w, tmp_h;
      png_get_IHDR( ctx, inf, &tmp_w, &tmp_h, &bpp, &srcFmt, nullptr, nullptr, nullptr );
      w = tmp_w;
      h = tmp_h;
   }
   // Convert palette color to true color
   if ( srcFmt == PNG_COLOR_TYPE_PALETTE )
   {
      DE_DEBUG("PNG_COLOR_TYPE_PALETTE to rgb")
      png_set_palette_to_rgb( ctx );
   }

   // Convert low bit colors to 8 bit colors
   if ( bpp < 8 )
   {
      DE_DEBUG("bpp < 8")
      if ( srcFmt == PNG_COLOR_TYPE_GRAY || srcFmt == PNG_COLOR_TYPE_GRAY_ALPHA )
      {
         DE_DEBUG("png_set_expand_gray_1_2_4_to_8")
         png_set_expand_gray_1_2_4_to_8( ctx );
      }
      else
      {
         DE_DEBUG("png_set_packing")
         png_set_packing( ctx );
      }
   }

   if ( png_get_valid( ctx, inf, PNG_INFO_tRNS ) )
   {
      png_set_tRNS_to_alpha(ctx);
   }

   // Convert high bit colors to 8 bit colors
   if ( bpp == 16 )
   {
      DE_DEBUG("bpp == 16")
      png_set_strip_16( ctx );
   }

   // Convert gray color to true color
   if ( srcFmt == PNG_COLOR_TYPE_GRAY || srcFmt == PNG_COLOR_TYPE_GRAY_ALPHA )
   {
      DE_DEBUG("PNG_COLOR_TYPE_GRAY or PNG_COLOR_TYPE_GRAY_ALPHA")
      png_set_gray_to_rgb( ctx );
   }

   int intent;
   const double screen_gamma = 2.2;

   if ( png_get_sRGB( ctx, inf, &intent ) )
   {
      png_set_gamma( ctx, screen_gamma, 0.45455 );
   }
   else
   {
      double image_gamma;
      if ( png_get_gAMA( ctx, inf, &image_gamma ) )
      {
         png_set_gamma( ctx, screen_gamma, image_gamma );
      }
      else
      {
         png_set_gamma( ctx, screen_gamma, 0.45455 );
      }
   }

   // Update the changes in between, as we need to get the new color type
   // for proper processing of the RGBA type
   png_read_update_info( ctx, inf );
   {
      // Use temporary variables to avoid passing casted pointers
      png_uint_32 tmp_w, tmp_h;
      // Extract info
      png_get_IHDR(ctx, inf, &tmp_w, &tmp_h, &bpp, &srcFmt, NULL, NULL, NULL);
      w = tmp_w;
      h = tmp_h;
   }

   // Convert RGBA to BGRA
   if ( srcFmt == PNG_COLOR_TYPE_RGB_ALPHA )
   {
      //DE_DEBUG("PNG_COLOR_TYPE_RGBA")
#ifdef __BIG_ENDIAN__
      png_set_swap_alpha( ctx );
#else
      // png_set_bgr( ctx );
#endif
   }

   PixelFormat srcType = PixelFormat::R8G8B8A8;
   PixelFormat dstType = PixelFormat::R8G8B8A8;
   // Create the image structure to be filled by png data
   if ( srcFmt == PNG_COLOR_TYPE_RGB_ALPHA )
   {
      srcType = PixelFormat::R8G8B8A8;
   }
   else
   {
      srcType = PixelFormat::R8G8B8;
   }

   PixelFormatConverter::Converter_t converter =
      PixelFormatConverter::getConverter( srcType, dstType );
   if ( !converter )
   {
      DE_ERROR("No <color-converter> from (", PixelFormatUtil::getString(srcType)," "
                                    "to (", PixelFormatUtil::getString(dstType), ")" )
      return false;
   }

   png_color_16 background;
   png_color_16p pBG = &background;
   if ( (PNG_INFO_bKGD) == png_get_bKGD( ctx, inf, &pBG ) )
   {
      //DE_DEBUG("PNG has background-color ", background.red,",",background.green,",",background.blue)
   }

   Image tmp( w, h, srcType );
   img.clear();
   img.setFormat( dstType );
   img.resize( w, h );

   // Create array of pointers to rows in image data
   std::vector< uint8_t* > rows;
   rows.reserve( h );
   for ( uint32_t i = 0; i < h; ++i )
   {
      rows.emplace_back( tmp.getRow( i ) );
   }

   // for proper error handling
   if ( setjmp( png_jmpbuf( ctx ) ) )
   {
      png_destroy_read_struct( &ctx, &inf, nullptr );
      return false;
   }

   png_read_image( ctx, rows.data() );
   png_read_end( ctx, nullptr );
   png_destroy_read_struct( &ctx, &inf, 0 ); // Clean up memory

   file.close();

   for ( int y = 0; y < tmp.getHeight(); ++y )
   {
      converter( tmp.getRow( y ), img.getRow( y ), img.getWidth() );
   }

//   else // No converter
//   {
//      // Create array of pointers to rows in image data
//      std::vector< uint8_t* > rows;
//      rows.reserve( h );
//      for ( uint32_t i = 0; i < h; ++i )
//      {
//         rows.emplace_back( img.getRow( i ) );
//      }

//      // for proper error handling
//      if ( setjmp( png_jmpbuf( ctx ) ) )
//      {
//         png_destroy_read_struct( &ctx, &inf, nullptr );
//         return false;
//      }

//      png_read_image( ctx, rows.data() );
//      png_read_end( ctx, nullptr );
//      png_destroy_read_struct( &ctx, &inf, 0 ); // Clean up memory
//      ::fclose( file );
//   }

   return true;
}

} // end namespace image
} // end namespace de.

#endif // DE_IMAGE_READER_PNG_ENABLED
