#include <de_gpu/de_TexManager_GL.h>
#include <de_image_formats/DarkImage.h>
#include <de_gpu/de_Tex2D_GL.h>
#include <de_core/de_RuntimeError.h>

namespace de {

// ===========================================================================
GL_TexManager::GL_TexManager()
// ===========================================================================
   : m_maxTex2DSize(1024)
{
   //init();
}

GL_TexManager::~GL_TexManager()
{
   //unbindAll();
}

void GL_TexManager::initTexManager( int logLevel )
{
   //::glPixelStorei( GL_UNPACK_ALIGNMENT, 1 ); GL_VALIDATE   // Affects glTexImage2D()
   //::glPixelStorei( GL_PACK_ALIGNMENT, 1 ); GL_VALIDATE     // Affects glReadPixels()
   auto nTexUnits = uint32_t( de_glGetInteger( GL_MAX_TEXTURE_IMAGE_UNITS ) );
   m_TexUnits.clear();
   m_TexUnits.resize( nTexUnits, 0 );
   if ( logLevel > 0 )
   {
      DE_DEBUG( "HardwareTexUnitCount = ", nTexUnits )
   }


   //::glPixelStorei( GL_UNPACK_ALIGNMENT, 1 ); GL_VALIDATE   // Affects glTexImage2D()
   //::glPixelStorei( GL_PACK_ALIGNMENT, 1 ); GL_VALIDATE     // Affects glReadPixels()
   m_maxTex2DSize = uint32_t( de_glGetInteger( GL_MAX_TEXTURE_SIZE ) );
   if ( logLevel > 0 )
   {
      DE_DEBUG( "Tex2D.MaxSize = ", m_maxTex2DSize )
      if ( logLevel > 1 )
      {
         dumpTexManager();
      }
   }
}

void
GL_TexManager::freeTexManager()
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
      cached->destroy();
      delete cached;
   }

   m_TexturesToRemove.clear();
}

void
GL_TexManager::dumpTexManager()
{
   DE_DEBUG( "TexUnit.Count = ", m_TexUnits.size() )
   for ( size_t i = 0; i < m_TexUnits.size(); ++i )
   {
      DE_DEBUG( "TexUnit[",i,"] = ", m_TexUnits[ i ] )
   }

   DE_DEBUG( "TexCache.Count = ", m_Textures.size() )
   for ( size_t i = 0; i < m_Textures.size(); ++i )
   {
      Tex* tex = m_Textures[ i ];
      if ( tex )
      {
         DE_DEBUG( "TexCache[",i,"] = ", tex->str() )
      }
      else
      {
         DE_ERROR( "TexCache[",i,"] = nullptr" )
      }
   }
}


// int32_t findTex( Tex* tex ) const;
// int32_t findTex( std::string const & name ) const;
// bool hasTex( std::string const & name ) const;
// TexRef getTex( std::string const & name ) const;
// TexRef load2D( std::string const & name, std::string uri, bool keepImage = true, TexOptions so = TexOptions() );
// TexRef add2D( std::string const & name, Image const & img, bool keepImage = true, TexOptions so = TexOptions() );
uint32_t
GL_TexManager::getMaxTex2DSize() const
{
   return m_maxTex2DSize;
}

uint32_t
GL_TexManager::getUnitCount() const
{
   return m_TexUnits.size();
}

bool
GL_TexManager::hasTexture( std::string const & name ) const
{
   auto it = m_lutTexNames.find( name ); // Find 'name' in cache
   if (it == m_lutTexNames.end()) // Cache miss => try load ( every time, yet )
   {
      return false;
   }
   else
   {
      return true;
   }
}

Tex*
GL_TexManager::getTexture( std::string const & name )
{
   // We abort on empty string, nothing todo, return invalid texref.
   if ( name.empty() ) { return nullptr; }

   // Ask cache if ever heard of 'name' before...
   auto it = m_lutTexNames.find( name );

   // (cachemiss)...
   // If cache has no string 'name' then it always tries to load
   // the given filename 'name'. This 'always loading' will be bad
   // behaviour if the ressource file does not exist.
   if (it == m_lutTexNames.end())
   {
      DE_RUNTIME_ERROR(name,"The texture does not exist, call createTexture before using it.")
      return nullptr; // Cache Miss
   }
   else
   {
      return it->second; // Cache Hit
   }
}

Tex*
GL_TexManager::loadTexture( std::string const & name, TexOptions so )
{
   de::Image img;
   if ( !dbLoadImage( img, name ) )
   {
      DE_RUNTIME_ERROR(name,"Cant load image for texture")
      //DE_ERROR("No image loaded for texture ", name )
      //return nullptr;
   }

   return createTexture( name, img, so );
}

Tex*
GL_TexManager::createTexture(
   std::string const & name,
   Image const & img,
   TexOptions so )
{
   if ( name.empty() )
   {
      DE_RUNTIME_ERROR("","Cant create texture with empty name")
      //return nullptr;
   }

   if ( img.empty() )
   {
      DE_RUNTIME_ERROR(name,"Cant create texture with empty image (yet)")
      //return nullptr;
   }

   if ( img.getFormat() != PixelFormat::R8G8B8A8 &&
        img.getFormat() != PixelFormat::R8G8B8 )
   {
      DE_RUNTIME_ERROR(img.getFormatStr(),"Unsupported image format to create texture (yet)")
      //return nullptr;
   }

   auto it = m_lutTexNames.find( name ); // Find 'name' in cache
   if (it != m_lutTexNames.end())
   {
      DE_RUNTIME_ERROR(name,"Texture already exist")
      //return it->second; // Cache hit
   }

   auto tex = new GL_Tex2D();
   if ( !tex->createFromImage( name, img, img.getFormat(), so ) )
   {
      delete tex;
      DE_RUNTIME_ERROR(name,"Cant create texture from this image")
      //return nullptr;
   }

   m_Textures.emplace_back( tex );
   m_lutTexNames[ name ] = tex;
   return m_lutTexNames[ name ];
}

bool
GL_TexManager::upload( Tex* tex, Image const & img )
{
   if ( !tex )
   {
      return false;
   }

   return tex->upload( img.w(), img.h(), img.data(), img.getFormat() );
}

void
GL_TexManager::updateTextures()
{
   for ( size_t i = 0; i < m_TexturesToRemove.size(); ++i )
   {
      Tex* markedAsTrash = m_TexturesToRemove[ i ];
      if ( markedAsTrash )
      {
         markedAsTrash->destroy();
         delete markedAsTrash;
      }
   }
   m_TexturesToRemove.clear();
}

void
GL_TexManager::clearTextures()
{
   for ( size_t i = 0; i < m_Textures.size(); ++i )
   {
      Tex* cached = m_Textures[ i ];
      if ( cached )
      {
         DE_DEBUG( "MarkAsObsolete Tex[",i,"] ", cached->str() )
         m_TexturesToRemove.emplace_back( cached );
      }
   }
   m_Textures.clear();
}

void
GL_TexManager::removeTexture( std::string const & name )
{
   removeTexture( getTexture( name ) );
}

void
GL_TexManager::removeTexture( Tex* tex )
{
   if ( !tex )
   {
      DE_DEBUG("Got nullptr")
      return;
   }

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
      DE_DEBUG("[TexCache] DeleteEntry ", cached->str() )
   }
   unbindTexture( tex );
   m_lutTexNames.erase( cached->getName() );
   m_Textures.erase( cacheIt );
   m_TexturesToRemove.emplace_back( tex );
}

int32_t
GL_TexManager::findUnit( uint32_t texId ) const
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
GL_TexManager::bindTextureId( int stage, uint32_t texId )
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
   de_glActiveTexture( GL_TEXTURE0 + stage );
   de_glBindTexture( GL_TEXTURE_2D, texId );
//   bool ok = GL_VALIDATE;
//   if ( !ok )
//   {
//      throw std::runtime_error( dbStrJoin( "No stage(",stage,") and texId(",texId,")" ) );
//   }
   // ShaderUtil::setSamplerOptions2D( tex->getSamplerOptions() );
   return true;
}

bool
GL_TexManager::unbindTextureId( uint32_t texId )
{
   if ( texId < 1 ) return true;  // Nothing to unbind.

   // Search unit with this tex...
   int found = findUnit( texId );
   if ( found < 0 )
   {
      return true; // Nothing to unbind.
   }

   m_TexUnits[ found ] = 0;
   de_glActiveTexture( GL_TEXTURE0 + found );
   de_glBindTexture( GL_TEXTURE_2D, 0 );
   // DE_DEBUG( "Unbound texture ", texId, " from stage ", found, " and target=", target )
   return true;
}


// More intelligent GL: stage/unit is auto select.
// AutoSelect a free tex unit and return its index for use in setSamplerUniform().
int
GL_TexManager::bindTextureId( uint32_t texId )
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
GL_TexManager::bindTexture( int stage, Tex* tex )
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
GL_TexManager::bindTexture( Tex* tex )
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
GL_TexManager::unbindTexture( Tex* tex )
{
   if ( !tex ) { return true; }
   if ( tex->getType() == TexType::Tex2D )
   {
      if ( !unbindTextureId( tex->getTextureId() ) )
      {
         DE_ERROR("Cant unbind tex ", tex->str())
         return false;
      }
      else
      {
         return true;
      }
   }
   else
   {
      DE_ERROR("Unsupported tex type ", tex->str())
      return false;
   }
}

} // end namespace de.
