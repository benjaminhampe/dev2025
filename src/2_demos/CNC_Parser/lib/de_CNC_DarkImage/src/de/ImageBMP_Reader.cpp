#include "ImageBMP_Reader.hpp"

#ifdef DE_IMAGE_READER_BMP_ENABLED

namespace de {
namespace image {

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
            out[x+0] = RGBA_B( color ); // palette[ (in[0] << 2 ) + 2];   // b -> r
            out[x+1] = RGBA_G( color ); // palette[ (in[0] << 2 ) + 1];   // g -> g
            out[x+2] = RGBA_R( color ); // palette[ (in[0] << 2 ) + 0];   // r -> b
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

bool
ImageReaderBMP::load( Image & img, Binary & file )
{
   if ( !file.is_open() )
   {
      DE_ERROR("Cant open file(", file.getUri(), ") for binary read.")
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

   DE_DEBUG("BMP.Uri = ", file.getUri() )
   DE_DEBUG("BMP.Bytes = ", fileBytes )
   DE_DEBUG("BMP.Width = ", w )
   DE_DEBUG("BMP.Height = ", h )
   DE_DEBUG("BMP.BitsPerPixel = ", bpp )
   DE_DEBUG("BMP.Compression = ",header.Compression)
   DE_DEBUG("BMP.Colors = ",header.Colors)
   DE_DEBUG("BMP.DataOffset = ", dataOffset )
   DE_DEBUG("BMP.DataBytes = ", dataBytes )
   DE_FLUSH

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


   ColorConverter::Converter_t converter = nullptr;

   if ( bpp == 24 )
   {
      std::vector< uint8_t > tmpBuffer( dataBytes );
      file.seek( dataOffset );
      file.read( tmpBuffer.data(), dataBytes );
      file.close();

      auto fmt = ColorFormat::RGB888;
      converter = ColorConverter::getConverter( ColorFormat::BGR888, fmt );
      img.clear();
      img.setFormat( fmt );
      img.resize( w, h );
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

      auto fmt = ColorFormat::RGBA8888;
      converter = ColorConverter::getConverter( ColorFormat::BGRA8888, fmt );
      img.clear();
      img.setFormat( fmt );
      img.resize( w, h );
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

      img.clear();
      img.setFormat( ColorFormat::RGB888 );
      img.resize( w, h );
      convert8BitTo24Bit( tmpBuffer.data(), img.data(), w, h, palette, 0, false );
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

bool
ImageReaderBMP::load( Image & img, std::string const & uri )
{
   Binary bin( uri );
   if ( !bin.is_open() )
   {
      DE_ERROR( "Cant open binary ", uri )
      return false;
   }

   return load( img, bin );
}

} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_READER_BMP_ENABLED
