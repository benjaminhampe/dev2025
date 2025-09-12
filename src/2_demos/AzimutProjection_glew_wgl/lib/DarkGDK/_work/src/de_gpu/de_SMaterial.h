#pragma once
//#include <de_gpu/de_Math3D.h>
#include <de_gpu/de_State.h>
#include <de_gpu/de_TexRef.h>
#include <tinyxml2/tinyxml2.h>

namespace de {

// ===========================================================================
struct SMaterialLight
// ===========================================================================
{
   int typ;
   glm::vec3 siz;   // world size of light source
   glm::vec3 pos;
   glm::vec3 color;
   glm::vec3 dir;
   float maxdist;    // distance
   //float power;      // distance
   //float m_Radius = 500.0f;
   glm::vec3 clipPos; // for Shader, computed by CPU

   //SMeshBuffer mesh;
};

// ===========================================================================
// Material = State + Textures + Fog + Transparenz + Lighting = TnL 1-4 textures
// ===========================================================================
//# 3ds Max Wavefront OBJ Exporter v0.97b - (c)2007 guruware
//# File Created: 10.01.2012 12:09:34
//
//newmtl 12993_diffuse
//	Ns 30.0000
//	Ni 1.5000
//	d 1.0000
//	Tr 0.0000
//	illum 2
//	Ka 1.0000 1.0000 1.0000    //	Ka 0.5882 0.5882 0.5882
//	Kd 1.0000 1.0000 1.0000    //	Kd 0.5882 0.5882 0.5882
//	Ks 0.3600 0.3600 0.3600    //	Ks 0.0000 0.0000 0.0000
//	Ke 0.0000 0.0000 0.0000    //	Ke 0.0000 0.0000 0.0000
//	Tf 1.0000 1.0000 1.0000
//	map_Ka fish.jpg            //	map_Ka moon_diffuse.jpg
//	map_Kd fish.jpg            //	map_Kd moon_diffuse.jpg
//	map_bump moon_normal.jpg
//	bump moon_normal.jpg
//
//      Illumination Properties that are turned on in the model Property Editor
//      0 Color on and Ambient off
//      1 Color on and Ambient on
//      2 Highlight on
//      3 Reflection on and Ray trace on
//      4 Transparency: Glass on, Reflection: Ray trace on
//      5 Reflection: Fresnel on and Ray trace on
//      6 Transparency: Refraction on, Reflection: Fresnel off and Ray trace on
//      7 Transparency: Refraction on, Reflection: Fresnel on and Ray trace on
//      8 Reflection on and Ray trace off
//      9 Transparency: Glass on, Reflection: Ray trace off
//      10 Casts shadows onto invisible surfaces
//

// ===========================================================================
struct SMaterial
// ===========================================================================
{
   DE_CREATE_LOGGER("de_gpu.SMaterial")
   State state;
   int Lighting;   // 2 Highlight on
   bool Wireframe;
   bool CloudTransparent;
   bool FogEnable;
   uint32_t FogColor;
   uint32_t FogMode;
   float FogLinearStart;
   float FogLinearEnd;
   float FogExpDensity;
   glm::vec4 Kd; // diffuse_color; // Kd
   glm::vec4 Ka; // ambient_color; // Ka
   glm::vec4 Ks; // specular_color; // Ks
   glm::vec4 Ke; // emissive_color; // Ke
   glm::vec4 Kt; // TransparentColor; // Kt, Tf
   glm::vec4 Kr; // ReflectiveColor = Emissive?
   //uint32_t TransparentColor; // Kt, Tf
   //uint32_t ReflectiveColor; // Kr
   float Opacity; // d in MTL?
   float Reflectivity; // Tr
   float Shininess; // Ns
   float Shininess_strength; // Ni
   // 3ds
   uint32_t MaterialType; // EMT_SOLID, EMT_TRANSPARENT_VERTEX_ALPHA
   bool GouraudShading;
   TexRef DiffuseMap;
   TexRef BumpMap;
   float BumpScale;
   TexRef NormalMap;
   float NormalScale;
   TexRef SpecularMap;
   TexRef DetailMap;
   std::string Name;

   SMaterial();

   std::string toString() const;
   uint32_t getTextureCount() const;
   bool hasTexture() const;
   TexRef const & getTexture( int stage ) const;
   TexRef & getTexture( int stage );
   void setTexture( int stage, TexRef const & ref );

   std::vector< TexRef > getTextures() const
   {
      std::vector< TexRef > textures;
      if ( !DiffuseMap.empty() ) textures.emplace_back( DiffuseMap );
      if ( !BumpMap.empty() ) textures.emplace_back( BumpMap );
      if ( !NormalMap.empty() ) textures.emplace_back( NormalMap );
      if ( !SpecularMap.empty() ) textures.emplace_back( SpecularMap );
      if ( !DetailMap.empty() ) textures.emplace_back( DetailMap );
      return textures;
   }


   void setFog( bool enabled ) { FogEnable = enabled; }
   void setLighting( int illum ) { Lighting = illum; }
   void setWireframe( bool enabled ) { Wireframe = enabled; }
   void setCulling( bool enable ) { state.culling.setEnabled( enable ); }
   void setDepth( bool enable ) { state.depth.setEnabled( enable ); }
   void setStencil( bool enable ) { state.stencil.setEnabled( enable ); }
   void setBlend( bool enable ) { state.blend.setEnabled( enable ); }
   void setCulling( Culling const & cull ) { state.culling = cull; }
   void setDepth( Depth const & depth ) { state.depth = depth; }
   void setStencil( Stencil const & stencil ) { state.stencil = stencil; }
   void setBlend( Blend const & blend ) { state.blend = blend; }
   void setTexture( int stage, Tex* tex ) { setTexture( stage, TexRef(tex) ); }
   void setBumpScale( float scale ) { BumpScale = scale; }
   void setDiffuseMap( TexRef ref ) { DiffuseMap = ref; }
   void setBumpMap( TexRef ref ) { BumpMap = ref; }
   void setNormalMap( TexRef ref ) { NormalMap = ref; }
   void setSpecularMap( TexRef ref ) { SpecularMap = ref; }
   void setDiffuseMap( Tex* tex ) { DiffuseMap = TexRef( tex ); }
   void setBumpMap( Tex* tex ) { BumpMap = TexRef( tex ); }
   void setNormalMap( Tex* tex ) { NormalMap = TexRef( tex ); }
   void setSpecularMap( Tex* tex ) { SpecularMap = TexRef( tex ); }
   bool hasDiffuseMap() const { return !DiffuseMap.empty(); }
   bool hasBumpMap() const { return !BumpMap.empty(); }
   bool hasNormalMap() const { return !NormalMap.empty(); }
   bool hasSpecularMap() const { return !SpecularMap.empty(); }
   bool hasDetailMap() const { return !DetailMap.empty(); }
   TexRef const & getDiffuseMap() const { return DiffuseMap; }
   TexRef const & getBumpMap() const { return BumpMap; }
   TexRef const & getNormalMap() const { return NormalMap; }
   TexRef const & getSpecularMap() const { return SpecularMap; }
   TexRef const & getDetailMap() const { return DetailMap; }
   TexRef & getDiffuseMap() { return DiffuseMap; }
   TexRef & getBumpMap() { return BumpMap; }
   TexRef & getNormalMap() { return NormalMap; }
   TexRef & getSpecularMap() { return SpecularMap; }
   TexRef & getDetailMap() { return DetailMap; }
//   bool
//   operator==( Material const & o ) const;
//   bool
//   operator!= ( Material const & o ) const { return !( o == *this ); }
//   bool
//   loadMTL( std::string uri, VideoDriver* driver );
//   bool
//   saveMTL( std::string uri ) const;
   bool
   writeXML( tinyxml2::XMLDocument & doc, tinyxml2::XMLElement* matNode ) const;
   bool
   saveXML( std::string uri ) const;
   bool
   loadXML( std::string uri );
};

} // end namespace de.

/*
inline std::ostream &
operator<< ( std::ostream & o, de::gpu::Uniform const & uniform )
{
   o << uniform.toString();
   return o;
}
*/
