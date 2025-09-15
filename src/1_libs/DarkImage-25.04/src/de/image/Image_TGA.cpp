#include <de/image/Image_TGA.h>

namespace de {
namespace image {

#if defined(DE_IMAGE_READER_TGA_ENABLED)

// read and process the bitmap's footer
/*
    TargaThumbnail thumbnail;
    if(isTARGA20(io, handle))
    {
        TGAFOOTER footer;
        const long footer_offset = start_offset + eof - sizeof(footer);

        io->seek_proc(handle, footer_offset, SEEK_SET);
        io->read_proc(&footer, sizeof(tagTGAFOOTER), 1, handle);

        #ifdef FREEIMAGE_BIGENDIAN
        SwapFooter(&footer);
        #endif
        BOOL hasExtensionArea = footer.extension_offset > 0;
        if(hasExtensionArea)
        {
            TGAEXTENSIONAREA extensionarea;
            io->seek_proc(handle, footer.extension_offset, SEEK_SET);
            io->read_proc(&extensionarea, sizeof(extensionarea), 1, handle);

            #ifdef FREEIMAGE_BIGENDIAN
            SwapExtensionArea(&extensionarea);
            #endif

            DWORD postage_stamp_offset = extensionarea.postage_stamp_offset;
            BOOL hasThumbnail = (postage_stamp_offset > 0) && (postage_stamp_offset < (DWORD)footer_offset);
            if(hasThumbnail)
            {
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
        DE_ERROR("No colormap in TGA ",file.uri() )
        return 0;
    }

    if ( header.bitsPerPixel != 8 )
    {
        DE_ERROR("No indexed pixels in TGA ",file.uri() )
        return 0;
    }

    if ( header.cm_entryCount < 1 )
    {
        DE_ERROR("No colormap entry in TGA ",file.uri() )
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
            palette.emplace_back( dbRGBA(r,r,r) );
        }
        else if ( header.cm_entrySize == 16 )
        {
            uint16_t rgb555 = file.readU16_lsb();
            //unsigned start = (unsigned)header.cm_first_entry;
            //unsigned stop = MIN((unsigned)256, (unsigned)header.cm_length);
            uint8_t r = (((rgb555 & 0x7C00) >> 10) * 0xFF) / 0x1F;
            uint8_t g = (((rgb555 & 0x03E0) >> 5) * 0xFF) / 0x1F;
            uint8_t b = ( (rgb555 & 0x001F)       * 0xFF) / 0x1F;
            palette.emplace_back( dbRGBA(r,g,b) );
        }
        else if ( header.cm_entrySize == 24 )
        {
            //unsigned start = (unsigned)header.cm_first_entry;
            //unsigned stop = MIN((unsigned)256, (unsigned)header.cm_length);
            uint8_t b = file.readU8();
            uint8_t g = file.readU8();
            uint8_t r = file.readU8();
            palette.emplace_back( dbRGBA(r,g,b) );
        }
        else if ( header.cm_entrySize == 32 )
        {
            //unsigned start = (unsigned)header.cm_first_entry;
            //unsigned stop = MIN((unsigned)256, (unsigned)header.cm_length);
            uint8_t b = file.readU8();
            uint8_t g = file.readU8();
            uint8_t r = file.readU8();
            uint8_t a = file.readU8();
            palette.emplace_back( dbRGBA(r,g,b,a) );
        }
        else
        {
            DE_ERROR("Unsupported colormap entry size ", int(header.cm_entrySize)," in TGA ",file.uri() )
        }
    }

    size_t pos1 = file.tell();
    return pos1-pos0;
}

bool
ImageReaderTGA::load( Image & img, const uint8_t* p, size_t n, const std::string& uri )
{
    DE_DEBUG( "TGA.uri = ",uri )

    Binary file(p,n,uri);
    if ( !file.is_open() )
    {
        DE_ERROR("No binary ",uri)
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
        img.setUri( uri );
        img.resize( m_w, m_h, PixelFormat::R8G8B8A8 );

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
                        img.setPixel( x,y, dbRGBA( r,r,r ) );
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
                    img.setPixel( x,y, dbRGBA( r,g,b ) );
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
                    img.setPixel( x,y, dbRGBA( r,g,b,a ) );
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
        img = Image( m_w, m_h, PixelFormat::RGBA_32 );
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
                img.setPixel( x,y, dbRGBA( r,g,b,a ) );
            }
        }
    }

    return true;
}

#endif // DE_IMAGE_READER_TGA_ENABLED

// ===========================================================================
// ===========================================================================
// ===========================================================================

#ifdef DE_IMAGE_WRITER_TGA_ENABLED

bool
ImageWriterTGA::save( Image const & img, std::string const & uri, uint32_t quality )
{
   if ( img.empty() )
   {
      DE_ERROR("Empty source image.")
      return false;
   }

   image::tga::TGAHeader header;
   header.idLength = 0;
   header.cm_type = 0;
   header.imageType = 2;
   header.cm_tableOffset = 0;
   header.cm_entryCount = 0;
   header.cm_entrySize = 0;
   header.xOrigin = 0;
   header.yOrigin = 0;
   header.w = uint16_t( img.w() );
   header.h = uint16_t( img.h() );
   // top left of image is the top. the image loader needs to
   // be fixed to only swap/flip
   header.imageDescriptor = (1 << 5);
   // chances are good we'll need to swizzle data, so i'm going
   // to convert and write one scan line at a time. it's also
   // a bit cleaner this way

   PixelFormatConverter::Converter_t converter = nullptr;

   if ( img.pixelFormat() == PixelFormat::R8G8B8 )
   {
      header.bitsPerPixel = 24;
      header.imageDescriptor |= 0;
      converter = PixelFormatConverter::convert_R8G8B8_to_B8G8R8;
   }
   else if ( img.pixelFormat() == PixelFormat::R8G8B8A8 )
   {
      header.bitsPerPixel = 32;
      header.imageDescriptor |= 8;
      converter = PixelFormatConverter::convert_R8G8B8A8_to_B8G8R8A8;
   }
   else if ( img.pixelFormat() == PixelFormat::R5G5B5A1 )
   {
      header.bitsPerPixel = 16;
      header.imageDescriptor |= 1;
      converter = PixelFormatConverter::convert_R5G5B5A1_to_R5G5B5A1;
   }
   else if ( img.pixelFormat() == PixelFormat::R5G6B5 )
   {
      header.bitsPerPixel = 16;
      header.imageDescriptor |= 1;
      converter = PixelFormatConverter::convert_R5G6B5_to_R5G5B5A1;
   }

   if ( !converter )
   {
      DE_ERROR("No color-converter found for source image.")
      return false;
   }

   FILE * file = ::fopen( uri.c_str(), "wb" );
   if ( !file )
   {
      DE_ERROR("Cant open write file. uri(",uri,")")
      return false;
   }

   // int32_t w = img.getWidth();
   // int32_t h = img.getHeight();
   size_t expectBytes = sizeof( header ); // used more than once
   size_t resultBytes = ::fwrite( &header, 1, expectBytes, file );
   if ( expectBytes != resultBytes )
   {
      DE_ERROR("Expected size differs.")
      return false;
   }

   uint8_t const * src = img.data();
   // length of one row of the source image in bytes
   size_t pitchBytes = img.stride();
   // length of one output row in bytes
   size_t row_size = header.getBytesPerPixel() * header.getWidth();
   // allocate a row do translate data into
   uint8_t* row_pointer = new uint8_t[ row_size ];

   for ( uint32_t y = 0; y < header.getHeight(); ++y )
   {
      // source, length [pixels], destination
      if ( img.pixelFormat() == PixelFormat::R8G8B8 )
      {
         PixelFormatConverter::convert24Bit_to_24Bit(
            &src[ y * pitchBytes],
            row_pointer,
            header.getWidth(),
            1,
            0,
            0,
            true );
      }
      else
      {
         converter( &src[ y * pitchBytes ], row_pointer, header.getWidth() );
      }

      resultBytes = ::fwrite( row_pointer, 1, row_size, file );
      if ( resultBytes != row_size )
      {
         break;
      }
   }

   delete [] row_pointer;

   image::tga::TGAFooter footer;
   footer.ExtensionOffset = 0;
   footer.DeveloperOffset = 0;
   ::strncpy( footer.Signature, "TRUEVISION-XFILE.", 18 );

   expectBytes = sizeof( footer ); // used more than once
   resultBytes = ::fwrite( &footer, 1, sizeof( footer ), file );
   if ( expectBytes != resultBytes )
   {
      DE_ERROR("ExpectBytes(",expectBytes,") differs from WrittenBytes(",resultBytes,")")
      //return false;
   }

   ::fclose( file );
   return true;
}

#endif // DE_IMAGE_WRITER_TGA_ENABLED

} // end namespace image.
} // end namespace de.
















































#if 0

// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CImageWriterTGA.h"

#ifdef _IRR_COMPILE_WITH_TGA_WRITER_

#include "CImageLoaderTGA.h"
#include "IWriteFile.h"
#include "CColorConverter.h"
#include "irrString.h"

namespace irr
{
namespace video
{

IImageWriter* createImageWriterTGA()
{
   return new CImageWriterTGA;
}

CImageWriterTGA::CImageWriterTGA()
{
#ifdef _DEBUG
   setDebugName("CImageWriterTGA");
#endif
}

bool CImageWriterTGA::isAWriteableFileExtension(const io::path& filename) const
{
   return core::hasFileExtension ( filename, "tga" );
}

bool CImageWriterTGA::writeImage(io::IWriteFile *file, IImage *image,uint32_t param) const
{
   STGAHeader header;
   header.IdLength = 0;
   header.cm_type = 0;
   header.imageType = 2;
   header.FirstEntryIndex[0] = 0;
   header.FirstEntryIndex[1] = 0;
   header.ColorMapLength = 0;
   header.ColorMapEntrySize = 0;
   header.XOrigin[0] = 0;
   header.XOrigin[1] = 0;
   header.YOrigin[0] = 0;
   header.YOrigin[1] = 0;
   header.ImageWidth = image->getDimension().Width;
   header.ImageHeight = image->getDimension().Height;

   // top left of image is the top. the image loader needs to
   // be fixed to only swap/flip
   header.ImageDescriptor = (1 << 5);

   // chances are good we'll need to swizzle data, so i'm going
   // to convert and write one scan line at a time. it's also
   // a bit cleaner this way
   void (*CColorConverter_convertFORMATtoFORMAT)(const void*, s32, void*) = 0;
   switch(image->getFormat())
   {
   case ECF_A8R8G8B8:
      CColorConverter_convertFORMATtoFORMAT
         = CColorConverter::convert_A8R8G8B8toA8R8G8B8;
      header.PixelDepth = 32;
      header.ImageDescriptor |= 8;
      break;
   case ECF_A1R5G5B5:
      CColorConverter_convertFORMATtoFORMAT
         = CColorConverter::convert_A1R5G5B5toA1R5G5B5;
      header.PixelDepth = 16;
      header.ImageDescriptor |= 1;
      break;
   case ECF_R5G6B5:
      CColorConverter_convertFORMATtoFORMAT
         = CColorConverter::convert_R5G6B5toA1R5G5B5;
      header.PixelDepth = 16;
      header.ImageDescriptor |= 1;
      break;
   case ECF_R8G8B8:
      CColorConverter_convertFORMATtoFORMAT
         = CColorConverter::convert_R8G8B8toR8G8B8;
      header.PixelDepth = 24;
      header.ImageDescriptor |= 0;
      break;
#ifndef _DEBUG
   default:
      break;
#endif
   }

   // couldn't find a color converter
   if (!CColorConverter_convertFORMATtoFORMAT)
      return false;

   if (file->write(&header, sizeof(header)) != sizeof(header))
      return false;

   uint8_t* scan_lines = (uint8_t*)image->lock();
   if (!scan_lines)
      return false;

   // size of one pixel in bytes
   uint32_t pixel_size = image->getBytesPerPixel();

   // length of one row of the source image in bytes
   uint32_t row_stride = (pixel_size * header.ImageWidth);

   // length of one output row in bytes
   s32 row_size = ((header.PixelDepth / 8) * header.ImageWidth);

   // allocate a row do translate data into
   uint8_t* row_pointer = new uint8_t[row_size];

   uint32_t y;
   for (y = 0; y < header.ImageHeight; ++y)
   {
      // source, length [pixels], destination
      if (image->getColorFormat()==ECF_R8G8B8)
         CColorConverter::convert24BitTo24Bit(&scan_lines[y * row_stride], row_pointer, header.ImageWidth, 1, 0, 0, true);
      else
         CColorConverter_convertFORMATtoFORMAT(&scan_lines[y * row_stride], header.ImageWidth, row_pointer);
      if (file->write(row_pointer, row_size) != row_size)
         break;
   }

   delete [] row_pointer;

   image->unlock();

   STGAFooter footer;
   footer.ExtensionOffset = 0;
   footer.DeveloperOffset = 0;
   strncpy(footer.Signature, "TRUEVISION-XFILE.", 18);

   if (file->write(&footer, sizeof(footer)) < (s32)sizeof(footer))
      return false;

   return header.ImageHeight <= y;
}

} // namespace video
} // namespace irr

#endif

#endif





