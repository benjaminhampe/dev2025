#include <de/image/Image_JPG.h>

#if defined(DE_IMAGE_READER_JPG_ENABLED) || defined(DE_IMAGE_WRITER_JPG_ENABLED)

#include <assert.h>
#include <cstdio>    // required for jpeglib.h
#include <setjmp.h>  // is used for the optional error recovery mechanism

extern "C"
{
   #include <jpeglib.h>
   #include <jerror.h>
}

#endif


namespace de {
namespace image {

#ifdef DE_IMAGE_READER_JPG_ENABLED

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

//bool
//ImageReaderJPG::load( Image & img, Binary & file )

bool
ImageReaderJPG::load( Image & img, const uint8_t* pSrcBytes, size_t nSrcBytes, const std::string& uri )
{
    Binary file(pSrcBytes,nSrcBytes,uri);
    if ( !file.is_open() )
    {
        DE_ERROR( "No memory data ", uri )
        return false;
    }

    img.setUri( uri );

    // 1. allocate and initialize JPEG decompression object
    // 2. We have to set up the error handler first, in case the initialization
    //step fails.  (Unlikely, but it could happen if you are out of memory.)
    //This routine fills in the contents of struct jerr, and returns jerr's
    //address which we place into the link field in ctx.
    // 3. compatibility fudge:
    // we need to use setjmp/longjmp for error handling as gcc-linux
    // crashes when throwing within external c code
    // If we get here, the JPEG code has signaled an error.
    // We need to clean up the JPEG object and return.
    // if the row pointer was created, we delete it.
    struct jpeg_decompress_struct ctx;
    struct jpg::JPGLoader::my_jpeg_error_mgr jerr;
    ctx.err = jpeg_std_error( &jerr.pub );
    ctx.err->error_exit = jpg::JPGLoader::error_exit;
    ctx.err->output_message = jpg::JPGLoader::output_message;
    if ( setjmp( jerr.jmp_buffer ) )
    {
        jpeg_destroy_decompress( &ctx );
        DE_ERROR( "Cant setjmp uri(", uri, ")")
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

    bool useCMYK = (ctx.jpeg_color_space == JCS_CMYK);
    if ( useCMYK )
    {
        ctx.out_color_space = JCS_CMYK;
        ctx.out_color_components = 4;
    }
    else
    {
        ctx.out_color_space = JCS_RGB;
        ctx.out_color_components = 3;  // ?
    }

    ctx.output_gamma = 2.2;
    ctx.do_fancy_upsampling = FALSE;

    // Start decompressor
    jpeg_start_decompress( &ctx );

    // Get image data
    uint32_t w = ctx.image_width;
    uint32_t h = ctx.image_height;
    img.resize( w, h, PixelFormat::R8G8B8 );
    //uint32_t rowspan = width * ctx.out_color_components;

    // convert image
    if (useCMYK)
    {
       DE_DEBUG("JPEG uses unsupported CMYK colorspace.")

       /*
        Image tmp( w, h );
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
            img = tmp;
        }
        */

   }


#if 0
   PixelFormatConverter::Converter_t converter = nullptr;

   if ( img.getFormat().equals( 8,8,8 ) ) // case ECF_R8G8B8:
   {
      converter = PixelFormatConverter::convert_R8G8B8toR8G8B8;
   }
   else if ( img.getFormat().equals( 8,8,8,8 ) ) // case ECF_A8R8G8B8:
   {
      converter = PixelFormatConverter::convert_A8R8G8B8toR8G8B8;
   }
   else if ( img.getFormat().equals( 5,6,5 ) ) // case ECF_R5G6B5:
   {
      converter = PixelFormatConverter::convert_R5G6B5toR8G8B8;
   }
   else if ( img.getFormat().equals( 5,5,5,1 ) ) // case ECF_A1R5G5B5:
   {
      converter = PixelFormatConverter::convert_A1R5G5B5toB8G8R8;
   }

   if ( !converter )
   {
      std::cout << "JPG." << __FUNCTION__ << " :: [Error] Cannot find color-converter for source image.\n";
      return false;
   }
#endif

    // Here we use the library's state variable ctx.output_scanline as the
    // loop counter, so that we don't have to keep track ourselves.
    // Create array of row pointers for lib
    //rowPtr = new uint8_t* [ height ];
    std::vector< uint8_t > scanline;
    scanline.resize( w * 4, 0 );

    PixelFormatConverter::Converter_t converter =
      PixelFormatConverter::getConverter( PixelFormat::R8G8B8, PixelFormat::R8G8B8 );

    //JSAMPARRAY
    JSAMPROW jpg_rows[ 1 ];      // pointer to JSAMPLE scanline[s]
    jpg_rows[ 0 ] = scanline.data();

    int32_t y = 0;
    while( ctx.output_scanline < ctx.output_height )
    {
        jpeg_read_scanlines( &ctx, jpg_rows, 1 );
        if (y < img.h())
        {
            converter( scanline.data(), img.getRow( y ), size_t( w ) );
        }
        ++y;
    }

    // Finish decompression
    jpeg_finish_decompress( &ctx );

    // Release JPEG decompression object
    // This is an important step since it will release a good deal of memory.
    jpeg_destroy_decompress( &ctx );


   return true;
}

#endif


#ifdef DE_IMAGE_WRITER_JPG_ENABLED

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

   // Always write RGB24
   PixelFormatConverter::Converter_t converter =
      PixelFormatConverter::getConverter( img.pixelFormat(), PixelFormat::R8G8B8 );

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

   int32_t w = img.w();
   int32_t h = img.h();

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

   size_t const scanlineBytes = img.stride();
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

#endif // DE_IMAGE_WRITER_JPG_ENABLED




} // end namespace image.
} // end namespace de.

