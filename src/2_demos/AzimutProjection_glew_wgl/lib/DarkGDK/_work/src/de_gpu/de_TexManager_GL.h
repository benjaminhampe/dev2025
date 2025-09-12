#pragma once
#include <de_gpu/de_TexManager.h>
#include <de_gpu/de_GL_debug_layer.h>

namespace de {

// ===========================================================================
struct GL_TexManager : public TexManager
// ===========================================================================
{
   DE_CREATE_LOGGER("de_gpu.GL_TexManager")

   GL_TexManager();
   ~GL_TexManager() override;

   void initTexManager(int logLevel = 0) override;
   void freeTexManager() override;
   void dumpTexManager() override;

   // ##########################
   // ###   TexUnitManager   ###
   // ##########################

   uint32_t getUnitCount() const override;
   int32_t  findUnit( uint32_t texId ) const override;

   bool     bindTextureId( int stage, uint32_t texId ) override;
   bool     bindTexture( int stage, Tex* tex ) override;

   // Benni's more intelligent calls for raw/higher level TexClass.
   // AutoSelect a free tex unit and return its index for use in setSamplerUniform().
   int      bindTextureId( uint32_t texId ) override;
   bool     unbindTextureId( uint32_t texId ) override;

   int      bindTexture( Tex* tex ) override;
   bool     unbindTexture( Tex* tex ) override;

   // ######################
   // ###   GL_TexManager   ###
   // ######################

   //Tex*   findTexture( std::string const & name ) const override;
   bool     hasTexture( std::string const & name ) const override;
   Tex*     getTexture( std::string const & name ) override;
   Tex*     loadTexture( std::string const & name, TexOptions so = TexOptions() ) override;
   Tex*     createTexture( std::string const & name, Image const & img, TexOptions so = TexOptions() ) override;

   bool     upload( Tex* tex, Image const & src ) override;
   void     updateTextures() override;
   void     clearTextures() override;
   void     removeTexture( std::string const & name ) override;
   void     removeTexture( Tex* tex ) override;

   uint32_t getMaxTex2DSize() const override;

   // int32_t findTex( Tex* tex ) const;
   // int32_t findTex( std::string const & name ) const;
   // bool hasTex( std::string const & name ) const;
   // TexRef getTex( std::string const & name ) const;
   // TexRef load2D( std::string const & name, std::string uri, bool keepImage = true, TexOptions so = TexOptions() );
   // TexRef add2D( std::string const & name, Image const & img, bool keepImage = true, TexOptions so = TexOptions() );

   // ###   UnitManager    ###
   std::vector< uint32_t > m_TexUnits; // Physical avail texture Units
   // ###   GL_TexManager   ###
   uint32_t m_maxTex2DSize;
   std::vector< Tex* > m_Textures; // All big memory pages ( atlas textures )
   std::vector< Tex* > m_TexturesToRemove;   // All textures that exist
   std::unordered_map< std::string, Tex* > m_lutTexNames;
   //std::unordered_map< std::string, TexRef > m_refs; // has indices into m_Textures.

   // ###   RefManager   ###
   //std::vector< TexRef > m_Refs;    // All small textures we can use
   //std::unordered_map< std::string, int > m_RefLUT; // has indices into m_TexRefs
};

} // end namespace de.
