#pragma once
#include <de_image/de_PixelFormatUtil.h>
#include <de_gpu/de_SamplerOptions.h>
#include <de_gpu/de_TexSemantic.h>
#include <de_gpu/de_TexType.h>
#include <de_gpu/de_GL_debug_layer.h>
#include <de_image/de_ImageUtils.h>

namespace de {

// ===========================================================================
struct GL_Texture2D
// ===========================================================================
{
   DE_CREATE_LOGGER("de.GL.Texture2D")

   uint32_t m_textureId;
   uint32_t m_w;
   uint32_t m_h;
   int32_t m_stage;
   PixelFormat m_fmt;
   //TexSemantic m_semantic;
   SO m_so;
   uint32_t m_internalFormat;
   uint32_t m_internalDataFormat;
   uint32_t m_internalDataType;

   std::string m_name;
   std::string m_uri;

   GL_Texture2D()
      : m_textureId(0)
      , m_w(0)
      , m_h(0)
      , m_stage(-1)
      , m_fmt(PixelFormat::Unknown)
      //, m_semantic()
      , m_internalFormat(0)
      , m_internalDataFormat(0)
      , m_internalDataType(0)
   { }

   ~GL_Texture2D()
   {
      if ( m_textureId )
      {
         std::cout << "Forgot to call de.GL.Texture2D.destroy() " << m_name << std::endl;
      }
   }

   uint32_t id() const { return m_textureId; }
   TexType type() const { return TexType::Tex2D; }
   PixelFormat format() const { return m_fmt; }
   std::string name() const { return m_name; }
   uint32_t width() const { return m_w; }
   uint32_t height() const { return m_h; }

   std::string str() const
   {
      std::ostringstream s; s <<
      "tex2d(" << m_textureId << "), "
      "name(" << m_name << "), "
      "w(" << m_w << "), "
      "h(" << m_h << "), "
      "fmt(" << PixelFormatUtil::getString(m_fmt) << "), "
      "so(" << m_so.toString() << "), "
      "uri(" << m_uri << ")";
      return s.str();
   }

   void destroy()
   {
      if ( !m_textureId )
      {
         return;
      }

      unbind();

      de_glDeleteTextures( 1, &m_textureId );
      m_textureId = 0;
   }

   // returns valid stage (texunit) of bound texture on success.
   // returns -2 on fail.
   // if stage is -1, then no glActiveTexture is called
   bool bind( int stage = -1 )
   {
      if ( !m_textureId )
      {
         DE_ERROR( "No texture id ", m_name )
         return false;
      }

      if ( stage > -1 )
      {
         de_glActiveTexture( GL_TEXTURE0 + stage );
         m_stage = stage;
      }
      else
      {
         de_glActiveTexture( GL_TEXTURE0 );
         m_stage = 0;
      }

      de_glBindTexture( GL_TEXTURE_2D, m_textureId );
      //DE_DEBUG( "Bound texture ", m_name, " to unit ", m_stage )
      return true;
   }

   void unbind()
   {
      if ( !m_textureId )
      {
         DE_ERROR( "No texture id ", m_name )
         return;
      }

      if ( m_stage > -1 )
      {
         de_glActiveTexture( GL_TEXTURE0 + m_stage );
      }

      de_glBindTexture( GL_TEXTURE_2D, 0 );
      m_stage = -1;
      return;
   }

   void setSamplerOptions( SO so ) { m_so = so; }

   void applySamplerOptions()
   {
      GLint magMode = GL_NEAREST;   // Low quality is default.
      if ( m_so.mag == SO::TexMag::Linear ) { magMode = GL_LINEAR; }// High quality on demand.

      GLint minMode = GL_NEAREST;   // Low quality is default.
      switch ( m_so.min )
      {
         case SO::TexMin::Linear: minMode = GL_LINEAR; break;
         case SO::TexMin::NearestMipmapNearest: minMode = GL_NEAREST_MIPMAP_NEAREST; break;
         case SO::TexMin::NearestMipmapLinear: minMode = GL_NEAREST_MIPMAP_LINEAR; break;
         case SO::TexMin::LinearMipmapNearest: minMode = GL_LINEAR_MIPMAP_NEAREST; break;
         case SO::TexMin::LinearMipmapLinear: minMode = GL_LINEAR_MIPMAP_LINEAR; break;
         default: break;
      }

      GLint wrapS = GL_REPEAT;
      switch ( m_so.wrapS )
      {
         case SO::TexWrap::RepeatMirrored: wrapS = GL_MIRRORED_REPEAT; break;
         case SO::TexWrap::ClampToEdge: wrapS = GL_CLAMP_TO_EDGE; break;
         case SO::TexWrap::ClampToBorder: wrapS = GL_CLAMP_TO_BORDER; break;
         default: break;
      }
      GLint wrapT = GL_REPEAT;
      switch ( m_so.wrapT )
      {
         case SO::TexWrap::RepeatMirrored: wrapT = GL_MIRRORED_REPEAT; break;
         case SO::TexWrap::ClampToEdge: wrapT = GL_CLAMP_TO_EDGE; break;
         case SO::TexWrap::ClampToBorder: wrapT = GL_CLAMP_TO_BORDER; break;
         default: break;
      }

      de_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minMode);
      de_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magMode);
      de_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
      de_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);

      if ( m_so.anisotropicFilter > 0.5f )
      {
         GLfloat maxAF = 0.0f;
         de_glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAF );
         de_glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAF );
         //DE_DEBUG("GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT = ",maxAF,")")
      }

      if ( m_so.hasMipmaps() )
      {
         de_glGenerateMipmap(GL_TEXTURE_2D);
      }

      // // Upload
      // de_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      // de_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      // de_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      // de_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      // de_glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAF );
   }

   bool
   upload( int w, int h, void const* const pixels, PixelFormat fmt )
   {
      if ( w < 1 || h < 1 )
      {
         DE_ERROR( "No tex size(",w,",",h,")")
         return false;
      }

      GLenum m_internalFormat = 0;
      GLenum m_internalDataFormat = 0;
      GLenum m_internalDataType = 0;

      if (fmt == PixelFormat::R8)
      {
         m_internalFormat = GL_RED;
         m_internalDataFormat = GL_RED;
         m_internalDataType = GL_UNSIGNED_BYTE;
      }
      else if (fmt == PixelFormat::R8G8B8)
      {
         m_internalFormat = GL_RGB; // GL_SRGB
         m_internalDataFormat = GL_RGB;
         m_internalDataType = GL_RGB;
         m_internalDataType = GL_UNSIGNED_BYTE;
      }
      else if (fmt == PixelFormat::R8G8B8A8)
      {
         m_internalFormat = GL_RGBA; // m_gammaCorrection ? GL_SRGB_ALPHA
         m_internalDataFormat = GL_RGBA;
         m_internalDataType = GL_UNSIGNED_BYTE;
      }
      else if (fmt == PixelFormat::R32F)
      {
         m_internalFormat = GL_R32F;
         m_internalDataFormat = GL_R32F;
         m_internalDataType = GL_FLOAT;
      }
      else if (fmt == PixelFormat::R16)
      {
         m_internalFormat = GL_R16I;
         m_internalDataFormat = GL_R16I;
         m_internalDataType = GL_SHORT;
      }
      else if (fmt == PixelFormat::D24S8)
      {
         m_internalFormat = GL_DEPTH24_STENCIL8;
         m_internalDataFormat = GL_DEPTH_STENCIL;
         m_internalDataType = GL_UNSIGNED_INT_24_8;
      }
      else
      {
         DE_ERROR( "Unsupported format ", str() )
         return false;
      }

      // GenTexture
      if ( !m_textureId )
      {
         de_glGenTextures(1, &m_textureId);
      }

      if ( !m_textureId )
      {
         DE_ERROR( "No textureId ", m_name )
         return false;
      }

      if ( !bind( -1 ) )
      {
         DE_ERROR( "Cant bind texture ", m_name )
         return false;
      }

      applySamplerOptions();

      de_glTexImage2D( GL_TEXTURE_2D,       // GLenum target
                     0,                   // GLint level
                     m_internalFormat,    // GLenum internalFormat
                     w,                 // GLsizei width
                     h,                 // GLsizei height
                     0,                   // GLint border
                     m_internalDataFormat,// GLenum format
                     m_internalDataType,  // GLenum type
                     pixels ); // void* pixel_data, if any, can be nullptr.

      m_fmt = fmt;
      m_w = w;
      m_h = h;

      unbind();
      // glBindTexture(GL_TEXTURE_2D, 0);

   //   // use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat
   //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_LINEAR_MIPMAP_LINEAR
   //   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      //DE_DEBUG("Uploaded Texture2D ", str() )
      return true;
   }


   bool
   createEmpty( std::string name, int w, int h, PixelFormat fmt, SO so )
   {
      m_name = name;
      m_uri = name;
      m_so = so;

      if ( !upload( w, h, nullptr, fmt ) )
      {
         DE_ERROR( "No tex upload, ", str() )
         return false;
      }

      return true;
   }

   bool
   createFromImage( std::string name, Image const & img, PixelFormat fmt, SO so )
   {
      if (name.empty())
      {
         DE_ERROR("Empty name")
         return false;
      }

      m_name = name;
      m_uri = img.getUri();
      m_so = so;

      if ( !upload( img.w(), img.h(), img.data(), fmt ) )
      {
         DE_ERROR( "No tex upload, ", str() )
         return false;
      }

      return true;
   }

   /*
   bool loadFromFile( std::string name, std::string uri, PixelFormat fmt, SO so )
   {
      if (name.empty())
      {
         DE_ERROR("Empty name")
         return false;
      }

      // Load image
      Image img;
      if ( !dbLoadImage( img, uri ) )
      {
         DE_ERROR( "Cant load image ", name, ", uri = ", uri)
         return false;
      }

      if (fmt == PixelFormat::R8G8B8 )
      {
         removeAlphaChannel(img);
      }

      return createFromImage( name, img, fmt, so );
   }

   bool loadFromFile( std::string uri, PixelFormat fmt, SO so )
   {
      return loadFromFile( uri, uri, fmt, so );
   }
   */
};

} // end namespace de
