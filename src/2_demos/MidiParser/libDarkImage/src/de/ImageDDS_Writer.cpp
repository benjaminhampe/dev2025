#include "ImageDDS_Writer.hpp"

#ifdef DE_IMAGE_WRITER_DDS_ENABLED

#include "ImageDDS.hpp"

namespace de {
namespace image {

bool
ImageWriterDDS::save( Image const & img, std::string const & uri, uint32_t quality )
{
   if ( img.empty() )
   {
      DE_ERROR("Empty source image.")
      return false;
   }

#if 0
   // try to open the file
   FILE* file = ::fopen( uri.c_str(), "wb");
   if ( !file )
   {
      DE_ERROR("Cant open file wb. uri(",uri,")")
      return false;
   }

   // verify the type of file
   char fourcc[4] = { 'D', 'D', 'S', ' ' };
   ::fwrite( fourcc, 1, 4, file );

   // get the surface desc
   dds::Header header;
   uint32_t height = uint32_t( img.getWidth() );
   uint32_t width = uint32_t( img.getHeight() );
   uint32_t linearSize = width * height;
   uint32_t mipMapCount = 0;
   uint32_t fourCC = *(uint32_t*)&fourcc;

   *(uint32_t*)&header.data[ 8 ] =  height;
   *(uint32_t*)&header.data[ 12 ] =  width;
   *(uint32_t*)&header.data[ 16 ] = linearSize;
   *(uint32_t*)&header.data[ 24 ] =  mipMapCount;
   *(uint32_t*)&header.data[ 80 ] =  fourCC;
   ::fwrite( &header, 1, sizeof( header ), file );

   // how big is it going to be including all mipmaps?
//   uint32_t bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
//   uint8_t* buffer = (uint8_t*)malloc( bufsize * sizeof( uint8_t ) );
//   ::fread( buffer, 1, bufsize, file );

//   uint32_t components  = (fourCC == FOURCC_DXT1) ? 3 : 4;
//   uint32_t format;

//   switch( fourCC )
//   {
//   case FOURCC_DXT1: format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; break;
//   case FOURCC_DXT3: format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; break;
//   case FOURCC_DXT5: format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; break;
//   default: free(buffer); return 0;
//   }


   ColorConverter::Converter_t converter =
      ColorConverter::getConverter( img.getFormat(), ColorFormat::RGB888 );

   if ( !converter )
   {
      ::fclose(file);
      DE_ERROR("No color-converter found for source image.")
      return false;
   }

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

   ::fclose(file);
#endif
   return true;
}

} // end namespace image.
} // end namespace de.

#endif // DE_IMAGE_WRITER_DDS_ENABLED
