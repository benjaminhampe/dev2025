#include "ImageJPG_Reader.hpp"

#include <assert.h>
#include <cstdio>    // required for jpeglib.h
#include <setjmp.h>  // is used for the optional error recovery mechanism

extern "C"
{
   #include <jpeglib.h>
   #include <jerror.h>
}

namespace de {
namespace image {
namespace jpg {

   struct JPGLoader
   {
      // struct for handling jpeg errors
      struct my_jpeg_error_mgr
      {
         struct jpeg_error_mgr pub; // public jpeg error fields
         jmp_buf jmp_buffer; // for longjmp, to return to caller on a fatal error
      };

      static void
      init_source ( j_decompress_ptr ctx ) {}

      static boolean
      fill_input_buffer ( j_decompress_ptr ctx ) { return 1; }

      static void
      skip_input_data ( j_decompress_ptr ctx, long count )
      {
         assert( ctx );
         jpeg_source_mgr * src = ctx->src;
         if( count > 0 )
         {
            src->bytes_in_buffer -= count;
            src->next_input_byte += count;
         }
      }

      static void
      term_source( j_decompress_ptr ctx ) {}

      static void
      error_exit ( j_common_ptr ctx )
      {
         assert( ctx );
         // unfortunately we need to use a goto rather than throwing an exception
         // as gcc crashes under linux crashes when using throw from within
         // extern c code

         // Always display the message
         (*ctx->err->output_message)( ctx );

         // ctx->err really points to a irr_error_mgr struct
         my_jpeg_error_mgr *myerr = (my_jpeg_error_mgr*)ctx->err;

         longjmp( myerr->jmp_buffer, 1 );
      }

      static void
      output_message( j_common_ptr ctx )
      {
         assert( ctx );
         char msg[ JMSG_LENGTH_MAX ];
         (*ctx->err->format_message)( ctx, msg );
         // std::cout << "JPG.loadImageFile(" << s_FileName << " :: Got error(" << msg << ")\n";
      }

   };

} // end namespace jpg


bool
ImageReaderJPG::load( Image & img, std::string const & uri )
{
   Binary file( uri );
   return load( img, file );
}

bool
ImageReaderJPG::load( Image & img, Binary & file )
{
   if ( !file.is_open() )
   {
      DE_ERROR( "No open binary ",file.getUri() )
      return false;
   }

   img.setUri( file.getUri() );

#if 0
   ColorConverter::Converter_t converter = nullptr;

   if ( img.getFormat().equals( 8,8,8 ) ) // case ECF_R8G8B8:
   {
      converter = ColorConverter::convert_R8G8B8toR8G8B8;
   }
   else if ( img.getFormat().equals( 8,8,8,8 ) ) // case ECF_A8R8G8B8:
   {
      converter = ColorConverter::convert_A8R8G8B8toR8G8B8;
   }
   else if ( img.getFormat().equals( 5,6,5 ) ) // case ECF_R5G6B5:
   {
      converter = ColorConverter::convert_R5G6B5toR8G8B8;
   }
   else if ( img.getFormat().equals( 5,5,5,1 ) ) // case ECF_A1R5G5B5:
   {
      converter = ColorConverter::convert_A1R5G5B5toB8G8R8;
   }

   if ( !converter )
   {
      std::cout << "JPG." << __FUNCTION__ << " :: [Error] Cannot find color-converter for source image.\n";
      return false;
   }
#endif

   // allocate and initialize JPEG decompression object
   struct jpeg_decompress_struct ctx;
   struct jpg::JPGLoader::my_jpeg_error_mgr jerr;

   //We have to set up the error handler first, in case the initialization
   //step fails.  (Unlikely, but it could happen if you are out of memory.)
   //This routine fills in the contents of struct jerr, and returns jerr's
   //address which we place into the link field in ctx.

   ctx.err = jpeg_std_error( &jerr.pub );
   ctx.err->error_exit = jpg::JPGLoader::error_exit;
   ctx.err->output_message = jpg::JPGLoader::output_message;

   // compatibility fudge:
   // we need to use setjmp/longjmp for error handling as gcc-linux
   // crashes when throwing within external c code
   if ( setjmp( jerr.jmp_buffer ) )
   {
      // If we get here, the JPEG code has signaled an error.
      // We need to clean up the JPEG object and return.
      jpeg_destroy_decompress( &ctx );
      // if the row pointer was created, we delete it.

      DE_ERROR( "Cant setjmp uri(", file.getUri(), ")")
      return false;
   }

   // Now we can initialize the JPEG decompression object.
   jpeg_create_decompress( &ctx );

   // specify data source
   jpeg_source_mgr jsrc;

   // Set up data pointer
   jsrc.bytes_in_buffer = file.size();
   jsrc.next_input_byte = (JOCTET*)file.data();
   jsrc.init_source = jpg::JPGLoader::init_source;
   jsrc.fill_input_buffer = jpg::JPGLoader::fill_input_buffer;
   jsrc.skip_input_data = jpg::JPGLoader::skip_input_data;
   jsrc.term_source = jpg::JPGLoader::term_source;
   jsrc.resync_to_restart = jpeg_resync_to_restart;
   ctx.src = &jsrc;

   // Decodes JPG bytes from whatever source
   // Does everything AFTER jpeg_create_decompress
   // and BEFORE jpeg_destroy_decompress
   // Caller is responsible for arranging these + setting up ctx
   jpeg_read_header( &ctx, TRUE );

   bool useCMYK=false;
   if ( ctx.jpeg_color_space == JCS_CMYK )
   {
      ctx.out_color_space = JCS_CMYK;
      ctx.out_color_components=4;
      useCMYK=true;
   }
   else
   {
      ctx.out_color_space=JCS_RGB;
      ctx.out_color_components=4;
   }
   ctx.output_gamma = 2.2;
   ctx.do_fancy_upsampling = FALSE;

   // Start decompressor
   jpeg_start_decompress( &ctx );

   // Get image data
   uint32_t width = ctx.image_width;
   uint32_t height = ctx.image_height;
   //uint32_t rowspan = width * ctx.out_color_components;
   img.resize( width, height );

   // Here we use the library's state variable ctx.output_scanline as the
   // loop counter, so that we don't have to keep track ourselves.
   // Create array of row pointers for lib
   //rowPtr = new uint8_t* [ height ];
   std::vector< uint8_t > row;
   row.resize( width * 4, 0 );

   ColorConverter::Converter_t converter =
      ColorConverter::getConverter( ColorFormat::RGB888, img.getFormat() );

   //JSAMPARRAY
   JSAMPROW rows[ 1 ];      // pointer to JSAMPLE row[s]
   rows[ 0 ] = row.data();

   int32_t y = 0;
   while( ctx.output_scanline < ctx.output_height )
   {
      jpeg_read_scanlines( &ctx, rows, 1 );
      converter( row.data(), img.getRow( y ), size_t( width ) );
      ++y;
   }

   // Finish decompression
   jpeg_finish_decompress( &ctx );

   // Release JPEG decompression object
   // This is an important step since it will release a good deal of memory.
   jpeg_destroy_decompress( &ctx );

   // convert image
   if (useCMYK)
   {
      DE_DEBUG("JPEG uses CMYK colorspace.")

      Image tmp( width, height );
      uint32_t const size = 4*width*height;
      uint8_t* tmp_data = (uint8_t*)tmp.getRow( 0 );
      uint8_t const * img_data = (uint8_t const*)img.getRow( 0 );
      if ( tmp_data )
      {
         for ( uint32_t i=0, j=0; i < size; i += 4, j+=4 )
         {
            tmp_data[i+0] = (char)( img_data[j+2]*(img_data[j+3]/255.f));
            tmp_data[i+1] = (char)( img_data[j+1]*(img_data[j+3]/255.f));
            tmp_data[i+2] = (char)( img_data[j+0]*(img_data[j+3]/255.f));
            tmp_data[i+3] = 255;
         }
      }
      img = tmp;
   }
   return true;
}

} // end namespace image.
} // end namespace de.
