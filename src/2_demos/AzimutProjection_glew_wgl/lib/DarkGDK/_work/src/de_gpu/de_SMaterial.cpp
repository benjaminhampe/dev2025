#include <de_gpu/de_SMaterial.h>

namespace de {

SMaterial::SMaterial()
   : state()
   , Lighting( 0 )
   , Wireframe( false ) // ? Not used, wireframes are overlays of debug geom
   , CloudTransparent( false ) // Benni special. uses only tex.r channel as color(r,r,r,r)
   , FogEnable( false )
   , FogColor( 0xFFFFFFFF )
   , FogMode( 0 )
   , FogLinearStart( 0.0f )
   , FogLinearEnd( 1.0f )
   , FogExpDensity( 0.001f )
   , Kd( 1,1,1,1 )
   , Ka( 0,0,0,0 )
   , Ks( 1,1,1,1 )
   , Ke( 0,0,0,0 )
   , Kt( 1,1,1,1 ) // ?
   , Kr( 1,1,1,1 ) // ?
   , Opacity( 1.0f ) // ?
   , Reflectivity( 0.1f ) // ?
   , Shininess( 30.0f ) // ?
   , Shininess_strength( 1.5f ) // ?
   , MaterialType( 0 ) // ?
   , GouraudShading( false ) // ?
   , DiffuseMap()
   , BumpMap()
   , BumpScale( 1.0f )
   , NormalMap()
   , NormalScale( 1.0f )
   , SpecularMap()
   , DetailMap()
   , Name("")
{} // Default ctr constructed is also always the default material.

std::string
SMaterial::toString() const
{
   std::ostringstream s;

   s << "Tex" << getTextureCount() << ", ";
   if ( state.culling.isEnabled() ) { s << state.culling.toString(); }
   if ( Wireframe ){ s << ", W"; }
   if ( FogEnable ){ s << ", F"; }
   //	illum 2
   if ( Lighting > 0) { s << ", illum"<<Lighting; }
   s << ", Ns:" << Shininess;   //	Ns 30.0000
   s << ", Ni:" << Shininess_strength;  //	Ni 1.5000
   s << ", Tr:" << Reflectivity;   //	Tr 0.0000
   if ( hasDiffuseMap() ){ s << ", Td:" << getDiffuseMap().toString(); }
   if ( hasBumpMap() ){ s << ", Tb:" << getBumpMap().toString(); }
   if ( hasNormalMap() ){ s << ", Tn:" << getNormalMap().toString(); }
   if ( hasSpecularMap() ){ s << ", Ts:" << getSpecularMap().toString(); }
   if ( hasDetailMap() ){ s << ", Tdet:" << getDetailMap().toString(); }
   //   { s << ", Ka:" << Ka; }   //	Ka 0.5882 0.5882 0.5882
   //   { s << ", Kd:" << Kd; }   //	Kd 1.0000 1.0000 1.0000
   //   { s << ", Ks:" << Ks; }   //	Ks 0.3600 0.3600 0.3600
   //   { s << ", Ke:" << Ke; }   //	Ke 0.0000 0.0000 0.0000
   //   { s << ", Kt:" << Kt; }
   //	Tf 1.0000 1.0000 1.0000
   //	d 1.0000
   return s.str();
}

//void setFog( bool enabled ) { FogEnable = enabled; }
//void setLighting( int illum ) { Lighting = illum; }
//void setWireframe( bool enabled ) { Wireframe = enabled; }
//void setCulling( bool enable ) { state.culling.setEnabled( enable ); }
//void setDepth( bool enable ) { state.depth.setEnabled( enable ); }
//void setStencil( bool enable ) { state.stencil.setEnabled( enable ); }
//void setBlend( bool enable ) { state.blend.setEnabled( enable ); }
//void setCulling( Culling const & cull ) { state.culling = cull; }
//void setDepth( Depth const & depth ) { state.depth = depth; }
//void setStencil( Stencil const & stencil ) { state.stencil = stencil; }
//void setBlend( Blend const & blend ) { state.blend = blend; }

uint32_t
SMaterial::getTextureCount() const
{
   uint32_t n = 0;
   n += DiffuseMap.empty() ? 0 : 1;
   n += BumpMap.empty() ? 0 : 1;
   n += NormalMap.empty() ? 0 : 1;
   n += SpecularMap.empty() ? 0 : 1;
   n += DetailMap.empty() ? 0 : 1;
   return n;
}

bool
SMaterial::hasTexture() const { return getTextureCount() > 0; }

TexRef const &
SMaterial::getTexture( int stage ) const
{
        if ( stage == 0 ) { return DiffuseMap; }
   else if ( stage == 1 ) { return BumpMap; }
   else if ( stage == 2 ) { return NormalMap; }
   else if ( stage == 3 ) { return SpecularMap; }
   else if ( stage == 4 ) { return DetailMap; }
   else
   {
      //DE_RUNTIME_ERROR( std::to_string( stage ), "Invalid tex stage" )
      DE_ERROR( "SMaterial(",Name,") :: Bad stage index " + std::to_string( stage ) )
      return DiffuseMap;
   }
}

TexRef &
SMaterial::getTexture( int stage )
{
        if ( stage == 0 ) { return DiffuseMap; }
   else if ( stage == 1 ) { return BumpMap; }
   else if ( stage == 2 ) { return NormalMap; }
   else if ( stage == 3 ) { return SpecularMap; }
   else if ( stage == 4 ) { return DetailMap; }
   else
   {
      //DE_RUNTIME_ERROR( std::to_string( stage ), "Invalid tex stage" )
      DE_ERROR( "SMaterial(",Name,") :: Bad stage index " + std::to_string( stage ) )
      return DiffuseMap;
   }
}

void SMaterial::setTexture( int stage, TexRef const & ref )
{
        if ( stage == 0 ) { DiffuseMap = ref; }
   else if ( stage == 1 ) { BumpMap = ref; }
   else if ( stage == 2 ) { NormalMap = ref; }
   else if ( stage == 3 ) { SpecularMap = ref; }
   else if ( stage == 4 ) { DetailMap = ref; }
   else
   {
      //throw std::runtime_error( "Invalid tex stage " + std::to_string( stage ) );
      DE_ERROR( "SMaterial(",Name,") :: Bad stage index " + std::to_string( stage ) )
   }
}

//void setTexture( int stage, Tex* tex ) { setTexture( stage, TexRef(tex) ); }
//void setBumpScale( float scale ) { BumpScale = scale; }

//void setDiffuseMap( TexRef ref ) { DiffuseMap = ref; }
//void setBumpMap( TexRef ref ) { BumpMap = ref; }
//void setNormalMap( TexRef ref ) { NormalMap = ref; }
//void setSpecularMap( TexRef ref ) { SpecularMap = ref; }

//void setDiffuseMap( Tex* tex ) { DiffuseMap = TexRef( tex ); }
//void setBumpMap( Tex* tex ) { BumpMap = TexRef( tex ); }
//void setNormalMap( Tex* tex ) { NormalMap = TexRef( tex ); }
//void setSpecularMap( Tex* tex ) { SpecularMap = TexRef( tex ); }

//bool hasDiffuseMap() const { return !DiffuseMap.empty(); }
//bool hasBumpMap() const { return !BumpMap.empty(); }
//bool hasNormalMap() const { return !NormalMap.empty(); }
//bool hasSpecularMap() const { return !SpecularMap.empty(); }
//bool hasDetailMap() const { return !DetailMap.empty(); }

//TexRef const & getDiffuseMap() const { return DiffuseMap; }
//TexRef const & getBumpMap() const { return BumpMap; }
//TexRef const & getNormalMap() const { return NormalMap; }
//TexRef const & getSpecularMap() const { return SpecularMap; }
//TexRef const & getDetailMap() const { return DetailMap; }

//TexRef & getDiffuseMap() { return DiffuseMap; }
//TexRef & getBumpMap() { return BumpMap; }
//TexRef & getNormalMap() { return NormalMap; }
//TexRef & getSpecularMap() { return SpecularMap; }
//TexRef & getDetailMap() { return DetailMap; }

//   bool
//   operator==( SMaterial const & o ) const
//   {
//      if ( state != o.state ) return false;
//      if ( illum != o.illum ) return false;
//      if ( fog != o.fog ) return false;
//      if ( wireframe != o.wireframe ) return false;
//      if ( state != o.state ) return false;
//      if ( cloud_transparent != o.cloud_transparent ) return false;
//      if ( m_DiffuseMap.tex != o.m_DiffuseMap.tex ) return false;
//      if ( m_BumpMap.tex != o.m_BumpMap.tex ) return false;
//      if ( m_NormalMap.tex != o.m_NormalMap.tex ) return false;
//      if ( m_SpecularMap.tex != o.m_SpecularMap.tex ) return false;
//      if ( m_DetailMap.tex != o.m_DetailMap.tex ) return false;
//      return true;
//   }

//   bool
//   operator!= ( SMaterial const & o ) const { return !( o == *this ); }

//# 3ds Max Wavefront OBJ Exporter v0.97b - (c)2007 guruware
//# File Created: 10.01.2012 12:09:34

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
//      Illumination Properties that are turned on in the
//      model Property Editor
//      0 Color on and Ambient off
//      1 Color on and Ambient on
//      2 Highlight on
//      3 Reflection on and Ray trace on
//      4 Transparency: Glass on //      Reflection: Ray trace on
//      5 Reflection: Fresnel on and Ray trace on
//      6 Transparency: Refraction on  //      Reflection: Fresnel off and Ray trace on
//      7 Transparency: Refraction on //      Reflection: Fresnel on and Ray trace on
//      8 Reflection on and Ray trace off
//      9 Transparency: Glass on //      Reflection: Ray trace off
//      10 Casts shadows onto invisible surfaces

//   bool
//   loadMTL( std::string uri, VideoDriver* driver );

//   bool
//   saveMTL( std::string uri ) const;


bool
SMaterial::writeXML( tinyxml2::XMLDocument & doc, tinyxml2::XMLElement* matNode ) const
{
   tinyxml2::XMLElement* cullNode = doc.NewElement( "culling" );
   cullNode->SetAttribute("on", state.culling.isEnabled() );
   cullNode->SetAttribute("front", state.culling.isFront() );
   cullNode->SetAttribute("back", state.culling.isFront() );
   cullNode->SetAttribute("cw", state.culling.isCW() );
   matNode->InsertEndChild( cullNode );

   tinyxml2::XMLElement* blendNode = doc.NewElement( "blend" );
   blendNode->SetAttribute("on", state.blend.enabled );
   blendNode->SetAttribute("eq", Blend::getEquationString(state.blend.equation).c_str() );
   blendNode->SetAttribute("src_rgb", Blend::getFunctionString(state.blend.src_rgb).c_str() );
   blendNode->SetAttribute("src_a", Blend::getFunctionString(state.blend.src_a).c_str() );
   blendNode->SetAttribute("dst_rgb", Blend::getFunctionString(state.blend.dst_rgb).c_str() );
   blendNode->SetAttribute("dst_a", Blend::getFunctionString(state.blend.dst_a).c_str() );
   matNode->InsertEndChild( blendNode );

   tinyxml2::XMLElement* depthNode = doc.NewElement( "depth" );
   depthNode->SetAttribute("on", state.depth.isEnabled() );
   depthNode->SetAttribute("zwrite", state.depth.isZWriteEnabled() );
   depthNode->SetAttribute("fn", Depth::getString( state.depth.getFunc() ).c_str() );
   matNode->InsertEndChild( depthNode );

   tinyxml2::XMLElement* stencilNode = doc.NewElement( "stencil" );
   stencilNode->SetAttribute("on", state.stencil.enabled );
   stencilNode->SetAttribute("sfail", Stencil::getActionString( state.stencil.sfail ).c_str() );
   stencilNode->SetAttribute("zfail", Stencil::getActionString( state.stencil.zfail ).c_str() );
   stencilNode->SetAttribute("zpass", Stencil::getActionString( state.stencil.zpass ).c_str() );
   stencilNode->SetAttribute("func", Stencil::getFuncString( state.stencil.testFunc ).c_str() );
   stencilNode->SetAttribute("refValue", state.stencil.testRefValue );
   stencilNode->SetAttribute("testMask", state.stencil.testMask );
   stencilNode->SetAttribute("stencilMask", state.stencil.stencilMask );
   matNode->InsertEndChild( stencilNode );

   tinyxml2::XMLElement* wireNode = doc.NewElement( "wireframe" );
   wireNode->SetAttribute("on", Wireframe );
   matNode->InsertEndChild( wireNode );

   tinyxml2::XMLElement* cloudtNode = doc.NewElement( "cloud" );
   cloudtNode->SetAttribute("on", CloudTransparent );
   matNode->InsertEndChild( cloudtNode );

   tinyxml2::XMLElement* illumNode = doc.NewElement( "illumination" );
   illumNode->SetAttribute("v", Lighting );
   matNode->InsertEndChild( illumNode );

   tinyxml2::XMLElement* fogNode = doc.NewElement( "fog" );
   fogNode->SetAttribute("v", FogEnable );
   matNode->InsertEndChild( fogNode );

   //	Ka 0.5882 0.5882 0.5882
   //	Kd 1.0000 1.0000 1.0000
   //	Ks 0.3600 0.3600 0.3600
   //	Ke 0.0000 0.0000 0.0000

   tinyxml2::XMLElement* ambientNode = doc.NewElement( "Ka" );
   ambientNode->SetAttribute("r", Ka.r );
   ambientNode->SetAttribute("g", Ka.g );
   ambientNode->SetAttribute("b", Ka.b );
   ambientNode->SetAttribute("a", Ka.a );
   matNode->InsertEndChild( ambientNode );

   tinyxml2::XMLElement* diffuseNode = doc.NewElement( "Kd" );
   diffuseNode->SetAttribute("r", Kd.r );
   diffuseNode->SetAttribute("g", Kd.g );
   diffuseNode->SetAttribute("b", Kd.b );
   diffuseNode->SetAttribute("a", Kd.a );
   matNode->InsertEndChild( diffuseNode );

   tinyxml2::XMLElement* specularNode = doc.NewElement( "Ks" );
   specularNode->SetAttribute("r", Ks.r );
   specularNode->SetAttribute("g", Ks.g );
   specularNode->SetAttribute("b", Ks.b );
   specularNode->SetAttribute("a", Ks.a );
   matNode->InsertEndChild( specularNode );

   tinyxml2::XMLElement* emissiveNode = doc.NewElement( "Ke" );
   emissiveNode->SetAttribute("r", Ke.r );
   emissiveNode->SetAttribute("g", Ke.g );
   emissiveNode->SetAttribute("b", Ke.b );
   emissiveNode->SetAttribute("a", Ke.a );
   matNode->InsertEndChild( emissiveNode );

   tinyxml2::XMLElement* reflectNode = doc.NewElement( "Kr" );
   reflectNode->SetAttribute("r", Kr.r );
   reflectNode->SetAttribute("g", Kr.g );
   reflectNode->SetAttribute("b", Kr.b );
   reflectNode->SetAttribute("a", Kr.a );
   matNode->InsertEndChild( reflectNode );

   tinyxml2::XMLElement* transparentNode = doc.NewElement( "Kt" );
   transparentNode->SetAttribute("r", Kt.r );
   transparentNode->SetAttribute("g", Kt.g );
   transparentNode->SetAttribute("b", Kt.b );
   transparentNode->SetAttribute("a", Kt.a );
   matNode->InsertEndChild( transparentNode );

   tinyxml2::XMLElement* shinyNode = doc.NewElement( "Ns" );
   shinyNode->SetAttribute("v", Shininess );
   matNode->InsertEndChild( shinyNode );

   tinyxml2::XMLElement* shinysNode = doc.NewElement( "Ni" );
   shinysNode->SetAttribute("v", Shininess_strength );
   matNode->InsertEndChild( shinysNode );

   tinyxml2::XMLElement* reflectivityNode = doc.NewElement( "Tr" );
   reflectivityNode->SetAttribute("v", Reflectivity );
   matNode->InsertEndChild( reflectivityNode );

   //	d 1.0000

//   if ( hasDiffuseMap() )
//   {
//      TexRef const & ref = getDiffuseMap();
//      tinyxml2::XMLElement* texNode = doc.NewElement( "Td" );
//      texNode->SetAttribute("name", ref.tex->getName().c_str() );
//      texNode->SetAttribute("src", ref.tex->getName().c_str() );
//      matNode->InsertEndChild( texNode );
//   }

//   if ( hasBumpMap() ){ s << ", Tb:" << getBumpMap().tex->getName(); }
//   if ( hasNormalMap() ){ s << ", Tn:" << getNormalMap().tex->getName(); }
//   if ( hasSpecularMap() ){ s << ", Ts:" << getSpecularMap().tex->getName(); }
//   if ( hasDetailMap() ){ s << ", Tdet:" << getDetailMap().tex->getName(); }
   return true;
}

bool
SMaterial::saveXML( std::string uri ) const
{
   tinyxml2::XMLDocument doc;
   tinyxml2::XMLElement* matNode = doc.NewElement( "SMaterial" );
   matNode->SetAttribute( "name", Name.c_str() );
   matNode->SetAttribute( "tex-count", int( getTextureCount() ) );
   writeXML( doc, matNode );

   doc.InsertEndChild( matNode );
   tinyxml2::XMLError err = doc.SaveFile( uri.c_str() );
   if ( err != tinyxml2::XML_SUCCESS )
   {
      DE_ERROR("Cant save xml ", uri)
      return false;
   }
   return true;
}

bool
SMaterial::loadXML( std::string uri )
{
   tinyxml2::XMLDocument doc;
   tinyxml2::XMLError err = doc.LoadFile( uri.c_str() );
   if ( err != tinyxml2::XML_SUCCESS )
   {
      DE_ERROR("Cant load xml ", uri)
      return false;
   }

/*
   std::string xmlDir = dbGetFileDir( uri );

   DE_DEBUG("=========================================" )
   DE_DEBUG("loadXml(",uri,")" )
   DE_DEBUG("=========================================" )
   //std::string baseName = dbGetFileBaseName( uri );
   //std::string dirName = dbGetFileDir( uri );
   //DE_DEBUG("URI baseName = ", baseName )
   //DE_DEBUG("URI dirName = ", dirName )

   tinyxml2::XMLElement* atlasNode = doc.FirstChildElement( "image-atlas" );
   if ( !atlasNode )
   {
      DE_ERROR("No image atlas node in xml ", uri)
      return false;
   }

   if ( atlasNode->Attribute( "name" ) )
   {
      Name = atlasNode->Attribute( "name" );
   }
   else
   {
      DE_ERROR("No atlas name found ", uri)
   }

   int pageCount = atlasNode->IntAttribute( "pages" );

   tinyxml2::XMLElement* pageNode = atlasNode->FirstChildElement( "page" );
   if ( !pageNode )
   {
      DE_ERROR("No image atlas pages found in xml ", uri)
      return false;
   }

   // Load Pages:
   while ( pageNode )
   {
      if ( !pageNode->Attribute("src") )
      {
         DE_ERROR("No page src" )
         continue;
      }

      int pageW = pageNode->IntAttribute("w");
      int pageH = pageNode->IntAttribute("h");
      pages.emplace_back();
      ImageAtlasPage & page = pages.back();
      page.pageId = int( pages.size() );
      page.name = pageNode->Attribute("src");

      std::string pageUri = xmlDir + "/" + page.name;
      if ( !dbLoadImage( page.img, pageUri ) )
      {
         DE_ERROR("No page ", page.pageId," src image uri ", pageUri)
         page.img.clear();
      }

      if ( pageW != page.img.getWidth() )
      {
         DE_ERROR("Differing src image width ", page.name)
      }
      if ( pageH != page.img.getHeight() )
      {
         DE_ERROR("Differing src image height ", page.name)
      }

      // Load Refs:
      int refCount = pageNode->IntAttribute("refs");

      // Load Refs:
      tinyxml2::XMLElement* refNode = pageNode->FirstChildElement( "img" );
      while ( refNode )
      {
         page.refs.emplace_back();
         ImageAtlasRef & ref = page.refs.back();

         int refId = refNode->IntAttribute("id");
         int refX = refNode->IntAttribute("x");
         int refY = refNode->IntAttribute("y");
         int refW = refNode->IntAttribute("w");
         int refH = refNode->IntAttribute("h");
         bool wantMipmaps = refNode->IntAttribute("mipmaps") != 0 ? true:false;
         std::string name = refNode->Attribute("name" );
         ref.id = refId;
         ref.name = name;
         ref.img = &page.img;
         ref.page = page.pageId;
         ref.mipmaps = wantMipmaps;
         ref.rect = Recti( refX, refY, refW, refH );

         //DE_DEBUG("Add ref ", ref.toString() )

         refNode = refNode->NextSiblingElement( "img" );
      }

      pageNode = pageNode->NextSiblingElement( "page" );
   }

   DE_DEBUG("Loaded atlas xml ", uri)

//   DE_DEBUG("PageCount = ", pages.size())
//   for ( int i = 0; i < pages.size(); ++i )
//   {
//      ImageAtlasPage & page = pages[ i ];
//      DE_DEBUG("Page[",i,"].RefCount = ", page.refs.size())
//      for ( int r = 0; r < page.refs.size(); ++r )
//      {
//         ImageAtlasRef & ref = page.refs[ r ];
//         DE_DEBUG("Page[",i,"].Ref[",r,"].Rect = ", ref.rect.toString() )
//      }
//   }
*/
   return true;
}

} // end namespace de.

