#include "ImageTGA_Writer.hpp"

#ifdef DE_IMAGE_WRITER_TGA_ENABLED

#include "ImageTGA.hpp"

namespace de {
namespace image {

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
   header.w = uint16_t( img.getWidth() );
   header.h = uint16_t( img.getHeight() );
   // top left of image is the top. the image loader needs to
   // be fixed to only swap/flip
   header.imageDescriptor = (1 << 5);
   // chances are good we'll need to swizzle data, so i'm going
   // to convert and write one scan line at a time. it's also
   // a bit cleaner this way

   ColorConverter::Converter_t converter = nullptr;

   if ( img.getFormat() == ColorFormat::RGB888 )
   {
      header.bitsPerPixel = 24;
      header.imageDescriptor |= 0;
      converter = ColorConverter::convert_RGB888toBGR888;
   }
   else if ( img.getFormat() == ColorFormat::RGBA8888 )
   {
      header.bitsPerPixel = 32;
      header.imageDescriptor |= 8;
      converter = ColorConverter::convert_ARGB8888toABGR8888;
   }
   else if ( img.getFormat() == ColorFormat::ARGB1555 )
   {
      header.bitsPerPixel = 16;
      header.imageDescriptor |= 1;
      converter = ColorConverter::convert_ARGB1555toARGB1555;
   }
   else if ( img.getFormat() == ColorFormat::RGB565 )
   {
      header.bitsPerPixel = 16;
      header.imageDescriptor |= 1;
      converter = ColorConverter::convert_RGB565toARGB1555;
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
   size_t pitchBytes = img.getStride();
   // length of one output row in bytes
   size_t row_size = header.getBytesPerPixel() * header.getWidth();
   // allocate a row do translate data into
   uint8_t* row_pointer = new uint8_t[ row_size ];

   for ( uint32_t y = 0; y < header.getHeight(); ++y )
   {
      // source, length [pixels], destination
      if ( img.getFormat() == ColorFormat::RGB888 )
      {
         ColorConverter::convert24BitTo24Bit( &src[ y * pitchBytes], row_pointer, header.getWidth(), 1, 0, 0, true );
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

} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_WRITER_TGA_ENABLED










































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
