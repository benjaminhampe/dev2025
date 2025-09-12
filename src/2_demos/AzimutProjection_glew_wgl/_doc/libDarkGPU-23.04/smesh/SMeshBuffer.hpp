#pragma once
#include "TMeshBuffer.hpp"

namespace de {
namespace gpu {

#pragma pack( push )
#pragma pack( 1 )

// ===========================================================================
struct S3DVertex // FVF_POSITION_XYZ | FVF_NORMAL_XYZ | FVF_RGBA | FVF_TEXCOORD0
// ===========================================================================
{
   glm::vec3 pos;
   glm::vec3 normal;
   uint32_t color;
   glm::vec2 tex;    // 12 + 12 + 4 + 8 = 36 Bytes if packed
public:
   S3DVertex()
      : pos(), normal( 0, 0, 1 ), color( 0xFFFF00FF ), tex()
   {}
   S3DVertex( float x, float y, float z, float nx, float ny, float nz, uint32_t crgba, float u, float v )
      : pos( x,y,z ), normal( nx,ny,nz ), color( crgba ), tex( u,v )
   {}
   S3DVertex( glm::vec3 const & p, glm::vec3 const & nrm, uint32_t crgba, glm::vec2 const & tex0 )
      : pos( p ), normal( nrm ), color( crgba ), tex( tex0 )
   {}

   void
   flipV()
   {
      tex.y = glm::clamp( 1.0f - tex.y, 0.f, 1.0f ); // Invert v for OpenGL
   }

   // STANDARD_FVF = POS_XYZ | NORMAL_XYZ | COLOR_RGBA | TEX0_UV = 36 Bytes.
   static FVF const &
   getFVF()
   {
      static FVF const fvf( sizeof( S3DVertex ),
         {  VertexAttrib( VertexAttribType::F32, 3, VertexAttribSemantic::Pos ),
            VertexAttrib( VertexAttribType::F32, 3, VertexAttribSemantic::Normal ),
            VertexAttrib( VertexAttribType::U8,  4, VertexAttribSemantic::Color ),
            VertexAttrib( VertexAttribType::F32, 2, VertexAttribSemantic::Tex )
         }
      );
      return fvf;
   }
   glm::vec3 const & getPos() const { return pos; }
   glm::vec3 & getPos() { return pos; }
   void setPos( glm::vec3 const & position ) { pos = position; }
   glm::vec3 const & getNormal() const { return normal; }
   uint32_t const & getColor() const { return color; }
   glm::vec2 const & getTexCoord( int stage = 0 ) const { return tex; }
   glm::vec3 getTangent( glm::vec3 const & ) { return {}; }
   glm::vec3 getBiTangent( glm::vec3 const & ) { return {}; }
   void setNormal( glm::vec3 const & nrm ) { this->normal = nrm; }
   void setTangent( glm::vec3 const & ) {}
   void setBiTangent( glm::vec3 const & ) {}
   void setColor( uint32_t const & colorRGBA ) { this->color = colorRGBA; }
   void setTexCoord( glm::vec2 const & texCoord, int stage = 0 ) { this->tex = texCoord; }
};
#pragma pack( pop )



// =======================================================================
template < typename T > struct MeshIntersectionResult
// =======================================================================
{
   glm::tvec3< T > position;
   T distance = T(0);
   Triangle3< T > triangle;
   int32_t triangleIndex = -1;
   int32_t bufferIndex = -1;
   int32_t meshIndex = -1;
   int32_t nodeIndex = -1;
};

// ===========================================================================
// Standard MeshBuffer : S3DVertex
// ===========================================================================
struct SMeshBuffer : public TMeshBuffer< S3DVertex >
{
   DE_CREATE_LOGGER("de.gpu.SMeshBuffer")

   /// @brief Constructor
   explicit SMeshBuffer( PrimitiveType primType = PrimitiveType::Triangles );
   ~SMeshBuffer() override;
   bool upload( int mode = 1 ) override;
   void destroy() override;
   void draw( int mode = 0 ) override;

   virtual glm::vec3 const & getPos( uint32_t i ) const { return Vertices[ i ].pos; }
   virtual glm::vec3 &       getPos( uint32_t i ) { return Vertices[ i ].pos; }
   virtual void              setPos( uint32_t i, glm::vec3 const & v ) { Vertices[ i ].pos = v; }

   void
   recalculateBoundingBox() override
   {
      if ( Vertices.empty() )
      {
         BoundingBox.reset();
      }
      else
      {
         BoundingBox.reset( Vertices[ 0 ].getPos() );

         for ( size_t i = 1; i < Vertices.size(); ++i )
         {
            BoundingBox.addInternalPoint( Vertices[ i ].getPos() );
         }
      }
   }

   virtual void rotate( glm::vec3 new_nrm, glm::vec3 old_nrm = glm::vec3(0.0f,0.0f,-1.0f) )
   {
      if ( old_nrm != new_nrm )
      {
         recalculateBoundingBox();
         Box3f bb = getBoundingBox();
         auto cc = bb.getCenter();
         Quatf qq = Quatf::from2Vectors( old_nrm, new_nrm );
         for ( size_t i = 0; i < Vertices.size(); ++i )
         {
            auto v1 = Vertices[ i ].getPos() - cc; // dir from center to pos
            auto l1 = glm::length( v1 ); // length
            auto v2 = qq.rotate( v1 ); // rotated dir with length one.
            auto p2 = cc + v2 * l1; // pos = center + rotated dir with length len1.
            setPos( i, p2 );
         }
      }
   }

   virtual std::vector< Triangle3f >
   createCollisionTriangles() const
   {
      if ( getPrimitiveType() != PrimitiveType::Triangles ) return {};

      std::vector< Triangle3f > tris;

      if ( getIndexCount() > 0 )
      {
         tris.reserve( tris.size() + getIndexCount()/3 );
         for ( size_t i = 0; i < getIndexCount()/3; ++i )
         {
            tris.emplace_back( getPos( getIndex( 3*i   ) ),
                               getPos( getIndex( 3*i+1 ) ),
                               getPos( getIndex( 3*i+2 ) ));
         }
      }
      else
      {
         tris.reserve( tris.size() + getVertexCount()/3 );
         for ( size_t i = 0; i < getVertexCount()/3; ++i )
         {
            tris.emplace_back( getPos( 3*i ),
                               getPos( 3*i+1 ),
                               getPos( 3*i+2 ) );
         }
      }

      return tris;
   }

   virtual void
   forAllTriangles( std::function< void(uint32_t, Triangle3f const &)> const & func ) const
   {
      // [Triangles]
      if ( getPrimitiveType() == PrimitiveType::Triangles )
      {
         if ( getVertexCount() < 3 )
         {
            return; // no triangles
         }

         if ( getIndexCount() > 0 )
         {
            if ( getIndexCount() < 3 ) { return; }
            for ( size_t i = 0; i < getIndexCount()/3; i++ )
            {
               auto const & a = getPos( getIndex( 3*i ) );
               auto const & b = getPos( getIndex( 3*i+1 ) );
               auto const & c = getPos( getIndex( 3*i+2 ) );
               func( i, Triangle3f(a,b,c) );
            }
         }
         else
         {
            for ( size_t i = 0; i < getVertexCount()/3; i++ )
            {
               auto const & a = getPos( 3*i );
               auto const & b = getPos( 3*i+1 );
               auto const & c = getPos( 3*i+2 );
               func( i, Triangle3f(a,b,c) );
            }
         }
      }

      // [TriangleStrip]
      else if ( getPrimitiveType() == PrimitiveType::TriangleStrip )
      {
         if ( getVertexCount() < 3 )
         {
            return; // no triangle strip
         }

         if ( getIndexCount() > 0 )
         {
            if ( getVertexCount() < 3 ) { return; }
            if ( getIndexCount() < 3 ) { return; }

            for ( size_t i = 2; i < getIndexCount(); i++ )
            {
               auto const & a = getPos( getIndex( i-2 ) );
               auto const & b = getPos( getIndex( i-1 ) );
               auto const & c = getPos( getIndex( i ) );
               func( i-2, Triangle3f(a,b,c) );
            }
         }
         else
         {
            if ( getVertexCount() < 3 ) { return; }

            for ( size_t i = 2; i < getVertexCount(); i++ )
            {
               auto const & a = getPos( i-2 );
               auto const & b = getPos( i-1 );
               auto const & c = getPos( i );
               func( i-2, Triangle3f(a,b,c) );
            }
         }
      }

      // [BenniQuads]
      else if ( getPrimitiveType() == PrimitiveType::Quads )
      {
         if ( getVertexCount() < 4 )
         {
            return; // no quads
         }

         if ( getIndexCount() > 0 )
         {
            for ( size_t i = 0; i < getIndexCount()/4; i++ )
            {
               auto const & a = getPos( getIndex( 4*i ) );
               auto const & b = getPos( getIndex( 4*i+1 ) );
               auto const & c = getPos( getIndex( 4*i+2 ) );
               auto const & d = getPos( getIndex( 4*i+3 ) );
               // process quad, aka 2 triangles from 4 indexed vertices
               func( 2*i, Triangle3f(a,b,c) );
               func( 2*i+1, Triangle3f(a,c,d) );
            }
         }
         else
         {
            for ( size_t i = 0; i < getVertexCount()/4; i++ )
            {
               auto const & a = getPos( 4*i );
               auto const & b = getPos( 4*i+1 );
               auto const & c = getPos( 4*i+2 );
               auto const & d = getPos( 4*i+3 );
               // process quad, aka 2 triangles from 4 indexed vertices
               func( 2*i, Triangle3f(a,b,c) );
               func( 2*i+1, Triangle3f(a,c,d) );
            }
         }

      }
      else
      {
         DEM_ERROR("Unsupported PrimType = ", getPrimitiveTypeStr())
      }
   }

   template < typename T >
   bool
   intersectRayWithBoundingBox( Ray3< T > const & ray, MeshIntersectionResult< T > & result ) const
   {
      Box3< T >         bbox( BoundingBox.getMin(), BoundingBox.getMax() );
      T                 beginDist = std::numeric_limits< T >::max();
      T                 globalHitDist = beginDist;
      glm::tvec3< T >   globalHitPos;
      Triangle3< T >    globalTriangle;
      int32_t           globalTriangleIndex = -1;
      bool              doesIntersect = false;

      for ( size_t i = 0; i < 12; ++i ) // Loop 12 bbox triangles
      {
         glm::tvec3< T > localHitPos; // local

         Triangle3< T > const & localTriangle = bbox.getTriangle( i );

         if ( localTriangle.intersectRay( ray.getPos(), ray.getDir(), localHitPos ) )
         {
            auto localHitDist = glm::length( ray.getPos() - localHitPos );
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
         result.distance = globalHitDist;
         result.position = globalHitPos;
      }
      return doesIntersect;
   }

   template < typename T >
   bool
   intersectRay( Ray3< T > const & ray, MeshIntersectionResult< T > & result ) const
   {
      if ( !intersectRayWithBoundingBox( ray, result ) )
      {
         return false;
      }

      T const distMax = std::numeric_limits< T >::max();
      T globalHitDistance = distMax;
      glm::tvec3< T > globalHitPosition;
      uint32_t globalTriangleIndex = 0;
      Triangle3< T > globalTriangle;
      bool doesIntersect = false;

      forAllTriangles(
         [&] ( uint32_t triIndex, Triangle3< T > const & triangle )
         {
            glm::tvec3< T > tmpPos;
            if ( triangle.intersectRay( ray.getPos(), ray.getDir(), tmpPos ) )
            {
               doesIntersect = true;
               auto localDistance = glm::length( ray.getPos() - tmpPos );
               if ( globalHitDistance == distMax || globalHitDistance >= localDistance )
               {
                  globalTriangle = triangle;
                  globalHitDistance = localDistance;
                  globalHitPosition = tmpPos;
                  globalTriangleIndex = triIndex;
               }
            }
         }
      );

      if ( doesIntersect )
      {
         result.meshIndex = -1;     // indicates bbox
         result.bufferIndex = -1;   // indicates bbox
         result.triangle = globalTriangle;
         result.triangleIndex = globalTriangleIndex; // triangle of bbox 0..11
         result.distance = globalHitDistance;
         result.position = globalHitPosition;
      }
      return doesIntersect;
   }


};

} // end namespace gpu.
} // end namespace de.





/*

   template < typename T >
   bool
   intersectRayWithBoundingBox( Ray3< T > const & ray, MeshIntersectionResult< T > & result, glm::tmat4x4<T> const & trs ) const
   {
      auto minCorner = Math::transformVector( trs,BoundingBox.getMin() );
      auto maxCorner = Math::transformVector( trs,BoundingBox.getMax() );
      Box3< T >         bbox( minCorner, maxCorner );

      T                 beginDist = std::numeric_limits< T >::max();

      T                 globalHitDist = beginDist;

      glm::tvec3< T >   globalHitPos;

      Triangle3< T >    globalTriangle;

      int32_t           globalTriangleIndex = -1;

      bool              doesIntersect = false;

      for ( size_t i = 0; i < 12; ++i ) // Loop 12 bbox triangles
      {
         glm::tvec3< T > localHitPos; // local

         Triangle3< T > const localTriangle = bbox.getTriangle( i ).transform( trs );

         if ( localTriangle.intersectRay( ray.getPos(), ray.getDir(), localHitPos ) )
         {
            auto localHitDist = glm::length( ray.getPos() - localHitPos );
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
         result.distance = globalHitDist;
         result.position = globalHitPos;
      }
      return doesIntersect;
   }

   template < typename T >
   bool
   intersectRay(
         Ray3< T > const & ray, MeshIntersectionResult< T > ) const
   {
      bool doesIntersect = false;
      uint32_t hitIndex = 0;
      auto const distMax = std::numeric_limits< T >::max();
      auto dist = distMax;
      glm::tvec3< T > hitPos;

      forAllTriangles(
         [&] ( uint32_t triIndex, Triangle3< T > const & triangle )
         {
            glm::tvec3< T > tmpPos;
            if ( triangle.intersectRay( ray.getPos(), ray.getDir(), tmpPos ) )
            {
               doesIntersect = true;
               auto d = glm::length( ray.getPos() - tmpPos );
               if ( dist == distMax || dist >= d )
               {
                  dist = d;
                  hitPos = tmpPos;
                  hitIndex = triIndex;
               }
            }
         }
      );

      if ( doesIntersect )
      {
         if ( hitPosition ) { *hitPosition = hitPos; }
         if ( triangleIndex ) { *triangleIndex = hitIndex; }
      }
      return doesIntersect;
   }




      template < typename T >
   bool
   intersectRay(
         Ray3< T > const & ray,
         glm::tvec3< T >* hitPosition = nullptr,
         uint32_t* triangleIndex = nullptr ) const
   {
      bool doesIntersect = false;
      uint32_t hitIndex = 0;
      auto const distMax = std::numeric_limits< T >::max();
      auto dist = distMax;
      glm::tvec3< T > hitPos;

      forAllTriangles(
         [&] ( uint32_t triIndex, Triangle3< T > const & triangle )
         {
            glm::tvec3< T > tmpPos;
            if ( triangle.intersectRay( ray.getPos(), ray.getDir(), tmpPos ) )
            {
               doesIntersect = true;
               auto d = glm::length( ray.getPos() - tmpPos );
               if ( dist == distMax || dist >= d )
               {
                  dist = d;
                  hitPos = tmpPos;
                  hitIndex = triIndex;
               }
            }
         }
      );

      if ( doesIntersect )
      {
         if ( hitPosition ) { *hitPosition = hitPos; }
         if ( triangleIndex ) { *triangleIndex = hitIndex; }
      }
      return doesIntersect;
   }

   template < typename T >
   bool
   intersectRay(
         Ray3< T > const & ray,
         glm::tvec3< T >* hitPosition = nullptr,
         uint32_t* triangleIndex = nullptr ) const
   {
      bool doesIntersect = false;
      uint32_t hitIndex = 0;
      auto const distMax = std::numeric_limits< T >::max();
      auto dist = distMax;
      glm::tvec3< T > hitPos;

      forAllTriangles(
         [&] ( uint32_t triIndex, Triangle3< T > const & triangle )
         {
            glm::tvec3< T > tmpPos;
            if ( triangle.intersectRay( ray.getPos(), ray.getDir(), tmpPos ) )
            {
               doesIntersect = true;
               auto d = glm::length( ray.getPos() - tmpPos );
               if ( dist == distMax || dist >= d )
               {
                  dist = d;
                  hitPos = tmpPos;
                  hitIndex = triIndex;
               }
            }
         }
      );

      if ( doesIntersect )
      {
         if ( hitPosition ) { *hitPosition = hitPos; }
         if ( triangleIndex ) { *triangleIndex = hitIndex; }
      }
      return doesIntersect;
   }
   */

//   bool
//   load( std::string uri )
//   {
//      return false;
//   }

//   bool
//   save( std::string uri ) const
//   {
//      std::stringstream s;
//      s << "Saved Benni Mesh\n";
//      s << "fileName: " << m_Name << "\n";
//      s << "bufferCount: " << m_MeshBuffers << "\n";

//      for ( size_t i = 0; i < m_MeshBuffers.size(); ++i )
//      {
//         SMeshBuffer const * const p = m_MeshBuffers[ i ];
//         if ( !p ) continue;
//         s << "buffer[" << i << "] " << p->getName() << "\n";

//         for ( size_t v = 0; v < m_MeshBuffers.Vertsize(); ++i )
//         {
//            SMeshBuffer const * const p = m_MeshBuffers[ i ];
//            if ( !p ) continue;
//            s << "buffer[" << i << "] " << p->getName() << "\n";

//            p->Material.wireframe = enable;
//         }

//         p->Material.wireframe = enable;
//      }

//      s << "[material-count] " << m_MeshBuffers << "\n";
//      s << " )";
//      return false;
//   }
