#include <de/gpu/smesh/SMesh.h>
#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>
#include <map>

namespace de {
namespace gpu {

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
   if ( hasDiffuseMap() ){ s << ", Td:" << getDiffuseMap().str(); }
   if ( hasBumpMap() ){ s << ", Tb:" << getBumpMap().str(); }
   if ( hasNormalMap() ){ s << ", Tn:" << getNormalMap().str(); }
   if ( hasSpecularMap() ){ s << ", Ts:" << getSpecularMap().str(); }
   if ( hasDetailMap() ){ s << ", Tdet:" << getDetailMap().str(); }
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

void
SMeshBuffer::destroy()
{
   if ( vao )
   {
      glDeleteVertexArrays(1, &vao);
      vao = 0;
   }

   if ( vbo )
   {
      glDeleteBuffers(1, &vbo);
      vbo = 0;
   }

   if ( ibo )
   {
      glDeleteBuffers(1, &ibo);
      ibo = 0;
   }
}

void
SMeshBuffer::upload( bool bForceUpload )
{
    bool bNeedUpload = bShouldUpload | bForceUpload;

    GL_VALIDATE

    if ( !vao )
    {
        glGenVertexArrays(1, &vao); GL_VALIDATE
        bNeedUpload = true;
    }

    if ( !vbo )
    {
        glGenBuffers(1, &vbo); GL_VALIDATE
        bNeedUpload = true;
    }

    if ( !ibo && indices.size() > 0 )
    {
        glGenBuffers(1, &ibo); GL_VALIDATE
        bNeedUpload = true;
    }

    if ( !bNeedUpload )
    {
        return;
    }

    glBindVertexArray(vao); GL_VALIDATE

    if ( vbo )
    {
        const size_t vertexCount = vertices.size();
        const size_t vertexSize = sizeof(S3DVertex);
        const size_t vertexBytes = vertexCount * vertexSize;
        const uint8_t* pVertices = reinterpret_cast< const uint8_t* >( vertices.data() );

        glBindBuffer(GL_ARRAY_BUFFER, vbo); GL_VALIDATE
        glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertexBytes), pVertices, GL_STATIC_DRAW); GL_VALIDATE

        // VertexAttribute[0] = Position3D (vec3f)
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(0) );
        glEnableVertexAttribArray( 0 );

        // VertexAttribute[1] = Normal3D (vec3f)
        glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(12) );
        glEnableVertexAttribArray( 1 );

        // VertexAttribute[2] = ColorRGBA (vec4ub)
        glVertexAttribPointer( 2, 4, GL_UNSIGNED_BYTE, GL_FALSE, vertexSize, reinterpret_cast<void*>(24) );
        glEnableVertexAttribArray( 2 );

        // VertexAttribute[3] = Tex2DCoord0 (vec2f)
        glVertexAttribPointer( 3, 2, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(28) );
        glEnableVertexAttribArray( 3 );
        //std::cout << "Upload " << vertexCount << " vertices" << std::endl;
        //std::cout << "Upload " << vertexBytes << " bytes" << std::endl;
        //std::cout << "Upload " << vertexSize << " stride" << std::endl;
         GL_VALIDATE
    }

    if ( ibo )
    {
        const size_t indexCount = indices.size();
        const size_t indexSize = sizeof(uint32_t);
        const size_t indexBytes = indexCount * indexSize;
        const uint8_t* pIndices = reinterpret_cast< const uint8_t* >( indices.data() );

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(indexBytes), pIndices, GL_STATIC_DRAW);

         GL_VALIDATE
        //std::cout << "Upload " << vertexCount << " vertices" << std::endl;
        //std::cout << "Upload " << vertexBytes << " bytes" << std::endl;
        //std::cout << "Upload " << vertexSize << " stride" << std::endl;
    }

    glBindVertexArray(0); GL_VALIDATE

    bShouldUpload = false;
}

void
SMeshBuffer::draw() const
{
   // upload();

   // Draw call

   glBindVertexArray(vao); GL_VALIDATE

   GLenum const primType = PrimitiveType::toOpenGL( primitiveType );
   if ( ibo )
   {
      glDrawElements( primType, GLint(indices.size()), GL_UNSIGNED_INT, nullptr ); GL_VALIDATE
   }
   else
   {
      glDrawArrays( primType, 0, GLint(vertices.size()) ); GL_VALIDATE
   }

   glBindVertexArray(0); GL_VALIDATE
}



// [MemorySizeInBytes]
size_t
SMeshBuffer::computeByteConsumption() const
{
    size_t byteCount = sizeof( *this );
    byteCount += vertices.capacity() * sizeof( S3DVertex );
    byteCount += indices.capacity() * sizeof( uint32_t );
    return byteCount;
}

std::string
SMeshBuffer::str( bool withMaterial ) const
{
    std::stringstream o;
    o << "(" << name << ")|";
    o << dbStrBytes( computeByteConsumption() ) << "|";
    o << "p:" << PrimitiveType::getShortString( primitiveType ) << "|";
    o << "v:" << getVertexCount() << "|";
    o << "i:" << getIndexCount() << "|";
    o << "bb:" << glm::ivec3(getBoundingBox().getSize());
    //if ( withFVF )
    //   o << "fvf(" << getFVF().toString() << "), ";
    //else
    //   o << "fvf(" << getFVF().getByteSize() << "), ";

    if ( withMaterial )
    o << ", mat(" << getMaterial().toString() << ")";

    //      for ( size_t i = 0; i < getMaterial().getTextureCount(); ++i )
    //      {
    //         if ( !getMaterial().getTexture( i ).empty() )
    //         {
    //            s << ", tex" << i << "(" << getMaterial().getTexture( i ).toString() << ")";
    //         }
    //      }
    return o.str();
}
// ===========================================================================


void
SMeshBufferTool::rotateTexCoords90_onlyQuads( SMeshBuffer & mesh )
{
    if ( mesh.getPrimitiveType() != PrimitiveType::Triangles )
    {
        DE_ERROR("Invalid primType")
        return;
    }

    //size_t p = mesh.getVertexCount() / 6; // interpret as Vertex only quads
    // interpret as Indexed quads
    if ( mesh.getIndexCount() > 6 )
    {
        for ( size_t k = 0; k < mesh.getVertexCount() / 4; ++k )
        {
            glm::vec2 a = mesh.getVertex( 4*k ).tex;
            glm::vec2 b = mesh.getVertex( 4*k+1 ).tex;
            glm::vec2 c = mesh.getVertex( 4*k+2 ).tex;
            glm::vec2 d = mesh.getVertex( 4*k+3 ).tex;

            mesh.getVertex( 4*k   ).tex = glm::vec2( d.y, d.x );
            mesh.getVertex( 4*k+1 ).tex = glm::vec2( a.y, a.x );
            mesh.getVertex( 4*k+2 ).tex = glm::vec2( b.y, b.x );
            mesh.getVertex( 4*k+3 ).tex = glm::vec2( c.y, c.x );
        }
    }
    else if ( mesh.getVertexCount() >= 6 )
    {
        //         for ( size_t k = 0; k < mesh.getVertexCount() / 4; ++k )
        //         {
        //            glm::vec2 a = mesh.getVertex( 4*k ).tex;
        //            glm::vec2 b = mesh.getVertex( 4*k+1 ).tex;
        //            glm::vec2 c = mesh.getVertex( 4*k+2 ).tex;
        //            glm::vec2 d = mesh.getVertex( 4*k+3 ).tex;

        //            mesh.getVertex( 4*k   ).tex = glm::vec2( d.y, d.x );
        //            mesh.getVertex( 4*k+1 ).tex = glm::vec2( a.y, a.x );
        //            mesh.getVertex( 4*k+2 ).tex = glm::vec2( b.y, b.x );
        //            mesh.getVertex( 4*k+3 ).tex = glm::vec2( c.y, c.x );
        //         }
        DE_ERROR("Vertex only not implemented")
    }
    else
    {
        DE_ERROR("Unsupported")
    }
}


void
SMeshBufferTool::fuse( SMeshBuffer & dst, SMeshBuffer const & src, glm::vec3 const & offset )
{
    if ( dst.getPrimitiveType() != src.getPrimitiveType() )
    {
        DE_ERROR("Differing PrimitiveTypes")
        return;
    }

    size_t v1 = dst.vertices.size();

    for ( size_t i = 0; i < src.vertices.size(); ++i )
    {
        auto v = src.vertices[ i ];
        v.pos += offset;
        dst.addVertex( v );
    }

    for ( size_t i = 0; i < src.indices.size(); ++i )
    {
        dst.addIndex( v1 + src.indices[ i ] );
    }

}

uint32_t
SMeshBufferTool::countVertices( std::vector< SMeshBuffer > const & liste )
{
    size_t n = 0;
    for ( SMeshBuffer const & b : liste ) { n += b.vertices.size(); }
    return n;
}

uint32_t
SMeshBufferTool::countIndices( std::vector< SMeshBuffer > const & liste )
{
    size_t n = 0;
    for ( SMeshBuffer const & b : liste ) { n += b.indices.size(); }
    return n;
}


void
SMeshBufferTool::fuse( SMeshBuffer & dst, std::vector< SMeshBuffer > const & liste )
{
    uint32_t nVerts = countVertices( liste );
    uint32_t nIndices = countVertices( liste );

    dst.vertices.reserve( nVerts + dst.vertices.size() );
    dst.indices.reserve( nIndices + dst.indices.size() );

    for ( SMeshBuffer const & src : liste )
    {
        if ( dst.getPrimitiveType() != src.getPrimitiveType() )
        {
            size_t v0 = dst.vertices.size();

            //         dst.vertices.insert( dst.vertices.end(), src.vertices.begin(), src.vertices.end() );
            //         dst.indices.insert( dst.indices.end(), src.indices.begin(), src.indices.end() );
            for ( auto vtx : src.vertices ) { dst.vertices.emplace_back( vtx ); }
            for ( auto idx : src.indices ) { dst.indices.emplace_back( idx + v0 ); }
        }
        else
        {
            DE_ERROR("Differing PrimitiveTypes")
        }
    }
}


/*
   size_t v0 = src.vertices.size();
   size_t i0 = src.indices.size();

   if ( v0 < 1 )
   {
      DE_ERROR("No vertices to add")
      return;
   }

   if ( (dst.getIndexCount() > 0) != (src.getIndexCount() > 0) )
   {
      DE_ERROR("Differing IndexTypes")
      return;
   }

   size_t v1 = dst.vertices.size();
   size_t i1 = dst.indices.size();

   // Dst is indexed...
   if ( i1 > 0 )
   {
      // Src is indexed...
      if ( i0 > 0 )
      {
         //DE_DEBUG( "Add src (indexed) to dst (indexed)" )
         for ( size_t i = 0; i < src.vertices.size(); ++i )
         {
            auto v = src.vertices[ i ];
            v.pos += offset;
            dst.addVertex( v );
         }
         for ( size_t i = 0; i < src.indices.size(); ++i )
         {
            dst.addIndex( v1 + src.indices[ i ] );
         }
      }
      // Src is vertex-only...
      else
      {
         //DE_DEBUG( "Add src (vertex-only) to dst (indexed)" )
         for ( size_t i = 0; i < src.vertices.size(); ++i )
         {
            auto v = src.vertices[ i ];
            v.pos += offset;
            dst.addVertex( v );
            dst.addIndex( v1 + i );
         }
      }
   }
   // Dst is vertex only...
   else
   {
      // Src is indexed...
      if ( i0 > 0 )
      {
         //DE_DEBUG( "Add src (indexed) to dst (vertex-only)" )
         for ( size_t i = 0; i < src.vertices.size(); ++i )
         {
            src.vertices[ i ].pos += offset;
         }

         //DE_DEBUG( "Add src (indexed) to dst (vertex-only)" )
         for ( size_t i = 0; i < src.indices.size(); ++i )
         {
            dst.addVertex( src.vertices[ src.indices[ i ] ] );
         }
      }
      // Src is vertex-only...
      else
      {
         //DE_DEBUG( "Add src (vertex-only) to dst (vertex-only)" )
         for ( size_t i = 0; i < src.vertices.size(); ++i )
         {
            auto v = src.vertices[ i ];
            v.pos += offset;
            dst.addVertex( v );
         }
      }
   }
   */
void
SMeshBufferTool::transformPoints( std::vector< glm::vec3 > & points,
                                 glm::dmat4 const & modelMat )
{
    for ( glm::vec3 & pos : points )
    {
        pos = glm::vec3( modelMat * glm::dvec4( pos, 1.0 ) );
    }
}


void
SMeshBufferTool::rotatePoints( std::vector< glm::vec3 > & points, float a, float b, float c )
{
    glm::mat4 m = glm::rotate( glm::mat4( 1.0f ), a * Math::DEG2RAD, glm::vec3(1,0,0) );
    m = glm::rotate( m, b * Math::DEG2RAD, glm::vec3(0,1,0) );
    m = glm::rotate( m, c * Math::DEG2RAD, glm::vec3(0,0,1) );
    glm::mat3 R( m );
    // glm::mat3 R = Math::buildRotationMatrixDEG3f( glm::vec3( a, b, c ) );

    for ( size_t i = 0; i < points.size(); ++i )
    {
        points[ i ] = R * points[ i ];
    }
    //o.recalculateBoundingBox();
}


SMeshBuffer
SMeshBufferTool::tesselate( SMeshBuffer const & src )
{
    SMeshBuffer dst;
    dst.primitiveType = src.primitiveType;
    dst.getMaterial() = src.getMaterial();
    dst.boundingBox = src.boundingBox;
    dst.vertices.reserve( src.vertices.size() * 2 );

    for ( size_t i = 0; i < src.vertices.size()/3; i++ )
    {
        S3DVertex const & A = src.vertices[ 3*i+0 ];
        S3DVertex const & B = src.vertices[ 3*i+1 ];
        S3DVertex const & C = src.vertices[ 3*i+2 ];

        float lAB = glm::length2( B.pos - A.pos );
        float lAC = glm::length2( C.pos - A.pos );
        float lBC = glm::length2( C.pos - B.pos );

        //     B
        //    /|
        //   D |
        //  /--|
        // A   C

        if ( lAB > lAC && lAB > lBC)
        {
            S3DVertex D;
            D.pos = (A.pos+B.pos)*0.5f;
            D.normal = glm::normalize(A.normal + B.normal);
            D.color = lerpColor( A.color, B.color, 0.5f );
            D.tex = (A.tex+B.tex)*0.5f;
            dst.addVertex( A );
            dst.addVertex( D );
            dst.addVertex( C );
            dst.addVertex( D );
            dst.addVertex( B );
            dst.addVertex( C );
        }
        //     A
        //    /|
        //   D |
        //  /--|
        // C   B

        else if ( lAC > lAB && lAC > lBC)
        {
            S3DVertex D;
            D.pos = (A.pos+C.pos)*0.5f;
            D.normal = glm::normalize(A.normal + C.normal);
            D.color = lerpColor( A.color, C.color, 0.5f );
            D.tex = (A.tex+C.tex)*0.5f;
            dst.addVertex( C );
            dst.addVertex( D );
            dst.addVertex( B );
            dst.addVertex( D );
            dst.addVertex( A );
            dst.addVertex( B );
        }
        //     C
        //    /|
        //   D |
        //  /--|
        // B   A
        else if ( lBC > lAC && lBC > lAB)
        {
            S3DVertex D;
            D.pos = (B.pos+C.pos)*0.5f;
            D.normal = glm::normalize(B.normal + C.normal);
            D.color = lerpColor( B.color, C.color, 0.5f );
            D.tex = (B.tex+C.tex)*0.5f;
            dst.addVertex( A );
            dst.addVertex( B );
            dst.addVertex( D );
            dst.addVertex( A );
            dst.addVertex( D );
            dst.addVertex( C );
        }


    }
    return dst;
}


std::vector< Triangle3f >
SMeshBufferTool::createCollisionTriangles( SMeshBuffer const & b )
{
    if ( b.getPrimitiveType() != PrimitiveType::Triangles ) return {};

    std::vector< Triangle3f > tris;

    if ( b.getIndexCount() > 0 )
    {
        tris.reserve( tris.size() + b.getIndexCount()/3 );
        for ( size_t i = 0; i < b.getIndexCount()/3; ++i )
        {
            glm::vec3 const & A = b.vertices[ b.indices[ 3*i ] ].pos;
            glm::vec3 const & B = b.vertices[ b.indices[ 3*i+1 ] ].pos;
            glm::vec3 const & C = b.vertices[ b.indices[ 3*i+2 ] ].pos;
            tris.push_back( { A, B, C } );
        }
    }
    else
    {
        tris.reserve( tris.size() + b.getVertexCount()/3 );
        for ( size_t i = 0; i < b.getVertexCount()/3; ++i )
        {
            glm::vec3 const & A = b.vertices[ 3*i ].pos;
            glm::vec3 const & B = b.vertices[ 3*i+1 ].pos;
            glm::vec3 const & C = b.vertices[ 3*i+2 ].pos;
            tris.push_back( { A, B, C } );
        }
    }

    return tris;
}

/// @brief Recalculates the bounding box. Should be called if the mesh changed.
void
SMeshBufferTool::computeBoundingBox( SMeshBuffer & o )
{
    o.recalculateBoundingBox();
}

Box3f
SMeshBufferTool::computeBoundingBox( std::vector< S3DVertex > const & vertices )
{
    Box3f bbox;
    if ( !vertices.empty() )
    {
        bbox.reset( vertices[ 0 ].pos );

        for ( size_t i = 1; i < vertices.size(); ++i )
        {
            bbox.addInternalPoint( vertices[ i ].pos );
        }
    }
    return bbox;
}



/// @brief Normalize all triangle normals.
void
SMeshBufferTool::bumpVertices( SMeshBuffer & o, Image* bumpMap, float bumpScale )
{
    if ( !bumpMap ) return;
    int w = bumpMap->w();
    int h = bumpMap->h();
    if ( w < 1 ) return;
    if ( h < 1 ) return;
    DE_DEBUG("bumpScale = ",bumpScale, ", img:", bumpMap->str() )

    //dbSaveImage( *bumpMap, "benni_bump_vertices.png" );

    auto getBump = [&] ( float u, float v )
    {
        int x = int( u * float(w) )%w;
        int y = int( v * float(h) )%h;
        int r = bumpMap->getPixel( x, y ) & 0xFF;
        return bumpScale * float( r ) / 255.0f;
    };

    for ( size_t i = 0; i < o.vertices.size(); ++i )
    {
        S3DVertex & v = o.vertices[ i ];
        v.pos = v.pos + v.normal * getBump( v.tex.x, v.tex.y );
    }

    computeNormals( o );
}

/// @brief Normalize all triangle normals.
void
SMeshBufferTool::computeNormals( std::vector< S3DVertex > & vertices, std::vector< uint32_t > const & indices )
{
    if ( indices.size() > 0 )
    {
        for ( size_t i = 0; i < indices.size()/3; ++i )
        {
            uint32_t iA = indices[ 3*i + 0 ];
            uint32_t iB = indices[ 3*i + 1 ];
            uint32_t iC = indices[ 3*i + 2 ];
            auto const & a = vertices[ iA ].pos;
            auto const & b = vertices[ iB ].pos;
            auto const & c = vertices[ iC ].pos;
            auto n = Math::getNormal3D( a,b,c );
            vertices[ iA ].normal = n;
            vertices[ iB ].normal = n;
            vertices[ iC ].normal = n;
        }
    }
    else
    {
        for ( size_t i = 0; i < vertices.size()/3; ++i )
        {
            uint32_t iA = 3*i;
            uint32_t iB = iA + 1;
            uint32_t iC = iA + 2;
            auto const & a = vertices[ iA ].pos;
            auto const & b = vertices[ iB ].pos;
            auto const & c = vertices[ iC ].pos;
            auto n = Math::getNormal3D( a,b,c );
            vertices[ iA ].normal = n;
            vertices[ iB ].normal = n;
            vertices[ iC ].normal = n;
        }
    }

    // DE_DEBUG( "v(", o.getVertexCount(), "), "
    // "primCount(", primCount, "), "
    // "primType(", o.getPrimitiveTypeStr(), ")" )
}

/// @brief Normalize all triangle normals.
void
SMeshBufferTool::computeNormals( SMeshBuffer & o )
{
    if ( o.getPrimitiveType() != PrimitiveType::Triangles )
    {
        //throw std::runtime_error( "Unsupported PrimType for recomputeNormals()");
        //DE_ERROR("No triangles ", o.getName())
        return;
    }

    //size_t primCount = o.getPrimitiveCount();

    if ( o.indices.size() > 0 )
    {
        // DE_DEBUG( "i(", o.getIndexCount(), "), "
        // "v(", o.getVertexCount(), "), "
        // "primCount(", primCount, "), "
        // "primType(", o.getPrimitiveTypeStr(), ")" )

        for ( size_t i = 0; i < o.indices.size()/3; ++i )
        {
            uint32_t iA = o.indices[ 3*i + 0 ];
            uint32_t iB = o.indices[ 3*i + 1 ];
            uint32_t iC = o.indices[ 3*i + 2 ];
            auto const & a = o.vertices[ iA ].pos;
            auto const & b = o.vertices[ iB ].pos;
            auto const & c = o.vertices[ iC ].pos;
            auto n = Math::getNormal3D( a,b,c );
            //if (n.y < 0.0f) n = -n;
            o.vertices[ iA ].normal = n;
            o.vertices[ iB ].normal = n;
            o.vertices[ iC ].normal = n;
        }
    }
    else
    {
        // DE_DEBUG( "v(", o.getVertexCount(), "), "
        // "primCount(", primCount, "), "
        // "primType(", o.getPrimitiveTypeStr(), ")" )

        for ( size_t i = 0; i < o.vertices.size()/3; ++i )
        {
            uint32_t iA = 3*i;
            uint32_t iB = iA + 1;
            uint32_t iC = iA + 2;
            auto const & a = o.vertices[ iA ].pos;
            auto const & b = o.vertices[ iB ].pos;
            auto const & c = o.vertices[ iC ].pos;
            auto n = Math::getNormal3D( a,b,c );
            //if (n.y < 0.0f) n = -n;
            o.vertices[ iA ].normal = n;
            o.vertices[ iB ].normal = n;
            o.vertices[ iC ].normal = n;
        }
    }
}

void
SMeshBufferTool::flipX( SMeshBuffer & o )
{
    for ( auto & vertex : o.vertices )
    {
        auto & pos = vertex.pos;
        pos.x = -pos.x;
    }
    o.recalculateBoundingBox();
}
void
SMeshBufferTool::flipY( SMeshBuffer & o )
{
    for ( auto & vertex : o.vertices )
    {
        auto & pos = vertex.pos;
        pos.y = -pos.y;
    }
    o.recalculateBoundingBox();
}
void
SMeshBufferTool::flipZ( SMeshBuffer & o )
{
    for ( auto & vertex : o.vertices )
    {
        auto & pos = vertex.pos;
        pos.z = -pos.z;
    }
    o.recalculateBoundingBox();
}
void
SMeshBufferTool::flipXY( SMeshBuffer & o )
{
    for ( auto & vertex : o.vertices )
    {
        auto & pos = vertex.pos;
        std::swap( pos.x, pos.y );
    }
    o.recalculateBoundingBox();
}
void
SMeshBufferTool::flipYZ( SMeshBuffer & o )
{
    for ( auto & vertex : o.vertices )
    {
        auto & pos = vertex.pos;
        std::swap( pos.y, pos.z );
    }
    o.recalculateBoundingBox();
}
void
SMeshBufferTool::flipXZ( SMeshBuffer & o )
{
    for ( auto & vertex : o.vertices )
    {
        auto & pos = vertex.pos;
        std::swap( pos.x, pos.z );
    }
    o.recalculateBoundingBox();
}


void
SMeshBufferTool::flipVertexPosYZ( SMeshBuffer & o )
{
    for ( size_t i = 0; i < o.vertices.size(); ++i )
    {
        std::swap( o.vertices[ i ].pos.y, o.vertices[ i ].pos.z );
    }
    std::swap( o.boundingBox.m_Min.y, o.boundingBox.m_Min.z );
    std::swap( o.boundingBox.m_Max.y, o.boundingBox.m_Max.z );
}

void
SMeshBufferTool::flipWinding( SMeshBuffer & o )
{
    if ( o.primitiveType == PrimitiveType::Triangles )
    {
        if ( o.indices.size() > 0 )
        {
            for ( size_t i = 0; i < o.indices.size()/3; ++i )
            {
                uint32_t tmp = o.indices[ 3*i ];
                o.indices[ 3*i ] = o.indices[ 3*i + 1 ];
                o.indices[ 3*i + 1 ] = o.indices[ 3*i + 2 ];
                o.indices[ 3*i + 2 ] = tmp;
            }
        }
        else
        {
            for ( size_t i = 0; i < o.vertices.size()/3; ++i )
            {
                S3DVertex tmp = o.vertices[ 3*i ];
                o.vertices[ 3*i ] = o.vertices[ 3*i + 1 ];
                o.vertices[ 3*i + 1 ] = o.vertices[ 3*i + 2 ];
                o.vertices[ 3*i + 2 ] = tmp;
            }
        }
    }

    SMeshBufferTool::computeNormals( o );
}

void
SMeshBufferTool::flipNormals( SMeshBuffer & o )
{
    for ( S3DVertex & v : o.vertices )
    {
        v.normal = -v.normal;
    }
}

void
SMeshBufferTool::translateVertices( SMeshBuffer & o, glm::vec3 const & offset )
{
    for ( auto & v : o.vertices )
    {
        v.pos += offset;
    }
    o.boundingBox.m_Min += offset;
    o.boundingBox.m_Max += offset;
}

void
SMeshBufferTool::translateVertices( SMeshBuffer & o, glm::vec3 const & offset, uint32_t vStart )
{
    for ( size_t i = vStart; i < o.vertices.size(); ++i )
    {
        o.vertices[ i ].pos += offset;
    }
    // o.recalculateBoundingBox();
}

void
SMeshBufferTool::translateVertices( SMeshBuffer & o, float x, float y, float z )
{
    translateVertices( o, glm::vec3( x,y,z ) );
}

void
SMeshBufferTool::randomRadialDisplace( SMeshBuffer & o, float h )
{
    o.recalculateBoundingBox();

    auto center = o.getBoundingBox().getCenter();

    for ( size_t i = 0; i < o.vertices.size(); ++i )
    {
        auto & pos = o.vertices[ i ].pos;
        auto dir = pos - center;
        auto len = glm::length( dir );
        dir = glm::normalize( dir );
        auto d = h * 0.01f * (rand()%100);
        pos = center + dir * (len + d);
    }
}

void
SMeshBufferTool::rotateVertices( SMeshBuffer & o, float a, float b, float c )
{
    glm::mat4 m = glm::rotate( glm::mat4( 1.0f ), a * Math::DEG2RAD, glm::vec3(1,0,0) );
    m = glm::rotate( m, b * Math::DEG2RAD, glm::vec3(0,1,0) );
    m = glm::rotate( m, c * Math::DEG2RAD, glm::vec3(0,0,1) );
    glm::mat3 R( m );
    // glm::mat3 R = Math::buildRotationMatrixDEG3f( glm::vec3( a, b, c ) );

    for ( size_t i = 0; i < o.vertices.size(); ++i )
    {
        o.vertices[ i ].pos = R * o.vertices[ i ].pos;
        o.vertices[ i ].normal = R * o.vertices[ i ].normal;
    }
    o.recalculateBoundingBox();
}

void
SMeshBufferTool::scaleVertices( SMeshBuffer & o, float scale )
{
    if ( scale != 1.0f )
    {
        for ( size_t i = 0; i < o.getVertexCount(); ++i )
        {
            o.vertices[ i ].pos *= scale;
        }
    }
    o.recalculateBoundingBox();
}

void
SMeshBufferTool::transformVertices( SMeshBuffer & o, glm::dmat4 const & modelMat )
{
    for ( S3DVertex & v : o.vertices )
    {
        v.pos = glm::vec3( modelMat * glm::dvec4( v.pos, 1.0 ) );
        v.normal = glm::vec3( modelMat * glm::dvec4( v.normal, 1.0 ) );
    }
    o.recalculateBoundingBox();
}


void
SMeshBufferTool::scaleVertices( SMeshBuffer & o, float sx, float sy, float sz )
{
    for ( size_t i = 0; i < o.getVertexCount(); ++i )
    {
        o.vertices[ i ].pos *= glm::vec3( sx,sy,sz);
    }
    o.recalculateBoundingBox();
}

void
SMeshBufferTool::translateNormals( SMeshBuffer & o, glm::vec3 const & offset )
{
    for ( auto & v : o.vertices )
    {
        v.normal += offset;
    }
}

void
SMeshBufferTool::translateNormals( SMeshBuffer & o, float nx, float ny, float nz )
{
    translateNormals( o, glm::vec3( nx, ny, nz) );
}

void
SMeshBufferTool::setNormals( SMeshBuffer & o, glm::vec3 const & normal )
{
    for ( auto & v : o.vertices )
    {
        v.normal = normal;
    }
}

void
SMeshBufferTool::setNormals( SMeshBuffer & o, float nx, float ny, float nz )
{
    setNormals( o, glm::vec3( nx, ny, nz) );
}

void
SMeshBufferTool::setNormalZ( SMeshBuffer & o, float nz )
{
    for ( auto & v : o.vertices )
    {
        v.normal.z = nz;
    }
}

void
SMeshBufferTool::scaleTexture( SMeshBuffer & o, float u, float v )
{
    if ( u != 1.0f || v != 1.0f )
    {
        for ( size_t i = 0; i < o.getVertexCount(); ++i )
        {
            o.vertices[ i ].tex.x *= u;
            o.vertices[ i ].tex.y *= v;
        }
    }
    //o.recalculateBoundingBox();
}

void
SMeshBufferTool::transformTexCoords( SMeshBuffer & o, Recti const & r_atlas, int atlas_w, int atlas_h )
{
    float scale_x = float( r_atlas.w ) / float( atlas_w );
    float scale_y = float( r_atlas.h ) / float( atlas_h );

    //glm::vec4 r_scale = r_atlas.toVec4( atlas_w, atlas_h );
    float trans_x = float( r_atlas.x ) / float( atlas_w-1 );
    float trans_y = float( r_atlas.y ) / float( atlas_h-1 );

    for ( size_t i = 0; i < o.vertices.size(); ++i )
    {
        float & u = o.vertices[ i ].tex.x;
        float & v = o.vertices[ i ].tex.y;
        u = ( scale_x * u ) + trans_x;
        v = ( scale_y * v ) + trans_y;
    }

    //o.recalculateBoundingBox();
}

void
SMeshBufferTool::colorVertices( SMeshBuffer & o, uint32_t color )
{
    for ( size_t i = 0; i < o.vertices.size(); ++i )
    {
        o.vertices[ i ].color = color;
    }
}

void
SMeshBufferTool::centerVertices( SMeshBuffer & o )
{
    glm::vec3 const center = o.getBoundingBox().getCenter();

    for ( size_t i = 0; i < o.vertices.size(); ++i )
    {
        o.vertices[ i ].pos -= center;
    }

    //o.recalculateBoundingBox();
}

/*
SMeshBuffer
SMeshBufferTool::createLines()
{
   SMeshBuffer o = SMeshBuffer( PrimitiveType::Lines );
   o.Material.Lighting = 0;
   o.Material.Wireframe = false;
   o.Material.FogEnable = false;
   o.Material.state.culling = Culling();
   o.Material.state.depth = Depth();
   return o;
}

SMeshBuffer
SMeshBufferTool::createTriangles()
{
   SMeshBuffer o( PrimitiveType::Triangles );
   return o;
}
*/

void
SMeshBufferTool::addVertex( SMeshBuffer & o, S3DVertex const & vertex )
{
    o.addVertex( vertex );
}

void
SMeshBufferTool::addVertex( SMeshBuffer & o, glm::vec3 const & pos, glm::vec3 const & nrm, uint32_t color, glm::vec2 const & tex )
{
    S3DVertex v;
    v.pos = pos;
    v.normal = nrm;
    v.color = color;
    v.tex = tex;
    o.addVertex( v );
}

void
SMeshBufferTool::addVertex( SMeshBuffer & o, glm::vec3 const & pos, uint32_t color )
{
    S3DVertex v;
    v.pos = pos;
    v.color = color;
    o.addVertex( v );
}

void
SMeshBufferTool::addLine( SMeshBuffer & o, S3DVertex const & a, S3DVertex const & b )
{
    addVertex( o, a );
    addVertex( o, b );
}

void
SMeshBufferTool::addTriangle( SMeshBuffer & o, S3DVertex const & a, S3DVertex const & b, S3DVertex const & c )
{
    o.addTriangle( a,b,c);
}

void
SMeshBufferTool::addLine( SMeshBuffer & o, glm::vec3 const & a, glm::vec3 const & b, uint32_t color )
{
    addVertex( o, a, color );
    addVertex( o, b, color );
}

void
SMeshBufferTool::addLine( SMeshBuffer & o, glm::vec3 const & a, glm::vec3 const & b, uint32_t colorA, uint32_t colorB )
{
    addVertex( o, a, colorA );
    addVertex( o, b, colorB );
}

void
SMeshBufferTool::addLine( SMeshBuffer & o, float x1, float y1, float x2, float y2, float z, uint32_t color )
{
    addLine( o, glm::vec3(x1,y1,z), glm::vec3(x2,y2,z), color );
}

void
SMeshBufferTool::addLine( SMeshBuffer & o, float x1, float y1, float x2, float y2, float z, uint32_t colorA, uint32_t colorB )
{
    addLine( o, glm::vec3(x1,y1,z), glm::vec3(x2,y2,z), colorA, colorB );
}

void
SMeshBufferTool::addLineTriangle( SMeshBuffer & o, glm::vec3 const & a, glm::vec3 const & b, glm::vec3 const & c, uint32_t color )
{
    addLine( o, a, b, color );
    addLine( o, b, c, color );
    addLine( o, c, a, color );
}

void
SMeshBufferTool::addLineTriangle( SMeshBuffer & o, glm::vec3 const & a, glm::vec3 const & b, glm::vec3 const & c, uint32_t colorA, uint32_t colorB, uint32_t colorC )
{
    addLine( o, a, b, colorA, colorB );
    addLine( o, b, c, colorB, colorC );
    addLine( o, c, a, colorC, colorA );
}

void
SMeshBufferTool::addLineQuad( SMeshBuffer & o, glm::vec3 const & a, glm::vec3 const & b, glm::vec3 const & c, glm::vec3 const & d, uint32_t color )
{
    addLine( o, a, b, color );
    addLine( o, b, c, color );
    addLine( o, c, d, color );
    addLine( o, d, a, color );
}

void
SMeshBufferTool::addLineQuad( SMeshBuffer & o, glm::vec3 const & a, glm::vec3 const & b, glm::vec3 const & c, glm::vec3 const & d,
                             uint32_t colorA, uint32_t colorB, uint32_t colorC, uint32_t colorD )
{
    addLine( o, a, b, colorA, colorB );
    addLine( o, b, c, colorB, colorC );
    addLine( o, c, d, colorC, colorD );
    addLine( o, d, a, colorD, colorA );
}

void
SMeshBufferTool::addLineBox( SMeshBuffer & o, Box3f const & box, uint32_t color )
{
    addLineQuad( o, box.getA(), box.getB(), box.getC(), box.getD(), color );
    addLineQuad( o, box.getE(), box.getF(), box.getG(), box.getH(), color );
    addLine( o, box.getA(), box.getE(), color );
    addLine( o, box.getB(), box.getF(), color );
    addLine( o, box.getC(), box.getG(), color );
    addLine( o, box.getD(), box.getH(), color );
}

// =======================================================================
// DebugGeometry:
// =======================================================================
void
SMeshBufferTool::addLineNormals( SMeshBuffer & o, SMeshBuffer const & src, float n_len, uint32_t color )
{
    for ( size_t i = 0; i < src.getVertexCount(); ++i )
    {
        S3DVertex a = src.getVertex( i );
        S3DVertex b = src.getVertex( i );
        a.color = color;
        b.color = color;
        b.pos = a.pos + ( a.normal * n_len );
        o.addLine( a, b );
    }
}

//static SMeshBuffer
//create( SMeshBuffer const & src )
//{
//   SMeshBuffer lines( PrimitiveType::Lines );
//   lines.setLighting( 0 );
//   lines.setTexture( 0, src.Material.getTexture( 0 ) );

//   for ( size_t i = 0; i < src.vertices.size()/3; ++i )
//   {
//      auto A = src.vertices[ 3*i + 0 ];
//      auto B = src.vertices[ 3*i + 1 ];
//      auto C = src.vertices[ 3*i + 2 ];
//      A.color = 0xFF0000FF;
//      B.color = 0xFF00FF00;
//      C.color = 0xFFFF0000;
//      lines.addVertexLine( A,B );
//      lines.addVertexLine( B,C );
//      lines.addVertexLine( C,A );
//   }
//   return lines;
//}

void
SMeshBufferTool::addWireframe( SMeshBuffer & o, SMeshBuffer const & src, float outScale, uint32_t color )
{
    if ( src.getPrimitiveType() == PrimitiveType::Triangles )
    {
        if ( src.getIndexCount() > 0 )
        {
            for ( size_t i = 0; i < src.getIndexCount()/3; ++i )
            {
                uint32_t iA = src.getIndex( 3*i );
                uint32_t iB = src.getIndex( 3*i+1 );
                uint32_t iC = src.getIndex( 3*i+2 );
                S3DVertex A = src.getVertex( iA );
                S3DVertex B = src.getVertex( iB );
                S3DVertex C = src.getVertex( iC );
                if ( outScale != 1.0f )
                {
                    A.pos *= outScale;
                    B.pos *= outScale;
                    C.pos *= outScale;
                }
                if ( color == 0 )
                {
                    A.color = 0xFF30A0FF;
                    B.color = 0xFF50FF50;
                    C.color = 0xFFFF3030;
                }
                else
                {
                    A.color = color;
                    B.color = color;
                    C.color = color;
                }
                o.addLineTriangle( A,B,C );
            }
        }
        else
        {
            for ( size_t i = 0; i < src.getVertexCount()/3; ++i )
            {
                S3DVertex A = src.getVertex( 3*i );
                S3DVertex B = src.getVertex( 3*i+1 );
                S3DVertex C = src.getVertex( 3*i+2 );
                if ( outScale != 1.0f )
                {
                    A.pos *= outScale;
                    B.pos *= outScale;
                    C.pos *= outScale;
                }
                if ( color == 0 )
                {
                    A.color = 0xFF5050FF;
                    B.color = 0xFF50FF50;
                    C.color = 0xFFFF5050;
                }
                else
                {
                    A.color = color;
                    B.color = color;
                    C.color = color;
                }
                o.addLineTriangle( A,B,C );
            }
        }
    }
}

SMeshBuffer
SMeshBufferTool::createWireframe( SMeshBuffer const & src, float scale, uint32_t color )
{
    SMeshBuffer o( PrimitiveType::Lines );
    addWireframe( o, src, scale, color );
    o.recalculateBoundingBox();
    return o;
}

SMeshBuffer
SMeshBufferTool::createScaled( SMeshBuffer const & src, float scale )
{
    SMeshBuffer o = src; // deep copy!
    scaleVertices( o, scale );
    o.recalculateBoundingBox();
    return o;
}

SMeshBuffer
SMeshBufferTool::createTranslated( SMeshBuffer const & src, glm::vec3 pos )
{
    SMeshBuffer o = src; // deep copy!
    translateVertices( o, pos );
    o.recalculateBoundingBox();
    return o;
}

SMeshBuffer
SMeshBufferTool::createLineBox( Box3f const & box, uint32_t color )
{
    SMeshBuffer o( PrimitiveType::Lines );
    addLineBox( o, box, color );
    return o;
}


SMeshBuffer
SMeshBufferTool::createLineNormals( SMeshBuffer const & src, float normalLen, uint32_t color )
{
    SMeshBuffer o( PrimitiveType::Lines );
    addLineNormals( o, src, normalLen, color );
    o.recalculateBoundingBox();
    return o;
}


//SMesh::SMesh()
//   : ModelMat( 1.0 )
//   , BoundingBox()
//   , Name("UntitledSMesh")
//   , Buffers()
//   , Visible( true )
//{}

SMesh::SMesh()
   : boundingBox()
   //, Name()
   //, ModelMat( 1.0 )
   , buffers()
   , bVisible( true )
{
}

/*
SMesh::SMesh( char const * name )
   : SMesh()
{
   // if ( !name )
   // {
   //    DE_RUNTIME_ERROR("SMesh(nullptr)", "Prevent crash with std::string(nullptr)")
   // }
   // else
   // {
   //    Name = name;
   // }
}


SMesh::SMesh( std::string const & name )
   : SMesh()
{
   // if ( name.empty() )
   // {
   //    DE_RUNTIME_ERROR("SMesh(\"\")", "Prevent creation with empty name")
   // }
   // else
   // {
   //    Name = name;
   // }
}
*/

SMesh::SMesh( SMeshBuffer const & buf )
   : SMesh()
{
   addMeshBuffer( buf );
}


std::vector< TexRef >
SMesh::getUniqueTextures() const
{
   std::vector< TexRef > textures;

   for (const auto& buffer : buffers)
   {
      for (TexRef const & ref : buffer.getMaterial().getTextures() )
      {
         if (!ref.tex) continue;

         auto found = std::find_if( textures.begin(), textures.end(),
            [&]( TexRef const & cached ) { return cached.tex == ref.tex; } );
         if ( found == textures.end() )
         {
            textures.emplace_back( ref );
         }
      }
   }
   return textures;
}



std::string
SMesh::getPrimTypesStr() const
{
   std::map< PrimitiveType, uint32_t > primMap;

   for ( auto const & buffer : buffers )
   {
      auto it = primMap.find( buffer.getPrimitiveType() );
      if ( it == primMap.end() )
      {
         primMap[ buffer.getPrimitiveType() ] = 1;
      }
      else
      {
         primMap[ buffer.getPrimitiveType() ]++;
      }
   }

   int k=0;
   std::ostringstream o;

   auto it = primMap.begin();
   auto itEnd = primMap.end();
   while ( it != itEnd )
   {
      if ( k > 0 ) o << "+";
      o << PrimitiveType::getString( it->first );
      k++;
      it++;
   }

   return o.str();
}


void
SMesh::upload()
{
   for ( auto & buf : buffers )
   {
      buf.upload();
   }
}

void
SMesh::addEmptyBuffer()
{
   //auto n = Buffers.size();
   buffers.emplace_back();
   //Buffers.back().Name = Name + std::to_string( n );
}

void
SMesh::addMeshBuffer( SMeshBuffer const & buf )
{
   if ( buffers.empty() )
   {
      boundingBox.reset( buf.getBoundingBox() );
   }
   else
   {
      boundingBox.addInternalBox( buf.getBoundingBox() );
   }

   // if ( Name.empty() )
   // {
   //    Name = buf.Name;
   // }

   buffers.emplace_back( std::move( buf ) );

//   if ( buffers.back().Name.empty() )
//   {
//      auto n = buffers.size()-1;
//      buffers.back().Name = Name + std::to_string( n );
//   }
}

void
SMesh::addMesh( SMesh const & other )
{
   for ( int i = 0; i < other.getMeshBufferCount(); ++i )
   {
      addMeshBuffer( other.getMeshBuffer( i ) );
   }
}

void
SMesh::clear()
{
   boundingBox.reset();
   buffers.clear();
}

bool
SMesh::isIndex( int i ) const
{
   if ( size_t( i ) >= buffers.size() ) return false;
   return true;
}

int
SMesh::getMeshBufferCount() const { return buffers.size(); }

SMeshBuffer const &
SMesh::getMeshBuffer( int i ) const
{
   if ( !isIndex( i ) )
   {
      std::ostringstream s;
      s << __FILE__ << ":" << __LINE__ << " invalid meshbuffer index " << i;
      throw std::runtime_error( s.str() );
   }
   return buffers[ i ];
}

SMeshBuffer &
SMesh::getMeshBuffer( int i )
{
   if ( !isIndex( i ) )
   {
      std::ostringstream s;
      s << __FILE__ << ":" << __LINE__ << " invalid meshbuffer index " << i;
      throw std::runtime_error( s.str() );
   }
   return buffers[ i ];
}

void
SMesh::removeMeshBuffer( int i )
{
   if ( !isIndex( i ) )
   {
      return;
   }
   buffers.erase( buffers.begin() + i );
   recalculateBoundingBox();
}

//   void
//   shiftMeshBuffer( int shifts )
//   {
//      printf( "shiftMeshBuffer( shifts = %d, buffercount = %d )\n", shifts, getMeshBufferCount() );
//      if ( shifts < 1 ) return;
//      uint32_t bufferCount = getMeshBufferCount();
//      for ( uint32_t b = 0; b < bufferCount; ++b )
//      {
//         uint32_t k = ( uint32_t(shifts) + b ) % bufferCount; // looks like no neg shifts, yet.
//         SMeshBuffer* tmp = Buffers[ k ];          // save value at target position
//         Buffers[ k ] = Buffers[ b ];// overwrite target position with new value
//         Buffers[ b ] = tmp;// overwrite source position with save row-data
//      }
//   }



void
SMesh::recalculateNormals()
{
   for ( auto & buf : buffers )
   {
      SMeshBufferTool::computeNormals( buf );
   }
}

void
SMesh::recalculateBoundingBox( bool fully )
{
   if ( buffers.empty() )
   {
      boundingBox.reset();
   }
   else
   {
      if ( fully )
      {
         buffers[ 0 ].recalculateBoundingBox();
      }

      boundingBox.reset( buffers[ 0 ].getBoundingBox() );

      for ( size_t i = 1; i < buffers.size(); ++i )
      {
         SMeshBuffer & buf = buffers[ i ];
         if ( fully )
         {
            buf.recalculateBoundingBox();
         }

         boundingBox.addInternalBox( buf.getBoundingBox() );
      }
   }
}

//std::vector< Triangle3f >
//SMesh::createCollisionTriangles() const
//{
//   std::vector< Triangle3f > tris;

//   for ( SMeshBuffer const & p : Buffers )
//   {
//      std::vector< Triangle3f > tmp = SMeshBufferTool::createCollisionTriangles( p );
//      if ( tmp.size() > 0)
//      {
//         tris.insert( tris.end(), tmp.begin(), tmp.end() );
//      }
//   }

//   return tris;
//}

void
SMesh::flipVertexPosYZ()
{
   SMeshTool::flipYZ( *this );
}

void
SMesh::flipNormals()
{
   SMeshTool::flipNormals( *this );
}

void
SMesh::transformVertices( glm::dmat4 const & m )
{
   for ( SMeshBuffer & p : buffers )
   {
      for ( size_t b = 0; b < p.vertices.size(); ++b )
      {
         glm::dvec4 dp( p.vertices[ b ].pos, 1.0 );
         glm::dvec4 dn( p.vertices[ b ].normal, 1.0 );
         p.vertices[ b ].pos = glm::dvec3( m * dp );
         p.vertices[ b ].normal = glm::dvec3( m * dn );
      }
   }
}

void
SMesh::translateVertices( glm::vec3 const & offset )
{
   for ( SMeshBuffer & p : buffers )
   {
      SMeshBufferTool::translateVertices( p, offset );
   }
}

void
SMesh::translateVertices( float x, float y, float z )
{
   translateVertices( glm::vec3( x,y,z ) );
}

void
SMesh::rotateVertices( float a, float b, float c )
{
   for ( SMeshBuffer & p : buffers )
   {
      SMeshBufferTool::rotateVertices( p, a, b, c );
   }
   recalculateBoundingBox();
}

void
SMesh::colorVertices( uint32_t color )
{
   for ( SMeshBuffer & p : buffers )
   {
      SMeshBufferTool::colorVertices( p, color );
   }
}

void
SMesh::centerVertices()
{
   recalculateBoundingBox();
   glm::vec3 const offset = -getBoundingBox().getCenter();

   for ( SMeshBuffer & p : buffers )
   {
      SMeshBufferTool::translateVertices( p, offset );
      p.recalculateBoundingBox();
   }

   recalculateBoundingBox();
}

void
SMesh::scaleTexture( float u, float v )
{
   for ( SMeshBuffer & p : buffers )
   {
      SMeshBufferTool::scaleTexture( p, u, v );
   }
}

void
SMesh::scaleVertices( float scale )
{
   for ( SMeshBuffer & p : buffers )
   {
      SMeshBufferTool::scaleVertices( p, scale );
   }
   recalculateBoundingBox();
}

void
SMesh::scaleVertices( float sx, float sy, float sz )
{
   for ( SMeshBuffer & p : buffers )
   {
      SMeshBufferTool::scaleVertices( p, sx,sy,sz );
   }
   recalculateBoundingBox();
}

std::string
SMesh::toString( bool printDetails ) const
{
   std::ostringstream o;
   o <<
       "Name(" << FileSystem::fileName( name ) << "), "
        "Limbs(" << buffers.size() << "), "
         "Size(" << getBoundingBox().getSize() << "), "
         "Vertices(" << getVertexCount() << "), "
         "Indices(" << getIndexCount() << "), "
         "Textures(" << getUniqueTextures().size() << "), "
         "Memory(" << dbStrBytes( computeByteConsumption() ) << ")";

   //s << "\n";

//      for ( size_t i = 0; i < Buffers.size(); ++i )
//      {
//         IMeshBuffer const * const p = Buffers[ i ];
//         if ( !p ) continue;
//         s << "Limb[" << i << "] " << p->toString() << "\n";
//      }
   return o.str();
}

size_t
SMesh::getVertexCount() const
{
   size_t n = 0;
   for ( SMeshBuffer const & p : buffers )
   {
      n += p.getVertexCount();
   }
   return n;
}

size_t
SMesh::getIndexCount() const
{
   size_t n = 0;
   for ( SMeshBuffer const & p : buffers )
   {
      n += p.getIndexCount();
   }
   return n;
}

size_t
SMesh::computeByteConsumption() const
{
   size_t byteCount = sizeof( *this );
   for ( SMeshBuffer const & p : buffers )
   {
      byteCount += p.computeByteConsumption();
   }
   return byteCount;
}

int
SMesh::getMaterialCount() const
{
   return getMeshBufferCount();
}

SMaterial const &
SMesh::getMaterial( int i ) const
{
   if ( !isIndex( i ) )
   {
      std::ostringstream o;
      o << __FILE__ << ":" << __LINE__ << " invalid meshbuffer index " << i;
      throw std::runtime_error( o.str() );
   }
   return getMeshBuffer( i ).getMaterial();
}

SMaterial &
SMesh::getMaterial( int i )
{
   if ( !isIndex( i ) )
   {
      std::ostringstream o;
      o << __FILE__ << ":" << __LINE__ << " invalid meshbuffer index " << i;
      throw std::runtime_error( o.str() );
   }
   return getMeshBuffer( i ).getMaterial();
}

void
SMesh::setCulling( Culling cull )
{
   for ( SMeshBuffer & p : buffers ) { p.setCulling( cull ); }
}

void
SMesh::setCulling( bool enable )
{
   for ( SMeshBuffer & p : buffers ) { p.setCulling( enable ); }
}

void
SMesh::setDepth( Depth depth )
{
   for ( SMeshBuffer & p : buffers ) { p.setDepth( depth ); }
}

void
SMesh::setStencil( Stencil stencil )
{
   for ( SMeshBuffer & p : buffers ) { p.setStencil( stencil ); }
}

void
SMesh::setBlend( Blend blend )
{
   for ( SMeshBuffer & p : buffers ) { p.setBlend( blend ); }
}

/*
void
SMesh::setTexture( int stage, ITexture* tex )
{
   for ( SMeshBuffer & p : Buffers ) { p.setTexture( stage, tex ); }
}
*/

void
SMesh::setTexture( int stage, TexRef ref )
{
   for ( SMeshBuffer & p : buffers ) { p.setTexture( stage, ref ); }
}

void
SMesh::setLighting( int illum )
{
   for ( SMeshBuffer & p : buffers ) { p.setLighting( illum ); }
}

int
SMesh::getLighting() const
{
   int illum = 0;
   int count = int( buffers.size() );
   if ( count > 0 )
   {
      for ( SMeshBuffer const & p : buffers )
      {
         illum += p.getMaterial().Lighting;
      }
      illum /= count;
   }

   return illum;
}


int
SMesh::getFogEnable() const
{
   int fogMode = 0;
   for ( SMeshBuffer const & p : buffers )
   {
      fogMode = std::max( fogMode, p.getMaterial().FogEnable ? 1:0 );
   }

   return fogMode;
}


void
SMesh::setFogEnable( bool enable )
{
   for ( SMeshBuffer & p : buffers ) { p.setFogEnable( enable ); }
}

int
SMesh::getWireframe() const
{
   int wireframe = 0;
   for ( SMeshBuffer const & p : buffers )
   {
      wireframe = std::max( wireframe, p.getMaterial().Wireframe ? 1:0 );
   }

   return wireframe;
}


void
SMesh::setWireframe( bool enable )
{
   for ( SMeshBuffer & p : buffers ) { p.setWireframe( enable ); }
}

bool
SMesh::intersectRayWithBoundingBox( Ray3< T > const & ray, MeshIntersectionResult< T > & result ) const
{
   Box3< T >         bbox( boundingBox.getMin(), boundingBox.getMax() );
   T                 beginDist = std::numeric_limits< T >::max();
   T                 globalHitDist = beginDist;
   glm::tvec3< T >   globalHitPos;
   Triangle3< T >    globalTriangle;
   int32_t           globalTriangleIndex = -1;
   bool              doesIntersect = false;
   for ( size_t i = 0; i < 12; ++i ) // Loop 12 bbox triangles
   {
      glm::tvec3< T > localHitPos; // local
      Triangle3< T > const localTriangle = bbox.getTriangle( i );
      if ( localTriangle.intersectRay( ray.getPos(), ray.getDir(), localHitPos ) )
      {
         auto localHitDist = glm::length2( ray.getPos() - localHitPos );
         if ( localHitDist == beginDist || globalHitDist > localHitDist )
         {
            doesIntersect = true;
            globalHitDist = localHitDist;
            globalHitPos = localHitPos;
            globalTriangle = localTriangle;
            globalTriangleIndex = i;
         }
      }
      //DE_DEBUG("Mesh[",i,"].Triangle[", t, "] = ", tri.toString() )
   }

   if ( doesIntersect )
   {
      result.meshIndex = -1;     // indicates bbox
      result.bufferIndex = -1;   // indicates bbox
      result.triangle = globalTriangle;
      result.triangleIndex = globalTriangleIndex; // triangle of bbox 0..11
      result.distance = dbSqrt(globalHitDist);
      result.position = globalHitPos;
   }
   return doesIntersect;
}


bool
SMesh::intersectRay( Ray3< T > const & ray, MeshIntersectionResult< T > & result ) const
{
   if ( !intersectRayWithBoundingBox( ray, result ) )
   {
      return false;
   }

   bool doesIntersect = false;

   T const beginDist = std::numeric_limits< T >::max();

   T globalHitDist = beginDist;

   glm::tvec3< T > globalHitPos;

   int32_t globalBufferIndex = 0;

   Triangle3< T > globalTriangle;

   size_t globalTriangleIndex = 0;

   for ( size_t i = 0; i < buffers.size(); ++i )
   {
      SMeshBuffer const & buffer = buffers[ i ];

      if ( !buffer.intersectRayWithBoundingBox( ray, result ) )
      {
         continue;
      }

      buffer.forAllTriangles(
         [&] ( uint32_t triangleIdx, Triangle3f const & tri )
         {
            glm::tvec3< T > tmpPos;
            //auto a = glm::tvec3< T >( trs * glm::tvec4< T >(tri.A,T(1)) );
            //auto b = glm::tvec3< T >( trs * glm::tvec4< T >(tri.B,T(1)) );
            //auto c = glm::tvec3< T >( trs * glm::tvec4< T >(tri.C,T(1)) );
            Triangle3< T > triangle( tri.A, tri.B, tri.C );
            if ( triangle.intersectRay( ray.getPos(), ray.getDir(), tmpPos ) )
            {
               auto tmpDist = glm::length2( ray.getPos() - tmpPos );
               //DE_DEBUG("Node[",i,"](", node->getName(),").Triangle[",t,"].Hit(", tmpPosition,"), "
               //   "d(", d, "), dist(", dist, "), tri(", tri.toString(), ")")
               if ( globalHitDist == beginDist || globalHitDist >= tmpDist )
               {
                  doesIntersect = true;
                  globalBufferIndex = int32_t( i );
                  globalHitDist = tmpDist;
                  globalHitPos = tmpPos;
                  globalTriangle = triangle;
                  globalTriangleIndex = triangleIdx;
               }
            }
            //DE_DEBUG("Mesh[",i,"].Triangle[", t, "] = ", tri.toString() )
         }
      );
   }

   if ( doesIntersect )
   {
      result.meshIndex = 0;
      result.bufferIndex = globalBufferIndex;
      result.triangle = globalTriangle;
      result.triangleIndex = globalTriangleIndex;
      result.distance = dbSqrt(globalHitDist);
      result.position = globalHitPos;

      //DE_DEBUG("Got intersect buffer ", result.bufferIndex, " of ", mesh.getMeshBufferCount())
   }

   return doesIntersect;
}

/*
static void
shiftMeshBuffer( int shifts )
{
    printf( "shiftMeshBuffer( shifts = %d, buffercount = %d )\n", shifts, getMeshBufferCount() );
    if ( shifts < 1 ) return;
    uint32_t bufferCount = getMeshBufferCount();
    for ( uint32_t b = 0; b < bufferCount; ++b )
    {
        uint32_t k = ( uint32_t(shifts) + b ) % bufferCount; // looks like no neg shifts, yet.
        SMeshBuffer* tmp = Buffers[ k ];          // save value at target position
        Buffers[ k ] = Buffers[ b ];// overwrite target position with new value
        Buffers[ b ] = tmp;// overwrite source position with save row-data
    }
}

static bool
loadMeshBufferXML( std::string uri, SMeshBuffer & buffer )
{
   return false;
}

static bool
saveMeshBufferXML( std::string uri, SMeshBuffer const & buffer )
{
   tinyxml2::XMLDocument doc;
   tinyxml2::XMLElement* limb = doc.NewElement( "SMeshBuffer" );
   limb->SetAttribute( "n", buffer.getName().c_str() );
   limb->SetAttribute( "p", buffer.getPrimitiveTypeStr().c_str() );
   limb->SetAttribute( "v", buffer.getVertexCount() );
   limb->SetAttribute( "i", buffer.getIndexCount() );

   // Write Material:

   tinyxml2::XMLElement* matNode = doc.NewElement( "Material" );
   matNode->SetAttribute( "n", buffer.Material.name.c_str() );

   for ( size_t i = 0; i < 4; ++i )
   {
      auto const & tex = buffer.Material.getTexture( i );
      if ( !tex.empty() )
      {
         tinyxml2::XMLElement* texNode = doc.NewElement( "Tex" );
         texNode->SetAttribute("stage", i );
         texNode->SetText( tex.tex->getName().c_str() );
         matNode->InsertEndChild( texNode );
      }
   }

   limb->InsertEndChild( matNode );

   // Write Vertices:

   for ( size_t vi=0; vi < buffer.Vertices.size(); ++vi )
   {
      S3DVertex const & v = buffer.Vertices[ vi ];
      tinyxml2::XMLElement* vlem = doc.NewElement( "v" );
      if ( v.pos.x != 0.f ) { vlem->SetAttribute( "x", v.pos.x ); }
      if ( v.pos.y != 0.f ) { vlem->SetAttribute( "y", v.pos.y ); }
      if ( v.pos.z != 0.f ) { vlem->SetAttribute( "z", v.pos.z ); }
      if ( v.normal.x != 0.f ) { vlem->SetAttribute( "nx", v.normal.x ); }
      if ( v.normal.y != 0.f ) { vlem->SetAttribute( "ny", v.normal.y ); }
      if ( v.normal.z != 0.f ) { vlem->SetAttribute( "nz", v.normal.z ); }
      if ( v.color != 0xFFFFFFFF ) { vlem->SetAttribute( "color", v.color ); }
      if ( v.tex.x != 0.f ) { vlem->SetAttribute( "u", v.tex.x ); }
      if ( v.tex.y != 0.f ) { vlem->SetAttribute( "v", v.tex.y ); }

      limb->InsertEndChild( vlem );
   }

   // Write Indices:

   if ( buffer.Indices.size() > 0 )
   {
      tinyxml2::XMLElement* ilem = doc.NewElement( "i" );

      std::ostringstream oss;
      oss << buffer.Indices[ 0 ];

      for ( size_t k = 1; k < buffer.Indices.size(); ++k )
      {
         oss << "," << buffer.Indices[ k ];
      }
      ilem->SetText( oss.str().c_str() );
      limb->InsertEndChild( ilem );
   }

   doc.InsertFirstChild( limb );

   tinyxml2::XMLError err = doc.SaveFile( uri.c_str() );
   if ( err != tinyxml2::XML_SUCCESS )
   {
      DE_ERROR("Cant save");
      return false;
   }

   return true;
}
*/




// ===========================================================================
// ===========================================================================
// ===========================================================================

void
SMeshTool::flipX( SMesh & o )
{
    for ( SMeshBuffer & buf : o.buffers )
    {
        SMeshBufferTool::flipX( buf );
    }
    o.recalculateBoundingBox();
}
void
SMeshTool::flipY( SMesh & o )
{
    for ( SMeshBuffer & buf : o.buffers )
    {
        SMeshBufferTool::flipY( buf );
    }
    o.recalculateBoundingBox();
}
void
SMeshTool::flipZ( SMesh & o )
{
    for ( SMeshBuffer & buf : o.buffers )
    {
        SMeshBufferTool::flipZ( buf );
    }
    o.recalculateBoundingBox();
}
void
SMeshTool::flipXY( SMesh & o )
{
    for ( SMeshBuffer & buf : o.buffers )
    {
        SMeshBufferTool::flipXY( buf );
    }
    o.recalculateBoundingBox();
}
void
SMeshTool::flipYZ( SMesh & o )
{
    for ( SMeshBuffer & buf : o.buffers )
    {
        SMeshBufferTool::flipYZ( buf );
    }
    o.recalculateBoundingBox();
}
void
SMeshTool::flipXZ( SMesh & o )
{
    for ( SMeshBuffer & buf : o.buffers )
    {
        SMeshBufferTool::flipXZ( buf );
    }
    o.recalculateBoundingBox();
}

void
SMeshTool::flipWinding( SMeshBuffer & buf )
{
    SMeshBufferTool::flipWinding( buf );
}

void
SMeshTool::flipWinding( SMesh & o )
{
    for ( SMeshBuffer & buf : o.buffers )
    {
        SMeshBufferTool::flipWinding( buf );
    }
}

void
SMeshTool::flipNormals( SMeshBuffer & o )
{
    SMeshBufferTool::flipNormals( o );
}

void
SMeshTool::flipNormals( SMesh & o )
{
    for ( SMeshBuffer & buf : o.buffers )
    {
        SMeshBufferTool::flipNormals( buf );
    }
}

void
SMeshTool::colorVertices( SMesh & o, uint32_t color )
{
    for ( SMeshBuffer & buf : o.buffers )
    {
        SMeshBufferTool::colorVertices( buf, color );
    }
}

void
SMeshTool::transformVertices( SMesh & o, glm::dmat4 const & trs )
{
    for ( SMeshBuffer & buf : o.buffers )
    {
        SMeshBufferTool::transformVertices( buf, trs );
    }

    o.recalculateBoundingBox();
}

void
SMeshTool::translateVertices( SMesh & o, glm::vec3 const & offset )
{
    for ( SMeshBuffer & buf : o.buffers )
    {
        for ( size_t j = 0; j < buf.getVertexCount(); ++j )
        {
            buf.vertices[ j ].pos += offset;
        }
        buf.recalculateBoundingBox();
    }

    o.recalculateBoundingBox();
}

void
SMeshTool::translateVertices( SMesh & o, float x, float y, float z )
{
    translateVertices( o, glm::vec3(x,y,z) );
}

void
SMeshTool::rotateVertices( SMesh & o, float a, float b, float c )
{
    for ( SMeshBuffer & buf : o.buffers )
    {
        SMeshBufferTool::rotateVertices( buf, a,b,c );
    }

    o.recalculateBoundingBox();
}

void
SMeshTool::rotateVertices( SMesh & o, glm::vec3 const & degrees )
{
    rotateVertices( o, degrees.x, degrees.y, degrees.z );
}

void
SMeshTool::scaleVertices( SMesh & o, glm::vec3 const & scale )
{
    for ( SMeshBuffer & buf : o.buffers )
    {
        for ( auto & vertex : buf.vertices )
        {
            vertex.pos *= scale;
        }
        buf.recalculateBoundingBox();
    }

    o.recalculateBoundingBox();
}

void
SMeshTool::scaleVertices( SMesh & o, float x, float y, float z )
{
    scaleVertices( o, glm::vec3(x,y,z) );
}

void
SMeshTool::centerVertices( SMesh & o )
{
    o.recalculateBoundingBox();
    translateVertices( o, -o.getBoundingBox().getCenter() );
}

void
SMeshTool::recalculateNormals( SMesh & o )
{
    o.recalculateNormals();
}

void
SMeshTool::fitVertices( SMesh & o, float maxSize )
{
    o.recalculateBoundingBox();
    float x = o.getBoundingBox().getCenter().x;
    float z = o.getBoundingBox().getCenter().z;
    float y = o.getBoundingBox().getMin().y;
    translateVertices( o, -glm::vec3(x,y,z) );

    glm::vec3 const s = o.getBoundingBox().getSize();
    float s_max = std::max( std::max( s.x, s.y ), s.z );
    float f = maxSize / s_max;
    scaleVertices( o, glm::vec3(f,f,f) );
}

void
SMeshTool::removeDoubleVertices( SMesh & mesh )
{
    DE_DEBUG("BEFORE: ", mesh.toString() )

    //   std::vector< TexRef > deball;
    //   std::vector< TexRef > sorted;
    //   for ( auto const & buffer : mesh.Buffers )
    //   {
    //      std::vector< TexRef > unsorted = buffer.getMaterial().getTextures();

    //      for ( auto const & ref : unsorted )
    //      {
    //         auto it = std::find_if( sorted.begin(), sorted.end(),
    //            [&]( TexRef const & cached )
    //            {
    //               if ( cached.m_tex != ref.m_tex ) return false;
    //               if ( cached.m_rect != ref.m_rect ) return false;
    //               if ( cached.m_repeat != ref.m_repeat )
    //               {
    //                  DE_WARN("Differs in m_repeat tex scaling")
    //                  return false;
    //               }
    //               return true;
    //            }
    //         );

    //         if ( it == sorted.end() )
    //         {
    //            sorted.emplace_back( ref );
    //         }

    //         deball.emplace_back( ref );
    //      }
    //   }

    //   DE_WARN("deball.size() = ", deball.size())
    //   DE_WARN("sorted.size() = ", sorted.size())
    DE_DEBUG("AFTER: ", mesh.toString())
}

void
SMeshTool::removeDoubleMaterials( SMesh & mesh )
{
    DE_DEBUG("BEFORE: ", mesh.toString() )

    std::vector< TexRef > deball;
    std::vector< TexRef > sorted;
    for ( auto const & buffer : mesh.buffers )
    {
        std::vector< TexRef > unsorted = buffer.getMaterial().getTextures();

        for ( auto const & ref : unsorted )
        {
            auto it = std::find_if( sorted.begin(), sorted.end(),
                                   [&]( TexRef const & cached )
                                   {
                                       if ( cached.tex != ref.tex ) return false;
                                       if ( cached.coords != ref.coords ) return false;
                                       if ( cached.repeat != ref.repeat )
                                       {
                                           DE_WARN("Differs in m_repeat tex scaling")
                                           return false;
                                       }
                                       return true;
                                   }
                                   );

            if ( it == sorted.end() )
            {
                sorted.emplace_back( ref );
            }

            deball.emplace_back( ref );
        }
    }


    for ( auto const & buffer : mesh.buffers )
    {
        std::vector< TexRef > unsorted = buffer.getMaterial().getTextures();

        for ( auto const & ref : unsorted )
        {
            auto it = std::find_if( sorted.begin(), sorted.end(),
                                   [&]( TexRef const & cached )
                                   {
                                       if ( cached.tex != ref.tex ) return false;
                                       if ( cached.coords != ref.coords ) return false;
                                       if ( cached.repeat != ref.repeat )
                                       {
                                           DE_WARN("Differs in m_repeat tex scaling")
                                           return false;
                                       }
                                       return true;
                                   }
                                   );

            if ( it == sorted.end() )
            {
                sorted.emplace_back( ref );
            }

            deball.emplace_back( ref );
        }
    }

    DE_WARN("deball.size() = ", deball.size())
    DE_WARN("sorted.size() = ", sorted.size())
    DE_DEBUG("AFTER: ", mesh.toString())
    /*
   SMesh tmp( mesh.getName() );
   tmp.Buffers.reserve( mesh.getMeshBufferCount() );

   for ( size_t i = 1; i < mesh.Buffers.size(); ++i )
   {
      Material const & unsorted = mesh.Buffers[ i ].getMaterial();

      auto it = std::find_if( tmp.Buffers.begin(), tmp.Buffers.end(),
         [&]( SMeshBuffer const & cached )
      {
         auto sorted = cached.getMaterial().getTextures();
         if ( sorted.m_tex != unsorted.m_tex ) return false;
            if ( other.m_rect != m_rect ) return false;
            if ( other.m_repeat != m_repeat )
            {

         if (buf.m_vertices.empty()) return false;
         if (buf.m_vertices.size() < 2 && buf.PrimType == PrimitiveType::Lines) return false;
         if (buf.m_vertices.size() < 3 && buf.PrimType == PrimitiveType::Triangles) return false;
         return true;
      }; );

      for ( size_t k = 0; i < mesh.Buffers.size(); ++i )
      {
      auto const & buf = mesh.Buffers[ i ];
      if ( shouldPersist( buf ) )
      {
         tmp.Buffers.emplace_back( std::move(buf) );
      }
   }

   if ( other.m_tex != m_tex ) return false;
   if ( other.m_rect != m_rect ) return false;
   if ( other.m_repeat != m_repeat )
   {
      DE_WARN("Differs in m_repeat tex scaling")
      return false;
   }
   */
}

void
SMeshTool::removeEmptyMeshBuffer( SMesh & mesh )
{
    // FAST version 2.0
    DE_DEBUG("BEFORE: ", mesh.toString() )

    auto shouldPersist = []( SMeshBuffer const & buf )
    {
        if (buf.vertices.empty()) return false;
        if (buf.vertices.size() < 2 && buf.primitiveType == PrimitiveType::Lines) return false;
        if (buf.vertices.size() < 3 && buf.primitiveType == PrimitiveType::Triangles) return false;
        return true;
    };

    size_t nExpect = std::count_if( mesh.buffers.begin(), mesh.buffers.end(), shouldPersist );

    SMesh tmp; // ( mesh.getName() );
    tmp.buffers.reserve( nExpect );

    for ( size_t i = 0; i < mesh.buffers.size(); ++i )
    {
        auto const & buf = mesh.buffers[ i ];
        if ( shouldPersist( buf ) )
        {
            tmp.buffers.emplace_back( std::move(buf) );
        }
    }

    if ( tmp.getMeshBufferCount() < mesh.getMeshBufferCount() )
    {
        mesh = tmp;
    }

    mesh.recalculateBoundingBox( true );

    DE_DEBUG("AFTER: ", mesh.toString() )

    /* SLOW version 1.0
   DE_DEBUG("BEFORE: mesh ", mesh.toString() )
   size_t n = 0;

   auto predicate = [&]( SMeshBuffer const & buf )
   {
      if (buf.m_vertices.empty())
      {
         n++;
         return true;
      }
      if (buf.m_vertices.size() < 2 && buf.PrimType == PrimitiveType::Lines)
      {
         n++;
         return true;
      }
      if (buf.m_vertices.size() < 3 && buf.PrimType == PrimitiveType::Triangles)
      {
         n++;
         return true;
      }
      return false;
   };

   auto it = std::find_if( mesh.Buffers.begin(), mesh.Buffers.end(), predicate );
   while ( it != mesh.Buffers.end() )
   {
      n++;
      mesh.Buffers.erase( it );
      it = std::find_if( mesh.Buffers.begin(), mesh.Buffers.end(), predicate );
   }

   DE_DEBUG("AFTER: mesh ", mesh.toString() )
   return n;
*/
}











} // end namespace gpu.
} // end namespace de.

