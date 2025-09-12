#pragma once
#include <de_opengl.h>
#include <cstdint>
#include <sstream>

namespace de {

// ===========================================================================
enum class TexType 
// ===========================================================================
{ 
    Tex1D = 0, Tex2D, Tex3D, Tex2DArray, TexCube, Unknown
};
// ===========================================================================
enum class TexFormat
// ===========================================================================
{ 
    Auto = 0, R8G8B8A8, R8G8B8, R8, D24S8, D32F, D32FS8, Unknown
};

// ===========================================================================
enum class TexSemantic
// ===========================================================================
{
   Unknown, RT_Msaa, RT_PostFx, RT_Bloom, 
   DiffuseMap, TransparencyMap, EmissiveMap, 
   AlphaMask, BumpMap, NormalMap, DetailMap, Reflection, Refraction, 
   ElevationMap = TexSemantic::BumpMap, // alias elevation = bump
   Default = TexSemantic::DiffuseMap
};

// ===========================================================================
struct SurfaceFormat
// ===========================================================================
{
    uint8_t R; 
    uint8_t G;
    uint8_t B;
    uint8_t A;
    uint8_t D;
    uint8_t S;
    
    bool bOffscreen;
    bool bRenderBuffer;

    SurfaceFormat()
       : R( 8 ), G( 8 ), B( 8 ), A( 8 ), D( 24 ), S( 8 ), bOffscreen( false ), bRenderBuffer( false )
    {}
    SurfaceFormat( int r, int g, int b, int a = 0, int d = 0, int s = 0, bool offscreen = false, bool bIsRenderBuffer = false,  )
        : R( r ), G( g ), B( b ), A( a ), D( d ), S( s ), bOffscreen( offscreen ), bRenderBuffer( bIsRenderBuffer )
    {}

    bool isRenderBuffer() const { return dummy; }
    int getColorBits() const { return R + G + B + A; }

    // bool hasTransparency() const { return A > 0; }
    // bool hasColor() const { return (R > 0 || G > 0 || B > 0); }
    // bool hasDepth() const { return D > 0; }
    // bool hasStencil() const { return S > 0; }
    // bool isOnlyColor() const { return (hasColor() || hasTransparency()) && D == 0 && S == 0; }

    std::string toString() const
    {
        std::ostringstream o;
        if ( R > 0 ) { o << "R"; }
        if ( G > 0 ) { o << "G"; }
        if ( B > 0 ) { o << "B"; }
        if ( A > 0 ) { o << "A"; }        
        if ( R > 0 ) { o << R; }
        if ( G > 0 ) { o << G; }
        if ( B > 0 ) { o << B; }
        if ( A > 0 ) { o << A; }        
        if ( D > 0 ) { o << "_D" << D; }
        if ( S > 0 ) { o << "_S" << S; }
        if ( offscreen ) { o << "_Offscreen"; }
        return o.str();
    }
};

// ===========================================================================
struct SO // SamplerOptions
// ===========================================================================
{
    enum EMinFilter { MinNearest = 0, MinLinear, MinMipmapNearestNearest, MinMipmapNearestLinear, MinMipmapLinearNearest, MinMipmapLinearLinear, EMinFilterCount };
    enum EMagFilter { MagNearest = 0, MagLinear, EMagFilterCount };
    enum EWrap { ClampToEdge = 0, ClampToBorder, Repeat, RepeatMirrored, EWrapCount };

    EMagFilter mag;
    EMinFilter min;
    uint8_t wrapS;
    uint8_t wrapT;
    uint8_t wrapR; // 3D
    int af; // anisotropicFilter

    static SO skybox() { return SO( MagNearest, MinNearest, ClampToBorder, ClampToBorder ); }
    
    SO()
        : mag( MagNearest ), min( MinNearest ), wrapS( ClampToEdge ), wrapT( ClampToEdge ), af( 4 )
    {}
    
    SO( EMagFilter u_mag, EMinFilter u_min, EWrap s_wrap, EWrap t_wrap )
        : mag( u_mag ), min( u_min ), wrapS( s_wrap ), wrapT( t_wrap ), af( 4 )
    {}

    SO( SO const & o )
       : mag( o.mag ), min( o.min ), wrapS( o.wrapS ), wrapT( o.wrapT ), af( o.af )
    {}

    bool hasMipmaps() const
    {
        if ( min <= MinLinear ) { return false; } else { return true; }
    }

    std::string toString() const
    {
        std::ostringstream s;
        s << int(min) << "," << int(mag) << "," << int(wrapS) << "," << int(wrapT) << "," << af;
        return s.str();
    }
};

//===========================================================================
struct TextureUtil
//===========================================================================
{
    // uint32_t programId = glCreateProgram();
    static uint32_t createShaderFromText(   
        std::string const & programName,// Program name,   e.g. "s3d+tex0+light2+bloom+fog"
        std::string const & vsText,     // Vertex shader   e.g. #version 460 core es\n int main() {}
        std::string const & fsText,     // Fragment shader e.g. #version 460 core es\n int main() {}
        std::string const & gsText = "" // Geometry shader e.g. #version 460 core es\n int main() {}
    );
    

    // static
    Tex2D*
    createTex2D( std::string const & name, Image const & img, SO const & so )
    {
       if ( name.empty() )
       {
          DE_ERROR("Got empty name")
          return nullptr;
       }

       GLuint texId = 0;
       ::glGenTextures( 1, &texId );
       bool ok = GL_VALIDATE;
       if ( !ok || texId < 1 )
       {
          DE_ERROR("Cant gen tex(",name,")")
          return nullptr;
       }

       auto tex = new Tex2D( unitmanager );
       tex->m_Name = name;
       tex->m_Type = TexType::Tex2D;
       tex->m_SamplerOptions = so;
       tex->m_TextureId = texId;
       tex->m_KeepImage = keepImage;
       if ( keepImage )
       {
          tex->m_Image = img;
       }

       ok = tex->upload( img );
       if ( !ok )
       {
          DE_ERROR("Cant upload tex(",name,") image ", tex->toString() )
          delete tex;
          return nullptr;
       }

       tex->m_debugLevel = debugLevel;

       if ( debugLevel > 0 )
       {
          DE_DEBUG("Created Texture2D ", tex->toString() )
       }

       return tex;
    }    

    static int32_t getShaderVersionMajor();
    static int32_t getShaderVersionMinor();
    static std::string getShaderVersionHeader();
    static std::string getShaderError( uint32_t shaderId );
    static std::string getProgramError( uint32_t programId );
    static std::vector< std::string > splitText( std::string const & txt, char searchChar );
    static std::string toStringWithLineNumbers( std::string const & src );
};

} // end namespace de.



void
Tex2D::destroyTexture()
{
   if ( m_TextureId > 0 )
   {
      GLuint id = m_TextureId;
      ::glDeleteTextures( 1, &id ); GL_VALIDATE
      m_TextureId = 0;

      if ( m_debugLevel > 0 )
      {
         DE_DEBUG( "Delete Texture2D texId[", id, "] ", toString() )
      }
   }

//   if ( m_TexManager )
//   {
//      m_TexManager->removeTexture( getName() );
//   }
}

// static
Tex2D*
Tex2D::create( TexUnitManager* unitmanager,
               std::string const & name,
               Image const & img,
               SO  const & so, bool keepImage, uint8_t debugLevel )
{
   if ( name.empty() )
   {
      DE_ERROR("Got empty name")
      return nullptr;
   }

   GLuint texId = 0;
   ::glGenTextures( 1, &texId );
   bool ok = GL_VALIDATE;
   if ( !ok || texId < 1 )
   {
      DE_ERROR("Cant gen tex(",name,")")
      return nullptr;
   }

   auto tex = new Tex2D( unitmanager );
   tex->m_Name = name;
   tex->m_Type = TexType::Tex2D;
   tex->m_SamplerOptions = so;
   tex->m_TextureId = texId;
   tex->m_KeepImage = keepImage;
   if ( keepImage )
   {
      tex->m_Image = img;
   }

   ok = tex->upload( img );
   if ( !ok )
   {
      DE_ERROR("Cant upload tex(",name,") image ", tex->toString() )
      delete tex;
      return nullptr;
   }

   tex->m_debugLevel = debugLevel;

   if ( debugLevel > 0 )
   {
      DE_DEBUG("Created Texture2D ", tex->toString() )
   }

   return tex;
}


bool
Tex2D::upload( Image const & img )
{
   if ( img.empty() ) { DE_ERROR("No image") return false; }
   if ( img.getFormat() != ColorFormat::RGBA8888 &&
        img.getFormat() != ColorFormat::RGB888 )
   {
      DE_ERROR("Unsupported image format ", img.toString() )
      return false;
   }

   if ( !m_TextureId ) { DE_ERROR("No texture id") return false; }
   if ( !m_TexManager ) { DE_ERROR("No texunit manager id") return false; }

   int stage = m_TexManager->bindTexture( this );
   if (stage < 0)
   {
      DE_ERROR("No free stage, tex ", toString() )
      return false;
   }
   //glActiveTexture( GL_TEXTURE0 + tex->getTexUnit() ); GL_VALIDATE
   //glBindTexture( GL_TEXTURE_2D, texId ); GL_VALIDATE

   TexFormat texFormat = TexFormat::R8G8B8A8;
   GLenum internal = GL_RGBA;
   GLenum internalFormat = GL_RGBA;
   GLenum internalType = GL_UNSIGNED_BYTE;
   if ( img.getFormat() == ColorFormat::RGB888 )
   {
      texFormat = TexFormat::R8G8B8;
      internal = GL_RGB;
      internalFormat = GL_RGB;
      internalType = GL_UNSIGNED_BYTE;
   }

   ShaderUtil::setSamplerOptions2D( m_SamplerOptions );

   GLsizei w = img.getWidth();
   GLsizei h = img.getHeight();
   ::glTexImage2D( GL_TEXTURE_2D, 0, // GLenum target + GLint level
                  internal, w, h, 0, // GLint border
                  internalFormat, internalType, img.data() );
   bool ok = GL_VALIDATE;
   if ( !ok ) { DE_ERROR("Cant upload" ) return false; }

   if ( m_SamplerOptions.hasMipmaps() )
   {
      ::glGenerateMipmap( GL_TEXTURE_2D ); GL_VALIDATE
   }

   // ::glBindTexture( GL_TEXTURE_2D, 0 ); GL_VALIDATE

   m_Width = w;
   m_Height = h;
   m_Format = texFormat;
   m_TexUnit = stage;
   m_InternalFormat.internal = internal;
   m_InternalFormat.format = internalFormat;
   m_InternalFormat.type = internalType;
   return true;
}

bool
Tex2D::resize( int w, int h )
{
   if ( !m_TextureId ) { DE_ERROR("No texture id") return false; }
   if ( !m_TexManager ) { DE_ERROR("No texunit manager id") return false; }
   int stage = m_TexManager->bindTexture( this );
   if (stage < 0)
   {
      DE_ERROR("No free stage, tex ", toString() )
      return false;
   }

   ::glTexImage2D( GL_TEXTURE_2D, 0, // GLenum target + GLint level
                  m_InternalFormat.internal, w, h, 0, // GLint border
                  m_InternalFormat.format,
                  m_InternalFormat.type, nullptr );
   bool ok = GL_VALIDATE;
   if ( !ok ) { DE_ERROR("Cant resize" ) return false; }

   if ( m_SamplerOptions.hasMipmaps() )
   {
      ::glGenerateMipmap( GL_TEXTURE_2D ); GL_VALIDATE
   }
   // ::glBindTexture( GL_TEXTURE_2D, 0 ); GL_VALIDATE
   m_Width = w;
   m_Height = h;
   return true;
}






struct TexUnitManager;

// ===========================================================================
struct TexInternalFormat
// ===========================================================================
{
   uint32_t internal = 0;  // GL_RGB;
   uint32_t format = 0;    // GL_RGB;
   uint32_t type = 0;      // GL_UNSIGNED_BYTE;
};

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
   virtual void setSamplerOptions( SO const & so ) = 0;
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
   virtual SO const & getSamplerOptions() const = 0;
   virtual bool hasTransparency() const = 0;
   virtual bool isRenderBuffer() const = 0;
};

// ===========================================================================
struct TexRef
// ===========================================================================
{
   Image* m_img;     // the CPU-Side original image, if avail
   Tex* m_tex;  // the full GPU texture(w,h)
   uint32_t m_w; // copy for faster lookup
   uint32_t m_h; // copy for faster lookup
   int m_id;
   int m_loadCounter;
   // int m_refCount;
   Recti m_rect;   // irect - integral subrect of (w,h) in pixels
   Rectf m_coords; // frect - fractional subrect of (w,h) in multiples of one.
   glm::vec2 m_repeat; // scale?
   std::string m_name;
   std::string m_uri;

   TexRef();
   TexRef( Tex* ptr, bool useOffset = false );
   TexRef( Tex* ptr, Recti const & pos, bool useOffset = false );

   // bool operator==( TexRef const & other ) const;

   Tex const* getTexture() const { return m_tex; }
   Tex* getTexture() { return m_tex; }

   Tex const* texture() const { return m_tex; }
   Tex* texture() { return m_tex; }

   Image* img();
   Image const* img() const;
   Rectf const & texCoords() const;
   float u1() const;
   float v1() const;
   float u2() const;
   float v2() const;
   float du() const;
   float dv() const;
   Rectf const & getTexCoords() const;
   float getU1() const;
   float getV1() const;
   float getU2() const;
   float getV2() const;
   float getDU() const;
   float getDV() const;

   // Sub irect
   Recti const & rect() const;
   int x() const;
   int y() const;
   int w() const;
   int h() const;
   Recti const & getRect() const;
   int getX() const;
   int getY() const;
   int getWidth() const;
   int getHeight() const;

   // Full texture stuff
   int texWidth() const;
   int texHeight() const;
   glm::ivec2 texSize() const;
   glm::vec2 texSizef() const;

   int getTexWidth() const;
   int getTexHeight() const;
   glm::ivec2 getTexSize() const;
   glm::vec2 getTexSizef() const;

   std::string toString() const;
   bool empty() const;
   void setMatrix( float tx, float ty, float sx, float sy );
   glm::vec4 getTexTransform() const;
/*
   // vec4.<x,y> = translation x,y ( atlas tex chunk pos )
   // vec4.<z,w> = scale x,y ( atlas tex chunk size )
   Image TexRef::copyImage() const;
*/
};

// TODO Add shared_ptr
// ===========================================================================
struct Tex2D : public Tex
// ===========================================================================
{
   Tex2D( TexUnitManager* unitManager );
   ~Tex2D();
   void destroyTexture();
   std::string toString() const;

   static Tex2D*
   create(  TexUnitManager* unitmanager,
            std::string const & name,
            Image const & img,
            SO const & so,
            bool keepImage,
            uint8_t debugLevel = 0 );

   bool upload( Image const & img );
   bool resize( int w, int h );

   void setTexUnitManager( TexUnitManager* driver );
   void setTexUnit( int32_t unit );
   void setName( std::string const & name );
   void setSamplerOptions( SO const & so );
   void setTexSemantic( TexSemantic semantic ) override { m_semantic = semantic;}

   TexFormat const & getFormat() const;
   TexType getType() const;
   TexSemantic const & getTexSemantic() const { return m_semantic; }

   std::string const & getName() const;
   int32_t getWidth() const;
   int32_t getHeight() const;
   int32_t getDepth() const;
   uint32_t getTextureId() const;
   int32_t getTexUnit() const;

   uint32_t getMaxMipLevel() const;
   SO const & getSamplerOptions() const;
   bool hasTransparency() const;
   bool isRenderBuffer() const;

   void clearCPUSide();
   Image const & getImage() const;
   Image & getImage();

   DE_CREATE_LOGGER("Tex2D")
   TexUnitManager* m_TexManager;
   TexType m_Type;        // e.g. Tex2D
   TexFormat m_Format;  // e.g. R8G8B8A8
   TexSemantic m_semantic;
   TexInternalFormat m_InternalFormat; // Filled by GL
   int32_t m_Width;
   int32_t m_Height;
   int32_t m_Depth;        // for 3d textures
   int32_t m_MaxMipLevel;  // for Mipmapping
   SO m_SamplerOptions;
   int32_t m_TexUnit;
   uint32_t m_TextureId;
   bool m_IsRenderBuffer;
   bool m_KeepImage;
   uint8_t m_debugLevel;
   std::string m_Name;  // Key the engine/modeler uses to find this tex by name.
   std::string m_Uri;   // The original image uri, if any.
   // CPU-side,
   // has the original uri string to export the texture elsewhere, single image or image atlas page
   Image m_Image; // CPU-side
};

// ===========================================================================
struct TexMipMapper
// ===========================================================================
{
   DE_CREATE_LOGGER("de.gpu.TexMipMapper")

   GLuint m_ReadFBO;
   GLuint m_DrawFBO;

   TexMipMapper();
   ~TexMipMapper();

   inline void init();
   void close();

   /// @brief Creates all mipmaps ( resized texture ) for all levels determined by Tex->getMaxMipmapLevel() if they are not up-to-date (already generated and the data was not changed)
   ///        and the texture is bound to a texture unit.
   /// @param[in] textureBase The texture, for which the mipmaps should be generated.
   /// @param[in] singleLayer Layer for which mipmaps need to be generated, or -1 to generate for all array texture layers

   bool
   createMipmaps( Tex2D* tex, int specificDepthLayer = -1 );
};

// ===========================================================================
struct TexUnitManager
// ===========================================================================
{
   TexUnitManager();
   ~TexUnitManager();

   void dump();

   void initGL(int logLevel = 0);

   void destroyGL(int logLevel = 0);

   uint32_t getUnitCount() const;

   int32_t findUnit( uint32_t texId ) const;

   // Raw
   bool
   bindTextureId( int stage, uint32_t texId );
   bool
   bindTexture( int stage, Tex* tex );

   // Benni's more intelligent calls for raw/higher level TexClass.
   // AutoSelect a free tex unit and return its index for use in setSamplerUniform().
   int
   bindTextureId( uint32_t texId );
   bool
   unbindTextureId( uint32_t texId );

   int
   bindTexture( Tex* tex );
   bool
   unbindTexture( Tex* tex );

   DE_CREATE_LOGGER("TexUnitManager")
   // ###   HardwareUnitManager    ###
   std::vector< uint32_t > m_TexUnits; // Physical avail texture Units
};

// ===========================================================================
struct TexManager
// ===========================================================================
{
   TexManager();
   ~TexManager();
   Tex*
   findTexture( std::string const & name ) const;
   bool
   hasTexture( std::string const & name ) const;
   TexRef
   getTexture( std::string const & name, SO const & so = SO(), bool keepImage = false );
   TexRef
   createTexture(
      std::string const & name,
      Image const & img,
      SO so = SO(),
      bool keepImage = false );
   void initGL( TexUnitManager* unitManager, int logLevel = 0 );
   void destroyGL(int logLevel = 0);
   void dump();
   bool upload( Tex* tex, Image const & src );
   void updateTextures();
   void clearTextures();
   void removeTexture( std::string const & name );
   void removeTexture( Tex* tex );

   // int32_t findTex( Tex* tex ) const;
   // int32_t findTex( std::string const & name ) const;
   // bool hasTex( std::string const & name ) const;
   // TexRef getTex( std::string const & name ) const;
   // TexRef load2D( std::string const & name, std::string uri, bool keepImage = true, SO so = SO() );
   // TexRef add2D( std::string const & name, Image const & img, bool keepImage = true, SO so = SO() );
   uint32_t getMaxTex2DSize() const;


   DE_CREATE_LOGGER("de.gpu.TexManager")
   // ###   HwTexUnitManager    ###
   TexUnitManager* m_unitManager;
   // ###   TexManager   ###
   uint32_t m_maxTex2DSize;
   std::vector< Tex* > m_Textures; // All big memory pages ( atlas textures )
   std::vector< Tex* > m_TexturesToRemove;   // All textures that exist
   std::unordered_map< std::string, TexRef > m_refs; // has indices into m_Textures.
   // ###   RefManager   ###
   //std::vector< TexRef > m_Refs;    // All small textures we can use
   //std::unordered_map< std::string, int > m_RefLUT; // has indices into m_TexRefs

};
