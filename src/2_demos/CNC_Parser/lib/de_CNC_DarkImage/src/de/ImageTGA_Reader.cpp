#include "ImageTGA_Reader.hpp"

#ifdef DE_IMAGE_READER_TGA_ENABLED

#include "ImageTGA.hpp"
#include <fstream>
#include <de/Hex.hpp>
#include <de/FileMagic.hpp>

namespace de {
namespace image {

// read and process the bitmap's footer
/*
   TargaThumbnail thumbnail;
   if(isTARGA20(io, handle)) {
      TGAFOOTER footer;
      const long footer_offset = start_offset + eof - sizeof(footer);

      io->seek_proc(handle, footer_offset, SEEK_SET);
      io->read_proc(&footer, sizeof(tagTGAFOOTER), 1, handle);

#ifdef FREEIMAGE_BIGENDIAN
      SwapFooter(&footer);
#endif
      BOOL hasExtensionArea = footer.extension_offset > 0;
      if(hasExtensionArea) {
         TGAEXTENSIONAREA extensionarea;
         io->seek_proc(handle, footer.extension_offset, SEEK_SET);
         io->read_proc(&extensionarea, sizeof(extensionarea), 1, handle);

#ifdef FREEIMAGE_BIGENDIAN
         SwapExtensionArea(&extensionarea);
#endif

         DWORD postage_stamp_offset = extensionarea.postage_stamp_offset;
         BOOL hasThumbnail = (postage_stamp_offset > 0) && (postage_stamp_offset < (DWORD)footer_offset);
         if(hasThumbnail) {
            io->seek_proc(handle, postage_stamp_offset, SEEK_SET);
            thumbnail.read(io, handle, footer_offset - postage_stamp_offset);
         }
      }
   }

*/

size_t
ImageReaderTGA::parseColorTable( std::vector< uint32_t > & palette, tga::TGAHeader const & header, de::Binary & file )
{
   palette.clear();

   if ( header.cm_type < 1 )
   {
      DE_ERROR("No colormap in TGA ",file.getUri() )
      return 0;
   }

   if ( header.bitsPerPixel != 8 )
   {
      DE_ERROR("No indexed pixels in TGA ",file.getUri() )
      return 0;
   }

   if ( header.cm_entryCount < 1 )
   {
      DE_ERROR("No colormap entry in TGA ",file.getUri() )
      return 0;
   }

   //thumbnail.setDepth(header.is_pixel_depth);
   //const int line = CalculateLine(header.is_width, header.is_pixel_depth);
   //const int pixel_bits = header.is_pixel_depth;
   //const int pixel_size = pixel_bits/8;
   //int fliphoriz = (header.imageDescriptor & 0x10) ? 1 : 0;
   //int flipvert = (header.imageDescriptor & 0x20) ? 1 : 0;


   //size_t count = 0;

   // calculate the color map size
   //size_t csize = header.cm_entryCount * header.cm_entrySize / 8;

   // read the palette ( color table )
   palette.reserve( header.cm_entryCount );

   size_t pos0 = file.tell();

   for ( size_t i = 0; i < header.cm_entryCount; ++i )
   {
      if ( header.cm_entrySize == 8 )
      {
         uint8_t r = file.readU8();
         palette.emplace_back( RGBA(r,r,r) );
      }
      else if ( header.cm_entrySize == 16 )
      {
         uint16_t rgb555 = file.readU16_lsb();
         //unsigned start = (unsigned)header.cm_first_entry;
         //unsigned stop = MIN((unsigned)256, (unsigned)header.cm_length);
         uint8_t r = (((rgb555 & 0x7C00) >> 10) * 0xFF) / 0x1F;
         uint8_t g = (((rgb555 & 0x03E0) >> 5) * 0xFF) / 0x1F;
         uint8_t b = ( (rgb555 & 0x001F)       * 0xFF) / 0x1F;
         palette.emplace_back( RGBA(r,g,b) );
      }
      else if ( header.cm_entrySize == 24 )
      {
         //unsigned start = (unsigned)header.cm_first_entry;
         //unsigned stop = MIN((unsigned)256, (unsigned)header.cm_length);
         uint8_t b = file.readU8();
         uint8_t g = file.readU8();
         uint8_t r = file.readU8();
         palette.emplace_back( RGBA(r,g,b) );
      }
      else if ( header.cm_entrySize == 32 )
      {
         //unsigned start = (unsigned)header.cm_first_entry;
         //unsigned stop = MIN((unsigned)256, (unsigned)header.cm_length);
         uint8_t b = file.readU8();
         uint8_t g = file.readU8();
         uint8_t r = file.readU8();
         uint8_t a = file.readU8();
         palette.emplace_back( RGBA(r,g,b,a) );
      }
      else
      {
         DE_ERROR("Unsupported colormap entry size ", int(header.cm_entrySize)," in TGA ",file.getUri() )
      }
   }

   size_t pos1 = file.tell();
   return pos1-pos0;
}


bool
ImageReaderTGA::load( Image & img, std::string const & uri )
{
   DE_DEBUG( "TGA.uri = ",uri )

   de::Binary file; // byte file to RAM
   if ( !file.load( uri ) )
   {
      DE_ERROR("Cant load binary ",uri,".")
      return false;
   }

   tga::TGAHeader header;
   file.read( &header, sizeof(header) );
   header.dump();

   constexpr static const uint8_t s_TGA_raw[12] = {0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
   constexpr static const uint8_t s_TGA_zip[12] = {0x0, 0x0, 0xA, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
   bool isRawImgData = (std::memcmp( s_TGA_raw, &header, sizeof(s_TGA_raw) ) == 0);
   bool isCompressed = (std::memcmp( s_TGA_zip, &header, sizeof(s_TGA_zip) ) == 0);
   DE_DEBUG( "TGA.isRawImgData = ",isRawImgData )
   DE_DEBUG( "TGA.isCompressed = ",isCompressed )

   // skip comment
   file.seek(header.idLength, SEEK_CUR);

   // read color table data
   std::vector< uint32_t > palette;
   size_t nCTBytes = parseColorTable( palette, header, file );
   DE_DEBUG( "TGA.ParsedColorTable.Bytes = ",nCTBytes )
   DE_DEBUG( "TGA.ParsedColorTable.Size = ",palette.size() )

   // read pixel data
   uint32_t m_bpp = header.bitsPerPixel;
   uint32_t m_w = header.w;
   uint32_t m_h = header.h;
   size_t size = 0;
   std::vector< uint8_t > bytes;

   if ( !isCompressed )
   {
      size = ((m_w * m_bpp + 31) / 32) * 4 * m_h;
      bytes.resize( size );
      file.read( bytes.data(), size);
      img.setFormat( ColorFormat::RGBA8888 );
      img.setUri( uri );
      img.resize( m_w, m_h );

      if (m_bpp == 8)
      {
         if (palette.empty())
         {
            DE_DEBUG("TGA raw 8bit color format.")
            size_t i = 0;
            for ( uint32_t y = 0; y < m_h; ++y )
            {
               for ( uint32_t x = 0; x < m_w; ++x )
               {
                  uint8_t r = bytes[ i++ ];
                  img.setPixel( x,y, RGBA( r,r,r ) );
               }
            }
         }
         else // if (palette.size() > 0)
         {
            DE_DEBUG("TGA indexed 8bit palette with ",header.cm_entrySize," bit colors.")

            size_t i = 0;
            for ( uint32_t y = 0; y < m_h; ++y )
            {
               for ( uint32_t x = 0; x < m_w; ++x )
               {
                  uint8_t index = bytes[ i++ ];
                  if ( index >= palette.size() )
                  {
                     DE_ERROR("Invalid palette(",palette.size(),") index(",index,")")
                     img.setPixel( x,y, 0x00000000 );
                  }
                  else
                  {
                     img.setPixel( x,y, palette[ index ] );
                  }
               }
            }
         }
         return true;
      }
      else if (m_bpp == 16)
      {
         DE_DEBUG("Unsupported raw 16 bit color TGA format. ", uri)

         if (palette.empty())
         {
//            size_t i = 0;
//            for ( uint32_t y = 0; y < m_h; ++y )
//            {
//               for ( uint32_t x = 0; x < m_w; ++x )
//               {
//                  uint8_t r = bytes[ i++ ];
//                  img.setPixel( x,y, RGBA( r,r,r ) );
//               }
//            }
         }
         else // if (palette.size() > 0)
         {
//            DE_DEBUG("TGA indexed 8bit palette with ",header.cm_entrySize," bit colors.")

//            size_t i = 0;
//            for ( uint32_t y = 0; y < m_h; ++y )
//            {
//               for ( uint32_t x = 0; x < m_w; ++x )
//               {
//                  uint8_t index = bytes[ i++ ];
//                  if ( index >= palette.size() )
//                  {
//                     DE_ERROR("Invalid palette(",palette.size(),") index(",index,")")
//                     img.setPixel( x,y, 0x00000000 );
//                  }
//                  else
//                  {
//                     img.setPixel( x,y, palette[ index ] );
//                  }
//               }
//            }
         }
         return false;
      }
      else if (m_bpp == 24)
      {
         DE_DEBUG("TGA raw 24bit format.")

         size_t i = 0;
         for ( uint32_t y = 0; y < m_h; ++y )
         {
            for ( uint32_t x = 0; x < m_w; ++x )
            {
               uint8_t b = bytes[ i++ ];
               uint8_t g = bytes[ i++ ];
               uint8_t r = bytes[ i++ ];
               img.setPixel( x,y, RGBA( r,g,b ) );
            }
         }
         return true;
      }
      else if (m_bpp == 32)
      {
         DE_DEBUG("TGA raw 32bit format.")

         size_t i = 0;
         for ( uint32_t y = 0; y < m_h; ++y )
         {
            for ( uint32_t x = 0; x < m_w; ++x )
            {
               uint8_t b = bytes[ i++ ];
               uint8_t g = bytes[ i++ ];
               uint8_t r = bytes[ i++ ];
               uint8_t a = bytes[ i++ ];
               img.setPixel( x,y, RGBA( r,g,b,a ) );
            }
         }
         return true;
      }
      else
      {
         DE_ERROR("Unsupported raw TGA bits(",m_bpp,").")
         return false;
      }
   }
   else // if ( isCompressed )
   {
      size  = ((m_w * m_bpp + 31) / 32) * 4 * m_h;

      if ((m_bpp != 24) && (m_bpp != 32))
      {
         file.close();
         DE_ERROR("Invalid compressed TGA format bits(",m_bpp,"). Require: 24 or 32 bit.")
         return false;
      }

      image::tga::PixelInfo pixel = {0};
      size_t pixelIndex = 0;
      uint8_t chunk = {0};
      int bytesPP = (m_bpp / 8);
      bytes.resize( m_w * m_h * sizeof(image::tga::PixelInfo) );

      size_t i = 0;
      do
      {
         //file.read( reinterpret_cast< char* >( &chunk ), sizeof( chunk ) );
         file.read( &chunk, sizeof( chunk ) );

         if(chunk < 128) // A 'RAW' Chunk
         {
            ++chunk; // Add 1 To The Value To Get Total Number Of Raw Pixels

            for ( int i = 0; i < chunk; ++i, ++pixelIndex )
            {
               //file.read(reinterpret_cast<char*>(&pixel), bytesPP);
               file.read( &pixel, bytesPP );

               bytes[ i++ ] = pixel.B;
               bytes[ i++ ] = pixel.G;
               bytes[ i++ ] = pixel.R;
               if (m_bpp > 24)
               {
                  bytes[i++] = pixel.A;
               }
            }
         }
         else // A 'RLE' Chunk
         {
            chunk -= 127; // Subtract 127 To Get Rid Of The ID Bit

            //file.read( reinterpret_cast<char*>( &pixel ), bytesPP );
            file.read( &pixel, bytesPP );

            for ( int i = 0; i < chunk; ++i, ++pixelIndex )
            {
               bytes[ i++ ] = pixel.B;
               bytes[ i++ ] = pixel.G;
               bytes[ i++ ] = pixel.R;
               if ( m_bpp > 24 )
               {
                  bytes[ i++ ] = pixel.A;
               }
            }
         }
      }
      while( pixelIndex < (m_w * m_h) );

      // Todo: gather actually image.
      img = Image( m_w, m_h );
      img.setUri( uri );
      i = 0;

      for ( uint32_t y = 0; y < m_h; ++y )
      {
         for ( uint32_t x = 0; x < m_w; ++x )
         {
            uint8_t b = bytes[ i++ ];
            uint8_t g = bytes[ i++ ];
            uint8_t r = bytes[ i++ ];
            uint8_t a = 0xFF;
            if ( m_bpp > 24 )
            {
               a = bytes[ i++ ];
            }
            img.setPixel( x,y, RGBA( r,g,b,a ) );
         }
      }
   }

   return true;
}

// ===========================================================================

} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_READER_TGA_ENABLED

