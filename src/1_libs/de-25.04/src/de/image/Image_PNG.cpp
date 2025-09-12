#include <de/image/Image_PNG.h>

#if defined(DE_IMAGE_READER_PNG_ENABLED) || defined(DE_IMAGE_WRITER_PNG_ENABLED)
    #include <cstdio>
    #include <setjmp.h>
    #include <png/png.h> // delib_png
#endif

namespace de {
namespace image {

#ifdef DE_IMAGE_READER_PNG_ENABLED

struct PNG_Util
{
    static void cb_error(png_structp ctx, png_const_charp msg)
    {
        DE_ERROR("PNG fatal error ", msg)
        longjmp( png_jmpbuf( ctx ), 1 );
    }

    static void cb_warn(png_structp ctx, png_const_charp msg)
    {
        DE_WARN("PNG warning", msg)
    }

    static void PNGAPI cb_read( png_structp ctx, png_bytep p, png_size_t n )
    {
        auto file = (Binary*)png_get_io_ptr( ctx );
        if (!file)
        {
            png_error(ctx, "PNG read error invalid file handle");
            DE_ERROR("PNG read error invalid file handle")
        }
        else
        {
            const size_t n0 = size_t( n );
            const size_t m0 = file->read( (uint8_t*)p, n0 );
            if ( m0 != n0 )
            {
                png_error( ctx, "PNG read error, unexpected bytes read");
                DE_ERROR("PNG read error, unexpected bytes read")
            }
        }
    }

    // PNG function for file writing
    static void PNGAPI cb_write( png_structp ctx, png_bytep p, png_size_t n )
    {
        FILE* file = (FILE*)png_get_io_ptr( ctx );
        size_t const n0 = size_t( n );
        size_t const m0 = fwrite( (const void*)p, 1, n0, file );
        if ( m0 != n0 )
        {
            png_error( ctx, "Write Error" );
        }
    }
};

bool
ImageReaderPNG::load( Image & img, const uint8_t* pSrcBytes, size_t nSrcBytes, const std::string& uri )
{
    Binary file(pSrcBytes,nSrcBytes,uri);

    if ( !file.is_open() )
    {
        DE_ERROR("Empty memory, ",uri)
        return false;
    }

    img.setUri( uri );

    // Read the first few bytes of the PNG file
    png_byte buffer[8];

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
        (png_error_ptr)PNG_Util::cb_error,
        (png_error_ptr)PNG_Util::cb_warn );

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
    png_set_read_fn( ctx, (void*)&file, PNG_Util::cb_read );
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
    //PixelFormat dstType = PixelFormat::R8G8B8A8;
    // Create the image structure to be filled by png data
    if ( srcFmt == PNG_COLOR_TYPE_RGB_ALPHA )
    {
        srcType = PixelFormat::R8G8B8A8;
        //dstType = PixelFormat::R8G8B8A8;
    }
    else
    {
        srcType = PixelFormat::R8G8B8;
        //dstType = PixelFormat::R8G8B8;
    }

    // PixelFormatConverter::Converter_t converter =
    //     PixelFormatConverter::getConverter( srcType, dstType );
    // if ( !converter )
    // {
    //     DE_ERROR("No converter ", srcType.str()," to ", dstType.str())
    //     return false;
    // }

    png_color_16 background;
    png_color_16p pBG = &background;
    if ( (PNG_INFO_bKGD) == png_get_bKGD( ctx, inf, &pBG ) )
    {
        //DE_DEBUG("PNG has background-color ", background.red,",",background.green,",",background.blue)
    }

    //Image tmp( w, h, srcType );
    //img.clear();
    //img.resize( w, h, dstType );
    img.resize( w, h, srcType );

    // Create array of pointers to rows in image data
    std::vector< uint8_t* > rows;
    rows.reserve( h );
    for ( uint32_t i = 0; i < h; ++i )
    {
        rows.emplace_back( img.getRow( i ) );
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

    // file.close();

    // for ( int y = 0; y < tmp.h(); ++y )
    // {
    //     converter( tmp.getRow( y ), img.getRow( y ), img.w() );
    // }

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

#endif // DE_IMAGE_READER_PNG_ENABLED

#ifdef DE_IMAGE_WRITER_PNG_ENABLED

bool
ImageWriterPNG::save( Image const & img, std::string const & uri, uint32_t quality )
{
    if ( img.empty() )
    {
        DE_ERROR( "Empty image(", img.str(), ")" )
        return false;
    }

    FILE* file = ::fopen( uri.c_str(), "wb" );
    if ( !file )
    {
        DE_ERROR( "Failed openToWriteBinary(", uri, ")." )
        return false;
    }

    // Allocate the png write struct
    png_struct* ctx = png_create_write_struct( PNG_LIBPNG_VER_STRING,
        nullptr,
        (png_error_ptr)PNG_Util::cb_error,
        (png_error_ptr)PNG_Util::cb_warn
    );

    if ( !ctx )
    {
        DE_ERROR( "Cant create PNG encoder for image(", img.str(), ")" )
        return false;
    }

    // Allocate the png info struct
    png_info* inf = png_create_info_struct( ctx );
    if ( !inf )
    {
        DE_ERROR( "Failed png_create_info_struct" )
        png_destroy_write_struct( &ctx, nullptr );
        return false;
    }

    // for proper error handling
    if ( setjmp( png_jmpbuf( ctx ) ) )
    {
        DE_ERROR( "Failed setjmp( png_jmpbuf( ctx ) )" )
        png_destroy_write_struct( &ctx, &inf );
        return false;
    }


    png_set_write_fn(
        ctx,
        file,
        PNG_Util::cb_write,
        nullptr
    );

    uint32_t w = img.w();
    uint32_t h = img.h();

    // png_destroy_write_struct( &ctx, &inf );
    int32_t pngTransformType = PNG_TRANSFORM_IDENTITY;
    int32_t pngColorType = PNG_COLOR_TYPE_RGBA;

    PixelFormat srcFmt = img.pixelFormat();
    PixelFormat dstFmt = PixelFormat::Unknown;

    if (img.channelCount() == 1)
    {
        dstFmt = PixelFormat::R8G8B8;
        pngColorType = PNG_COLOR_TYPE_RGB;
    }
    else if (img.channelCount() == 3)
    {
        dstFmt = PixelFormat::R8G8B8;
        pngColorType = PNG_COLOR_TYPE_RGB;
    }
    else if (img.channelCount() == 4)
    {
        dstFmt = PixelFormat::R8G8B8A8;
        //pngColorType = PNG_COLOR_TYPE_RGB;
    }

    if (srcFmt == PixelFormat::Unknown)
    {
        DE_ERROR( "Unsupported (srcFmt == PixelFormat::Unknown), srcFmt(", srcFmt.str(), ") -> dstFmt(", dstFmt.str(), ")." )
        return false;
    }

    if (dstFmt == PixelFormat::Unknown)
    {
        DE_ERROR( "Unsupported (dstFmt == PixelFormat::Unknown), srcFmt(", srcFmt.str(), ") -> dstFmt(", dstFmt.str(), ")." )
        return false;
    }

    // DE_TRACE( "Determined dstFmt(", dstFmt.str(), ") <- srcFmt(", srcFmt.str(), ")." )

    const uint32_t srcBPP = srcFmt.bytesPerPixel();
    const uint32_t dstBPP = dstFmt.bytesPerPixel();
    const size_t pixelCount = size_t( w ) * size_t( h );

    std::vector< uint8_t* > rowPointers;
    rowPointers.resize( h, nullptr );

    std::vector< uint8_t > tmpImage;

    if (srcFmt != dstFmt)
    {
        const size_t byteCount = pixelCount * dstBPP;
        tmpImage.resize( byteCount, 0 );

        auto converter = PixelFormatConverter::getConverter(srcFmt, dstFmt);
        if (!converter)
        {
            DE_ERROR( "Unsupported converter srcFmt(", srcFmt.str(), ") -> dstFmt(", dstFmt.str(), ")." )
            return false;
        }

        converter(img.data(), tmpImage.data(), pixelCount);

        auto pDst = const_cast< uint8_t* >( tmpImage.data() );
        const size_t stride = dstBPP * w;
        for ( size_t y = 0; y < h; ++y )
        {
            rowPointers[ y ] = pDst;
            pDst += stride;
        }
    }
    else
    {
        auto pDst = const_cast< uint8_t* >( img.data() );
        const size_t stride = img.stride();
        for ( size_t y = 0; y < h; ++y )
        {
            rowPointers[ y ] = pDst;
            pDst += stride;
        }
    }

    //DE_DEBUG( "Write image (", img.toString(), ")." )

    // Build array of scanlines (pointers to rows)

    // for proper error handling
    if ( setjmp( png_jmpbuf( ctx ) ) )
    {
        png_destroy_write_struct( &ctx, &inf );
        return false;
    }

    png_set_IHDR(
        ctx,
        inf,
        w,
        h,
        8,
        pngColorType,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );

    png_bytepp p = rowPointers.data();
    png_set_rows( ctx, inf, p );
    png_write_png( ctx, inf, pngTransformType, nullptr );
    png_destroy_write_struct( &ctx, &inf );

    ::fclose( file );

    return true;
}

#endif // DE_IMAGE_WRITER_PNG_ENABLED


} // end namespace image
} // end namespace de.

