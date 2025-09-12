
// ===========================================================================
struct SurfaceFormat
// ===========================================================================
{
   SurfaceFormat();
   SurfaceFormat( int r, int g, int b, int a = 0, int d = 0, int s = 0, bool isRendBuf = false, bool offscreen = false );
   bool isRenderBuffer() const;
   int getColorBits() const;
   bool hasTransparency() const;
   bool hasColor() const;
   bool hasDepth() const;
   bool hasStencil() const;
   bool isOnlyColor() const;
   std::string toString() const;
public:
   int r, g, b, a, d, s;
   bool offscreen;
   bool dummy; // IsRenderBuffer
};

enum class TexType { Tex2D = 0, Tex3D, Tex2DArray, TexCube, ETypeCount };
enum class TexFormat { Auto = 0, R8G8B8A8, R8G8B8, R8, D24S8, D32F, D32FS8, EFormatCount };
enum class TexSemantic
{
   None,
   Diffuse,
   AlphaMask,
   Bump,
   Normal,
   Detail,
   Reflection, // for water shader texture render targets
   Refraction, // for water shader texture render targets
   Max,
   Elevation = TexSemantic::Bump, // alias elevation = bump
   Default = TexSemantic::None
};


// ===========================================================================
struct SO // SamplerOptions
// ===========================================================================
{
   enum EMinFilter { MinNearest = 0, MinLinear, MinMipmapNearestNearest, MinMipmapNearestLinear, MinMipmapLinearNearest, MinMipmapLinearLinear, EMinFilterCount };
   enum EMagFilter { MagNearest = 0, MagLinear, EMagFilterCount };
   enum EWrap { ClampToEdge = 0, ClampToBorder, Repeat, RepeatMirrored, EWrapCount };

   static SO skybox();
   SO();
   SO( EMagFilter u_mag, EMinFilter u_min, EWrap s_wrap, EWrap t_wrap );
   SO( SO const & o );
   bool hasMipmaps() const;
   std::string toString() const;
public:
   EMagFilter mag;
   EMinFilter min;
   uint8_t wrapS;
   uint8_t wrapT;
   uint8_t wrapR; // 3D
   int af; // anisotropicFilter
};

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
