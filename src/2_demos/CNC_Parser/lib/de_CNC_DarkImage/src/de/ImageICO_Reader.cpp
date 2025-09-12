#include "ImageICO_Reader.hpp"

#ifdef DE_IMAGE_READER_ICO_ENABLED

#include "ImagePNG_Reader.hpp"
#include "ImageBMP_Reader.hpp"
#include <cstdio>
#include <de/FileMagic.hpp>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

#include <DarkImage.hpp>

namespace de {
namespace image {

size_t const IcoRgbCompression = 0;
int const MaxIcons = 1024;

struct IcoEntry
{
   uint8_t width;  // 0 means 256px
   uint8_t height;
   uint8_t colors;
   uint8_t reserved;
   uint16_t planes;
   uint16_t bits_per_pixel;
   uint32_t size;
   uint32_t offset;
};

struct IcoFile
{
   uint16_t reserved;
   uint16_t resource_type;
   uint16_t count;
   std::vector< IcoEntry > entries; //[ MaxIcons];
};

struct IcoInfo
{
   uint32_t file_size;
   uint32_t ba_offset;
   uint32_t offset_bits;
   uint32_t size;
   int32_t width;
   int32_t height;
   uint16_t planes;
   uint16_t bits_per_pixel;
   uint32_t compression;
   uint32_t image_size;
   uint32_t x_pixels;
   uint32_t y_pixels;
   uint32_t number_colors;
   uint32_t red_mask;
   uint32_t green_mask;
   uint32_t blue_mask;
   uint32_t alpha_mask;
   uint32_t colors_important;
   int32_t colorspace;
};

bool
ImageReaderICO::load( Image & img, std::string const & uri )
{
   Binary file( uri );
   if ( !file.is_open() )
   {
      DE_ERROR( "Cant open file stream ",uri )
      return false;
   }

   return load( img, file );
}

bool
ImageReaderICO::load( Image & img, Binary & file )
{
   std::string const & uri = file.getUri();
   if ( !file.is_open() )
   {
      DE_ERROR( "Cant open binary file(",uri,").")
      return false;
   }

   img.setUri( file.getUri() );

   // ICONDIR
   IcoFile icoFile;
   icoFile.reserved = file.readU16_lsb();
   icoFile.resource_type = file.readU16_lsb();
   icoFile.count = file.readU16_lsb();

   if ( (icoFile.reserved != 0)  // Must be zero
      || ((icoFile.resource_type != 1) && // 1 == ICO
          (icoFile.resource_type != 2))   // 2 == CUR
      || (icoFile.count > MaxIcons) )     // Max 1024 images per file?
   {
      DE_ERROR("ImproperIcoImageHeader")
      return false;
   }

   DE_INFO("IcoFile: ", uri)
   DE_INFO("reserved = ", int(icoFile.reserved))
   DE_INFO("resource_type = ", int(icoFile.resource_type))
   DE_INFO("count = ", int(icoFile.count))
#if defined(WIN32) || defined(_WIN32)
   DE_INFO("BITMAPINFOHEADER = ", sizeof(BITMAPINFOHEADER))
#else
   DE_INFO("BITMAPINFOHEADER = 40")
#endif
   //DE_INFO("tell = ", file.tell())

   // ICONDIRENTRY
   size_t extent = 0;
   for ( int i = 0; i < icoFile.count; i++ )
   {
      IcoEntry entry;
      entry.width = file.readU8();
      entry.height = file.readU8();
      entry.colors = file.readU8();
      entry.reserved = file.readU8();
      entry.planes = file.readU16_lsb();  // Hotspot-X for CUR
      entry.bits_per_pixel = file.readU16_lsb();  // Hotspot-Y for CUR
      entry.size = file.readU32_lsb();    // in Bytes
      entry.offset = file.readU32_lsb();  // in Bytes

      extent = std::max( extent, size_t( entry.size ) );
      int w = (entry.width == 0) ? 256 : entry.width;
      int h = (entry.height == 0) ? 256 : entry.height;

      DE_INFO(
         "IcoEntry[",i,"]: "
         "w(", w, "), "
         "h(", h, "), "
         "colors(", int(entry.colors), "), "
         "reserved(", int(entry.reserved), "), "
         "size(", entry.size, "), "
         "offset(", entry.offset, "), "
         "planes(", entry.planes, "), "
         "bpp(", entry.bits_per_pixel, "), "
         "extent(", extent, ")"
      )

      // DE_INFO("tell = ", file.tell())

      icoFile.entries.emplace_back( std::move( entry ) );
   }

   //size_t dataStartOffset = file.tell();

   for ( size_t i = 0; i < size_t( icoFile.count ); i++ )
   {
      auto & entry = icoFile.entries[ i ];
      /*
         Verify Icon identifier.
      */
      if ( !file.seek( entry.offset ) )
      {
         DE_ERROR("Seek failed for ico ",i)
         continue;
      }
      //offset=(ssize_t) SeekBlob(image,(MagickOffsetType)icon_file.directory[i].offset,SEEK_SET);
      // if (offset < 0) ThrowReaderException(CorruptImageError,"ImproperImageHeader");
      //DE_INFO("tell = ", file.tell())

//      IcoInfo icoInfo;
//      icoInfo.size = file.readU32_lsb();
//      icoInfo.width = file.readU32_lsb();
//      icoInfo.height = file.readU32_lsb() / 2;
//      icoInfo.planes = file.readU16_lsb();
//      icoInfo.bits_per_pixel = file.readU16_lsb();

      bool isPNG = getFileMagic( file.data() + entry.offset ) == EFT_PNG;

//      if ( ((icoInfo.planes == 18505) &&
//            (icoInfo.bits_per_pixel == 21060))
//           || (icoInfo.size == 0x474e5089) )
//      {
//         isPNG = true;
//      }

//      DE_INFO(
//         "IcoInfo[",i,"] "
//         "size(", icoInfo.size, "), "
//         "w(", icoInfo.width, "), "
//         "h(", icoInfo.height, "), "
//         "planes(", icoInfo.planes, "), "
//         "bpp(", icoInfo.bits_per_pixel, ")"
//         ", ", isPNG ? "PNG (compressed)" : "BMP (raw)"
//      )

      DE_INFO(
         "IcoInfo[",i,"] ",
         isPNG ? "PNG (compressed)" : "BMP (raw)"
      )
//      if (EOFBlob(image) != MagickFalse)
//      {
//        ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
//          image->filename);
//        break;
//      }
   // Icon image encoded as a compressed PNG image.
      if ( isPNG )
      {
         int w = (entry.width == 0) ? 256 : entry.width;
         int h = (entry.height == 0) ? 256 : entry.height;
         int b = entry.bits_per_pixel;
         std::string urii = dbStrJoin( uri, "_entry",i,"_", w, "x", h, "_", b, ".png" );

         Binary bin = Binary::fromRAM( file.data() + entry.offset, entry.size, urii);

         if ( bin.save( urii ) )
         {
            DE_INFO("Saved png ", urii)
         }
         else
         {
            DE_ERROR("Cant save png ", urii)
         }
      }
      else
      {
         //DE_INFO("Icon[",i,"] image encoded as Bitmap image.")

         int w = (entry.width == 0) ? 256 : entry.width;
         int h = (entry.height == 0) ? 256 : entry.height;
         int bpp = entry.bits_per_pixel;

         //int pad = ((((w*bpp)+31) & ~31) - (w*bpp)) >> 3;
         //int pad = ((w * bpp + 31) / 32) * 4;
         int pad = 0;
         std::string urii = dbStrJoin( uri, "_entry",i,"_", w, "x", h, "_", bpp, ".tga" );

         Image bmp( w,h ); // Default constructed image format is R8G8B8A8
         if ( bpp == 32 )
         {
            DE_DEBUG( "ExtractBMP w(",w,"), h(",h,"), bpp(",bpp,"), pad(",pad,")")

#if defined(WIN32) || defined(_WIN32)
            uint8_t const* p = file.data() + entry.offset + sizeof(BITMAPINFOHEADER); // Benni correct offset 40
#else
            uint8_t const* p = file.data() + entry.offset + 40; // Benni correct offset 40
#endif


            size_t n = 0;
//            for ( int y = h - 1; y >= 0; y-- )
//            {
            for ( int y = 0; y < h; y++ )
            {

               for ( int x = 0; x < w; x++ )
               {
                  uint8_t b = *p++;
                  uint8_t g = *p++;
                  uint8_t r = *p++;
                  uint8_t a = *p++;
                  bmp.setPixel( x,h-1-y, RGBA( r,g,b,a ) );
                  n++;

               }

               for ( int x = 0; x < pad; x++ )
               {
                  p++;
               }
            }

            DE_DEBUG( "n(",n,") loops, bmp(",bmp.toString(false),")")

            dbSaveImage( bmp, urii );
         }
         else // if ( b == 32 )
         {
            DE_ERROR( "Only 32 bit embedded *.bmp supported, yet")
         }
      }

/*
        if (icon_info.bits_per_pixel > 32)
          ThrowReaderException(CorruptImageError,"ImproperImageHeader");
        icon_info.compression=ReadBlobLSBLong(image);
        icon_info.image_size=ReadBlobLSBLong(image);
        icon_info.x_pixels=ReadBlobLSBLong(image);
        icon_info.y_pixels=ReadBlobLSBLong(image);
        icon_info.number_colors=ReadBlobLSBLong(image);
        if (icon_info.number_colors > GetBlobSize(image))
          ThrowReaderException(CorruptImageError,"InsufficientImageDataInFile");
        icon_info.colors_important=ReadBlobLSBLong(image);
        image->alpha_trait=BlendPixelTrait;
        image->columns=(size_t) icon_file.directory[i].width;
        if ((ssize_t) image->columns > icon_info.width)
          image->columns=(size_t) icon_info.width;
        if (image->columns == 0)
          image->columns=256;
        image->rows=(size_t) icon_file.directory[i].height;
        if ((ssize_t) image->rows > icon_info.height)
          image->rows=(size_t) icon_info.height;
        if (image->rows == 0)
          image->rows=256;
        image->depth=icon_info.bits_per_pixel;
        if (image->depth > 16)
          image->depth=8;
        if (image->debug != MagickFalse)
          {
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              " scene    = %.20g",(double) i);
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "   size   = %.20g",(double) icon_info.size);
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "   width  = %.20g",(double) icon_file.directory[i].width);
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "   height = %.20g",(double) icon_file.directory[i].height);
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "   colors = %.20g",(double ) icon_info.number_colors);
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "   planes = %.20g",(double) icon_info.planes);
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "   bpp    = %.20g",(double) icon_info.bits_per_pixel);
          }
      if ((icon_info.number_colors != 0) || (icon_info.bits_per_pixel <= 16U))
        {
          image->storage_class=PseudoClass;
          image->colors=icon_info.number_colors;
          if ((image->colors == 0) || (image->colors > 256))
            image->colors=one << icon_info.bits_per_pixel;
        }
      if (image->storage_class == PseudoClass)
        {
          ssize_t
            j;

          unsigned char
            *icon_colormap;

          // Read Icon raster colormap.
          if (image->colors > GetBlobSize(image))
            ThrowReaderException(CorruptImageError,
              "InsufficientImageDataInFile");
          if (AcquireImageColormap(image,image->colors,exception) ==
              MagickFalse)
            ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
          icon_colormap=(unsigned char *) AcquireQuantumMemory((size_t)
            image->colors,4UL*sizeof(*icon_colormap));
          if (icon_colormap == (unsigned char *) NULL)
            ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
          count=ReadBlob(image,(size_t) (4*image->colors),icon_colormap);
          if (count != (ssize_t) (4*image->colors))
            {
              icon_colormap=(unsigned char *) RelinquishMagickMemory(
                icon_colormap);
              ThrowReaderException(CorruptImageError,
                "InsufficientImageDataInFile");
            }
          p=icon_colormap;
          for (j=0; j < (ssize_t) image->colors; j++)
          {
            image->colormap[j].blue=(Quantum) ScaleCharToQuantum(*p++);
            image->colormap[j].green=(Quantum) ScaleCharToQuantum(*p++);
            image->colormap[j].red=(Quantum) ScaleCharToQuantum(*p++);
            p++;
          }
          icon_colormap=(unsigned char *) RelinquishMagickMemory(icon_colormap);
        }
        // Convert Icon raster image to pixel packets.
        if ((image_info->ping != MagickFalse) &&
            (image_info->number_scenes != 0))
          if (image->scene >= (image_info->scene+image_info->number_scenes-1))
            break;
        status=SetImageExtent(image,image->columns,image->rows,exception);
        if (status == MagickFalse)
          return(DestroyImageList(image));
        bytes_per_line=(((image->columns*icon_info.bits_per_pixel)+31) &
          ~31) >> 3;
        (void) bytes_per_line;
        scanline_pad=((((image->columns*icon_info.bits_per_pixel)+31) & ~31)-
          (image->columns*icon_info.bits_per_pixel)) >> 3;
        switch (icon_info.bits_per_pixel)
        {
          case 1:
          {
            // Convert bitmap scanline.
            for (y=(ssize_t) image->rows-1; y >= 0; y--)
            {
              q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
              if (q == (Quantum *) NULL)
                break;
              for (x=0; x < (ssize_t) (image->columns-7); x+=8)
              {
                byte=(size_t) ReadBlobByte(image);
                for (bit=0; bit < 8; bit++)
                {
                  SetPixelIndex(image,((byte & (0x80 >> bit)) != 0 ? 0x01 :
                    0x00),q);
                  q+=GetPixelChannels(image);
                }
              }
              if ((image->columns % 8) != 0)
                {
                  byte=(size_t) ReadBlobByte(image);
                  for (bit=0; bit < (image->columns % 8); bit++)
                  {
                    SetPixelIndex(image,((byte & (0x80 >> bit)) != 0 ? 0x01 :
                      0x00),q);
                    q+=GetPixelChannels(image);
                  }
                }
              for (x=0; x < (ssize_t) scanline_pad; x++)
                (void) ReadBlobByte(image);
              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
              if (image->previous == (Image *) NULL)
                {
                  status=SetImageProgress(image,LoadImageTag,image->rows-y-1,
                    image->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
            break;
          }
          case 4:
          {
            // Read 4-bit Icon scanline.
            for (y=(ssize_t) image->rows-1; y >= 0; y--)
            {
              q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
              if (q == (Quantum *) NULL)
                break;
              for (x=0; x < ((ssize_t) image->columns-1); x+=2)
              {
                byte=(size_t) ReadBlobByte(image);
                SetPixelIndex(image,((byte >> 4) & 0xf),q);
                q+=GetPixelChannels(image);
                SetPixelIndex(image,((byte) & 0xf),q);
                q+=GetPixelChannels(image);
              }
              if ((image->columns % 2) != 0)
                {
                  byte=(size_t) ReadBlobByte(image);
                  SetPixelIndex(image,((byte >> 4) & 0xf),q);
                  q+=GetPixelChannels(image);
                }
              for (x=0; x < (ssize_t) scanline_pad; x++)
                (void) ReadBlobByte(image);
              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
              if (image->previous == (Image *) NULL)
                {
                  status=SetImageProgress(image,LoadImageTag,image->rows-y-1,
                    image->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
            break;
          }
          case 8:
          {
            // Convert PseudoColor scanline.
            for (y=(ssize_t) image->rows-1; y >= 0; y--)
            {
              q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
              if (q == (Quantum *) NULL)
                break;
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                byte=(size_t) ReadBlobByte(image);
                SetPixelIndex(image,(Quantum) byte,q);
                q+=GetPixelChannels(image);
              }
              for (x=0; x < (ssize_t) scanline_pad; x++)
                (void) ReadBlobByte(image);
              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
              if (image->previous == (Image *) NULL)
                {
                  status=SetImageProgress(image,LoadImageTag,image->rows-y-1,
                    image->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
            break;
          }
          case 16:
          {
            // Convert PseudoColor scanline.
            for (y=(ssize_t) image->rows-1; y >= 0; y--)
            {
              q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
              if (q == (Quantum *) NULL)
                break;
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                byte=(size_t) ReadBlobByte(image);
                byte|=((size_t) ReadBlobByte(image) << 8);
                SetPixelIndex(image,(Quantum) byte,q);
                q+=GetPixelChannels(image);
              }
              for (x=0; x < (ssize_t) scanline_pad; x++)
                (void) ReadBlobByte(image);
              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
              if (image->previous == (Image *) NULL)
                {
                  status=SetImageProgress(image,LoadImageTag,image->rows-y-1,
                    image->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
            break;
          }
          case 24:
          case 32:
          {
            // Convert DirectColor scanline.
            for (y=(ssize_t) image->rows-1; y >= 0; y--)
            {
              q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
              if (q == (Quantum *) NULL)
                break;
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                SetPixelBlue(image,ScaleCharToQuantum((unsigned char)
                  ReadBlobByte(image)),q);
                SetPixelGreen(image,ScaleCharToQuantum((unsigned char)
                  ReadBlobByte(image)),q);
                SetPixelRed(image,ScaleCharToQuantum((unsigned char)
                  ReadBlobByte(image)),q);
                if (icon_info.bits_per_pixel == 32)
                  SetPixelAlpha(image,ScaleCharToQuantum((unsigned char)
                    ReadBlobByte(image)),q);
                q+=GetPixelChannels(image);
              }
              if (icon_info.bits_per_pixel == 24)
                for (x=0; x < (ssize_t) scanline_pad; x++)
                  (void) ReadBlobByte(image);
              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
              if (image->previous == (Image *) NULL)
                {
                  status=SetImageProgress(image,LoadImageTag,image->rows-y-1,
                    image->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
            break;
          }
          default:
            ThrowReaderException(CorruptImageError,"ImproperImageHeader");
        }
        if ((image_info->ping == MagickFalse) &&
            (icon_info.bits_per_pixel <= 16))
          (void) SyncImage(image,exception);
        if (icon_info.bits_per_pixel != 32)
          {
            // Read the ICON alpha mask.
            image->storage_class=DirectClass;
            for (y=(ssize_t) image->rows-1; y >= 0; y--)
            {
              q=GetAuthenticPixels(image,0,y,image->columns,1,exception);
              if (q == (Quantum *) NULL)
                break;
              for (x=0; x < ((ssize_t) image->columns-7); x+=8)
              {
                byte=(size_t) ReadBlobByte(image);
                for (bit=0; bit < 8; bit++)
                {
                  SetPixelAlpha(image,(((byte & (0x80 >> bit)) != 0) ?
                    TransparentAlpha : OpaqueAlpha),q);
                  q+=GetPixelChannels(image);
                }
              }
              if ((image->columns % 8) != 0)
                {
                  byte=(size_t) ReadBlobByte(image);
                  for (bit=0; bit < (image->columns % 8); bit++)
                  {
                    SetPixelAlpha(image,(((byte & (0x80 >> bit)) != 0) ?
                      TransparentAlpha : OpaqueAlpha),q);
                    q+=GetPixelChannels(image);
                  }
                }
              if ((image->columns % 32) != 0)
                for (x=0; x < (ssize_t) ((32-(image->columns % 32))/8); x++)
                  (void) ReadBlobByte(image);
              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
            }
          }
        if (EOFBlob(image) != MagickFalse)
          {
            ThrowFileException(exception,CorruptImageError,
              "UnexpectedEndOfFile",image->filename);
            break;
          }
      }
    // Proceed to next image.
    if (image_info->number_scenes != 0)
      if (image->scene >= (image_info->scene+image_info->number_scenes-1))
        break;
    if (i < (ssize_t) (icon_file.count-1))
      {
        // Allocate next image structure.
        AcquireNextImage(image_info,image,exception);
        if (GetNextImageInList(image) == (Image *) NULL)
          {
            status=MagickFalse;
            break;
          }
        image=SyncNextImageInList(image);
        status=SetImageProgress(image,LoadImagesTag,TellBlob(image),
          GetBlobSize(image));
        if (status == MagickFalse)
          break;
      }
  }

  (void) CloseBlob(image);
  if (status == MagickFalse)
    return(DestroyImageList(image));
  return(GetFirstImageInList(image));
*/
   } // end-for


   // Create the image structure to be filled by png data
/*
   if ( srcFmt == PNG_COLOR_TYPE_RGB_ALPHA )
   {
      srcType = ColorFormat::RGBA8888;
   }
   else
   {
      srcType = ColorFormat::RGB888;
   }

   ColorConverter::Converter_t converter =
   ColorConverter::getConverter( srcType, dstType );
   if ( !converter )
   {
      DE_ERROR("No color converter found from (", srcType," to (", dstType, ")" )
      return false;
   }
*/

   ColorFormat srcType = ColorFormat::RGBA8888;
   ColorFormat dstType = ColorFormat::RGBA8888;
   int w = 10;
   int h = 10;
   Image tmp( w, h, srcType );
   img.clear();
   img.setFormat( dstType );
   img.resize( w, h );

   // Create array of pointers to rows in image data
//   std::vector< uint8_t* > rows;
//   rows.reserve( h );
//   for ( uint32_t i = 0; i < h; ++i )
//   {
//      rows.emplace_back( tmp.getRow( i ) );
//   }

   file.close();

//   for ( int y = 0; y < tmp.getHeight(); ++y )
//   {
//      converter( tmp.getRow( y ), img.getRow( y ), img.getWidth() );
//   }


   return true;
}

#if 0
/*
  Define declarations.
*/
#define IconRgbCompression (size_t) 0
#define MaxIcons  1024

/*
  Typedef declarations.
*/
typedef struct _IconEntry
{
  unsigned char
    width,
    height,
    colors,
    reserved;

  unsigned short int
    planes,
    bits_per_pixel;

  size_t
    size,
    offset;
} IconEntry;

typedef struct _IconFile
{
  short
    reserved,
    resource_type,
    count;

  IconEntry
    directory[MaxIcons];
} IconFile;

typedef struct _IconInfo
{
  size_t
    file_size,
    ba_offset,
    offset_bits,
    size;

  ssize_t
    width,
    height;

  unsigned short
    planes,
    bits_per_pixel;

  size_t
    compression,
    image_size,
    x_pixels,
    y_pixels,
    number_colors,
    red_mask,
    green_mask,
    blue_mask,
    alpha_mask,
    colors_important;

  ssize_t
    colorspace;
} IconInfo;

/*
  Forward declaractions.
*/
static MagickBooleanType
  WriteICONImage(const ImageInfo *,Image *,ExceptionInfo *);

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e a d I C O N I m a g e                                                 %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  ReadICONImage() reads a Microsoft icon image file and returns it.  It
%  allocates the memory necessary for the new Image structure and returns a
%  pointer to the new image.
%
%  The format of the ReadICONImage method is:
%
%      Image *ReadICONImage(const ImageInfo *image_info,
%        ExceptionInfo *exception)
%
%  A description of each parameter follows:
%
%    o image_info: the image info.
%
%    o exception: return any errors or warnings in this structure.
%
*/
static Image *ReadICONImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
  IconFile
    icon_file = { 0 };

  IconInfo
    icon_info;

  Image
    *image;

  MagickBooleanType
    status;

  MagickSizeType
    extent;

  ssize_t
    i,
    x;

  Quantum
    *q;

  unsigned char
    *p;

  size_t
    bit,
    byte,
    bytes_per_line,
    one,
    scanline_pad;

  ssize_t
    count,
    offset,
    y;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  (void) LogMagickEvent(CoderEvent,GetMagickModule(),"%s",image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  image=AcquireImage(image_info,exception);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  icon_file.reserved=(short) ReadBlobLSBShort(image);
  icon_file.resource_type=(short) ReadBlobLSBShort(image);
  icon_file.count=(short) ReadBlobLSBShort(image);
  if ((icon_file.reserved != 0) ||
      ((icon_file.resource_type != 1) && (icon_file.resource_type != 2)) ||
      (icon_file.count > MaxIcons))
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  extent=0;
  for (i=0; i < icon_file.count; i++)
  {
    icon_file.directory[i].width=(unsigned char) ReadBlobByte(image);
    icon_file.directory[i].height=(unsigned char) ReadBlobByte(image);
    icon_file.directory[i].colors=(unsigned char) ReadBlobByte(image);
    icon_file.directory[i].reserved=(unsigned char) ReadBlobByte(image);
    icon_file.directory[i].planes=(unsigned short) ReadBlobLSBShort(image);
    icon_file.directory[i].bits_per_pixel=(unsigned short)
      ReadBlobLSBShort(image);
    icon_file.directory[i].size=ReadBlobLSBLong(image);
    icon_file.directory[i].offset=ReadBlobLSBLong(image);
    if (EOFBlob(image) != MagickFalse)
      break;
    extent=MagickMax(extent,icon_file.directory[i].size);
  }
  if ((EOFBlob(image) != MagickFalse) || (extent > GetBlobSize(image)))
    ThrowReaderException(CorruptImageError,"UnexpectedEndOfFile");
  one=1;
  for (i=0; i < icon_file.count; i++)
  {
    /*
      Verify Icon identifier.
    */
    offset=(ssize_t) SeekBlob(image,(MagickOffsetType)
      icon_file.directory[i].offset,SEEK_SET);
    if (offset < 0)
      ThrowReaderException(CorruptImageError,"ImproperImageHeader");
    icon_info.size=ReadBlobLSBLong(image);
    icon_info.width=(unsigned char) ReadBlobLSBSignedLong(image);
    icon_info.height=(unsigned char) (ReadBlobLSBSignedLong(image)/2);
    icon_info.planes=ReadBlobLSBShort(image);
    icon_info.bits_per_pixel=ReadBlobLSBShort(image);
    if (EOFBlob(image) != MagickFalse)
      {
        ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
          image->filename);
        break;
      }
    if (((icon_info.planes == 18505) && (icon_info.bits_per_pixel == 21060)) ||
        (icon_info.size == 0x474e5089))
      {
        Image
          *icon_image;

        ImageInfo
          *read_info;

        size_t
          length;

        unsigned char
          *png;

        /*
          Icon image encoded as a compressed PNG image.
        */
        length=icon_file.directory[i].size;
        if ((length < 16) || (~length < 16))
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
        png=(unsigned char *) AcquireQuantumMemory(length,sizeof(*png));
        if (png == (unsigned char *) NULL)
          ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
        (void) memcpy(png,"\211PNG\r\n\032\n\000\000\000\015",12);
        png[12]=(unsigned char) icon_info.planes;
        png[13]=(unsigned char) (icon_info.planes >> 8);
        png[14]=(unsigned char) icon_info.bits_per_pixel;
        png[15]=(unsigned char) (icon_info.bits_per_pixel >> 8);
        count=ReadBlob(image,length-16,png+16);
        if (count != (ssize_t) (length-16))
          {
            png=(unsigned char *) RelinquishMagickMemory(png);
            ThrowReaderException(CorruptImageError,
                "InsufficientImageDataInFile");
          }
        read_info=CloneImageInfo(image_info);
        (void) CopyMagickString(read_info->magick,"PNG",MagickPathExtent);
        icon_image=BlobToImage(read_info,png,length,exception);
        read_info=DestroyImageInfo(read_info);
        png=(unsigned char *) RelinquishMagickMemory(png);
        if (icon_image == (Image *) NULL)
          return(DestroyImageList(image));
        DestroyBlob(icon_image);
        icon_image->blob=ReferenceBlob(image->blob);
        ReplaceImageInList(&image,icon_image);
      }
    else
      {
        if (icon_info.bits_per_pixel > 32)
          ThrowReaderException(CorruptImageError,"ImproperImageHeader");
        icon_info.compression=ReadBlobLSBLong(image);
        icon_info.image_size=ReadBlobLSBLong(image);
        icon_info.x_pixels=ReadBlobLSBLong(image);
        icon_info.y_pixels=ReadBlobLSBLong(image);
        icon_info.number_colors=ReadBlobLSBLong(image);
        if (icon_info.number_colors > GetBlobSize(image))
          ThrowReaderException(CorruptImageError,"InsufficientImageDataInFile");
        icon_info.colors_important=ReadBlobLSBLong(image);
        image->alpha_trait=BlendPixelTrait;
        image->columns=(size_t) icon_file.directory[i].width;
        if ((ssize_t) image->columns > icon_info.width)
          image->columns=(size_t) icon_info.width;
        if (image->columns == 0)
          image->columns=256;
        image->rows=(size_t) icon_file.directory[i].height;
        if ((ssize_t) image->rows > icon_info.height)
          image->rows=(size_t) icon_info.height;
        if (image->rows == 0)
          image->rows=256;
        image->depth=icon_info.bits_per_pixel;
        if (image->depth > 16)
          image->depth=8;
        if (image->debug != MagickFalse)
          {
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              " scene    = %.20g",(double) i);
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "   size   = %.20g",(double) icon_info.size);
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "   width  = %.20g",(double) icon_file.directory[i].width);
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "   height = %.20g",(double) icon_file.directory[i].height);
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "   colors = %.20g",(double ) icon_info.number_colors);
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "   planes = %.20g",(double) icon_info.planes);
            (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "   bpp    = %.20g",(double) icon_info.bits_per_pixel);
          }
      if ((icon_info.number_colors != 0) || (icon_info.bits_per_pixel <= 16U))
        {
          image->storage_class=PseudoClass;
          image->colors=icon_info.number_colors;
          if ((image->colors == 0) || (image->colors > 256))
            image->colors=one << icon_info.bits_per_pixel;
        }
      if (image->storage_class == PseudoClass)
        {
          ssize_t
            j;

          unsigned char
            *icon_colormap;

          /*
            Read Icon raster colormap.
          */
          if (image->colors > GetBlobSize(image))
            ThrowReaderException(CorruptImageError,
              "InsufficientImageDataInFile");
          if (AcquireImageColormap(image,image->colors,exception) ==
              MagickFalse)
            ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
          icon_colormap=(unsigned char *) AcquireQuantumMemory((size_t)
            image->colors,4UL*sizeof(*icon_colormap));
          if (icon_colormap == (unsigned char *) NULL)
            ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
          count=ReadBlob(image,(size_t) (4*image->colors),icon_colormap);
          if (count != (ssize_t) (4*image->colors))
            {
              icon_colormap=(unsigned char *) RelinquishMagickMemory(
                icon_colormap);
              ThrowReaderException(CorruptImageError,
                "InsufficientImageDataInFile");
            }
          p=icon_colormap;
          for (j=0; j < (ssize_t) image->colors; j++)
          {
            image->colormap[j].blue=(Quantum) ScaleCharToQuantum(*p++);
            image->colormap[j].green=(Quantum) ScaleCharToQuantum(*p++);
            image->colormap[j].red=(Quantum) ScaleCharToQuantum(*p++);
            p++;
          }
          icon_colormap=(unsigned char *) RelinquishMagickMemory(icon_colormap);
        }
        /*
          Convert Icon raster image to pixel packets.
        */
        if ((image_info->ping != MagickFalse) &&
            (image_info->number_scenes != 0))
          if (image->scene >= (image_info->scene+image_info->number_scenes-1))
            break;
        status=SetImageExtent(image,image->columns,image->rows,exception);
        if (status == MagickFalse)
          return(DestroyImageList(image));
        bytes_per_line=(((image->columns*icon_info.bits_per_pixel)+31) &
          ~31) >> 3;
        (void) bytes_per_line;
        scanline_pad=((((image->columns*icon_info.bits_per_pixel)+31) & ~31)-
          (image->columns*icon_info.bits_per_pixel)) >> 3;
        switch (icon_info.bits_per_pixel)
        {
          case 1:
          {
            /*
              Convert bitmap scanline.
            */
            for (y=(ssize_t) image->rows-1; y >= 0; y--)
            {
              q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
              if (q == (Quantum *) NULL)
                break;
              for (x=0; x < (ssize_t) (image->columns-7); x+=8)
              {
                byte=(size_t) ReadBlobByte(image);
                for (bit=0; bit < 8; bit++)
                {
                  SetPixelIndex(image,((byte & (0x80 >> bit)) != 0 ? 0x01 :
                    0x00),q);
                  q+=GetPixelChannels(image);
                }
              }
              if ((image->columns % 8) != 0)
                {
                  byte=(size_t) ReadBlobByte(image);
                  for (bit=0; bit < (image->columns % 8); bit++)
                  {
                    SetPixelIndex(image,((byte & (0x80 >> bit)) != 0 ? 0x01 :
                      0x00),q);
                    q+=GetPixelChannels(image);
                  }
                }
              for (x=0; x < (ssize_t) scanline_pad; x++)
                (void) ReadBlobByte(image);
              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
              if (image->previous == (Image *) NULL)
                {
                  status=SetImageProgress(image,LoadImageTag,image->rows-y-1,
                    image->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
            break;
          }
          case 4:
          {
            /*
              Read 4-bit Icon scanline.
            */
            for (y=(ssize_t) image->rows-1; y >= 0; y--)
            {
              q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
              if (q == (Quantum *) NULL)
                break;
              for (x=0; x < ((ssize_t) image->columns-1); x+=2)
              {
                byte=(size_t) ReadBlobByte(image);
                SetPixelIndex(image,((byte >> 4) & 0xf),q);
                q+=GetPixelChannels(image);
                SetPixelIndex(image,((byte) & 0xf),q);
                q+=GetPixelChannels(image);
              }
              if ((image->columns % 2) != 0)
                {
                  byte=(size_t) ReadBlobByte(image);
                  SetPixelIndex(image,((byte >> 4) & 0xf),q);
                  q+=GetPixelChannels(image);
                }
              for (x=0; x < (ssize_t) scanline_pad; x++)
                (void) ReadBlobByte(image);
              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
              if (image->previous == (Image *) NULL)
                {
                  status=SetImageProgress(image,LoadImageTag,image->rows-y-1,
                    image->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
            break;
          }
          case 8:
          {
            /*
              Convert PseudoColor scanline.
            */
            for (y=(ssize_t) image->rows-1; y >= 0; y--)
            {
              q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
              if (q == (Quantum *) NULL)
                break;
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                byte=(size_t) ReadBlobByte(image);
                SetPixelIndex(image,(Quantum) byte,q);
                q+=GetPixelChannels(image);
              }
              for (x=0; x < (ssize_t) scanline_pad; x++)
                (void) ReadBlobByte(image);
              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
              if (image->previous == (Image *) NULL)
                {
                  status=SetImageProgress(image,LoadImageTag,image->rows-y-1,
                    image->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
            break;
          }
          case 16:
          {
            /*
              Convert PseudoColor scanline.
            */
            for (y=(ssize_t) image->rows-1; y >= 0; y--)
            {
              q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
              if (q == (Quantum *) NULL)
                break;
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                byte=(size_t) ReadBlobByte(image);
                byte|=((size_t) ReadBlobByte(image) << 8);
                SetPixelIndex(image,(Quantum) byte,q);
                q+=GetPixelChannels(image);
              }
              for (x=0; x < (ssize_t) scanline_pad; x++)
                (void) ReadBlobByte(image);
              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
              if (image->previous == (Image *) NULL)
                {
                  status=SetImageProgress(image,LoadImageTag,image->rows-y-1,
                    image->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
            break;
          }
          case 24:
          case 32:
          {
            /*
              Convert DirectColor scanline.
            */
            for (y=(ssize_t) image->rows-1; y >= 0; y--)
            {
              q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
              if (q == (Quantum *) NULL)
                break;
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                SetPixelBlue(image,ScaleCharToQuantum((unsigned char)
                  ReadBlobByte(image)),q);
                SetPixelGreen(image,ScaleCharToQuantum((unsigned char)
                  ReadBlobByte(image)),q);
                SetPixelRed(image,ScaleCharToQuantum((unsigned char)
                  ReadBlobByte(image)),q);
                if (icon_info.bits_per_pixel == 32)
                  SetPixelAlpha(image,ScaleCharToQuantum((unsigned char)
                    ReadBlobByte(image)),q);
                q+=GetPixelChannels(image);
              }
              if (icon_info.bits_per_pixel == 24)
                for (x=0; x < (ssize_t) scanline_pad; x++)
                  (void) ReadBlobByte(image);
              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
              if (image->previous == (Image *) NULL)
                {
                  status=SetImageProgress(image,LoadImageTag,image->rows-y-1,
                    image->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
            break;
          }
          default:
            ThrowReaderException(CorruptImageError,"ImproperImageHeader");
        }
        if ((image_info->ping == MagickFalse) &&
            (icon_info.bits_per_pixel <= 16))
          (void) SyncImage(image,exception);
        if (icon_info.bits_per_pixel != 32)
          {
            /*
              Read the ICON alpha mask.
            */
            image->storage_class=DirectClass;
            for (y=(ssize_t) image->rows-1; y >= 0; y--)
            {
              q=GetAuthenticPixels(image,0,y,image->columns,1,exception);
              if (q == (Quantum *) NULL)
                break;
              for (x=0; x < ((ssize_t) image->columns-7); x+=8)
              {
                byte=(size_t) ReadBlobByte(image);
                for (bit=0; bit < 8; bit++)
                {
                  SetPixelAlpha(image,(((byte & (0x80 >> bit)) != 0) ?
                    TransparentAlpha : OpaqueAlpha),q);
                  q+=GetPixelChannels(image);
                }
              }
              if ((image->columns % 8) != 0)
                {
                  byte=(size_t) ReadBlobByte(image);
                  for (bit=0; bit < (image->columns % 8); bit++)
                  {
                    SetPixelAlpha(image,(((byte & (0x80 >> bit)) != 0) ?
                      TransparentAlpha : OpaqueAlpha),q);
                    q+=GetPixelChannels(image);
                  }
                }
              if ((image->columns % 32) != 0)
                for (x=0; x < (ssize_t) ((32-(image->columns % 32))/8); x++)
                  (void) ReadBlobByte(image);
              if (SyncAuthenticPixels(image,exception) == MagickFalse)
                break;
            }
          }
        if (EOFBlob(image) != MagickFalse)
          {
            ThrowFileException(exception,CorruptImageError,
              "UnexpectedEndOfFile",image->filename);
            break;
          }
      }
    /*
      Proceed to next image.
    */
    if (image_info->number_scenes != 0)
      if (image->scene >= (image_info->scene+image_info->number_scenes-1))
        break;
    if (i < (ssize_t) (icon_file.count-1))
      {
        /*
          Allocate next image structure.
        */
        AcquireNextImage(image_info,image,exception);
        if (GetNextImageInList(image) == (Image *) NULL)
          {
            status=MagickFalse;
            break;
          }
        image=SyncNextImageInList(image);
        status=SetImageProgress(image,LoadImagesTag,TellBlob(image),
          GetBlobSize(image));
        if (status == MagickFalse)
          break;
      }
  }
  (void) CloseBlob(image);
  if (status == MagickFalse)
    return(DestroyImageList(image));
  return(GetFirstImageInList(image));
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   R e g i s t e r I C O N I m a g e                                         %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  RegisterICONImage() adds attributes for the Icon image format to
%  the list of supported formats.  The attributes include the image format
%  tag, a method to read and/or write the format, whether the format
%  supports the saving of more than one frame to the same file or blob,
%  whether the format supports native in-memory I/O, and a brief
%  description of the format.
%
%  The format of the RegisterICONImage method is:
%
%      size_t RegisterICONImage(void)
%
*/
ModuleExport size_t RegisterICONImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("ICON","CUR","Microsoft icon");
  entry->decoder=(DecodeImageHandler *) ReadICONImage;
  entry->encoder=(EncodeImageHandler *) WriteICONImage;
  entry->flags^=CoderAdjoinFlag;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  entry->flags|=CoderEncoderSeekableStreamFlag;
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("ICON","ICO","Microsoft icon");
  entry->decoder=(DecodeImageHandler *) ReadICONImage;
  entry->encoder=(EncodeImageHandler *) WriteICONImage;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  entry->flags|=CoderEncoderSeekableStreamFlag;
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("ICON","ICON","Microsoft icon");
  entry->decoder=(DecodeImageHandler *) ReadICONImage;
  entry->encoder=(EncodeImageHandler *) WriteICONImage;
  entry->flags^=CoderAdjoinFlag;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  entry->flags|=CoderEncoderSeekableStreamFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   U n r e g i s t e r I C O N I m a g e                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  UnregisterICONImage() removes format registrations made by the
%  ICON module from the list of supported formats.
%
%  The format of the UnregisterICONImage method is:
%
%      UnregisterICONImage(void)
%
*/
ModuleExport void UnregisterICONImage(void)
{
  (void) UnregisterMagickInfo("CUR");
  (void) UnregisterMagickInfo("ICO");
  (void) UnregisterMagickInfo("ICON");
}

/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%   W r i t e I C O N I m a g e                                               %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  WriteICONImage() writes an image in Microsoft Windows bitmap encoded
%  image format, version 3 for Windows or (if the image has a matte channel)
%  version 4.
%
%  It encodes any subimage as a compressed PNG image ("BI_PNG)", only when its
%  dimensions are 256x256 and image->compression is undefined or is defined as
%  ZipCompression.
%
%  The format of the WriteICONImage method is:
%
%      MagickBooleanType WriteICONImage(const ImageInfo *image_info,
%        Image *image,ExceptionInfo *exception)
%
%  A description of each parameter follows.
%
%    o image_info: the image info.
%
%    o image:  The image.
%
%    o exception: return any errors or warnings in this structure.
%
*/

static Image *AutoResizeImage(const Image *image,const char *option,
  MagickOffsetType *count,ExceptionInfo *exception)
{
#define MAX_SIZES 16

  char
    *q;

  const char
    *p;

  Image
    *images,
    *resized;

  ssize_t
    i;

  size_t
    sizes[MAX_SIZES]={256, 192, 128, 96, 64, 48, 40, 32, 24, 16};

  images=NULL;
  *count=0;
  i=0;
  p=option;
  while ((*p != '\0') && (i < MAX_SIZES))
  {
    size_t
      size;

    while ((isspace((int) ((unsigned char) *p)) != 0))
      p++;
    size=(size_t) strtol(p,&q,10);
    if ((p == q) || (size < 16) || (size > 256))
      return((Image *) NULL);
    p=q;
    sizes[i++]=size;
    while ((isspace((int) ((unsigned char) *p)) != 0) || (*p == ','))
      p++;
  }
  if (i == 0)
    i=10;
  *count=i;
  for (i=0; i < *count; i++)
  {
    resized=ResizeImage(image,sizes[i],sizes[i],image->filter,exception);
    if (resized == (Image *) NULL)
      return(DestroyImageList(images));
    if (images == (Image *) NULL)
      images=resized;
    else
      AppendImageToList(&images,resized);
  }
  return(images);
}

static MagickBooleanType WriteICONImage(const ImageInfo *image_info,
  Image *image,ExceptionInfo *exception)
{
  const char
    *option;

  IconFile
    icon_file;

  IconInfo
    icon_info;

  Image
    *images,
    *next;

  MagickBooleanType
    adjoin,
    status;

  MagickOffsetType
    offset,
    scene;

  const Quantum
    *p;

  ssize_t
    i,
    x;

  unsigned char
    *q;

  size_t
    bytes_per_line,
    imageListLength,
    scanline_pad;

  ssize_t
    y;

  unsigned char
    *pixels;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,exception);
  if (status == MagickFalse)
    return(status);
  images=(Image *) NULL;
  adjoin=image_info->adjoin;
  option=GetImageOption(image_info,"icon:auto-resize");
  if (option != (const char *) NULL)
    {
      images=AutoResizeImage(image,option,&scene,exception);
      if (images == (Image *) NULL)
        ThrowWriterException(ImageError,"InvalidDimensions");
      adjoin=MagickTrue;
    }
  else
    {
      scene=0;
      next=image;
      do
      {
        if ((image->columns > 256L) || (image->rows > 256L))
          ThrowWriterException(ImageError,"WidthOrHeightExceedsLimit");
        scene++;
        next=SyncNextImageInList(next);
      } while ((next != (Image *) NULL) && (adjoin != MagickFalse));
    }
  /*
    Dump out a ICON header template to be properly initialized later.
  */
  (void) WriteBlobLSBShort(image,0);
  (void) WriteBlobLSBShort(image,1);
  (void) WriteBlobLSBShort(image,(unsigned char) scene);
  (void) memset(&icon_file,0,sizeof(icon_file));
  (void) memset(&icon_info,0,sizeof(icon_info));
  scene=0;
  next=(images != (Image *) NULL) ? images : image;
  do
  {
    (void) WriteBlobByte(image,icon_file.directory[scene].width);
    (void) WriteBlobByte(image,icon_file.directory[scene].height);
    (void) WriteBlobByte(image,icon_file.directory[scene].colors);
    (void) WriteBlobByte(image,icon_file.directory[scene].reserved);
    (void) WriteBlobLSBShort(image,icon_file.directory[scene].planes);
    (void) WriteBlobLSBShort(image,icon_file.directory[scene].bits_per_pixel);
    (void) WriteBlobLSBLong(image,(unsigned int)
      icon_file.directory[scene].size);
    (void) WriteBlobLSBLong(image,(unsigned int)
      icon_file.directory[scene].offset);
    scene++;
    next=SyncNextImageInList(next);
  } while ((next != (Image *) NULL) && (adjoin != MagickFalse));
  scene=0;
  next=(images != (Image *) NULL) ? images : image;
  imageListLength=GetImageListLength(image);
  do
  {
    if ((next->columns > 255L) && (next->rows > 255L) &&
        ((next->compression == UndefinedCompression) ||
        (next->compression == ZipCompression)))
      {
        Image
          *write_image;

        ImageInfo
          *write_info;

        size_t
          length;

        unsigned char
          *png;

        write_image=CloneImage(next,0,0,MagickTrue,exception);
        if (write_image == (Image *) NULL)
          {
            images=DestroyImageList(images);
            return(MagickFalse);
          }
        write_info=CloneImageInfo(image_info);
        (void) CopyMagickString(write_info->magick,"PNG",MagickPathExtent);
        length=0;
        /*
          Don't write any ancillary chunks except for gAMA.
        */
        (void) SetImageArtifact(write_image,"png:include-chunk","none,gama");
        /*
          Only write PNG32 formatted PNG (32-bit RGBA), 8 bits per channel.
        */
        (void) SetImageArtifact(write_image,"png:format","png32");
        (void) SetImageArtifact(write_image,"png:color-type","6");
        png=(unsigned char *) ImageToBlob(write_info,write_image,&length,
          exception);
        write_image=DestroyImageList(write_image);
        write_info=DestroyImageInfo(write_info);
        if (png == (unsigned char *) NULL)
          {
            images=DestroyImageList(images);
            return(MagickFalse);
          }
        icon_file.directory[scene].width=0;
        icon_file.directory[scene].height=0;
        icon_file.directory[scene].colors=0;
        icon_file.directory[scene].reserved=0;
        icon_file.directory[scene].planes=1;
        icon_file.directory[scene].bits_per_pixel=32;
        icon_file.directory[scene].size=(size_t) length;
        icon_file.directory[scene].offset=(size_t) TellBlob(image);
        (void) WriteBlob(image,(size_t) length,png);
        png=(unsigned char *) RelinquishMagickMemory(png);
      }
    else
      {
        /*
          Initialize ICON raster file header.
        */
        (void) TransformImageColorspace(next,sRGBColorspace,exception);
        icon_info.file_size=14+12+28;
        icon_info.offset_bits=icon_info.file_size;
        icon_info.compression=IconRgbCompression;
        if ((next->storage_class != DirectClass) && (next->colors > 256))
          (void) SetImageStorageClass(next,DirectClass,exception);
        if (next->storage_class == DirectClass)
          {
            /*
              Full color ICON raster.
            */
            icon_info.number_colors=0;
            icon_info.bits_per_pixel=32;
            icon_info.compression=IconRgbCompression;
          }
        else
          {
            size_t
              one;

            /*
              Colormapped ICON raster.
            */
            icon_info.bits_per_pixel=8;
            if (next->colors <= 16)
              icon_info.bits_per_pixel=4;
            if (next->colors <= 2)
              icon_info.bits_per_pixel=1;
            one=1;
            icon_info.number_colors=one << icon_info.bits_per_pixel;
            if (icon_info.number_colors < next->colors)
              {
                (void) SetImageStorageClass(next,DirectClass,exception);
                icon_info.number_colors=0;
                icon_info.bits_per_pixel=(unsigned short) 24;
                icon_info.compression=IconRgbCompression;
              }
            else
              {
                one=1;
                icon_info.file_size+=3*(one << icon_info.bits_per_pixel);
                icon_info.offset_bits+=3*(one << icon_info.bits_per_pixel);
                icon_info.file_size+=(one << icon_info.bits_per_pixel);
                icon_info.offset_bits+=(one << icon_info.bits_per_pixel);
              }
          }
        bytes_per_line=(((next->columns*icon_info.bits_per_pixel)+31) & ~31) >>
          3;
        icon_info.ba_offset=0;
        icon_info.width=(ssize_t) next->columns;
        icon_info.height=(ssize_t) next->rows;
        icon_info.planes=1;
        icon_info.image_size=bytes_per_line*next->rows;
        icon_info.size=40;
        icon_info.size+=(4*icon_info.number_colors);
        icon_info.size+=icon_info.image_size;
        icon_info.size+=(((icon_info.width+31) & ~31) >> 3)*icon_info.height;
        icon_info.file_size+=icon_info.image_size;
        icon_info.x_pixels=0;
        icon_info.y_pixels=0;
        switch (next->units)
        {
          case UndefinedResolution:
          case PixelsPerInchResolution:
          {
            icon_info.x_pixels=(size_t) (100.0*next->resolution.x/2.54);
            icon_info.y_pixels=(size_t) (100.0*next->resolution.y/2.54);
            break;
          }
          case PixelsPerCentimeterResolution:
          {
            icon_info.x_pixels=(size_t) (100.0*next->resolution.x);
            icon_info.y_pixels=(size_t) (100.0*next->resolution.y);
            break;
          }
        }
        icon_info.colors_important=icon_info.number_colors;
        /*
          Convert MIFF to ICON raster pixels.
        */
        pixels=(unsigned char *) AcquireQuantumMemory((size_t)
          icon_info.image_size,sizeof(*pixels));
        if (pixels == (unsigned char *) NULL)
          {
            images=DestroyImageList(images);
            ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
          }
        (void) memset(pixels,0,(size_t) icon_info.image_size);
        switch (icon_info.bits_per_pixel)
        {
          case 1:
          {
            size_t
              bit,
              byte;

            /*
              Convert PseudoClass image to a ICON monochrome image.
            */
            for (y=0; y < (ssize_t) next->rows; y++)
            {
              p=GetVirtualPixels(next,0,y,next->columns,1,exception);
              if (p == (const Quantum *) NULL)
                break;
              q=pixels+(next->rows-y-1)*bytes_per_line;
              bit=0;
              byte=0;
              for (x=0; x < (ssize_t) next->columns; x++)
              {
                byte<<=1;
                byte|=GetPixelIndex(next,p) != 0 ? 0x01 : 0x00;
                bit++;
                if (bit == 8)
                  {
                    *q++=(unsigned char) byte;
                    bit=0;
                    byte=0;
                  }
                p+=GetPixelChannels(next);
              }
              if (bit != 0)
                *q++=(unsigned char) (byte << (8-bit));
              if (next->previous == (Image *) NULL)
                {
                  status=SetImageProgress(next,SaveImageTag,y,next->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
            break;
          }
          case 4:
          {
            size_t
              nibble,
              byte;

            /*
              Convert PseudoClass image to a ICON monochrome image.
            */
            for (y=0; y < (ssize_t) next->rows; y++)
            {
              p=GetVirtualPixels(next,0,y,next->columns,1,exception);
              if (p == (const Quantum *) NULL)
                break;
              q=pixels+(next->rows-y-1)*bytes_per_line;
              nibble=0;
              byte=0;
              for (x=0; x < (ssize_t) next->columns; x++)
              {
                byte<<=4;
                byte|=((size_t) GetPixelIndex(next,p) & 0x0f);
                nibble++;
                if (nibble == 2)
                  {
                    *q++=(unsigned char) byte;
                    nibble=0;
                    byte=0;
                  }
                p+=GetPixelChannels(next);
              }
              if (nibble != 0)
                *q++=(unsigned char) (byte << 4);
              if (next->previous == (Image *) NULL)
                {
                  status=SetImageProgress(next,SaveImageTag,y,next->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
            break;
          }
          case 8:
          {
            /*
              Convert PseudoClass packet to ICON pixel.
            */
            for (y=0; y < (ssize_t) next->rows; y++)
            {
              p=GetVirtualPixels(next,0,y,next->columns,1,exception);
              if (p == (const Quantum *) NULL)
                break;
              q=pixels+(next->rows-y-1)*bytes_per_line;
              for (x=0; x < (ssize_t) next->columns; x++)
              {
                *q++=(unsigned char) GetPixelIndex(next,p);
                p+=GetPixelChannels(next);
              }
              if (next->previous == (Image *) NULL)
                {
                  status=SetImageProgress(next,SaveImageTag,y,next->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
            break;
          }
          case 24:
          case 32:
          {
            /*
              Convert DirectClass packet to ICON BGR888 or BGRA8888 pixel.
            */
            for (y=0; y < (ssize_t) next->rows; y++)
            {
              p=GetVirtualPixels(next,0,y,next->columns,1,exception);
              if (p == (const Quantum *) NULL)
                break;
              q=pixels+(next->rows-y-1)*bytes_per_line;
              for (x=0; x < (ssize_t) next->columns; x++)
              {
                *q++=ScaleQuantumToChar(GetPixelBlue(next,p));
                *q++=ScaleQuantumToChar(GetPixelGreen(next,p));
                *q++=ScaleQuantumToChar(GetPixelRed(next,p));
                if (next->alpha_trait == UndefinedPixelTrait)
                  *q++=ScaleQuantumToChar(QuantumRange);
                else
                  *q++=ScaleQuantumToChar(GetPixelAlpha(next,p));
                p+=GetPixelChannels(next);
              }
              if (icon_info.bits_per_pixel == 24)
                for (x=3L*(ssize_t) next->columns; x < (ssize_t) bytes_per_line; x++)
                  *q++=0x00;
              if (next->previous == (Image *) NULL)
                {
                  status=SetImageProgress(next,SaveImageTag,y,next->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
            break;
          }
        }
        /*
          Write 40-byte version 3+ bitmap header.
        */
        icon_file.directory[scene].width=(unsigned char) icon_info.width;
        icon_file.directory[scene].height=(unsigned char) icon_info.height;
        icon_file.directory[scene].colors=(unsigned char)
          icon_info.number_colors;
        icon_file.directory[scene].reserved=0;
        icon_file.directory[scene].planes=icon_info.planes;
        icon_file.directory[scene].bits_per_pixel=icon_info.bits_per_pixel;
        icon_file.directory[scene].size=icon_info.size;
        icon_file.directory[scene].offset=(size_t) TellBlob(image);
        (void) WriteBlobLSBLong(image,(unsigned int) 40);
        (void) WriteBlobLSBLong(image,(unsigned int) icon_info.width);
        (void) WriteBlobLSBLong(image,(unsigned int) icon_info.height*2);
        (void) WriteBlobLSBShort(image,icon_info.planes);
        (void) WriteBlobLSBShort(image,icon_info.bits_per_pixel);
        (void) WriteBlobLSBLong(image,(unsigned int) icon_info.compression);
        (void) WriteBlobLSBLong(image,(unsigned int) icon_info.image_size);
        (void) WriteBlobLSBLong(image,(unsigned int) icon_info.x_pixels);
        (void) WriteBlobLSBLong(image,(unsigned int) icon_info.y_pixels);
        (void) WriteBlobLSBLong(image,(unsigned int) icon_info.number_colors);
        (void) WriteBlobLSBLong(image,(unsigned int)
          icon_info.colors_important);
        if (next->storage_class == PseudoClass)
          {
            unsigned char
              *icon_colormap;

            /*
              Dump colormap to file.
            */
            icon_colormap=(unsigned char *) AcquireQuantumMemory((size_t)
              (1UL << icon_info.bits_per_pixel),4UL*sizeof(*icon_colormap));
            if (icon_colormap == (unsigned char *) NULL)
              {
                images=DestroyImageList(images);
                ThrowWriterException(ResourceLimitError,
                  "MemoryAllocationFailed");
              }
            q=icon_colormap;
            for (i=0; i < (ssize_t) next->colors; i++)
            {
              *q++=ScaleQuantumToChar(next->colormap[i].blue);
              *q++=ScaleQuantumToChar(next->colormap[i].green);
              *q++=ScaleQuantumToChar(next->colormap[i].red);
              *q++=(unsigned char) 0x0;
            }
            for ( ; i < (ssize_t) (1UL << icon_info.bits_per_pixel); i++)
            {
              *q++=(unsigned char) 0x00;
              *q++=(unsigned char) 0x00;
              *q++=(unsigned char) 0x00;
              *q++=(unsigned char) 0x00;
            }
            (void) WriteBlob(image,(size_t) (4UL*(1UL <<
              icon_info.bits_per_pixel)),icon_colormap);
            icon_colormap=(unsigned char *) RelinquishMagickMemory(
              icon_colormap);
          }
        (void) WriteBlob(image,(size_t) icon_info.image_size,pixels);
        pixels=(unsigned char *) RelinquishMagickMemory(pixels);
        /*
          Write matte mask.
        */
        scanline_pad=(((next->columns+31) & ~31)-next->columns) >> 3;
        for (y=((ssize_t) next->rows - 1); y >= 0; y--)
        {
          unsigned char
            bit,
            byte;

          p=GetVirtualPixels(next,0,y,next->columns,1,exception);
          if (p == (const Quantum *) NULL)
            break;
          bit=0;
          byte=0;
          for (x=0; x < (ssize_t) next->columns; x++)
          {
            byte<<=1;
            if ((next->alpha_trait != UndefinedPixelTrait) &&
                (GetPixelAlpha(next,p) == (Quantum) TransparentAlpha))
              byte|=0x01;
            bit++;
            if (bit == 8)
              {
                (void) WriteBlobByte(image,(unsigned char) byte);
                bit=0;
                byte=0;
              }
            p+=GetPixelChannels(next);
          }
          if (bit != 0)
            (void) WriteBlobByte(image,(unsigned char) (byte << (8-bit)));
          for (i=0; i < (ssize_t) scanline_pad; i++)
            (void) WriteBlobByte(image,(unsigned char) 0);
        }
      }
    if (GetNextImageInList(next) == (Image *) NULL)
      break;
    status=SetImageProgress(next,SaveImagesTag,scene++,imageListLength);
    if (status == MagickFalse)
      break;
    next=SyncNextImageInList(next);
  } while ((next != (Image *) NULL) && (adjoin != MagickFalse));
  offset=SeekBlob(image,0,SEEK_SET);
  (void) offset;
  (void) WriteBlobLSBShort(image,0);
  (void) WriteBlobLSBShort(image,1);
  (void) WriteBlobLSBShort(image,(unsigned short) (scene+1));
  scene=0;
  next=(images != (Image *) NULL) ? images : image;
  do
  {
    (void) WriteBlobByte(image,icon_file.directory[scene].width);
    (void) WriteBlobByte(image,icon_file.directory[scene].height);
    (void) WriteBlobByte(image,icon_file.directory[scene].colors);
    (void) WriteBlobByte(image,icon_file.directory[scene].reserved);
    (void) WriteBlobLSBShort(image,icon_file.directory[scene].planes);
    (void) WriteBlobLSBShort(image,icon_file.directory[scene].bits_per_pixel);
    (void) WriteBlobLSBLong(image,(unsigned int)
      icon_file.directory[scene].size);
    (void) WriteBlobLSBLong(image,(unsigned int)
      icon_file.directory[scene].offset);
    scene++;
    next=SyncNextImageInList(next);
  } while ((next != (Image *) NULL) && (adjoin != MagickFalse));
  (void) CloseBlob(image);
  images=DestroyImageList(images);
  return(MagickTrue);
}

#endif

} // end namespace image
} // end namespace de.

#endif // DE_IMAGE_READER_ICO_ENABLED
