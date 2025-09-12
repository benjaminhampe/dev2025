#include <de/image/Image_BMP.h>

namespace de {
namespace image {

#if defined(DE_IMAGE_READER_BMP_ENABLED) || defined(DE_IMAGE_WRITER_BMP_ENABLED)

namespace bmp {
	
#pragma pack( push )
#pragma pack( 1 )

// ============================================================================
struct BMPHeader
// ============================================================================
{
   uint16_t	Id;					// [ 0...1] 2Bytes
                              //          BM - Windows 3.1x, 95, NT, 98, 2000, ME, XP
                              //          BA - OS/2 Bitmap Array
                              //          CI - OS/2 Color Icon
                              //          CP - OS/2 Color Pointer
                              //          IC - OS/2 Icon
                              //          PT - OS/2 Pointer
   uint32_t	FileSize;         // [ 2...5] 4Bytes
   uint32_t	Reserved;         // [ 6...9] 4Bytes
   uint32_t	BitmapDataOffset; // [10..13] 4Bytes
   uint32_t	BitmapHeaderSize;	// [14..17] 4Bytes // should be 28h for windows bitmaps or
                                        // 0Ch for OS/2 1.x or F0h for OS/2 2.x
   uint32_t Width;            // [18..21] 4Bytes
   uint32_t Height;           // [22..25] 4Bytes
   uint16_t Planes;           // [26..27] 2Bytes
   uint16_t BPP;					// [28..29] 2Bytes
                              //          1: Monochrome bitmap
                              //          4: 16 color bitmap
                              //          8: 256 color bitmap
                              //          16: 16bit (high color) bitmap
                              //          24: 24bit (true color) bitmap
                              //          32: 32bit (true color) bitmap

   uint32_t  Compression;		// [30..33] 4Bytes
                              //          0: none (Also identified by BI_RGB)
                              //          1: RLE 8-bit / pixel (Also identified by BI_RLE4)
                              //          2: RLE 4-bit / pixel (Also identified by BI_RLE8)
                              //          3: Bitfields  (Also identified by BI_BITFIELDS)

   uint32_t  BitmapDataSize;	// [34..37] 4Bytes // Size of the bitmap data in bytes. This number must be rounded to the next 4 byte boundary.
   uint32_t  PixelPerMeterX;  // [38..41] 4Bytes
   uint32_t  PixelPerMeterY;  // [42..45] 4Bytes
   uint32_t  Colors;          // [46..49] 4Bytes
   uint32_t  ImportantColors; // [50..53] 4Bytes
};

// ============================================================================
typedef union PixelInfo
// ============================================================================
{
    uint32_t ARGB;
    struct
    {
      uint8_t B, G, R, A;
    };
} *PPixelInfo;

#pragma pack( pop )


//! converts a 8 bit palettized or non palettized image (A8) into R8G8B8
void convert8BitTo24Bit( uint8_t const * in,
                         uint8_t* out,
                         int width,
                         int height,
                         std::vector< uint32_t > const & palette, // uint8_t const* palette,
                         int linepad,
                         bool flip )
{
   if (!in || !out ) return;

   int const lineWidth = 3 * width;
   if (flip)
   {
      out += lineWidth * height;
   }

   for (int y=0; y<height; ++y)
   {
      if (flip)
      {
         out -= lineWidth; // one line back
      }

      for (int x=0; x< lineWidth; x += 3)
      {
         if ( palette.size() )
         {
#ifdef __BIG_ENDIAN__
            out[x+0] = palette[ (in[0] << 2 ) + 0];
            out[x+1] = palette[ (in[0] << 2 ) + 1];
            out[x+2] = palette[ (in[0] << 2 ) + 2];
#else
            uint32_t color = palette[ in[0] ];
            out[x+0] = dbRGBA_B( color ); // palette[ (in[0] << 2 ) + 2];   // b -> r
            out[x+1] = dbRGBA_G( color ); // palette[ (in[0] << 2 ) + 1];   // g -> g
            out[x+2] = dbRGBA_R( color ); // palette[ (in[0] << 2 ) + 0];   // r -> b
#endif
         }
         else
         {
            out[x+0] = in[0];
            out[x+1] = in[0];
            out[x+2] = in[0];
         }
         ++in;
      }

      if (!flip)
      {
         out += lineWidth;
      }
      in += linepad;
   }
}

//typedef struct tagBITMAPINFOHEADER{
//  DWORD biSize;
//  LONG  biWidth;
//  LONG  biHeight;
//  WORD  biPlanes;
//  WORD  biBitCount;
//  DWORD biCompression;
//  DWORD biSizeImage;
//  LONG  biXPelsPerMeter;
//  LONG  biYPelsPerMeter;
//  DWORD biClrUsed;
//  DWORD biClrImportant;
//} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

// keep some general information about the bitmap

//unsigned used_colors	= bih.biClrUsed;
//int width				= bih.biWidth;
//int height				= bih.biHeight;		// WARNING: height can be < 0 => check each call using 'height' as a parameter
//unsigned bit_count		= bih.biBitCount;
//unsigned compression	= bih.biCompression;
//unsigned pitch			= CalculatePitch(CalculateLine(width, bit_count));


} // end namespace bmp

#endif

#if defined(DE_IMAGE_READER_BMP_ENABLED)

// ============================================================================
bool ImageReaderBMP::load( Image & img, const uint8_t* p, size_t n, const std::string& uri )
// ============================================================================
{
    Binary file(p,n,uri);
    if ( !file.is_open() )
    {
        DE_ERROR("Cant open file(", uri, ") for binary read.")
        return false;
    }

    size_t fileBytes = file.size();

    bmp::BMPHeader header;
    file.seek( 0 );
    file.read( &header, sizeof( header ) );

    uint32_t w = header.Width; //header[18] + (header[19] << 8);
    uint32_t h = header.Height; //header[22] + (header[23] << 8);
    uint32_t bpp = header.BPP; //header[28];
    size_t dataOffset = header.BitmapDataOffset; // size_t( header[10] ) + (uint32_t( header[11] ) << 8);
    size_t dataBytes = size_t( 4 ) * h * ((w * bpp + 31) / 32);

    DE_DEBUG("BMP.Uri = ", uri )
    DE_DEBUG("BMP.Bytes = ", fileBytes )
    DE_DEBUG("BMP.Width = ", w )
    DE_DEBUG("BMP.Height = ", h )
    DE_DEBUG("BMP.BitsPerPixel = ", bpp )
    DE_DEBUG("BMP.Compression = ",header.Compression)
    DE_DEBUG("BMP.Colors = ",header.Colors)
    DE_DEBUG("BMP.DataOffset = ", dataOffset )
    DE_DEBUG("BMP.DataBytes = ", dataBytes )

    if ( header.Id != 0x4d42 )
    {
        DE_ERROR( "Invalid bmp header id ", header.Id )
        return false;
    }

    //size_t pos = file.tell();

    /*
    switch (bpp)
    {
    case 1 :
    case 4 :
    case 8 :
    {
    if ((used_colors == 0) || (used_colors > CalculateUsedPaletteEntries(bit_count))) {
    used_colors = CalculateUsedPaletteEntries(bit_count);
    }
    // allocate enough memory to hold the bitmap (header, palette, pixels) and read the palette
    // set resolution information
    FreeImage_SetDotsPerMeterX(dib, bih.biXPelsPerMeter);
    FreeImage_SetDotsPerMeterY(dib, bih.biYPelsPerMeter);
    // seek to the end of the header (depending on the BMP header version)
    // type == sizeof(BITMAPVxINFOHEADER)
    switch(type) {
    case 40:	// sizeof(BITMAPINFOHEADER) - all Windows versions since Windows 3.0
    break;
    case 52:	// sizeof(BITMAPV2INFOHEADER) (undocumented)
    case 56:	// sizeof(BITMAPV3INFOHEADER) (undocumented)
    case 108:	// sizeof(BITMAPV4HEADER) - all Windows versions since Windows 95/NT4 (not supported)
    case 124:	// sizeof(BITMAPV5HEADER) - Windows 98/2000 and newer (not supported)
    io->seek_proc(handle, (long)(type - sizeof(BITMAPINFOHEADER)), SEEK_CUR);
    break;
    }

    // load the palette
    io->read_proc(FreeImage_GetPalette(dib), used_colors * sizeof(RGBQUAD), 1, handle);
    #if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_RGB
    RGBQUAD *pal = FreeImage_GetPalette(dib);
    for(int i = 0; i < used_colors; i++) {
    INPLACESWAP(pal[i].rgbRed, pal[i].rgbBlue);
    }
    #endif
    if(header_only) {
    // header only mode
    return dib;
    }

    // seek to the actual pixel data.
    // this is needed because sometimes the palette is larger than the entries it contains predicts
    io->seek_proc(handle, bitmap_bits_offset, SEEK_SET);

    // read the pixel data

    switch (compression) {
    case BI_RGB :
    if( LoadPixelData(io, handle, dib, height, pitch, bit_count) ) {
    return dib;
    } else {
    throw "Error encountered while decoding BMP data";
    }
    break;

    case BI_RLE4 :
    if( LoadPixelDataRLE4(io, handle, width, height, dib) ) {
    return dib;
    } else {
    throw "Error encountered while decoding RLE4 BMP data";
    }
    break;

    case BI_RLE8 :
    if( LoadPixelDataRLE8(io, handle, width, height, dib) ) {
    return dib;
    } else {
    throw "Error encountered while decoding RLE8 BMP data";
    }
    break;

    default :
    throw FI_MSG_ERROR_UNSUPPORTED_COMPRESSION;
    }
    }
    break; // 1-, 4-, 8-bit

    case 16 :
    {
    int use_bitfields = 0;
    if (bih.biCompression == BI_BITFIELDS) use_bitfields = 3;
    else if (bih.biCompression == BI_ALPHABITFIELDS) use_bitfields = 4;
    else if (type == 52) use_bitfields = 3;
    else if (type >= 56) use_bitfields = 4;

    if (use_bitfields > 0) {
    DWORD bitfields[4];
    io->read_proc(bitfields, use_bitfields * sizeof(DWORD), 1, handle);
    dib = FreeImage_AllocateHeader(header_only, width, height, bit_count, bitfields[0], bitfields[1], bitfields[2]);
    } else {
    dib = FreeImage_AllocateHeader(header_only, width, height, bit_count, FI16_555_RED_MASK, FI16_555_GREEN_MASK, FI16_555_BLUE_MASK);
    }

    if (dib == NULL) {
    throw FI_MSG_ERROR_DIB_MEMORY;
    }

    // set resolution information
    FreeImage_SetDotsPerMeterX(dib, bih.biXPelsPerMeter);
    FreeImage_SetDotsPerMeterY(dib, bih.biYPelsPerMeter);

    if(header_only) {
    // header only mode
    return dib;
    }

    // seek to the actual pixel data
    io->seek_proc(handle, bitmap_bits_offset, SEEK_SET);

    // load pixel data and swap as needed if OS is Big Endian
    LoadPixelData(io, handle, dib, height, pitch, bit_count);

    return dib;
    }
    break; // 16-bit

    case 24 :
    case 32 :
    {
    int use_bitfields = 0;
    if (bih.biCompression == BI_BITFIELDS) use_bitfields = 3;
    else if (bih.biCompression == BI_ALPHABITFIELDS) use_bitfields = 4;
    else if (type == 52) use_bitfields = 3;
    else if (type >= 56) use_bitfields = 4;

    if (use_bitfields > 0) {
    DWORD bitfields[4];
    io->read_proc(bitfields, use_bitfields * sizeof(DWORD), 1, handle);
    dib = FreeImage_AllocateHeader(header_only, width, height, bit_count, bitfields[0], bitfields[1], bitfields[2]);
    } else {
    if( bit_count == 32 ) {
    dib = FreeImage_AllocateHeader(header_only, width, height, bit_count, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
    } else {
    dib = FreeImage_AllocateHeader(header_only, width, height, bit_count, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
    }
    }

    if (dib == NULL) {
    throw FI_MSG_ERROR_DIB_MEMORY;
    }

    // set resolution information
    FreeImage_SetDotsPerMeterX(dib, bih.biXPelsPerMeter);
    FreeImage_SetDotsPerMeterY(dib, bih.biYPelsPerMeter);

    if(header_only) {
    // header only mode
    return dib;
    }

    // Skip over the optional palette
    // A 24 or 32 bit DIB may contain a palette for faster color reduction
    // i.e. you can have (FreeImage_GetColorsUsed(dib) > 0)

    // seek to the actual pixel data
    io->seek_proc(handle, bitmap_bits_offset, SEEK_SET);

    // read in the bitmap bits
    // load pixel data and swap as needed if OS is Big Endian
    LoadPixelData(io, handle, dib, height, pitch, bit_count);

    // check if the bitmap contains transparency, if so enable it in the header

    FreeImage_SetTransparent(dib, (FreeImage_GetColorType(dib) == FIC_RGBALPHA));

    return dib;
    }
    break; // 24-, 32-bit
    }
    */

    if ( header.Compression > 2 ) // we'll only handle RLE-Compression
    {
        DE_ERROR( "Invalid bmp Unsupported compression mode ", header.Compression )
        return false;
    }

    //   if( header[0] != 'B' && header[1] != 'M' )
    //   {
    //      file.close();
    //      DE_DEBUG( "header[0] != 'B' && header[1] != 'M'" )
    //      // throw std::invalid_argument("Error: Invalid File Format. Bitmap Required.");
    //      return false;
    //   }

    PixelFormatConverter::Converter_t converter = nullptr;
    if ( bpp == 24 )
    {
        std::vector< uint8_t > tmpBuffer( dataBytes );
        file.seek( dataOffset );
        file.read( tmpBuffer.data(), dataBytes );
        file.close();
        auto fmt = PixelFormat::R8G8B8;
        converter = PixelFormatConverter::getConverter( PixelFormat::B8G8R8, fmt );
        img.resize( w, h, fmt );
        converter( tmpBuffer.data(), img.data(), size_t( w ) * h );
        img.flipVertical();
        return true;
    }
    else if ( bpp == 32 )
    {
        std::vector< uint8_t > tmpBuffer( dataBytes );
        file.seek( dataOffset );
        file.read( reinterpret_cast< char* >( tmpBuffer.data() ), dataBytes );
        file.close();
        auto fmt = PixelFormat::R8G8B8A8;
        converter = PixelFormatConverter::getConverter( PixelFormat::B8G8R8A8, fmt );
        img.resize( w, h, fmt );
        converter( tmpBuffer.data(), img.data(), size_t( w ) * h );
        img.flipVertical();
        return true;
    }
    else if ( bpp == 8 && header.Compression == 0 )
    {
        // Read palette, each pixel is an 8bit index into a RGBA color-table
        // ( highly efficient method we need to support ).
        auto pos = file.tell();
        uint32_t paletteSize = ( header.BitmapDataOffset - pos) / 4;
        DE_DEBUG("Read BMP Palette color table size = ", paletteSize)
        if ( paletteSize > 256 )
        {
            DE_DEBUG("Too big 8bit table = ", paletteSize)
            paletteSize = 256;
        }
        std::vector< uint32_t > palette( paletteSize );
        if (palette.size())
        {
            file.read( palette.data(), paletteSize * sizeof(uint32_t) );
            #ifdef __BIG_ENDIAN__
            for (int32_t i=0; i<paletteSize; ++i) palette[i] = byteswap(palette[i]);
            #endif
        }
        std::vector< uint8_t > tmpBuffer( dataBytes, 0x00 );
        file.seek( dataOffset );
        file.read( tmpBuffer.data(), dataBytes );
        file.close();
        img.resize( w, h, PixelFormat::R8G8B8 );
        bmp::convert8BitTo24Bit( tmpBuffer.data(), img.data(), w, h, palette, 0, false );
        img.flipVertical();
        return true;
    }
    else
    {
        file.close();
        DE_DEBUG( "Invalid file format. 8, 24 or 32 bit image are required" )
        return false;
    }
}

#endif // DE_IMAGE_READER_BMP_ENABLED

#if defined(DE_IMAGE_WRITER_BMP_ENABLED)

// ============================================================================
bool ImageWriterBMP::save( Image const & img, std::string const & uri, uint32_t quality )
// ============================================================================
{
    if ( img.empty() )
    {
        DE_ERROR("Image is empty.")
        return false;
    }

    PixelFormatConverter::Converter_t converter =
    PixelFormatConverter::getConverter( img.pixelFormat(), PixelFormat::B8G8R8 );

    if ( !converter )
    {
        DE_ERROR("No supported color converter from ",img.pixelFormatStr()," to BGR888")
        return false;
    }

    File file( uri.c_str(), "wb" );
    if ( !file.is_open() )
    {
        DE_ERROR("Cant open for write (",uri,")")
        return false;
    }

    image::bmp::BMPHeader header;
    size_t const headerBytes = sizeof( header ); // used more than once
    header.Id = 0x4d42;
    header.Reserved = 0;
    header.BitmapDataOffset = headerBytes;
    header.BitmapHeaderSize = 0x28;
    header.Width = uint32_t( img.w() );
    header.Height = uint32_t( img.h() );
    header.Planes = 1;
    header.BPP = 24;
    header.Compression = 0;
    header.PixelPerMeterX = 0;
    header.PixelPerMeterY = 0;
    header.Colors = 0;
    header.ImportantColors = 0;
    // data size is rounded up to next larger 4 bytes boundary
    header.BitmapDataSize = header.Width * header.BPP / 8;
    header.BitmapDataSize = (header.BitmapDataSize + 3) & ~3;
    header.BitmapDataSize *= header.Height;
    // file size is data size plus offset to data
    header.FileSize = header.BitmapDataOffset + header.BitmapDataSize;
    // bitmaps are stored upside down and padded so we always do this

    // write the bitmap header
    size_t resultBytes = file.write( &header, headerBytes );
    if ( resultBytes != headerBytes )
    {
        DE_ERROR("Cant write headerBytes(",headerBytes,")")
        return false;
    }

    uint8_t const * pixels = img.data();

    // allocate and clear memory for our scan line
    std::vector< uint8_t > scanline;
    size_t const scanlineBytes = img.stride();
    scanline.resize( scanlineBytes, 0 );

    // length of one row of the source image in bytes
    // uint32_t row_stride = (img.getBytesPerPixel() * header.Width);

    //uint8_t* row = new uint8_t[ row_size ];
    //::memset( row, 0, row_size );

    // length of one row in bytes, rounded up to nearest 4-byte boundary
    // BACKUP: int32_t row_size = ((3 * header.Width) + 3) & ~3;
    int32_t row_size = ((3 * header.Width) + 3) & ~3;

    // convert the image to 24-bit BGR and flip it over
    int32_t y;
    for ( y = int32_t(header.Height) - 1; 0 <= y; --y )
    {
        if ( img.pixelFormat() == PixelFormat::R8G8B8 )
        {
            PixelFormatConverter::convert24Bit_to_24Bit( &pixels[ y * scanlineBytes ],
                scanline.data(), int32_t(header.Width), 1, 0, false, true );
        }
        else
        {
            // source, length [pixels], destination
            converter( &pixels[ y * scanlineBytes ], scanline.data(), header.Width );
        }

        resultBytes = file.write( scanline.data(), row_size );
        if ( resultBytes < size_t( row_size ) )
        {
            //std::cout << "[Warn] JPG." << __FUNCTION__ << " :: Exited loop.\n";
            break;
        }
    }

    //bool ok = y < 0;
    return true;
}

#endif // DE_IMAGE_WRITER_BMP_ENABLED


} // end namespace image.
} // end namespace de.


#if 0
namespace detail {

struct BMP
{
   // ----------------------------------------------------------------------------
   static void
   decompress8BitRLE( Image & img, int32_t size, int32_t width, int32_t height, int32_t pitch )
   {
      size_t const byteCount = size_t( width + pitch ) * size_t( height );

      std::vector< uint8_t > newBmp;
      newBmp.resize( byteCount, 0 );

      uint8_t* bmpData = img.data();
      uint8_t* p = bmpData;
      uint8_t* d = newBmp.data();
      uint8_t* destEnd = newBmp.data() + byteCount;
      int32_t line = 0;

      while ( bmpData - p < size && d < destEnd )
      {
         if (*p == 0)
         {
            ++p;

            switch(*p)
            {
            case 0: // end of line
               ++p;
               ++line;
               d = newBmp.data() + (line*(width+pitch));
               break;
            case 1: // end of bmp
               //bmpData = newBmp.data();
               img.m_Data = newBmp;
               img.m_Width = width;
               img.m_Pitch = width + pitch;
               img.m_Height = height;
               return;
            case 2:
               ++p; d +=(uint8_t)*p;  // delta
               ++p; d += ((uint8_t)*p)*(width+pitch);
               ++p;
               break;
            default:
               {
                  // absolute mode
                  int32_t count = (uint8_t)*p; ++p;
                  int32_t readAdditional = ((2-(count%2))%2);
                  int32_t i;

                  for (i=0; i<count; ++i)
                  {
                     *d = *p;
                     ++p;
                     ++d;
                  }

                  for (i=0; i<readAdditional; ++i)
                     ++p;
               }
            }
         }
         else
         {
            int32_t count = (uint8_t)*p; ++p;
            uint8_t color = *p; ++p;
            for (int32_t i=0; i<count; ++i)
            {
               *d = color;
               ++d;
            }
         }
      }

      img.m_Data = newBmp;
      img.m_Width = width;
      img.m_Pitch = width + pitch;
      img.m_Height = height;
   }

   // ----------------------------------------------------------------------------
   static void
   decompress4BitRLE( Image & img, int32_t size, int32_t width, int32_t height, int32_t pitch)
   {
      uint8_t* bmpData = img.data();
      uint8_t* p = bmpData;

      int32_t lineWidth = ( width + 1 ) / 2 + pitch;

      size_t const byteCount = size_t( lineWidth ) * size_t( height );
      std::vector< uint8_t > newBmp;
      newBmp.resize( byteCount, 0 );

      uint8_t* d = newBmp.data();
      uint8_t* destEnd = newBmp.data() + byteCount;

      int32_t line = 0;
      int32_t shift = 4;

      while (bmpData - p < size && d < destEnd)
      {
         if (*p == 0)
         {
            ++p;

            switch(*p)
            {
            case 0: // end of line
               ++p;
               ++line;
               d = newBmp.data() + (line*lineWidth);
               shift = 4;
               break;
            case 1: // end of bmp
               // bmpData = newBmp;
               img.m_Data = newBmp;
               img.m_Width = width;
               img.m_Pitch = width + pitch;
               img.m_Height = height;
               return;
            case 2:
               {
                  ++p;
                  int32_t x = (uint8_t)*p; ++p;
                  int32_t y = (uint8_t)*p; ++p;
                  d += x/2 + y*lineWidth;
                  shift = x%2==0 ? 4 : 0;
               }
               break;
            default:
               {
                  // absolute mode
                  int32_t count = (uint8_t)*p; ++p;
                  int32_t readAdditional = ((2-((count)%2))%2);
                  int32_t readShift = 4;
                  int32_t i;

                  for (i=0; i<count; ++i)
                  {
                     int32_t color = (((uint8_t)*p) >> readShift) & 0x0f;
                     readShift -= 4;
                     if (readShift < 0)
                     {
                        ++*p;
                        readShift = 4;
                     }

                     uint8_t mask = 0x0f << shift;
                     *d = (*d & (~mask)) | ((color << shift) & mask);

                     shift -= 4;
                     if (shift < 0)
                     {
                        shift = 4;
                        ++d;
                     }

                  }

                  for (i=0; i<readAdditional; ++i)
                     ++p;
               }
            }
         }
         else
         {
            int32_t count = (uint8_t)*p; ++p;
            int32_t color1 = (uint8_t)*p; color1 = color1 & 0x0f;
            int32_t color2 = (uint8_t)*p; color2 = (color2 >> 4) & 0x0f;
            ++p;

            for (int32_t i=0; i<count; ++i)
            {
               uint8_t mask = 0x0f << shift;
               uint8_t toSet = (shift==0 ? color1 : color2) << shift;
               *d = (*d & (~mask)) | (toSet & mask);

               shift -= 4;
               if (shift < 0)
               {
                  shift = 4;
                  ++d;
               }
            }
         }
      }

      // bmpData = newBmp;
      img.m_Data = newBmp;
      img.m_Width = width;
      img.m_Pitch = width + pitch;
      img.m_Height = height;
   }

};

struct BMPFile
{


// ----------------------------------------------------------------------------
Image*
ImageReaderBMP::load( std::string const & fileName )
{
   PerformanceTimer timer;
   timer.start();

   Image* img = nullptr;

   ColorConverter::Converter_t converter = nullptr;

   FILE * file = ::fopen( fileName.c_str(), "rb" );
   if ( !file )
   {
      std::cout << "BMP." << __FUNCTION__ << " :: [Error] Cant open file for binary reading.\n";
      return nullptr;
   }

   ::fseeko64( file, 0, SEEK_END );
   int64_t fileBytes = ::ftello64( file );
   ::fseeko64( file, 0, SEEK_SET );

   bmp::BMPHeader header;
   ::fread( &header, 1, sizeof( header ), file );
   int64_t pos = ::ftello64( file );

   #ifdef __BIG_ENDIAN__
   header.Id = os::Byteswap::byteswap(header.Id);
   header.FileSize = os::Byteswap::byteswap(header.FileSize);
   header.BitmapDataOffset = os::Byteswap::byteswap(header.BitmapDataOffset);
   header.BitmapHeaderSize = os::Byteswap::byteswap(header.BitmapHeaderSize);
   header.Width = os::Byteswap::byteswap(header.Width);
   header.Height = os::Byteswap::byteswap(header.Height);
   header.Planes = os::Byteswap::byteswap(header.Planes);
   header.BPP = os::Byteswap::byteswap(header.BPP);
   header.Compression = os::Byteswap::byteswap(header.Compression);
   header.BitmapDataSize = os::Byteswap::byteswap(header.BitmapDataSize);
   header.PixelPerMeterX = os::Byteswap::byteswap(header.PixelPerMeterX);
   header.PixelPerMeterY = os::Byteswap::byteswap(header.PixelPerMeterY);
   header.Colors = os::Byteswap::byteswap(header.Colors);
   header.ImportantColors = os::Byteswap::byteswap(header.ImportantColors);
   #endif

   if ( header.Id != 0x4d42 )
   {
      std::cout << "BMP." << __FUNCTION__ << " :: [Error] Invalid header id.\n";
      return nullptr; // return if the header is false
   }

   if ( header.Compression > 2 ) // we'll only handle RLE-Compression
   {
      std::cout << "BMP." << __FUNCTION__ << " :: [Error] Unsupported compression mode.\n";
      return nullptr;
   }

   int32_t pitch = 0;

   // adjust bitmap data size to dword boundary
   header.BitmapDataSize += ( 4 - ( header.BitmapDataSize % 4 ) ) % 4;

   // read palette
   std::vector< int32_t > paletteData;
   int32_t paletteSize = ( header.BitmapDataOffset - pos ) / 4;
   if ( paletteSize > 0 )
   {
      paletteData.resize( paletteSize, 0 );
      ::fread( paletteData, 1, size_t( paletteSize ) * sizeof( int32_t ), file );
   #ifdef __BIG_ENDIAN__
      for (int32_t i=0; i<paletteSize; ++i)
         paletteData[i] = os::Byteswap::byteswap(paletteData[i]);
   #endif
   }

   // read image data
   if ( !header.BitmapDataSize )
   {
      // okay, lets guess the size
      // some tools simply don't set it
      header.BitmapDataSize = static_cast< uint32_t >( fileBytes - header.BitmapDataOffset );
   }

   ::fseek( file, header.BitmapDataOffset, SEEK_SET );

   float32_t t = (header.Width) * (header.BPP / 8.0f);
   int32_t widthInBytes = (int32_t)t;
   t -= widthInBytes;
   if ( t != 0.0f ) ++widthInBytes;
   int32_t lineData = widthInBytes + ((4-(widthInBytes%4)))%4;
   pitch = lineData - widthInBytes;

   uint8_t* bmpData = new uint8_t[header.BitmapDataSize];

   // ::fread( bmpData, 1, header.BitmapDataSize, file );
   file.read( bmpData, header.BitmapDataSize );

   // decompress data if needed
   switch( header.Compression )
   {
   case 1: // 8 bit rle
      bmp::BMP::decompress8BitRLE( bmpData, header.BitmapDataSize, header.Width, header.Height, pitch );
      break;
   case 2: // 4 bit rle
      bmp::BMP::decompress4BitRLE( bmpData, header.BitmapDataSize, header.Width, header.Height, pitch );
      break;
   }

   // create surface

   // no default constructor from packed area! ARM problem!
   int32_t w = header.Width;
   int32_t h = header.Height;

   Image* img = nullptr;
   if ( header.BPP == 24 )
   {
      img = Image::createImage( w, h, ColorFormat( 8,8,8 ) );
      ColorConverter::convert24BitTo24Bit( bmpData, img->data(), header.Width, header.Height, pitch, true, true);
   }
   else if ( header.BPP == 32 )
   {
      img = Image::createImage( w, h, ColorFormat( 8,8,8,8 ) );
      converter = ColorConverter::convert32BitTo32Bit;
   }
   else {
      std::cout << "BMP." << __FUNCTION__ << " :: [Error] Unsupported bpp(" << header.BPP << ").\n";
   }

   // clean up
   delete [] bmpData;


   ::fclose( file );
   return img;
}


//! returns true if the file maybe is able to be loaded by this class
//! based on the file extension (e.g. ".tga")
bool CImageLoaderBMP::isALoadableFileExtension(const io::path& filename) const
{
    return core::hasFileExtension ( filename, "bmp" );
}


//! returns true if the file maybe is able to be loaded by this class
bool CImageLoaderBMP::isALoadableFileFormat(io::IReadFile* file) const
{
   uint16_t headerID;
   file->read(&headerID, sizeof(uint16_t));
   #ifdef __BIG_ENDIAN__
    headerID = os::Byteswap::byteswap(headerID);
   #endif
    return headerID == 0x4d42;
}


void CImageLoaderBMP::decompress8BitRLE(uint8_t*& bmpData, int32_t size, int32_t width, int32_t height, int32_t pitch) const
{
   uint8_t* p = bmpData;
   uint8_t* newBmp = new uint8_t[(width+pitch)*height];
   uint8_t* d = newBmp;
   uint8_t* destEnd = newBmp + (width+pitch)*height;
   int32_t line = 0;

    while (bmpData - p < size && d < destEnd)
    {
        if (*p == 0)
        {
            ++p;

            switch(*p)
            {
            case 0: // end of line
                ++p;
                ++line;
                d = newBmp + (line*(width+pitch));
                break;
            case 1: // end of bmp
                delete [] bmpData;
                bmpData = newBmp;
                return;
            case 2:
            ++p; d +=(uint8_t)*p;  // delta
            ++p; d += ((uint8_t)*p)*(width+pitch);
                ++p;
                break;
            default:
                {
                    // absolute mode
               int32_t count = (uint8_t)*p; ++p;
               int32_t readAdditional = ((2-(count%2))%2);
               int32_t i;

                    for (i=0; i<count; ++i)
                    {
                        *d = *p;
                        ++p;
                        ++d;
                    }

                    for (i=0; i<readAdditional; ++i)
                        ++p;
                }
            }
        }
        else
        {
         int32_t count = (uint8_t)*p; ++p;
         uint8_t color = *p; ++p;
         for (int32_t i=0; i<count; ++i)
            {
                *d = color;
                ++d;
            }
        }
    }

    delete [] bmpData;
    bmpData = newBmp;
}


void CImageLoaderBMP::decompress4BitRLE(uint8_t*& bmpData, int32_t size, int32_t width, int32_t height, int32_t pitch) const
{
   int32_t lineWidth = (width+1)/2+pitch;
   uint8_t* p = bmpData;
   uint8_t* newBmp = new uint8_t[lineWidth*height];
   uint8_t* d = newBmp;
   uint8_t* destEnd = newBmp + lineWidth*height;
   int32_t line = 0;
   int32_t shift = 4;

    while (bmpData - p < size && d < destEnd)
    {
        if (*p == 0)
        {
            ++p;

            switch(*p)
            {
            case 0: // end of line
                ++p;
                ++line;
                d = newBmp + (line*lineWidth);
                shift = 4;
                break;
            case 1: // end of bmp
                delete [] bmpData;
                bmpData = newBmp;
                return;
            case 2:
                {
                    ++p;
               int32_t x = (uint8_t)*p; ++p;
               int32_t y = (uint8_t)*p; ++p;
                    d += x/2 + y*lineWidth;
                    shift = x%2==0 ? 4 : 0;
                }
                break;
            default:
                {
                    // absolute mode
               int32_t count = (uint8_t)*p; ++p;
               int32_t readAdditional = ((2-((count)%2))%2);
               int32_t readShift = 4;
               int32_t i;

                    for (i=0; i<count; ++i)
                    {
                  int32_t color = (((uint8_t)*p) >> readShift) & 0x0f;
                        readShift -= 4;
                        if (readShift < 0)
                        {
                            ++*p;
                            readShift = 4;
                        }

                  uint8_t mask = 0x0f << shift;
                        *d = (*d & (~mask)) | ((color << shift) & mask);

                        shift -= 4;
                        if (shift < 0)
                        {
                            shift = 4;
                            ++d;
                        }

                    }

                    for (i=0; i<readAdditional; ++i)
                        ++p;
                }
            }
        }
        else
        {
         int32_t count = (uint8_t)*p; ++p;
         int32_t color1 = (uint8_t)*p; color1 = color1 & 0x0f;
         int32_t color2 = (uint8_t)*p; color2 = (color2 >> 4) & 0x0f;
            ++p;

         for (int32_t i=0; i<count; ++i)
            {
            uint8_t mask = 0x0f << shift;
            uint8_t toSet = (shift==0 ? color1 : color2) << shift;
                *d = (*d & (~mask)) | (toSet & mask);

                shift -= 4;
                if (shift < 0)
                {
                    shift = 4;
                    ++d;
                }
            }
        }
    }

    delete [] bmpData;
    bmpData = newBmp;
}



//! creates a surface from the file
IImage* CImageLoaderBMP::loadImage(io::IReadFile* file) const
{
    SBMPHeader header;

    file->read(&header, sizeof(header));

#ifdef __BIG_ENDIAN__
    header.Id = os::Byteswap::byteswap(header.Id);
    header.FileSize = os::Byteswap::byteswap(header.FileSize);
    header.BitmapDataOffset = os::Byteswap::byteswap(header.BitmapDataOffset);
    header.BitmapHeaderSize = os::Byteswap::byteswap(header.BitmapHeaderSize);
    header.Width = os::Byteswap::byteswap(header.Width);
    header.Height = os::Byteswap::byteswap(header.Height);
    header.Planes = os::Byteswap::byteswap(header.Planes);
    header.BPP = os::Byteswap::byteswap(header.BPP);
    header.Compression = os::Byteswap::byteswap(header.Compression);
    header.BitmapDataSize = os::Byteswap::byteswap(header.BitmapDataSize);
    header.PixelPerMeterX = os::Byteswap::byteswap(header.PixelPerMeterX);
    header.PixelPerMeterY = os::Byteswap::byteswap(header.PixelPerMeterY);
    header.Colors = os::Byteswap::byteswap(header.Colors);
    header.ImportantColors = os::Byteswap::byteswap(header.ImportantColors);
#endif

   int32_t pitch = 0;

    //! return if the header is false

    if (header.Id != 0x4d42)
        return 0;

    if (header.Compression > 2) // we'll only handle RLE-Compression
    {
        os::Printer::log("Compression mode not supported.", ELL_ERROR);
        return 0;
    }

    // adjust bitmap data size to dword boundary
    header.BitmapDataSize += (4-(header.BitmapDataSize%4))%4;

    // read palette

    long pos = file->getPos();
   int32_t paletteSize = (header.BitmapDataOffset - pos) / 4;

   int32_t* paletteData = 0;
    if (paletteSize)
    {
      paletteData = new int32_t[paletteSize];
      file->read(paletteData, paletteSize * sizeof(int32_t));
#ifdef __BIG_ENDIAN__
      for (int32_t i=0; i<paletteSize; ++i)
            paletteData[i] = os::Byteswap::byteswap(paletteData[i]);
#endif
    }

    // read image data

    if (!header.BitmapDataSize)
    {
        // okay, lets guess the size
        // some tools simply don't set it
        header.BitmapDataSize = static_cast<u32>(file->getSize()) - header.BitmapDataOffset;
    }

    file->seek(header.BitmapDataOffset);

    f32 t = (header.Width) * (header.BPP / 8.0f);
   int32_t widthInBytes = (int32_t)t;
    t -= widthInBytes;
    if (t!=0.0f)
        ++widthInBytes;

   int32_t lineData = widthInBytes + ((4-(widthInBytes%4)))%4;
    pitch = lineData - widthInBytes;

   uint8_t* bmpData = new uint8_t[header.BitmapDataSize];
    file->read(bmpData, header.BitmapDataSize);

    // decompress data if needed
    switch(header.Compression)
    {
    case 1: // 8 bit rle
        decompress8BitRLE(bmpData, header.BitmapDataSize, header.Width, header.Height, pitch);
        break;
    case 2: // 4 bit rle
        decompress4BitRLE(bmpData, header.BitmapDataSize, header.Width, header.Height, pitch);
        break;
    }

    // create surface

    // no default constructor from packed area! ARM problem!
    core::dimension2d<u32> dim;
    dim.Width = header.Width;
    dim.Height = header.Height;

    IImage* image = 0;
    switch(header.BPP)
    {
    case 1:
        image = new CImage(ECF_A1R5G5B5, dim);
        if (image)
            CColorConverter::convert1BitTo16Bit(bmpData, (s16*)image->lock(), header.Width, header.Height, pitch, true);
        break;
    case 4:
        image = new CImage(ECF_A1R5G5B5, dim);
        if (image)
            CColorConverter::convert4BitTo16Bit(bmpData, (s16*)image->lock(), header.Width, header.Height, paletteData, pitch, true);
        break;
    case 8:
        image = new CImage(ECF_A1R5G5B5, dim);
        if (image)
            CColorConverter::convert8BitTo16Bit(bmpData, (s16*)image->lock(), header.Width, header.Height, paletteData, pitch, true);
        break;
    case 16:
        image = new CImage(ECF_A1R5G5B5, dim);
        if (image)
            CColorConverter::convert16BitTo16Bit((s16*)bmpData, (s16*)image->lock(), header.Width, header.Height, pitch, true);
        break;
    case 24:
        image = new CImage(ECF_R8G8B8, dim);
        if (image)
         CColorConverter::convert24BitTo24Bit(bmpData, (uint8_t*)image->lock(), header.Width, header.Height, pitch, true, true);
        break;
    case 32: // thx to Reinhard Ostermeier
        image = new CImage(ECF_A8R8G8B8, dim);
        if (image)
         CColorConverter::convert32BitTo32Bit((int32_t*)bmpData, (int32_t*)image->lock(), header.Width, header.Height, pitch, true);
        break;
    };
    if (image)
        image->unlock();

    // clean up

    delete [] paletteData;
    delete [] bmpData;

    return image;
}


//! creates a loader which is able to load windows bitmaps
IImageLoader* createImageLoaderBMP()
{
    return new CImageLoaderBMP;
}

#endif

