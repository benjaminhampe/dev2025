#include "ImageJPG_Writer.hpp"

#ifdef DE_IMAGE_WRITER_JPG_ENABLED

#include <sstream>
#include <cstdio> // required for jpeglib.h
extern "C"
{
   #include <jpeglib.h>
   #include <jerror.h>
}

namespace de {
namespace image {
namespace jpg {

struct JPGWriter
{
   DE_CREATE_LOGGER("de.image.JPGWriter")

   // The writer uses a 4k buffer and flushes to disk each time it's filled
   #define OUTPUT_BUF_SIZE 4096

   struct mem_destination_mgr
   {
      struct jpeg_destination_mgr pub; // public fields
      FILE* file;                      // output file
      JOCTET buffer[OUTPUT_BUF_SIZE];	// image buffer
   };

   typedef mem_destination_mgr* mem_dest_ptr;

   // init
   static void
   callback_init_destination( j_compress_ptr ctx )
   {
      if ( !ctx )
      {
         DE_ERROR("Invalid j_compress_ptr.")
         return;
      }

      // std::cout << "[Info] JPG." << __FUNCTION__ << "\n";

      mem_dest_ptr dst = (mem_dest_ptr)ctx->dest;
      if ( !dst )
      {
         DE_ERROR("Invalid mem_dest_ptr.")
         return;
      }
      dst->pub.next_output_byte = dst->buffer;
      dst->pub.free_in_buffer = OUTPUT_BUF_SIZE;
   }

   // flush to disk and reset buffer
   static boolean
   callback_empty_output_buffer( j_compress_ptr ctx )
   {
      if ( !ctx )
      {
         DE_ERROR("Invalid j_compress_ptr.")
         return FALSE;
      }

      // std::cout << "[Info] JPG." << __FUNCTION__ << "\n";

      mem_dest_ptr dst = (mem_dest_ptr)ctx->dest;
      if ( !dst )
      {
         DE_ERROR("Invalid mem_dest_ptr.")
         return FALSE;
      }
      if ( !dst->file )
      {
         DE_ERROR("Invalid FILE ptr.")
         return FALSE;
      }
      // if ( !dst->buffer )
      // {
      //    DE_ERROR("Invalid dst_buffer_ptr.")
      //    return FALSE;
      // }
      // for now just exit upon file error

      size_t const writtenBytes = ::fwrite( dst->buffer, 1, OUTPUT_BUF_SIZE, dst->file );
      if ( writtenBytes != OUTPUT_BUF_SIZE )
      {
         DE_ERROR("bufferBytes() != writtenBytes().")
         ERREXIT( ctx, JERR_FILE_WRITE );
      }

      dst->pub.next_output_byte = dst->buffer;
      dst->pub.free_in_buffer = OUTPUT_BUF_SIZE;
      return TRUE;
   }

   static void
   callback_term_destination( j_compress_ptr ctx )
   {
      if ( !ctx )
      {
         DE_ERROR("Invalid j_compress_ptr.")
         return;
      }

      // std::cout << "[Info] JPG." << __FUNCTION__ << " :: Terminate destination file.\n";

      mem_dest_ptr dst = (mem_dest_ptr)ctx->dest;
      if ( !dst )
      {
         DE_ERROR("Invalid mem_dest_ptr.")
         return;
      }
      if ( !dst->file )
      {
         DE_ERROR("Invalid FILE ptr.")
         return;
      }

      size_t const remainBytes = size_t( OUTPUT_BUF_SIZE - dst->pub.free_in_buffer );
      size_t const writtenBytes = ::fwrite( dst->buffer, 1, remainBytes, dst->file );

      // for now just exit upon file error
      if ( writtenBytes != remainBytes )
      {
         DE_ERROR("remainBytes() != writtenBytes().")
         ERREXIT( ctx, JERR_FILE_WRITE );
      }
   }

   // set up buffer data
   static void
   setup( j_compress_ptr ctx, FILE* file )
   {
      if ( !ctx )
      {
         DE_ERROR("Invalid pointer to context.")
         return;
      }
      if ( !file )
      {
         DE_ERROR("Invalid pointer to FILE.")
         return;
      }

      // Allocate on first time
      if ( !ctx->dest )
      {
         // std::cout << "[Init] JPG." << __FUNCTION__ << " :: Allocate.\n";
         auto a = j_common_ptr( ctx );
         auto b = JPOOL_PERMANENT;
         auto c = sizeof( mem_destination_mgr );
         ctx->dest = (struct jpeg_destination_mgr *)
            (*ctx->mem->alloc_small) ( a, b, c );
      }

      if ( !ctx->dest )
      {
         DE_ERROR("Allocation failed to destination_mgr.")
         return;
      }

      mem_dest_ptr dst = (mem_dest_ptr)ctx->dest;            // cast
      dst->pub.init_destination = callback_init_destination;       // Callback
      dst->pub.empty_output_buffer = callback_empty_output_buffer; // Callback
      dst->pub.term_destination = callback_term_destination;       // Callback
      dst->file = file; // Initialize private member 'a file stream'
   }

};

} // end namespace jpg.

bool
ImageWriterJPG::save( Image const & img, std::string const & fileName, uint32_t quality )
{
   if ( img.empty() )
   {
      DE_ERROR("Empty source image.")
      return false;
   }

   ColorConverter::Converter_t converter =
      ColorConverter::getConverter( img.getFormat(), ColorFormat::RGB888 );

   if ( !converter )
   {
      DE_ERROR("Cannot find color-converter for source image.")
      return false;
   }

   FILE * file = ::fopen( fileName.c_str(), "wb" );
   if ( !file )
   {
      DE_ERROR("Cant open write file.")
      return false;
   }

   int32_t w = img.getWidth();
   int32_t h = img.getHeight();

   struct jpeg_compress_struct ctx;
   struct jpeg_error_mgr jerr;
   ctx.err = jpeg_std_error( &jerr ); // fill error struct?

   jpeg_create_compress( &ctx );
   image::jpg::JPGWriter::setup( &ctx, file );
   ctx.image_width = w;
   ctx.image_height = h;
   ctx.input_components = 3;
   ctx.in_color_space = JCS_RGB;

   jpeg_set_defaults( &ctx );

   if ( quality < 1 ) quality = 100; // 85;

   jpeg_set_quality( &ctx, quality, TRUE );
   jpeg_start_compress( &ctx, TRUE );

   std::vector< uint8_t > scanline;
   scanline.resize( size_t( 3 ) * w, 0 );
   JSAMPROW scanlines[ 1 ];      // pointer to JSAMPLE row[s]
   scanlines[ 0 ] = scanline.data();

   uint8_t const * src = img.data();

   size_t const scanlineBytes = img.getStride();
   while ( ctx.next_scanline < ctx.image_height )
   {
      // convert next line
      converter( src, scanline.data(), size_t( w ) );
      jpeg_write_scanlines( &ctx, scanlines, 1 );
      src += scanlineBytes;
   }

   // Step 6: Finish compression
   jpeg_finish_compress( &ctx );

   // Step 7: Destroy
   jpeg_destroy_compress( &ctx );

   if ( file )
   {
      ::fclose( file );
      // file = nullptr;
   }

   return true;
}

} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_WRITER_JPG_ENABLED
