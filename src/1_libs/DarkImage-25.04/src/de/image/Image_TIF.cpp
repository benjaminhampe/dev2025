#include <de/image/Image_TIF.h>

#if defined(DE_IMAGE_READER_TIF_ENABLED) || defined(DE_IMAGE_WRITER_TIF_ENABLED)

#include <tiff4/tiffio.h>

namespace de {
namespace image {
namespace {

    struct AutoTIFF
   {
      TIFF* m_tif;

      AutoTIFF()
         : m_tif( nullptr )
      {}

      AutoTIFF( std::string uri )
         : m_tif( nullptr )
      {
         openToRead( uri );
      }

      void openToRead( std::string uri )
      {
         if ( m_tif )
         {
            return;
         }
         m_tif = TIFFOpen( uri.c_str(), "r");
      }

      ~AutoTIFF()
      {
         closeFile();
      }

      bool is_open() const
      {
         return( m_tif != nullptr );
      }

      void closeFile()
      {
         if ( !m_tif )
         {
            return;
         }
         TIFFClose( m_tif );
         m_tif = nullptr;
      }
   };

} // end namespace tif.
} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_READER_TIF_ENABLED || DE_IMAGE_WRITER_TIF_ENABLED

namespace de {
namespace image {

#if defined(DE_IMAGE_READER_TIF_ENABLED)

/*
#include <tiffio.h>
#include <vector>
#include <cstring>
#include <iostream>

struct MemoryStream {
    const uint8_t* data;
    size_t size;
    size_t pos;
};

// Custom TIFF read function from memory
tsize_t readFromMemory(thandle_t handle, void* buf, tsize_t size) {
    MemoryStream* stream = static_cast<MemoryStream*>(handle);
    if (stream->pos + size > stream->size)
        size = stream->size - stream->pos; // Prevent overflow
    std::memcpy(buf, stream->data + stream->pos, size);
    stream->pos += size;
    return size;
}

// Seek function for memory stream
toff_t seekInMemory(thandle_t handle, toff_t offset, int whence) {
    MemoryStream* stream = static_cast<MemoryStream*>(handle);
    if (whence == SEEK_SET) {
        stream->pos = offset;
    } else if (whence == SEEK_CUR) {
        stream->pos += offset;
    } else if (whence == SEEK_END) {
        stream->pos = stream->size + offset;
    }
    return stream->pos;
}

// Close function (no-op for memory)
int closeMemory(thandle_t handle) {
    return 0;
}

// Size function for memory stream
toff_t sizeOfMemory(thandle_t handle) {
    return static_cast<MemoryStream*>(handle)->size;
}

void readTIFFFromMemory(std::vector<uint8_t>& tiffData) {
    MemoryStream stream = { tiffData.data(), tiffData.size(), 0 };

    TIFF* tif = TIFFClientOpen("MemoryTIFF", "r", &stream,
                               readFromMemory, readFromMemory, seekInMemory, closeMemory,
                               sizeOfMemory, nullptr, nullptr);

    if (!tif) {
        std::cerr << "Failed to open TIFF from memory" << std::endl;
        return;
    }

    uint32 width, height;
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
    std::cout << "TIFF dimensions: " << width << "x" << height << std::endl;

    TIFFClose(tif);
}

int main() {
    std::vector<uint8_t> tiffData = ...; // Load TIFF binary data here
    readTIFFFromMemory(tiffData);
    return 0;
}
*/

struct TIF_Util
{
    // Custom TIFF read function from memory
    static tsize_t readFromMemory(thandle_t handle, void* buf, tsize_t n)
    {
        Binary* file = static_cast<Binary*>(handle);
        auto m = file->read(buf,n);
        if (m != n)
        {
            DE_ERROR("m != n")
        }
        return n;
    }

    // Seek function for memory stream
    static toff_t seekInMemory(thandle_t handle, toff_t offset, int whence)
    {
        Binary* file = static_cast<Binary*>(handle);
        file->seek(offset,whence);
        return file->tell();
    }

    // Close function (no-op for memory)
    static int closeMemory(thandle_t handle) { return 0; }

    // Size function for memory stream
    static toff_t sizeOfMemory(thandle_t handle) { return static_cast<Binary*>(handle)->size(); }
};

bool
ImageReaderTIF::load( Image & img, const uint8_t* p, size_t n, const std::string& uri)
{
    //AutoTIFF file( uri );
    //if ( !file.is_open() )
    //{
    //  DE_ERROR( "Cant open TIFF file ", uri )
    //  return false;
    //}
    //TIFF* tif = file.m_tif;

    Binary stream(p,n,uri);

    TIFF* tif = TIFFClientOpen("MemoryTIFF", "r", &stream,
                               TIF_Util::readFromMemory,
                               TIF_Util::readFromMemory,
                               TIF_Util::seekInMemory,
                               TIF_Util::closeMemory,
                               TIF_Util::sizeOfMemory,
                               nullptr,
                               nullptr);

    if (!tif)
    {
        DE_ERROR("Failed to open TIFF from memory")
        return false;
    }


    uint32_t w = 0;
    uint32_t h = 0;
    uint32_t res_unit = 0;
    uint32_t sample_fmt = 0;
    uint32_t compression = 0;
    uint32_t photometric = 0;
    uint32_t planar_config = 0;

    //TIFFLoadMethod loadMethod = LoadAsGenericStrip;

    TIFFGetField( tif, TIFFTAG_IMAGEWIDTH, &w );
    TIFFGetField( tif, TIFFTAG_IMAGELENGTH, &h );
    TIFFGetField( tif, TIFFTAG_RESOLUTIONUNIT, &res_unit );
    TIFFGetField( tif, TIFFTAG_SAMPLEFORMAT, &sample_fmt );
    TIFFGetField( tif, TIFFTAG_COMPRESSION, &compression );
    TIFFGetField( tif, TIFFTAG_PHOTOMETRIC, &photometric);
    TIFFGetFieldDefaulted(tif, TIFFTAG_PLANARCONFIG, &planar_config);

    uint32_t samples_per_pixel = 0;
    uint32_t bits_per_sample = 0;
    TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samples_per_pixel);
    TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bits_per_sample);
    uint32_t bpp = samples_per_pixel * bits_per_sample;

    uint32_t iccSize = 0;
    uint32_t iccBuf = 0;
    TIFFGetField(tif, TIFFTAG_ICCPROFILE, &iccSize, &iccBuf);

    bool isTiled = TIFFIsTiled(tif) != 0;

    DE_DEBUG( "TIFF.w = ",w )
    DE_DEBUG( "TIFF.h = ",h )
    DE_DEBUG( "TIFF.bpp = ",bpp )
    DE_DEBUG( "TIFF.samples_per_pixel = ",samples_per_pixel )
    DE_DEBUG( "TIFF.bits_per_sample = ",bits_per_sample )
    DE_DEBUG( "TIFF.res_unit = ",res_unit )
    DE_DEBUG( "TIFF.sample_fmt = ",sample_fmt )
    DE_DEBUG( "TIFF.sample_fmt == SAMPLEFORMAT_IEEEFP = ", SAMPLEFORMAT_IEEEFP == sample_fmt )
    DE_DEBUG( "TIFF.res_unit = ",res_unit )
    DE_DEBUG( "TIFF.compression = ", compression )
    DE_DEBUG( "TIFF.photometric = ", photometric )
    DE_DEBUG( "TIFF.planar_config = ", planar_config )
    DE_DEBUG( "TIFF.isTiled = ", isTiled )
    DE_DEBUG( "TIFF.iccSize = ", iccSize )
    DE_DEBUG( "TIFF.iccBuf = ", iccBuf )

//   int dircount = 0;
//   do
//   {
//      dircount++;
//   } while (TIFFReadDirectory(tif));

//   DE_DEBUG( "TIFF has ", dircount, " directories in ", uri )


   // ---------------------------------------------------------------------------------
   // Tiled image loading
   // ---------------------------------------------------------------------------------

    uint32 tileWidth, tileHeight;

   // create a new DIB
   //dib = CreateImageType( header_only, image_type, width, height, bitspersample, samplesperpixel);
   //if (dib == NULL) {
   //   throw FI_MSG_ERROR_MEMORY;
   //}

   // ReadResolution(tif, dib);// fill in the resolution (english or universal)
   // ReadPalette(tif, photometric, bitspersample, dib); // set up the colormap based on photometric

    // get the tile geometry
    if( isTiled && (
        !TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tileWidth) ||
        !TIFFGetField(tif, TIFFTAG_TILELENGTH, &tileHeight) ) )
    {
        throw std::runtime_error("Invalid tiled TIFF image");
    }

   // ---------------------------------------------------------------------------------
   // Generic strip loading
   // ---------------------------------------------------------------------------------

   // create a new DIB
//   const uint16 chCount = MIN<uint16>(samplesperpixel, 4);
//   dib = CreateImageType(header_only, image_type, width, height, bitspersample, chCount);
//   if (dib == NULL) {
//      throw FI_MSG_ERROR_MEMORY;
//   }

    // calculate the line + pitch (separate for scr & dest)

    uint64_t scanlineBytes = uint64_t( TIFFScanlineSize(tif) ) * sizeof( uint8_t );
    uint64_t stripBytes = uint64_t( TIFFStripSize(tif) );
    uint32_t stripRows = 0;
    TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &stripRows);

    DE_DEBUG( "TIFF.scanlineBytes = ", scanlineBytes )
    DE_DEBUG( "TIFF.stripBytes = ", stripBytes )
    DE_DEBUG( "TIFF.stripRows = ", stripRows )

    // read the tiff lines and save them in the DIB
    uint8_t* scanline = (uint8_t*)::malloc( scanlineBytes );
    if ( !scanline )
    {
        throw std::runtime_error("EOM for TIFF");
    }

    if ( bits_per_sample == 1 )
    {
        if ( samples_per_pixel == 1 )
        {
            img.resize( w, h, PixelFormat::R8G8B8A8 );

            if ( planar_config == PLANARCONFIG_CONTIG )
            {
                DE_DEBUG("PLANAR_CONFIG Monochrome 1bit 1sample per pixel")

                for ( uint32 y = 0; y < h; ++y )
                {
                    ::memset( scanline, 0, scanlineBytes );

                    auto strip = TIFFComputeStrip(tif, y, 0);

                    if ( TIFFReadEncodedStrip( tif, strip, scanline, scanlineBytes ) == -1)
                    {
                        DE_ERROR("Got -1, y = ", y)
                        continue;
                    }

                    //DE_DEBUG("Strip ",strip, ", y = ", y)
                    //uint32_t strips = (y + stripRows > h ? h - y : stripRows);
                    uint8_t const* src = reinterpret_cast< uint8_t const* >( scanline );
                    uint32* dst = reinterpret_cast< uint32* >( img.getRow( int32_t( h ) - 1 - int32_t( y ) ) );
                    for ( uint32_t x = 0; x < w/8; ++x )
                    {
                        uint8_t z = *src++;

                        *dst++ = ( z & 0x01 ) ? 0xFFFFFFFF : 0x00000000;
                        *dst++ = ( z & 0x02 ) ? 0xFFFFFFFF : 0x00000000;
                        *dst++ = ( z & 0x04 ) ? 0xFFFFFFFF : 0x00000000;
                        *dst++ = ( z & 0x08 ) ? 0xFFFFFFFF : 0x00000000;
                        *dst++ = ( z & 0x10 ) ? 0xFFFFFFFF : 0x00000000;
                        *dst++ = ( z & 0x20 ) ? 0xFFFFFFFF : 0x00000000;
                        *dst++ = ( z & 0x40 ) ? 0xFFFFFFFF : 0x00000000;
                        *dst++ = ( z & 0x80 ) ? 0xFFFFFFFF : 0x00000000;
                    }
                }
            }
        }
    }
    else if ( bits_per_sample == 4 )
    {
        img.resize( w, h, PixelFormat::R8G8B8A8 );

        if ( planar_config == PLANARCONFIG_CONTIG )
        {
            DE_DEBUG("PLANAR_CONFIG = SINGLE_PLANE")

            for ( uint32 y = 0; y < h; ++y )
            {
                ::memset( scanline, 0, scanlineBytes );

                auto strip = TIFFComputeStrip(tif, y, 0);

                if ( TIFFReadEncodedStrip( tif, strip, scanline, scanlineBytes ) == -1)
                {
                    DE_ERROR("Got -1, y = ", y)
                    continue;
                }

                //DE_DEBUG("Strip ",strip, ", y = ", y)
                //uint32_t strips = (y + stripRows > h ? h - y : stripRows);
                uint8_t const* src = reinterpret_cast< uint8_t const* >( scanline );
                uint32* dst = reinterpret_cast< uint32* >( img.getRow( int32_t( h ) - 1 - int32_t( y ) ) );
                for ( uint32_t x = 0; x < w/2; ++x )
                {
                    uint8_t cc = *src++;
                    int r1 = int(float(cc & 0X0F) * float(255.0/15.0));
                    int r2 = int(float((cc >> 4) & 0X0F)* float(255.0/15.0));
                    *dst++ = dbRGBA(r1,r1,r1);
                    *dst++ = dbRGBA(r2,r2,r2);
                }
            }
        }
    }

    else if ( bits_per_sample == 8 )
    {
        if ( samples_per_pixel == 1 )
        {
            img.resize( w, h, PixelFormat::R8G8B8 );

            if ( planar_config == PLANARCONFIG_CONTIG )
            {
                DE_DEBUG("PLANAR_CONFIG 8 bit RGB")

                for ( uint32 y = 0; y < h; ++y )
                {
                    ::memset( scanline, 0, scanlineBytes );

                    auto strip = TIFFComputeStrip(tif, y, 0);

                    if ( TIFFReadEncodedStrip( tif, strip, scanline, scanlineBytes ) == -1)
                    {
                        DE_ERROR("Got -1, y = ", y)
                        continue;
                    }

                    uint8 const* src = reinterpret_cast< uint8 const* >( scanline );
                    uint32* dst = reinterpret_cast< uint32* >( img.getRow( int32_t( h ) - 1 - int32_t( y ) ) );
                    for ( uint32_t x = 0; x < w; ++x )
                    {
                        uint8 r = *src++;
                        *dst = dbRGBA( r,r,r );
                    }
                }
            }
        }
        else if ( samples_per_pixel == 3 )
        {
            img.resize( w, h, PixelFormat::R8G8B8 );

            if ( planar_config == PLANARCONFIG_CONTIG )
            {
                DE_DEBUG("PLANAR_CONFIG 24 bit RGB")

                for ( uint32 y = 0; y < h; ++y )
                {
                    ::memset( scanline, 0, scanlineBytes );

                    auto strip = TIFFComputeStrip(tif, y, 0);

                    if ( TIFFReadEncodedStrip( tif, strip, scanline, scanlineBytes ) == -1)
                    {
                        DE_ERROR("Got -1, y = ", y)
                        continue;
                    }

                    uint8 const* src = reinterpret_cast< uint8 const* >( scanline );
                    uint32* dst = reinterpret_cast< uint32* >( img.getRow( int32_t( h ) - 1 - int32_t( y ) ) );
                    for ( uint32_t x = 0; x < w; ++x )
                    {
                        uint8 r = *src++;
                        uint8 g = *src++;
                        uint8 b = *src++;
                        *dst = dbRGBA( r,g,b );
                    }
                }
            }
        }
        else if ( samples_per_pixel == 4 )
        {
            img.resize( w, h, PixelFormat::R8G8B8A8 );

            if ( planar_config == PLANARCONFIG_CONTIG )
            {
                DE_DEBUG("PLANAR_CONFIG 32 bit RGBA")

                for ( uint32 y = 0; y < h; ++y )
                {
                    ::memset( scanline, 0, scanlineBytes );

                    auto strip = TIFFComputeStrip(tif, y, 0);

                    if ( TIFFReadEncodedStrip( tif, strip, scanline, scanlineBytes ) == -1)
                    {
                        DE_ERROR("Got -1, y = ", y)
                        continue;
                    }

                    uint8 const* src = reinterpret_cast< uint8 const* >( scanline );
                    uint32* dst = reinterpret_cast< uint32* >( img.getRow( int32_t( h ) - 1 - int32_t( y ) ) );
                    for ( uint32_t x = 0; x < w; ++x )
                    {
                        uint8 r = *src++;
                        uint8 g = *src++;
                        uint8 b = *src++;
                        uint8 a = *src++;
                        *dst = dbRGBA( r,g,b,a );
                    }
                }
            }
        }
    }
    else if ( bits_per_sample == 32 )
    {
        img.resize( w, h, PixelFormat::R32F );

        if ( planar_config == PLANARCONFIG_CONTIG )
        {
            DE_DEBUG("PLANAR_CONFIG = SINGLE_PLANE")

            for ( uint32 y = 0; y < h; ++y )
            {
                ::memset( scanline, 0, scanlineBytes );

                auto strip = TIFFComputeStrip(tif, y, 0);

                if ( TIFFReadEncodedStrip( tif, strip, scanline, scanlineBytes ) == -1)
                {
                    DE_ERROR("Got -1, y = ", y)
                    continue;
                }

                //DE_DEBUG("Strip ",strip, ", y = ", y)
                //uint32_t strips = (y + stripRows > h ? h - y : stripRows);
                float const* src = reinterpret_cast< float const* >( scanline );
                float* dst = reinterpret_cast< float* >( img.getRow( int32_t( h ) - 1 - int32_t( y ) ) );
                for ( uint32_t x = 0; x < w; ++x )
                {
                    *dst = *src;
                    src++;
                    dst++;
                }
            }
        }
    }


   ::free( scanline );

   return true;
}

#endif // DE_IMAGE_READER_TIF_ENABLED



#if defined(DE_IMAGE_WRITER_TIF_ENABLED)

bool
ImageWriterTIF::save( Image const & img, std::string const & uri, uint32_t quality )
{
    return false;
}

#endif // DE_IMAGE_WRITER_TIF_ENABLED

} // end namespace image.
} // end namespace de.


/*
   else if(planar_config == PLANARCONFIG_SEPARATE) {

      const unsigned Bpc = bitspersample / 8;
      BYTE* dib_strip = bits;
      // - loop for strip blocks -

      for (uint32 y = 0; y < height; y += rowsperstrip) {
         const int32 strips = (y + rowsperstrip > height ? height - y : rowsperstrip);

         // - loop for channels (planes) -

         for(uint16 sample = 0; sample < samplesperpixel; sample++) {

            if (TIFFReadEncodedStrip(tif, TIFFComputeStrip(tif, y, sample), buf, strips * src_line) == -1) {
               // ignore errors as they can be frequent and not really valid errors, especially with fax images
               bThrowMessage = TRUE;
            }

            if(sample >= chCount) {
               // TODO Write to Extra Channel
               break;
            }

            const unsigned channelOffset = sample * Bpc;

            // - loop for strips in block -

            BYTE* src_line_begin = buf;
            BYTE* dst_line_begin = dib_strip;
            for (int l = 0; l < strips; l++, src_line_begin += src_line, dst_line_begin -= dst_pitch ) {

               // - loop for pixels in strip -

               const BYTE* const src_line_end = src_line_begin + src_line;

               for (BYTE* src_bits = src_line_begin, * dst_bits = dst_line_begin; src_bits < src_line_end; src_bits += Bpc, dst_bits += Bpp) {
                  // actually assigns channel
                  AssignPixel(dst_bits + channelOffset, src_bits, Bpc);
               } // line

            } // strips

         } // channels

         // done with a strip block, incr to the next
         dib_strip -= strips * dst_pitch;

      } // height

   }
   free(buf);

//   if(bThrowMessage) {
//      FreeImage_OutputMessageProc(s_format_id, "Warning: parsing error. Image may be incomplete or contain invalid data !");
//   }

//#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
//   SwapRedBlue32(dib);
//#endif

















   // read the tiff lines and save them in the DIB
   bool header_only = false;
   if( planar_config == PLANARCONFIG_CONTIG && !header_only)
   {
      // get the maximum number of bytes required to contain a tile
      tmsize_t tileSize = TIFFTileSize(tif);

      // allocate tile buffer
      uint8_t* tileBuffer = (uint8_t*)malloc( tileSize * sizeof(uint8_t) );
      if( !tileBuffer )
      {
         throw std::runtime_error("No TIFF tileBuffer");
      }

      // calculate src line and dst pitch
      int dst_pitch = img.getStride();
      uint32 tileRowSize = (uint32)TIFFTileRowSize(tif);
      uint32 imageRowSize = (uint32)TIFFScanlineSize(tif);

      // In the tiff file the lines are saved from up to down
      // In a DIB the lines must be saved from down to up

      uint8_t* bits = img.getRow( h - 1 );

      for ( uint32 y = 0; y < h; y += tileHeight )
      {
         int32 nrows = (y + tileHeight > h ? h - y : tileHeight);

         for ( uint32 x = 0, rowSize = 0; x < w; x += tileWidth, rowSize += tileRowSize )
         {
            memset(tileBuffer, 0, tileSize);

            // read one tile
            if (TIFFReadTile(tif, tileBuffer, x, y, 0, 0) < 0)
            {
               free( tileBuffer );
               throw "Corrupted tiled TIFF file";
            }
            // convert to strip
            if( x + tileWidth > w )
            {
               src_line = imageRowSize - rowSize;
            }
            else
            {
               src_line = tileRowSize;
            }
            uint8_t* src_bits = tileBuffer;
            uint8_t* dst_bits = bits + rowSize;
            for ( int k = 0; k < nrows; ++k )
            {
               ::memcpy(dst_bits, src_bits, src_line);
               src_bits += tileRowSize;
               dst_bits -= dst_pitch;
            }
         }

         bits -= nrows * dst_pitch;
      }

//#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
//      SwapRedBlue32(dib);
//#endif
      free(tileBuffer);
   }
   else if(planar_config == PLANARCONFIG_SEPARATE) {
      throw "Separated tiled TIFF images are not supported";
   }
*/
