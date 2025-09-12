#include "SMeshTool.hpp"

namespace de {
namespace gpu {

void
SMeshBufferTool::rotateTexCoords90_onlyQuads( de::gpu::SMeshBuffer & mesh )
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

   size_t v1 = dst.Vertices.size();

   for ( size_t i = 0; i < src.Vertices.size(); ++i )
   {
      auto v = src.Vertices[ i ];
      v.pos += offset;
      dst.addVertex( v );
   }

   for ( size_t i = 0; i < src.Indices.size(); ++i )
   {
      dst.addIndex( v1 + src.Indices[ i ] );
   }

}

uint32_t
SMeshBufferTool::countVertices( std::vector< SMeshBuffer > const & liste )
{
   size_t n = 0;
   for ( SMeshBuffer const & b : liste ) { n += b.Vertices.size(); }
   return n;
}

uint32_t
SMeshBufferTool::countIndices( std::vector< SMeshBuffer > const & liste )
{
   size_t n = 0;
   for ( SMeshBuffer const & b : liste ) { n += b.Indices.size(); }
   return n;
}


void
SMeshBufferTool::fuse( SMeshBuffer & dst, std::vector< SMeshBuffer > const & liste )
{
   uint32_t nVerts = countVertices( liste );
   uint32_t nIndices = countVertices( liste );

   dst.Vertices.reserve( nVerts + dst.Vertices.size() );
   dst.Indices.reserve( nIndices + dst.Indices.size() );

   for ( SMeshBuffer const & src : liste )
   {
      if ( dst.getPrimitiveType() != src.getPrimitiveType() )
      {
         size_t v0 = dst.Vertices.size();

//         dst.Vertices.insert( dst.Vertices.end(), src.Vertices.begin(), src.Vertices.end() );
//         dst.Indices.insert( dst.Indices.end(), src.Indices.begin(), src.Indices.end() );
         for ( auto vtx : src.Vertices ) { dst.Vertices.emplace_back( vtx ); }
         for ( auto idx : src.Indices ) { dst.Indices.emplace_back( idx + v0 ); }
      }
      else
      {
         DE_ERROR("Differing PrimitiveTypes")
      }
   }
}


/*
   size_t v0 = src.Vertices.size();
   size_t i0 = src.Indices.size();

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

   size_t v1 = dst.Vertices.size();
   size_t i1 = dst.Indices.size();

   // Dst is indexed...
   if ( i1 > 0 )
   {
      // Src is indexed...
      if ( i0 > 0 )
      {
         //DE_DEBUG( "Add src (indexed) to dst (indexed)" )
         for ( size_t i = 0; i < src.Vertices.size(); ++i )
         {
            auto v = src.Vertices[ i ];
            v.pos += offset;
            dst.addVertex( v );
         }
         for ( size_t i = 0; i < src.Indices.size(); ++i )
         {
            dst.addIndex( v1 + src.Indices[ i ] );
         }
      }
      // Src is vertex-only...
      else
      {
         //DE_DEBUG( "Add src (vertex-only) to dst (indexed)" )
         for ( size_t i = 0; i < src.Vertices.size(); ++i )
         {
            auto v = src.Vertices[ i ];
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
         for ( size_t i = 0; i < src.Vertices.size(); ++i )
         {
            src.Vertices[ i ].pos += offset;
         }

         //DE_DEBUG( "Add src (indexed) to dst (vertex-only)" )
         for ( size_t i = 0; i < src.Indices.size(); ++i )
         {
            dst.addVertex( src.Vertices[ src.Indices[ i ] ] );
         }
      }
      // Src is vertex-only...
      else
      {
         //DE_DEBUG( "Add src (vertex-only) to dst (vertex-only)" )
         for ( size_t i = 0; i < src.Vertices.size(); ++i )
         {
            auto v = src.Vertices[ i ];
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
   dst.PrimType = src.PrimType;
   dst.getMaterial() = src.getMaterial();
   dst.BoundingBox = src.BoundingBox;
   dst.Vertices.reserve( src.Vertices.size() * 2 );

   for ( size_t i = 0; i < src.Vertices.size()/3; i++ )
   {
      S3DVertex const & A = src.Vertices[ 3*i+0 ];
      S3DVertex const & B = src.Vertices[ 3*i+1 ];
      S3DVertex const & C = src.Vertices[ 3*i+2 ];

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
         glm::vec3 const & A = b.Vertices[ b.Indices[ 3*i ] ].pos;
         glm::vec3 const & B = b.Vertices[ b.Indices[ 3*i+1 ] ].pos;
         glm::vec3 const & C = b.Vertices[ b.Indices[ 3*i+2 ] ].pos;
         tris.push_back( { A, B, C } );
      }
   }
   else
   {
      tris.reserve( tris.size() + b.getVertexCount()/3 );
      for ( size_t i = 0; i < b.getVertexCount()/3; ++i )
      {
         glm::vec3 const & A = b.Vertices[ 3*i ].pos;
         glm::vec3 const & B = b.Vertices[ 3*i+1 ].pos;
         glm::vec3 const & C = b.Vertices[ 3*i+2 ].pos;
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
      bbox.reset( vertices[ 0 ].getPos() );

      for ( size_t i = 1; i < vertices.size(); ++i )
      {
         bbox.addInternalPoint( vertices[ i ].getPos() );
      }
   }
   return bbox;
}



/// @brief Normalize all triangle normals.
void
SMeshBufferTool::bumpVertices( SMeshBuffer & o, Image* bumpMap, float bumpScale )
{
   if ( !bumpMap ) return;
   int w = bumpMap->getWidth();
   int h = bumpMap->getHeight();
   if ( w < 1 ) return;
   if ( h < 1 ) return;
   DE_DEBUG("bumpScale = ",bumpScale, ", img:", bumpMap->toString() )

   dbSaveImage( *bumpMap, "benni_bump_vertices.png" );

   auto getBump = [&] ( float u, float v )
   {
      int x = int( u * float(w) )%w;
      int y = int( v * float(h) )%h;
      int r = bumpMap->getPixel( x, y ) & 0xFF;
      return bumpScale * float( r ) / 255.0f;
   };

   for ( size_t i = 0; i < o.Vertices.size(); ++i )
   {
      S3DVertex & v = o.Vertices[ i ];
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
      DE_ERROR("No triangles ", o.getName())
      return;
   }

   //size_t primCount = o.getPrimitiveCount();

   if ( o.Indices.size() > 0 )
   {
      // DE_DEBUG( "i(", o.getIndexCount(), "), "
                // "v(", o.getVertexCount(), "), "
                // "primCount(", primCount, "), "
                // "primType(", o.getPrimitiveTypeStr(), ")" )

      for ( size_t i = 0; i < o.Indices.size()/3; ++i )
      {
         uint32_t iA = o.Indices[ 3*i + 0 ];
         uint32_t iB = o.Indices[ 3*i + 1 ];
         uint32_t iC = o.Indices[ 3*i + 2 ];
         auto const & a = o.Vertices[ iA ].pos;
         auto const & b = o.Vertices[ iB ].pos;
         auto const & c = o.Vertices[ iC ].pos;
         auto n = Math::getNormal3D( a,b,c );
         o.Vertices[ iA ].normal = n;
         o.Vertices[ iB ].normal = n;
         o.Vertices[ iC ].normal = n;
      }
   }
   else
   {
      // DE_DEBUG( "v(", o.getVertexCount(), "), "
                // "primCount(", primCount, "), "
                // "primType(", o.getPrimitiveTypeStr(), ")" )

      for ( size_t i = 0; i < o.Vertices.size()/3; ++i )
      {
         uint32_t iA = 3*i;
         uint32_t iB = iA + 1;
         uint32_t iC = iA + 2;
         auto const & a = o.Vertices[ iA ].pos;
         auto const & b = o.Vertices[ iB ].pos;
         auto const & c = o.Vertices[ iC ].pos;
         auto n = Math::getNormal3D( a,b,c );
         o.Vertices[ iA ].normal = n;
         o.Vertices[ iB ].normal = n;
         o.Vertices[ iC ].normal = n;
      }
   }
}

void
SMeshBufferTool::flipX( SMeshBuffer & o )
{
   for ( auto & vertex : o.Vertices )
   {
      auto & pos = vertex.pos;
      pos.x = -pos.x;
   }
   o.recalculateBoundingBox();
}
void
SMeshBufferTool::flipY( SMeshBuffer & o )
{
   for ( auto & vertex : o.Vertices )
   {
      auto & pos = vertex.pos;
      pos.y = -pos.y;
   }
   o.recalculateBoundingBox();
}
void
SMeshBufferTool::flipZ( SMeshBuffer & o )
{
   for ( auto & vertex : o.Vertices )
   {
      auto & pos = vertex.pos;
      pos.z = -pos.z;
   }
   o.recalculateBoundingBox();
}
void
SMeshBufferTool::flipXY( SMeshBuffer & o )
{
   for ( auto & vertex : o.Vertices )
   {
      auto & pos = vertex.pos;
      std::swap( pos.x, pos.y );
   }
   o.recalculateBoundingBox();
}
void
SMeshBufferTool::flipYZ( SMeshBuffer & o )
{
   for ( auto & vertex : o.Vertices )
   {
      auto & pos = vertex.pos;
      std::swap( pos.y, pos.z );
   }
   o.recalculateBoundingBox();
}
void
SMeshBufferTool::flipXZ( SMeshBuffer & o )
{
   for ( auto & vertex : o.Vertices )
   {
      auto & pos = vertex.pos;
      std::swap( pos.x, pos.z );
   }
   o.recalculateBoundingBox();
}


void
SMeshBufferTool::flipVertexPosYZ( SMeshBuffer & o )
{
   for ( size_t i = 0; i < o.Vertices.size(); ++i )
   {
      std::swap( o.Vertices[ i ].pos.y, o.Vertices[ i ].pos.z );
   }
   std::swap( o.BoundingBox.m_Min.y, o.BoundingBox.m_Min.z );
   std::swap( o.BoundingBox.m_Max.y, o.BoundingBox.m_Max.z );
}

void
SMeshBufferTool::flipWinding( SMeshBuffer & o )
{
   if ( o.PrimType == PrimitiveType::Triangles )
   {
      if ( o.Indices.size() > 0 )
      {
         for ( size_t i = 0; i < o.Indices.size()/3; ++i )
         {
            uint32_t tmp = o.Indices[ 3*i ];
            o.Indices[ 3*i ] = o.Indices[ 3*i + 1 ];
            o.Indices[ 3*i + 1 ] = o.Indices[ 3*i + 2 ];
            o.Indices[ 3*i + 2 ] = tmp;
         }
      }
      else
      {
         for ( size_t i = 0; i < o.Vertices.size()/3; ++i )
         {
            S3DVertex tmp = o.Vertices[ 3*i ];
            o.Vertices[ 3*i ] = o.Vertices[ 3*i + 1 ];
            o.Vertices[ 3*i + 1 ] = o.Vertices[ 3*i + 2 ];
            o.Vertices[ 3*i + 2 ] = tmp;
         }
      }
   }

   SMeshBufferTool::computeNormals( o );
}

void
SMeshBufferTool::flipNormals( SMeshBuffer & o )
{
   for ( S3DVertex & v : o.Vertices )
   {
      v.normal = -v.normal;
   }
}

void
SMeshBufferTool::translateVertices( SMeshBuffer & o, glm::vec3 const & offset )
{
   for ( auto & v : o.Vertices )
   {
      v.pos += offset;
   }
   o.BoundingBox.m_Min += offset;
   o.BoundingBox.m_Max += offset;
}

void
SMeshBufferTool::translateVertices( SMeshBuffer & o, glm::vec3 const & offset, uint32_t vStart )
{
   for ( size_t i = vStart; i < o.Vertices.size(); ++i )
   {
      o.Vertices[ i ].pos += offset;
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

   for ( size_t i = 0; i < o.Vertices.size(); ++i )
   {
      auto & pos = o.Vertices[ i ].pos;
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

   for ( size_t i = 0; i < o.Vertices.size(); ++i )
   {
      o.Vertices[ i ].pos = R * o.Vertices[ i ].pos;
      o.Vertices[ i ].normal = R * o.Vertices[ i ].normal;
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
         o.Vertices[ i ].pos *= scale;
      }
   }
   o.recalculateBoundingBox();
}

void
SMeshBufferTool::transformVertices( SMeshBuffer & o, glm::dmat4 const & modelMat )
{
   for ( S3DVertex & v : o.Vertices )
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
      o.Vertices[ i ].pos *= glm::vec3( sx,sy,sz);
   }
   o.recalculateBoundingBox();
}

void
SMeshBufferTool::translateNormals( SMeshBuffer & o, glm::vec3 const & offset )
{
   for ( auto & v : o.Vertices )
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
   for ( auto & v : o.Vertices )
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
   for ( auto & v : o.Vertices )
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
         o.Vertices[ i ].tex.x *= u;
         o.Vertices[ i ].tex.y *= v;
      }
   }
   //o.recalculateBoundingBox();
}

void
SMeshBufferTool::transformTexCoords( SMeshBuffer & o, Recti const & r_atlas, int atlas_w, int atlas_h )
{
   float scale_x = float( r_atlas.w() ) / float( atlas_w );
   float scale_y = float( r_atlas.h() ) / float( atlas_h );

   //glm::vec4 r_scale = r_atlas.toVec4( atlas_w, atlas_h );
   float trans_x = float( r_atlas.x() ) / float( atlas_w-1 );
   float trans_y = float( r_atlas.y() ) / float( atlas_h-1 );

   for ( size_t i = 0; i < o.Vertices.size(); ++i )
   {
      float & u = o.Vertices[ i ].tex.x;
      float & v = o.Vertices[ i ].tex.y;
      u = ( scale_x * u ) + trans_x;
      v = ( scale_y * v ) + trans_y;
   }

   //o.recalculateBoundingBox();
}

void
SMeshBufferTool::colorVertices( SMeshBuffer & o, uint32_t color )
{
   for ( size_t i = 0; i < o.Vertices.size(); ++i )
   {
      o.Vertices[ i ].color = color;
   }
}

void
SMeshBufferTool::centerVertices( SMeshBuffer & o )
{
   glm::vec3 const center = o.getBoundingBox().getCenter();

   for ( size_t i = 0; i < o.Vertices.size(); ++i )
   {
      o.Vertices[ i ].pos -= center;
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

//   for ( size_t i = 0; i < src.Vertices.size()/3; ++i )
//   {
//      auto A = src.Vertices[ 3*i + 0 ];
//      auto B = src.Vertices[ 3*i + 1 ];
//      auto C = src.Vertices[ 3*i + 2 ];
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

// ===========================================================================
// ===========================================================================
// ===========================================================================

void
SMeshTool::flipX( SMesh & o )
{
   for ( SMeshBuffer & buf : o.Buffers )
   {
      SMeshBufferTool::flipX( buf );
   }
   o.recalculateBoundingBox();
}
void
SMeshTool::flipY( SMesh & o )
{
   for ( SMeshBuffer & buf : o.Buffers )
   {
      SMeshBufferTool::flipY( buf );
   }
   o.recalculateBoundingBox();
}
void
SMeshTool::flipZ( SMesh & o )
{
   for ( SMeshBuffer & buf : o.Buffers )
   {
      SMeshBufferTool::flipZ( buf );
   }
   o.recalculateBoundingBox();
}
void
SMeshTool::flipXY( SMesh & o )
{
   for ( SMeshBuffer & buf : o.Buffers )
   {
      SMeshBufferTool::flipXY( buf );
   }
   o.recalculateBoundingBox();
}
void
SMeshTool::flipYZ( SMesh & o )
{
   for ( SMeshBuffer & buf : o.Buffers )
   {
      SMeshBufferTool::flipYZ( buf );
   }
   o.recalculateBoundingBox();
}
void
SMeshTool::flipXZ( SMesh & o )
{
   for ( SMeshBuffer & buf : o.Buffers )
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
   for ( SMeshBuffer & buf : o.Buffers )
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
   for ( SMeshBuffer & buf : o.Buffers )
   {
      SMeshBufferTool::flipNormals( buf );
   }
}

void
SMeshTool::colorVertices( SMesh & o, uint32_t color )
{
   for ( SMeshBuffer & buf : o.Buffers )
   {
      SMeshBufferTool::colorVertices( buf, color );
   }
}

void
SMeshTool::transformVertices( SMesh & o, glm::dmat4 const & trs )
{
   for ( SMeshBuffer & buf : o.Buffers )
   {
      SMeshBufferTool::transformVertices( buf, trs );
   }

   o.recalculateBoundingBox();
}

void
SMeshTool::translateVertices( SMesh & o, glm::vec3 const & offset )
{
   for ( SMeshBuffer & buf : o.Buffers )
   {
      for ( size_t j = 0; j < buf.getVertexCount(); ++j )
      {
         buf.Vertices[ j ].pos += offset;
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
   for ( SMeshBuffer & buf : o.Buffers )
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
   for ( SMeshBuffer & buf : o.Buffers )
   {
      for ( auto & vertex : buf.Vertices )
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
   for ( auto const & buffer : mesh.Buffers )
   {
      std::vector< TexRef > unsorted = buffer.getMaterial().getTextures();

      for ( auto const & ref : unsorted )
      {
         auto it = std::find_if( sorted.begin(), sorted.end(),
            [&]( TexRef const & cached )
            {
               if ( cached.m_tex != ref.m_tex ) return false;
               if ( cached.m_rect != ref.m_rect ) return false;
               if ( cached.m_repeat != ref.m_repeat )
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


   for ( auto const & buffer : mesh.Buffers )
   {
      std::vector< TexRef > unsorted = buffer.getMaterial().getTextures();

      for ( auto const & ref : unsorted )
      {
         auto it = std::find_if( sorted.begin(), sorted.end(),
            [&]( TexRef const & cached )
            {
               if ( cached.m_tex != ref.m_tex ) return false;
               if ( cached.m_rect != ref.m_rect ) return false;
               if ( cached.m_repeat != ref.m_repeat )
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

         if (buf.Vertices.empty()) return false;
         if (buf.Vertices.size() < 2 && buf.PrimType == PrimitiveType::Lines) return false;
         if (buf.Vertices.size() < 3 && buf.PrimType == PrimitiveType::Triangles) return false;
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
      if (buf.Vertices.empty()) return false;
      if (buf.Vertices.size() < 2 && buf.PrimType == PrimitiveType::Lines) return false;
      if (buf.Vertices.size() < 3 && buf.PrimType == PrimitiveType::Triangles) return false;
      return true;
   };

   size_t nExpect = std::count_if( mesh.Buffers.begin(), mesh.Buffers.end(), shouldPersist );

   SMesh tmp( mesh.getName() );
   tmp.Buffers.reserve( nExpect );

   for ( size_t i = 0; i < mesh.Buffers.size(); ++i )
   {
      auto const & buf = mesh.Buffers[ i ];
      if ( shouldPersist( buf ) )
      {
         tmp.Buffers.emplace_back( std::move(buf) );
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
      if (buf.Vertices.empty())
      {
         n++;
         return true;
      }
      if (buf.Vertices.size() < 2 && buf.PrimType == PrimitiveType::Lines)
      {
         n++;
         return true;
      }
      if (buf.Vertices.size() < 3 && buf.PrimType == PrimitiveType::Triangles)
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
