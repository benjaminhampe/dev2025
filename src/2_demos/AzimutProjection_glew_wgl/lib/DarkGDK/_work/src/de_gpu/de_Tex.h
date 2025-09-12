#pragma once
#include <de_image/de_PixelFormatUtil.h>
#include <de_gpu/de_TexOptions.h>
#include <de_gpu/de_TexSemantic.h>
#include <de_gpu/de_TexType.h>
#include <de_image/de_ImageUtils.h>

namespace de {

// ===========================================================================
struct Tex
// ===========================================================================
{
   uint32_t m_textureId;
   uint32_t m_w;  // width
   uint32_t m_h;  // height
   uint32_t m_d;  // depth/slices used for Texture3D and Texture2DArray
   int32_t m_stage; // bound to one hardware tex unit, or -1
   PixelFormat m_pixelFormat; // pixel type
   //TexSemantic m_semantic;
   TexOptions m_so;
   uint32_t m_internalFormat;
   uint32_t m_internalDataFormat;
   uint32_t m_internalDataType;

   std::string m_name;
   std::string m_uri;

   Tex()
      : m_textureId(0)
      , m_w(0)
      , m_h(0)
      , m_stage(-1)
      , m_pixelFormat(PixelFormat::Unknown)
      //, m_semantic()
      , m_internalFormat(0)
      , m_internalDataFormat(0)
      , m_internalDataType(0)
   { }

   virtual ~Tex() {}
   /*
   {
      if ( m_textureId )
      {
         std::cout << "Forgot to call de.GL.Texture2D.destroy() " << m_name << std::endl;
      }
   }
   */

   // TODO: Best would be determining that on dbLoadImage and have it accessible by Image.hasTransparency();
   // Is true if image/texture has AlphaChannel and atleast one alpha value below 255 or 1.0.
   // Is kinda expensive op, so at load time would be best.
   virtual bool hasTransparency() const { return false; }

   virtual void setTexUnit( int unit ) { m_stage = unit; }

   virtual TexType getType() const = 0;
   virtual uint32_t getTextureId() const { return m_textureId; }
   virtual uint32_t getWidth() const { return m_w; }
   virtual uint32_t getHeight() const { return m_h; }
   virtual uint32_t getDepth() const { return m_d; }
   virtual PixelFormat getPixelFormat() const { return m_pixelFormat; }
   virtual std::string getName() const { return m_name; }
   virtual TexOptions const & getSamplerOptions() const { return m_so; }

   virtual std::string str() const
   {
      std::ostringstream o; o <<
      "id(" << m_textureId << "), "
      "name(" << m_name << "), "
      "w(" << m_w << "), "
      "h(" << m_h << "), "
      "fmt(" << PixelFormatUtil::getString(m_pixelFormat) << "), "
      "so(" << m_so.toString() << "), "
      "uri(" << m_uri << ")";
      return o.str();
   }

   virtual void destroy() = 0;

   // returns valid stage (texunit) of bound texture on success.
   // returns -2 on fail.
   // if stage is -1, then no glActiveTexture is called
   virtual bool bind( int stage = -1 ) = 0;

   virtual void unbind() = 0;

   virtual void setSamplerOptions( TexOptions so ) = 0;

   virtual void applySamplerOptions() = 0;

   virtual bool upload( int w, int h, void const* const pixels, PixelFormat fmt ) = 0;

   virtual bool createEmpty( std::string name, int w, int h, PixelFormat fmt, TexOptions so ) = 0;

   virtual bool createFromImage( std::string name, Image const & img, PixelFormat fmt, TexOptions so ) = 0;

   /*
   virtual bool loadFromFile( std::string name, std::string uri, PixelFormat fmt, TexOptions so ) = 0;

   virtual bool loadFromFile( std::string uri, PixelFormat fmt, TexOptions so ) = 0;
   */
};

} // end namespace de



/*

// ===========================================================================
struct Tex
// ===========================================================================
{
   virtual ~Tex() {}
   virtual bool resize( int w, int h ) = 0;
   virtual bool upload( Image const & img ) = 0;
   virtual void destroyTexture() = 0;

   virtual void setTexUnitManager( TexUnitManager* driver ) = 0;
   virtual void setTexUnit( int32_t unit ) = 0;
   virtual void setName( std::string const & name ) = 0;
   virtual void setSamplerOptions( TexOptions const & so ) = 0;
   virtual void setTexSemantic( TexSemantic semantic ) = 0;

   virtual void clearCPUSide() = 0;
   virtual Image const & getImage() const = 0;
   virtual Image & getImage() = 0;

   virtual std::string toString() const = 0;
   virtual TexSemantic const & getTexSemantic() const = 0;
   virtual TexFormat const & getFormat() const = 0;
   virtual TexType getType() const = 0;
   virtual std::string const & getName() const = 0;
   virtual int32_t getWidth() const = 0;
   virtual int32_t getHeight() const = 0;
   virtual int32_t getDepth() const = 0;
   virtual uint32_t getTextureId() const = 0;
   virtual int32_t getTexUnit() const = 0;
   virtual uint32_t getMaxMipLevel() const = 0;
   virtual TexOptions const & getSamplerOptions() const = 0;
   virtual bool hasTransparency() const = 0;
   virtual bool isRenderBuffer() const = 0;
};

*/
