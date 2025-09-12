#include "ImageBMP_Writer.hpp"

#ifdef DE_IMAGE_WRITER_BMP_ENABLED

namespace de {
namespace image {

bool
ImageWriterBMP::save( Image const & img, std::string const & uri, uint32_t quality )
{
   if ( img.empty() )
   {
      DE_ERROR("Image is empty.")
      return false;
   }

   ColorConverter::Converter_t converter =
      ColorConverter::getConverter( img.getFormat(), ColorFormat::BGR888 );

   if ( !converter )
   {
      DE_ERROR("No supported color converter from ",img.getFormatStr()," to BGR888")
      return false;
   }

   FILE * file = ::fopen( uri.c_str(), "wb" );
   if ( !file )
   {
      DE_ERROR("Cant open for write (",uri,")")
      return false;
   }

   // int32_t w = img.getWidth();
   // int32_t h = img.getHeight();

   image::bmp::BMPHeader header;
   size_t const headerBytes = sizeof( header ); // used more than once
   header.Id = 0x4d42;
   header.Reserved = 0;
   header.BitmapDataOffset = headerBytes;
   header.BitmapHeaderSize = 0x28;
   header.Width = uint32_t( img.getWidth() );
   header.Height = uint32_t( img.getHeight() );
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
   size_t resultBytes = ::fwrite( &header, 1, headerBytes, file );
   if ( resultBytes != headerBytes )
   {
      DE_ERROR("Cant write headerBytes(",headerBytes,")")
      ::fclose( file );
      return false;
   }

   uint8_t const * pixels = img.data();

   // allocate and clear memory for our scan line
   std::vector< uint8_t > scanline;
   size_t const scanlineBytes = img.getStride();
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
   for ( y = header.Height - 1; 0 <= y; --y )
   {
      if ( img.getFormat() == ColorFormat::RGB888 )
      {
         ColorConverter::convert24BitTo24Bit( &pixels[ y * scanlineBytes ], scanline.data(), header.Width, 1, 0, false, true );
      }
      else
      {
         // source, length [pixels], destination
         converter( &pixels[ y * scanlineBytes ], scanline.data(), header.Width );
      }

      resultBytes = ::fwrite( scanline.data(), 1, row_size, file );
      if ( resultBytes < size_t( row_size ) )
      {
         //std::cout << "[Warn] JPG." << __FUNCTION__ << " :: Exited loop.\n";
         break;
      }
   }

   ::fclose( file );
   bool ok = y < 0;
   return ok;
}

} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_WRITER_BMP_ENABLED
