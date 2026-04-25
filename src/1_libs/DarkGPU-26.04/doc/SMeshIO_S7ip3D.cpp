#include "SMeshIO_S7ip3D.hpp"
#include "VideoDriver.hpp"
#include <sstream>

namespace de {
namespace gpu {

// static
inline bool
readSVG3D_MeshBuffer( SMeshBuffer & mb, tinyxml2::XMLElement* xmlMeshBuffer,
                      VideoDriver* driver )
{
   if ( !xmlMeshBuffer )
   {
      return false;
   }

   // Read <vertices>
   tinyxml2::XMLElement* vertices = xmlMeshBuffer->FirstChildElement( "vertices" );
   if ( !vertices )
   {
      //DE_ERROR("No <vertices> found in <mesh-buffer>")
      return false;
   }

   // Read <vertices> attribute "crc"
   size_t nVertices = 0;
   if ( vertices->Attribute("crc") )
   {
      nVertices = atoll( vertices->Attribute("crc") );
   }
   if ( nVertices < 1 )
   {
      //DE_ERROR("No <vertex> found in <vertices>")
      return false;
   }

   mb.Vertices.clear();
   mb.Vertices.reserve( nVertices );

   tinyxml2::XMLElement* vertex = vertices->FirstChildElement( "vertex" );
   if ( !vertex )
   {
      //DE_ERROR("No <vertex> found in <vertices>")
      return false;
   }

   while ( vertex && nVertices > 0 )
   {
      S3DVertex s;
      // Read a_pos
      if ( vertex->Attribute("x") )
      {
         s.pos.x = atof( vertex->Attribute("x") );
      }
      if ( vertex->Attribute("y") )
      {
         s.pos.y = atof( vertex->Attribute("y") );
      }
      if ( vertex->Attribute("z") )
      {
         s.pos.z = atof( vertex->Attribute("z") );
      }
      // Read a_normal
      if ( vertex->Attribute("nx") )
      {
         s.normal.x = atof( vertex->Attribute("nx") );
      }
      if ( vertex->Attribute("ny") )
      {
         s.normal.y = atof( vertex->Attribute("ny") );
      }
      if ( vertex->Attribute("nz") )
      {
         s.normal.z = atof( vertex->Attribute("nz") );
      }
      // Read a_colorRGBA
      float r = 0.0f;
      if ( vertex->Attribute("r") )
      {
         r = atof( vertex->Attribute("r") );
      }
      float g = 0.0f;
      if ( vertex->Attribute("g") )
      {
         g = atof( vertex->Attribute("g") );
      }
      float b = 0.0f;
      if ( vertex->Attribute("b") )
      {
         b = atof( vertex->Attribute("b") );
      }
      float a = 0.0f;
      if ( vertex->Attribute("a") )
      {
         a = atof( vertex->Attribute("a") );
      }
      s.color = RGBA( glm::vec4(r,g,b,a) );

      // Read a_texCoord0
      if ( vertex->Attribute("u") )
      {
         s.tex.x = atof( vertex->Attribute("u") );
      }
      if ( vertex->Attribute("v") )
      {
         s.tex.y = atof( vertex->Attribute("v") );
      }

      // Read a_texCoord1 -> S3DVertex has only one texcoord ( yet ), should be 4-8
//      if ( vertex->Attribute("u2") )
//      {
//         s.tex.x = atof( vertex->Attribute("u2") );
//      }
//      if ( vertex->Attribute("v2") )
//      {
//         s.tex.y = atof( vertex->Attribute("v2") );
//      }

      vertex = vertex->NextSiblingElement( "vertex" );
      nVertices--;

      mb.Vertices.emplace_back( std::move( s ) );
   }

   return true;
}

// static
bool
S7ip3D::load( SMesh & mesh, std::string uri, VideoDriver* driver )
{
   // Open document for read (only).
   tinyxml2::XMLDocument doc;
   tinyxml2::XMLError err = doc.LoadFile( uri.c_str() );
   if ( err != tinyxml2::XML_SUCCESS )
   {
      DE_ERROR("Cant load xml ", uri)
      return false;
   }

   // Find SVG3D root tag <s7ip3d>
   tinyxml2::XMLElement* s7ip3d = doc.FirstChildElement( "s7ip3d" );
   if ( !s7ip3d )
   {
      DE_ERROR("No <s7ip3d> tag in xml ", uri)
      return false;
   }

   // Find Mesh-Buffers:
   tinyxml2::XMLElement* pmb = s7ip3d->FirstChildElement( "mesh-buffer" );
   if ( !pmb )
   {
      DE_ERROR("No <mesh-buffer> found in xml ", uri)
      return false;
   }

   // Load Mesh-Buffers:
   size_t n = 0;
   while ( pmb )
   {
      DE_DEBUG("Load <mesh-buffer> [",n,"]", uri)
      SMeshBuffer mb;
      if ( !readSVG3D_MeshBuffer( mb, pmb, driver ) )
      {
         DE_ERROR("No verticesNode found in xml ", uri)
         //return false;
      }
      else
      {
         mesh.addMeshBuffer( mb );
      }

      pmb = pmb->NextSiblingElement( "mesh-buffer" );
      n++;
   }

   return true;
}

/*

//   if ( s7ip3d->Attribute( "name" ) )
//   {
//      Name = s7ip3d->Attribute( "name" );
//   }
//   else
//   {
//      DE_ERROR("No s7ip3d name found ", uri)
//   }

//   int nMeshes = s7ip3d->IntAttribute( "meshes" );
//   int nBuffers = s7ip3d->IntAttribute( "buffers" );
//   int nVertices = s7ip3d->IntAttribute( "vertices" );
//   int nIndices = s7ip3d->IntAttribute( "indices" );

bool
loadXML( std::string uri )
{
   tinyxml2::XMLDocument doc;
   tinyxml2::XMLError err = doc.LoadFile( uri.c_str() );
   if ( err != tinyxml2::XML_SUCCESS )
   {
      DE_ERROR("Cant load xml ", uri)
      return false;
   }

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
   return true;
}

*/




// static
bool
S7ip3D::writeXML( tinyxml2::XMLDocument & doc, tinyxml2::XMLElement* bufNode,
   SMeshBuffer const & buf, VideoDriver* driver )
{
/*
   Material const & mat = buf.getMaterial();
   tinyxml2::XMLElement* matNode = doc.NewElement( "Material" );
   matNode->SetAttribute( "name", mat.Name.c_str() );
   matNode->SetAttribute( "tex-count", int( mat.getTextureCount() ) );
   mat.writeXML( doc, matNode );
   bufNode->InsertEndChild( matNode );

   for ( size_t i = 0; i < buf.getVertexCount(); ++i )
   {
      S3DVertex const & V = buf.getVertex( i );
      tinyxml2::XMLElement* vNode = doc.NewElement( "S3DVertex" );
      vNode->SetAttribute( "x", V.pos.x );
      vNode->SetAttribute( "y", V.pos.y );
      vNode->SetAttribute( "z", V.pos.z );
      vNode->SetAttribute( "nx", V.normal.x );
      vNode->SetAttribute( "ny", V.normal.y );
      vNode->SetAttribute( "nz", V.normal.z );
      vNode->SetAttribute( "r", RGBA_Rf(V.color) );
      vNode->SetAttribute( "g", RGBA_Gf(V.color) );
      vNode->SetAttribute( "b", RGBA_Bf(V.color) );
      vNode->SetAttribute( "a", RGBA_Af(V.color) );
      vNode->SetAttribute( "u", V.tex.x );
      vNode->SetAttribute( "v", V.tex.y );
      bufNode->InsertEndChild( vNode );
   }

   tinyxml2::XMLElement* iNode = doc.NewElement( "Indices" );
   iNode->SetAttribute( "count", buf.getIndexCount()  );

   std::ostringstream s;
   s << "\n";
   int k = 0;

   for ( size_t i = 0; i < buf.getIndexCount(); ++i )
   {
      if ( i > 0 ) s << " ";
      auto iA = buf.getIndex( i );
      s << iA;
      if ( k >= 12 )
      {
         s << "\n";
         k = 0;
      }
      else
      {
         ++k;
      }
   }

   iNode->SetText( s.str().c_str() );
   bufNode->InsertEndChild( iNode );
*/
   return true;
}

// static
bool
S7ip3D::save( SMesh const & mesh, std::string uri, VideoDriver* driver )
{
   DE_DEBUG("Writing XML mesh = ", uri )
/*
   tinyxml2::XMLDocument doc;
   tinyxml2::XMLElement* meshNode = doc.NewElement( "SMesh" );
   meshNode->SetAttribute( "name", mesh.Name.c_str() );
   meshNode->SetAttribute( "buf-count", mesh.getMeshBufferCount() );

   for ( size_t b = 0; b < mesh.getMeshBufferCount(); ++b )
   {
      SMeshBuffer const & buf = mesh.getMeshBuffer( b );
      tinyxml2::XMLElement* bufNode = doc.NewElement( "SMeshBuffer" );
      bufNode->SetAttribute( "name", buf.Name.c_str() );
      bufNode->SetAttribute( "prim-type", PrimitiveType::getString( buf.PrimType ).c_str() );
      writeXML( doc, bufNode, buf, driver );
      meshNode->InsertEndChild( bufNode );
   }

   doc.InsertEndChild( meshNode );
   tinyxml2::XMLError err = doc.SaveFile( uri.c_str() );
   if ( err != tinyxml2::XML_SUCCESS )
   {
      DEM_ERROR("Cant save xml ", uri)
   }
*/
   return true;
}

// static
bool
S7ip3D::save( SMeshBuffer const & buf, std::string uri, VideoDriver* driver )
{
   DE_DEBUG("Writing XML mesh = ", uri )

   tinyxml2::XMLDocument doc;
   tinyxml2::XMLElement* bufNode = doc.NewElement( "SMeshBuffer" );
   bufNode->SetAttribute( "name", buf.Name.c_str() );
   bufNode->SetAttribute( "prim-type", buf.getPrimitiveTypeStr().c_str() );
   writeXML( doc, bufNode, buf, driver );
   doc.InsertEndChild( bufNode );

   tinyxml2::XMLError err = doc.SaveFile( uri.c_str() );
   if ( err != tinyxml2::XML_SUCCESS )
   {
      DEM_ERROR("Cant save xml ", uri)
   }

   return true;
}





} // end namespace
} // end namespace
