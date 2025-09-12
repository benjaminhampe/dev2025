#include "ImagePNG_Writer.hpp"

#ifdef DE_IMAGE_WRITER_PNG_ENABLED

#include <png/png.h>

namespace de {
namespace image {
namespace png {

struct PNG
{
   // PNG function for error handling
   static void
   cb_error( png_structp ctx, png_const_charp msg )
   {
      // os::Printer::log("PNG fatal error", msg, ELL_ERROR);
      longjmp( png_jmpbuf( ctx ), 1 );
   }

   // PNG function for warning handling
   static void
   cb_warning( png_structp ctx, png_const_charp msg )
   {
      //os::Printer::log("PNG warning", msg, ELL_WARNING);
   }

   // PNG function for file writing
   static void PNGAPI
   cb_write_data_user( png_structp ctx, png_bytep data, png_size_t length )
   {
      FILE* file = (FILE*)png_get_io_ptr( ctx );
      size_t const writeBytes = size_t( length );
      size_t const checkBytes = (png_size_t)::fwrite( (const void*)data, 1, writeBytes, file );
      if ( checkBytes != writeBytes )
      {
         png_error( ctx, "Write Error" );
      }
   }
};

} // end namespace png

bool
ImageWriterPNG::save( Image const & img, std::string const & uri, uint32_t quality )
{
   if ( img.empty() )
   {
      DE_ERROR( "Empty image(", img.toString(), ")" )
      return false;
   }

   // Allocate the png write struct
   png_struct* ctx = png_create_write_struct(
      PNG_LIBPNG_VER_STRING,
      nullptr,
      (png_error_ptr)image::png::PNG::cb_error,
      (png_error_ptr)image::png::PNG::cb_warning
   );

   if ( !ctx )
   {
      DE_ERROR( "Cant create PNG encoder for image(", img.toString(), ")" )
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

   FILE * file = ::fopen( uri.c_str(), "wb" );
   if ( !file )
   {
      DE_ERROR( "Failed openToWriteBinary(", uri, ")." )
      return false;
   }

   png_set_write_fn(
      ctx,
      file,
      image::png::PNG::cb_write_data_user,
      nullptr
   );

   uint32_t w = img.getWidth();
   uint32_t h = img.getHeight();
   uint32_t scanlineBytes = img.getStride();

   size_t byteCount = size_t( scanlineBytes ) * h;
   std::vector< uint8_t > tmp;
   tmp.resize( byteCount, 0 );

   // DE_DEBUG( "Reserved scanline bytes(", byteCount, ")." )

   // png_destroy_write_struct( &ctx, &inf );
   int32_t transformType = PNG_TRANSFORM_IDENTITY;
   int32_t colorType = PNG_COLOR_TYPE_RGBA;
   uint8_t* psrc = const_cast< uint8_t* >( img.data() );
   uint8_t* pdst = const_cast< uint8_t* >( tmp.data() );
   size_t pixelCount = size_t( w ) * h;

   if ( img.getFormat() == ColorFormat::RGBA8888 )
   {
      //transformType = PNG_TRANSFORM_BGR;
      //DE_DEBUG( "ColorFormat is ARGB8888 for uri(", uri, ")." )
      ColorConverter::convert_ARGB8888toARGB8888( psrc, pdst, pixelCount );
   }
   else if ( img.getFormat() == ColorFormat::ARGB1555 )
   {
      //transformType = PNG_TRANSFORM_BGR;
      //DE_DEBUG( "ColorFormat is ARGB1555 for uri(", uri, ")." )
      ColorConverter::convert_ARGB1555toARGB8888( psrc, pdst, pixelCount );
   }
   else if ( img.getFormat() == ColorFormat::RGB888 )
   {
      colorType = PNG_COLOR_TYPE_RGB;
      //DE_DEBUG( "ColorFormat is RGB888 for uri(", uri, ")." )
      //ColorConverter::convert_RGB888toRGB888( pixels, w * h, tmp.data() );
      ColorConverter::convert_RGB888toARGB8888( psrc, pdst, pixelCount );
   }
   else if ( img.getFormat() == ColorFormat::RGB565 )
   {
      colorType = PNG_COLOR_TYPE_RGB;
      //DE_DEBUG( "ColorFormat is RGB565 for uri(", uri, ")." )
      ColorConverter::convert_RGB565toRGB888( psrc, pdst, pixelCount );
   }
   else
   {
      DE_ERROR( "Unsupported write format (", img.getFormatStr(), ")." )
   }

   //DE_DEBUG( "Write image (", img.toString(), ")." )

   // Fill array of pointers to rows in image data
   std::vector< uint8_t* > scanlinesToEncode;
   scanlinesToEncode.resize( h, nullptr );

   for ( size_t y = 0; y < h; ++y )
   {
      scanlinesToEncode[ y ] = pdst;
      pdst += scanlineBytes;
   }

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
      colorType,
      PNG_INTERLACE_NONE,
      PNG_COMPRESSION_TYPE_DEFAULT,
      PNG_FILTER_TYPE_DEFAULT
   );

   png_bytepp p = scanlinesToEncode.data();
   png_set_rows( ctx, inf, p );
   png_write_png( ctx, inf, transformType, nullptr );
   png_destroy_write_struct( &ctx, &inf );

   ::fclose( file );

   return true;
}

} // end namespace image
} // end namespace de.

#endif // DE_IMAGE_WRITER_PNG_ENABLED





































#if 0

// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifdef _IRR_COMPILE_WITH_PNG_WRITER_

#include "CImageLoaderPNG.h"
#include "CColorConverter.h"
#include "IWriteFile.h"
#include "irrString.h"
#include "os.h" // for logging

#ifdef _IRR_COMPILE_WITH_LIBPNG_
#ifndef _IRR_USE_NON_SYSTEM_LIB_PNG_
   #include <png.h> // use system lib png
#else // _IRR_USE_NON_SYSTEM_LIB_PNG_
   #include "libpng/png.h" // use irrlicht included lib png
#endif // _IRR_USE_NON_SYSTEM_LIB_PNG_
#endif // _IRR_COMPILE_WITH_LIBPNG_

namespace irr{
namespace video{

IImageWriter* createImageWriterPNG()
{
   return new CImageWriterPNG;
}

#ifdef _IRR_COMPILE_WITH_LIBPNG_
// PNG function for error handling
static void png_cpexcept_error(png_structp ctx, png_const_charp msg)
{
   os::Printer::log("PNG fatal error", msg, ELL_ERROR);
   longjmp(png_jmpbuf(ctx), 1);
}

// PNG function for warning handling
static void png_cpexcept_warning(png_structp ctx, png_const_charp msg)
{
   os::Printer::log("PNG warning", msg, ELL_WARNING);
}

// PNG function for file writing
void PNGAPI user_write_data_fcn(png_structp ctx, png_bytep data, png_size_t length)
{
   png_size_t check;

   io::IWriteFile* file=(io::IWriteFile*)png_get_io_ptr(ctx);
   check=(png_size_t) file->write((const void*)data,(u32)length);

   if (check != length)
      png_error(ctx, "Write Error");
}
#endif // _IRR_COMPILE_WITH_LIBPNG_

CImageWriterPNG::CImageWriterPNG()
{
#ifdef _DEBUG
   setDebugName("CImageWriterPNG");
#endif
}

bool CImageWriterPNG::isAWriteableFileExtension(const io::path& filename) const
{
#ifdef _IRR_COMPILE_WITH_LIBPNG_
   return core::hasFileExtension ( filename, "png" );
#else
   return false;
#endif
}

bool CImageWriterPNG::writeImage(io::IWriteFile* file, IImage* image,u32 param) const
{
#ifdef _IRR_COMPILE_WITH_LIBPNG_
   if (!file || !image)
      return false;

   // Allocate the png write struct
   png_structp ctx = png_create_write_struct(PNG_LIBPNG_VER_STRING,
      NULL, (png_error_ptr)png_cpexcept_error, (png_error_ptr)png_cpexcept_warning);
   if (!ctx)
   {
      os::Printer::log("PNGWriter: Internal PNG create write struct failure\n", file->getFileName(), ELL_ERROR);
      return false;
   }

   // Allocate the png info struct
   png_infop inf = png_create_info_struct(ctx);
   if (!inf)
   {
      os::Printer::log("PNGWriter: Internal PNG create info struct failure\n", file->getFileName(), ELL_ERROR);
      png_destroy_write_struct(&ctx, NULL);
      return false;
   }

   // for proper error handling
   if (setjmp(png_jmpbuf(ctx)))
   {
      png_destroy_write_struct(&ctx, &inf);
      return false;
   }

   png_set_write_fn(ctx, file, user_write_data_fcn, NULL);

   // Set info
   switch(image->getColorFormat())
   {
      case ECF_A8R8G8B8:
      case ECF_A1R5G5B5:
         png_set_IHDR(ctx, inf,
            image->getDimension().Width, image->getDimension().Height,
            8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
      break;
      default:
         png_set_IHDR(ctx, inf,
            image->getDimension().Width, image->getDimension().Height,
            8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
   }

   s32 lineWidth = image->getDimension().Width;
   switch(image->getColorFormat())
   {
   case ECF_R8G8B8:
   case ECF_R5G6B5:
      lineWidth*=3;
      break;
   case ECF_A8R8G8B8:
   case ECF_A1R5G5B5:
      lineWidth*=4;
      break;
   // TODO: Error handling in case of unsupported color format
   default:
      break;
   }
   uint8_t* tmpImage = new uint8_t[image->getDimension().Height*lineWidth];
   if (!tmpImage)
   {
      os::Printer::log("PNGWriter: Internal PNG create image failure\n", file->getFileName(), ELL_ERROR);
      png_destroy_write_struct(&ctx, &inf);
      return false;
   }

   uint8_t* data = (uint8_t*)image->lock();
   switch(image->getColorFormat())
   {
   case ECF_R8G8B8:
      CColorConverter::convert_R8G8B8toR8G8B8(data,image->getDimension().Height*image->getDimension().Width,tmpImage);
      break;
   case ECF_A8R8G8B8:
      CColorConverter::convert_A8R8G8B8toA8R8G8B8(data,image->getDimension().Height*image->getDimension().Width,tmpImage);
      break;
   case ECF_R5G6B5:
      CColorConverter::convert_R5G6B5toR8G8B8(data,image->getDimension().Height*image->getDimension().Width,tmpImage);
      break;
   case ECF_A1R5G5B5:
      CColorConverter::convert_A1R5G5B5toA8R8G8B8(data,image->getDimension().Height*image->getDimension().Width,tmpImage);
      break;
#ifndef _DEBUG
      // TODO: Error handling in case of unsupported color format
   default:
      break;
#endif
   }
   image->unlock();

   // Create array of pointers to rows in image data

   //Used to point to image rows
   uint8_t** RowPointers = new png_bytep[image->getDimension().Height];
   if (!RowPointers)
   {
      os::Printer::log("PNGWriter: Internal PNG create row pointers failure\n", file->getFileName(), ELL_ERROR);
      png_destroy_write_struct(&ctx, &inf);
      delete [] tmpImage;
      return false;
   }

   data=tmpImage;
   // Fill array of pointers to rows in image data
   for (u32 i=0; i<image->getDimension().Height; ++i)
   {
      RowPointers[i]=data;
      data += lineWidth;
   }
   // for proper error handling
   if (setjmp(png_jmpbuf(ctx)))
   {
      png_destroy_write_struct(&ctx, &inf);
      delete [] RowPointers;
      delete [] tmpImage;
      return false;
   }

   png_set_rows(ctx, inf, RowPointers);

   if (image->getFormat()==ECF_A8R8G8B8 || image->getFormat()==ECF_A1R5G5B5)
      png_write_png(ctx, inf, PNG_TRANSFORM_BGR, NULL);
   else
   {
      png_write_png(ctx, inf, PNG_TRANSFORM_IDENTITY, NULL);
   }

   delete [] RowPointers;
   delete [] tmpImage;
   png_destroy_write_struct(&ctx, &inf);
   return true;
#else
   return false;
#endif
}

} // namespace video
} // namespace irr

#endif

#endif
