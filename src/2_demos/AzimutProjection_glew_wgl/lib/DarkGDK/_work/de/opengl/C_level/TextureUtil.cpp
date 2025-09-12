



Tex2D::Tex2D( TexUnitManager* unitManager )
   : m_TexManager( unitManager )
   , m_Type( TexType::Tex2D )
   , m_Format( TexFormat::R8G8B8A8 )
   , m_semantic( TexSemantic::None )
   , m_Width( 0 )
   , m_Height( 0 )
   , m_Depth( 0 )
   , m_MaxMipLevel( 4 )
   , m_SamplerOptions()
   , m_TexUnit( -1 )
   , m_TextureId( 0 )
   , m_IsRenderBuffer( false )
   , m_KeepImage( true )
   , m_debugLevel(0)
   , m_Name()
   , m_Uri()
   , m_Image()
{
}

Tex2D::~Tex2D()
{
   destroyTexture();

//   if ( m_TexManager )
//   {
//      m_TexManager->removeTexture( getName() );
//   }
}

std::string
Tex2D::toString() const
{
   std::ostringstream s;
   s  << "w:" << getWidth() << ", "
      << "h:" << getHeight()   << ", "
      << "id:" << m_TextureId << ", "
      << "u:" << m_TexUnit << ", "
      << "s:" << int(m_semantic) << ", "
      << dbGetFileName(m_Name);
   return s.str();
}

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

void
Tex2D::setTexUnitManager( TexUnitManager* driver ) { m_TexManager = driver; }
void
Tex2D::setTexUnit( int32_t unit ) { m_TexUnit = unit; }
void
Tex2D::setName( std::string const & name ) { m_Name = name; }
void
Tex2D::setSamplerOptions( SO const & so ) { m_SamplerOptions = so; }

TexFormat const &
Tex2D::getFormat() const { return m_Format; }
TexType
Tex2D::getType() const { return m_Type; }
std::string const &
Tex2D::getName() const { return m_Name; }
int32_t
Tex2D::getWidth() const { return m_Width; }
int32_t
Tex2D::getHeight() const { return m_Height; }
int32_t
Tex2D::getDepth() const { return m_Depth; }
uint32_t
Tex2D::getTextureId() const { return m_TextureId; }
int32_t
Tex2D::getTexUnit() const { return m_TexUnit; }
uint32_t
Tex2D::getMaxMipLevel() const { return m_MaxMipLevel; }
SO const &
Tex2D::getSamplerOptions() const { return m_SamplerOptions; }
bool
Tex2D::hasTransparency() const { return m_Format == TexFormat::R8G8B8A8; }
bool
Tex2D::isRenderBuffer() const { return m_IsRenderBuffer; }

void
Tex2D::clearCPUSide() { m_Image.clear( true ); }
Image const &
Tex2D::getImage() const { return m_Image; }
Image &
Tex2D::getImage() { return m_Image; }



TexMipMapper::TexMipMapper()
   : m_ReadFBO( 0 )
   , m_DrawFBO( 0 )
{}

TexMipMapper::~TexMipMapper()
{
   close();
}

void
TexMipMapper::init()
{
   if ( !m_ReadFBO )
   {
      glGenFramebuffers( 1, &m_ReadFBO ); GL_VALIDATE
   }
   if ( !m_DrawFBO )
   {
      glGenFramebuffers( 1, &m_DrawFBO ); GL_VALIDATE
   }
}

void
TexMipMapper::close()
{
   if ( m_ReadFBO )
   {
      glDeleteFramebuffers( 1, &m_ReadFBO ); GL_VALIDATE
      m_ReadFBO = 0;
   }
   if ( m_DrawFBO )
   {
      glDeleteFramebuffers( 1, &m_DrawFBO ); GL_VALIDATE
      m_DrawFBO = 0;
   }
}

/// @brief Creates all mipmaps ( resized texture ) for all levels determined by Tex->getMaxMipmapLevel() if they are not up-to-date (already generated and the data was not changed)
///        and the texture is bound to a texture unit.
/// @param[in] textureBase The texture, for which the mipmaps should be generated.
/// @param[in] singleLayer Layer for which mipmaps need to be generated, or -1 to generate for all array texture layers

bool
TexMipMapper::createMipmaps( Tex2D* tex, int specificDepthLayer )
{
   if ( !tex ) return true; // Nothing was done very successfully.

   int texId = tex->getTextureId();
   if ( !texId )
   {
      DE_ERROR("No texId ", tex->toString() )
      return false;
   }

   init();
   GLint oldReadFBO = 0;
   GLint oldDrawFBO = 0;
   glGetIntegerv( GL_READ_FRAMEBUFFER_BINDING, &oldReadFBO );
   glGetIntegerv( GL_DRAW_FRAMEBUFFER_BINDING, &oldDrawFBO );
   glBindFramebuffer( GL_READ_FRAMEBUFFER, m_ReadFBO ); GL_VALIDATE
   glBindFramebuffer( GL_DRAW_FRAMEBUFFER, m_DrawFBO ); GL_VALIDATE

   int w = tex->getWidth();
   int h = tex->getHeight();
   int w2 = w >> 1; // Maybe your mipmaps do look wrong because your tex is not a multiple of 2.
   int h2 = h >> 1; // Maybe your mipmaps do look wrong because your tex is not a multiple of 2.

   // For Tex3D, Tex2DArray, TexCube?
   if ( tex->getDepth() > 0 )
   {
      int depthS = 0;
      int depthE = tex->getDepth();
      if ( specificDepthLayer > -1 )
      {
         depthS = depthE = specificDepthLayer;
      }

      for ( int i = 1; i <= int(tex->getMaxMipLevel()); ++i )
      {
         for ( int d = depthS; d <= depthE; ++d )
         {
            glFramebufferTextureLayer( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texId, i - 1, d );
            glFramebufferTextureLayer( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texId, i, d );
            glBlitFramebuffer( 0, 0, w, h, 0, 0, w2, h2, GL_COLOR_BUFFER_BIT, GL_LINEAR );
         }
         w = w2;
         h = h2;
         w2 >>= 1;
         h2 >>= 1;
      }
      return true;
   }
   // For Tex2D
   else
   {
      if ( tex->getType() != TexType::Tex2D )
      {
         for ( int i = 1; i <= int(tex->getMaxMipLevel()); ++i )
         {
            glFramebufferTexture2D( GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, i - 1 );
            glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, i );
            glBlitFramebuffer( 0, 0, w, h, 0, 0, w2, h2, GL_COLOR_BUFFER_BIT, GL_LINEAR );
            w = w2;
            h = h2;
            w2 >>= 1;
            h2 >>= 1;
         }

         return true;
      }
      else
      {
         DE_ERROR("Unsupported texType ", tex->toString() )
         return false;
      }
   }
}

// ===========================================================================
TexUnitManager::TexUnitManager() {}
TexUnitManager::~TexUnitManager() {}

void
TexUnitManager::dump()
{
   DE_DEBUG( "TexUnit.Count = ", m_TexUnits.size() )
   for ( size_t i = 0; i < m_TexUnits.size(); ++i )
   {
      DE_DEBUG( "TexUnit[",i,"] = ", m_TexUnits[ i ] )
   }
}

void
TexUnitManager::initGL( int logLevel )
{
   //::glPixelStorei( GL_UNPACK_ALIGNMENT, 1 ); GL_VALIDATE   // Affects glTexImage2D()
   //::glPixelStorei( GL_PACK_ALIGNMENT, 1 ); GL_VALIDATE     // Affects glReadPixels()
   int nTexUnits = glGetIntegerDE( GL_MAX_TEXTURE_IMAGE_UNITS );
   m_TexUnits.clear();
   m_TexUnits.resize( nTexUnits, 0 );
   if ( logLevel > 0 )
   {
      DE_DEBUG( "HardwareTexUnitCount = ", nTexUnits )
   }

}

void
TexUnitManager::destroyGL( int logLevel )
{}

uint32_t
TexUnitManager::getUnitCount() const
{
   return m_TexUnits.size();
}

int32_t
TexUnitManager::findUnit( uint32_t texId ) const
{
   if ( texId < 1 ) return -1; // Not an id.
   for ( size_t i = 0; i < m_TexUnits.size(); ++i )
   {
      if ( m_TexUnits[ i ] == texId )
      {
         return int( i );
      }
   }
   return -1;
}

bool
TexUnitManager::bindTextureId( int stage, uint32_t texId )
{
   if ( texId < 1 )
   {
      DE_WARN("More an unbind with texId = ", texId )
   }

   if ( stage < 0 || stage >= int(m_TexUnits.size()) )
   {
      DE_ERROR("No valid stage ",stage,"/", int(m_TexUnits.size()) )
      return false;
   }

   if ( m_TexUnits[ stage ] == texId )
   {
      return true; // Cache hit, nothing todo.
   }

   m_TexUnits[ stage ] = texId; // Update unit
   ::glActiveTexture( GL_TEXTURE0 + stage ); GL_VALIDATE
   ::glBindTexture( GL_TEXTURE_2D, texId );
   bool ok = GL_VALIDATE;
   if ( !ok )
   {
      throw std::runtime_error( dbStrJoin( "No stage(",stage,") and texId(",texId,")" ) );
   }
   // ShaderUtil::setSamplerOptions2D( tex->getSamplerOptions() );
   return true;
}

bool
TexUnitManager::unbindTextureId( uint32_t texId )
{
   if ( texId < 1 ) return true;  // Nothing to unbind.

   // Search unit with this tex...
   int found = findUnit( texId );
   if ( found < 0 )
   {
      return true; // Nothing to unbind.
   }

   m_TexUnits[ found ] = 0;
   ::glActiveTexture( GL_TEXTURE0 + found ); GL_VALIDATE
   ::glBindTexture( GL_TEXTURE_2D, 0 ); GL_VALIDATE
   // DE_DEBUG( "Unbound texture ", texId, " from stage ", found, " and target=", target )
   return true;
}


// More intelligent GL: stage/unit is auto select.
// AutoSelect a free tex unit and return its index for use in setSamplerUniform().
int
TexUnitManager::bindTextureId( uint32_t texId )
{
   if ( texId < 1 ) { return -1; }

   int unit = findUnit( texId );
   if ( unit > -1 )
   {
      return unit; // Cache hit, activated and bound.
   }

   // Find free unit, if any, and activate it
   for ( size_t i = 0; i < m_TexUnits.size(); ++i )
   {
      if ( !m_TexUnits[ i ] )
      {
         if ( bindTextureId( i, texId ) )
         {
            //DE_DEBUG("Bound tex ", texId, " at unit ", i)
            return int( i );
         }
         else
         {
            DE_ERROR("Not bound unit(", i, ") with texId(",texId,")")
         }
      }
   }

   static int unitCounter = 0;
   unitCounter = (unitCounter + 1) % int(m_TexUnits.size());
   unit = unitCounter;
   if ( !bindTextureId( unit, texId ) )
   {
      DE_ERROR("No force bound unit(", unit, ") with texId(",texId,")")
      return -1;
   }

   return unit;
}

// Benni's HighLevelCalls (class Tex) to native bindTexture2D

bool
TexUnitManager::bindTexture( int stage, Tex* tex )
{
   if ( !tex ) { DE_ERROR("No tex") return false; }
   if ( tex->getType() == TexType::Tex2D )
   {
      if ( bindTextureId( stage, tex->getTextureId() ) )
      {
         tex->setTexUnit( stage );
         return true;
      }
   }
//   else if ( tex->isRenderBuffer() )
//   {
//      ::glBindRenderbuffer( GL_RENDERBUFFER, tex->getTextureId() ); GL_VALIDATE
////      ::glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h ); GL_VALIDATE
////      ::glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
////            GL_RENDERBUFFER, m_depthTex ); GL_VALIDATE
//   }
   return false;
}


// AutoSelect a free tex unit and return its index for use in setSamplerUniform().
int
TexUnitManager::bindTexture( Tex* tex )
{
   if ( !tex ) { DE_ERROR("No tex") return -1; }
   if ( tex->getType() == TexType::Tex2D )
   {
      return bindTextureId( tex->getTextureId() );
   }
   else
   {
      // throw std::runtime_error("Invalid texture type");
   }
   return -1;
}



bool
TexUnitManager::unbindTexture( Tex* tex )
{
   if ( !tex ) { return true; }
   if ( tex->getType() == TexType::Tex2D )
   {
      if ( !unbindTextureId( tex->getTextureId() ) )
      {
         DE_ERROR("Cant unbind tex ", tex->toString())
         return false;
      }
      else
      {
         return true;
      }
   }
   else
   {
      DE_ERROR("Unsupported tex type ", tex->toString())
      return false;
   }
}

// ===========================================================================
// ===========================================================================

TexManager::TexManager()
   : m_maxTex2DSize(1024)
{
   //init();
}
TexManager::~TexManager()
{
   //unbindAll();
}

TexRef
TexManager::getTexture( std::string const & name, SO const & so, bool keepImage )
{
   // We abort on empty string, nothing todo, return invalid texref.
   if ( name.empty() ) { return TexRef(); }

   // Ask cache if ever heard of 'name' before...
   auto it = m_refs.find( name );

   // (cachemiss)...
   // If cache has no string 'name' then it always tries to load
   // the given filename 'name'. This 'always loading' will be bad
   // behaviour if the ressource file does not exist.
   if (it != m_refs.end())
   {
      return it->second; // Cache Hit
   }

   de::Image img;
   if ( !dbLoadImage( img, name ) )
   {
      DE_ERROR("No image loaded for texture ", name )
      return TexRef();
   }

   Tex2D* tex = Tex2D::create( m_unitManager, name, img, so, keepImage );
   if ( !tex )
   {
      //DE_RUNTIME_ERROR(name, "Got tex nullptr")
      DE_ERROR( "No tex(",name,") created")
      return TexRef();
   }

   m_Textures.emplace_back( tex );
   m_refs[ name ] = TexRef( tex );
   it = m_refs.find( name );

   if ( it == m_refs.end() )
   {
      //DE_RUNTIME_ERROR(name, "Got m_Textures.end() iterator")
      DE_ERROR( "No tex(",name,") added to cache")
      return TexRef();
   }
   else
   {
      return it->second; // Cache Hit
   }
}

TexRef
TexManager::createTexture(
   std::string const & name,
   Image const & img,
   SO so,
   bool keepImage )
{
   if ( name.empty() ) { return TexRef(); }
   if ( img.empty() ) { return TexRef(); }
   if ( img.getFormat() != ColorFormat::RGBA8888 &&
        img.getFormat() != ColorFormat::RGB888 )
   {
      DE_ERROR("Unsupported image format ", img.toString() )
      return TexRef();
   }

   auto it = m_refs.find( name ); // Find 'name' in cache
   if (it != m_refs.end()) { return it->second; } // Cache hit

   auto tex = Tex2D::create( m_unitManager, name, img, so, keepImage );
   if ( !tex )
   {
      return TexRef();
   }

   m_Textures.emplace_back( tex );
   m_refs[ name ] = TexRef(tex);
   return m_refs[ name ];

}

void
TexManager::initGL( TexUnitManager* unitManager, int logLevel )
{
   m_unitManager = unitManager;

   //::glPixelStorei( GL_UNPACK_ALIGNMENT, 1 ); GL_VALIDATE   // Affects glTexImage2D()
   //::glPixelStorei( GL_PACK_ALIGNMENT, 1 ); GL_VALIDATE     // Affects glReadPixels()
   m_maxTex2DSize = glGetIntegerDE( GL_MAX_TEXTURE_SIZE );
   if ( logLevel > 0 )
   {
      DE_DEBUG( "Tex2D.MaxSize = ", m_maxTex2DSize )
      if ( logLevel > 1 )
      {
         dump();
      }
   }
}

void
TexManager::destroyGL( int logLevel )
{
   for ( auto cached : m_Textures )
   {
      if ( !cached ) continue;
      delete cached;
   }

   m_Textures.clear();

   for ( auto cached : m_TexturesToRemove )
   {
      if ( !cached ) continue;
      cached->destroyTexture();
      delete cached;
   }

   m_TexturesToRemove.clear();
}


void
TexManager::dump()
{
   DE_DEBUG( "TexCache.Count = ", m_Textures.size() )
   for ( size_t i = 0; i < m_Textures.size(); ++i )
   {
      Tex* tex = m_Textures[ i ];
      if ( tex )
      {
         DE_DEBUG( "TexCache[",i,"] = ", tex->toString() )
      }
      else
      {
         DE_ERROR( "TexCache[",i,"] = nullptr" )
      }
   }
}

bool
TexManager::upload( Tex* tex, Image const & img )
{
   if ( !tex )
   {
      return false;
   }

   return tex->upload( img );
}

void
TexManager::updateTextures()
{
   for ( size_t i = 0; i < m_TexturesToRemove.size(); ++i )
   {
      Tex* markedAsTrash = m_TexturesToRemove[ i ];
      if ( markedAsTrash )
      {
         markedAsTrash->destroyTexture();
         delete markedAsTrash;
      }
   }
   m_TexturesToRemove.clear();
}

void
TexManager::clearTextures()
{
   for ( size_t i = 0; i < m_Textures.size(); ++i )
   {
      Tex* cached = m_Textures[ i ];
      if ( cached )
      {
         DE_DEBUG( "MarkAsObsolete Tex[",i,"] ", cached->toString() )
         m_TexturesToRemove.emplace_back( cached );
      }
   }
   m_Textures.clear();
}

void
TexManager::removeTexture( std::string const & name )
{
   auto ref = getTexture( name );
   removeTexture( ref.m_tex );
}

void
TexManager::removeTexture( Tex* tex )
{
   if ( !tex ) { DE_DEBUG("No tex") return; }
   if ( !m_unitManager ) { DE_DEBUG("No m_unitManager") return; }

   // Delete cache entry
   auto cacheIt = std::find_if(
         m_Textures.begin(),
         m_Textures.end(),
         [&] ( Tex const * const p ) { return p == tex; } );
   if ( cacheIt == m_Textures.end() )
   {
      return;
   }

   Tex* cached = *cacheIt;
   if ( cached )
   {
      DE_DEBUG("[TexCache] DeleteEntry ", cached->toString() )
   }
   m_unitManager->unbindTexture( tex );
   m_Textures.erase( cacheIt );
   m_TexturesToRemove.emplace_back( tex );
}



// int32_t findTex( Tex* tex ) const;
// int32_t findTex( std::string const & name ) const;
// bool hasTex( std::string const & name ) const;
// TexRef getTex( std::string const & name ) const;
// TexRef load2D( std::string const & name, std::string uri, bool keepImage = true, SO so = SO() );
// TexRef add2D( std::string const & name, Image const & img, bool keepImage = true, SO so = SO() );
uint32_t
TexManager::getMaxTex2DSize() const
{
   return m_maxTex2DSize;
}

Tex*
TexManager::findTexture( std::string const & name ) const
{
   return nullptr;
}

bool
TexManager::hasTexture( std::string const & name ) const
{
   auto it = m_refs.find( name ); // Find 'name' in cache
   if (it == m_refs.end()) // Cache miss => try load ( every time, yet )
   {
      return false;
   }
   else
   {
      return true;
   }
}

