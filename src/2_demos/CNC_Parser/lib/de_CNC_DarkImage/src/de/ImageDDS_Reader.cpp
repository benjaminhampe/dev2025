#include "ImageDDS_Reader.hpp"

#ifdef DE_IMAGE_READER_DDS_ENABLED

#include "ImageDDS.hpp"

namespace de {
namespace image {

bool
ImageReaderDDS::load( Image & img, std::string const & uri )
{
   if ( uri.empty() )
   {
      DE_ERROR("Empty uri")
      return false;
   }

   Binary file( uri );
   bool ok = load( img, file );
   if ( !ok )
   {
      DE_ERROR("Cant load")
   }
   return ok;
}

bool
ImageReaderDDS::load( Image & img, Binary & file )
{
   auto const & uri = file.m_Uri;
   if ( !file.is_open() )
   {
      DE_ERROR("Binary not open ", uri)
      return false;
   }

#if 0
   // Verify DDS filemagic
   char fourcc[4];
   file.read( fourcc, 4 );
   if ( ::strncmp( fourcc, "DDS ", 4 ) != 0 )
   {
      DE_ERROR("Invalid filemagic for DDS file. uri(", file.m_Uri,")")
      return false;
   }

   // get the surface desc
   uint8_t header[124]; // == 31 uint32_t
   file.read( &header, 124 );
   uint32_t height      = *(uint32_t*)&(header[8 ]);
   uint32_t width       = *(uint32_t*)&(header[12]);
   uint32_t linearSize	= *(uint32_t*)&(header[16]);
   uint32_t mipMapCount = *(uint32_t*)&(header[24]);
   uint32_t fourCC      = *(uint32_t*)&(header[80]);
   // how big is it going to be including all mipmaps?
   uint32_t bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;

   std::vector< uint8_t > dataBytes( bufsize, 0x00 );
   file.read( dataBytes.data(), dataBytes.size() );

   // We got all data
   file.close();

   // Process data
   uint32_t channelCount  = (fourCC == FOURCC_DXT1) ? 3 : 4;
   uint32_t format = 0;

   switch( fourCC )
   {
      case FOURCC_DXT1: format = 1; break; // GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; break;
      case FOURCC_DXT3: format = 3; break; // GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; break;
      case FOURCC_DXT5: format = 5; break; // GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; break;
      default: break; //return false;
   }

   ColorConverter::Converter_t converter =
      ColorConverter::getConverter( img.getFormat(), ColorFormat::RGB888 );


#endif

   dds::ddsBuffer* header = (dds::ddsBuffer*)file.data();
   int32_t w, h;
   dds::eDDSPixelFormat pixelFormat;

   if ( dds::DDSGetInfo( header, &w, &h, &pixelFormat) != 0 )
   {
      DE_ERROR("Cant read dds header. ", uri)
      return false;
   }

   DE_DEBUG("DDS ddsBuffer = ",sizeof(dds::ddsBuffer))
   DE_DEBUG("DDS w = ",header->width)
   DE_DEBUG("DDS h = ",header->height)
   DE_DEBUG("DDS alphaBitDepth = ",header->alphaBitDepth)
   DE_DEBUG("DDS linearSize = ",header->linearSize)
   DE_DEBUG("DDS expectSize = ",header->width * header->height)
   DE_DEBUG("DDS mipMapCount = ",header->mipMapCount)
   //DE_DEBUG("DDS fourCC = ",header->fourCC)
   //uint32_t fourCC      = *(uint32_t*)&(header[80]);
   // how big is it going to be including all mipmaps?
   //uint32_t bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;

   DE_DEBUG("DDS pf = ",int(pixelFormat))

   //img.clear();
   img.setUri( uri );
   img.setFormat( ColorFormat::RGBA8888 );
   img.resize( w, h );

   if ( dds::DDSDecompress( header, img.data() ) == -1)
   {
      DE_ERROR("Cant decode dds data. ", uri)
      img.clear();
      return false;
   }

   return true;
//   if ( !converter )
//   {
//      DE_ERROR("No color-converter found for source image.")
//      return false;
//   }

#if 0
   uint8_t *memFile = new uint8_t [ file->getSize() ];
   file->read ( memFile, file->getSize() );

   ddsBuffer *header = (ddsBuffer*) memFile;
   IImage* image = 0;
   int32_t width, height;
   eDDSPixelFormat pixelFormat;

   if ( 0 == DDSGetInfo( header, &width, &height, &pixelFormat) )
   {
      image = new CImage(ECF_A8R8G8B8, core::dimension2d<uint32_t>(width, height));

      if ( DDSDecompress( header, (uint8_t*) image->lock() ) == -1)
      {
         image->unlock();
         image->drop();
         image = 0;
      }
   }

   delete [] memFile;
   if ( image )
      image->unlock();

   return image;
#endif



   //   // Create one OpenGL texture
   //   GLuint textureID;
   //   glGenTextures(1, &textureID);

   //   // "Bind" the newly created texture : all future texture functions will modify this texture
   //   glBindTexture(GL_TEXTURE_2D, textureID);
   //   glPixelStorei(GL_UNPACK_ALIGNMENT,1);

   //   uint32_t blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
   //   uint32_t offset = 0;

   //   /* load the mipmaps */
   //   for (uint32_t level = 0; level < mipMapCount && (width || height); ++level)
   //   {
   //      uint32_t size = ((width+3)/4)*((height+3)/4)*blockSize;
   //      glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
   //         0, size, buffer + offset);

   //      offset += size;
   //      width  /= 2;
   //      height /= 2;

   //      // Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
   //      if(width < 1) width = 1;
   //      if(height < 1) height = 1;

   //   }

   //   free(buffer);

   //   return textureID;
   //return true;
}

} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_READER_DDS_ENABLED
