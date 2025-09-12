#pragma once
#include "IMesh.hpp"
#include "SMeshBuffer.hpp"

namespace de {
namespace gpu {

// ===========================================================================
struct SMesh : public IMesh
// ===========================================================================
{
   typedef double T;
   typedef std::shared_ptr< SMesh > SharedPtr;
   typedef std::unique_ptr< SMesh > UniquePtr;
   typedef std::weak_ptr< SMesh > WeakPtr;

   static SharedPtr
   createShared( std::string const & name = "" ) { return std::make_shared< SMesh >( name ); }

   static SharedPtr
   createShared( SMeshBuffer const & buf ) { return std::make_shared< SMesh >( buf ); }

   SMesh();
   SMesh( char const* name );
   SMesh( std::string const & name );
   SMesh( SMeshBuffer const & buf );

   std::vector< Tex* >  getUniqueTextures() const;
   std::string getPrimTypesStr() const override;

   void upload() override;
   void clear() override;

   glm::dmat4 const &   getModelMatrix() const override { return ModelMat; }
   void  setModelMatrix( glm::dmat4 const & modelMat ) override { ModelMat = modelMat; }

   std::string getName() const override { return Name; }
   void        setName( std::string const & name ) override { Name = name; }

   bool isVisible() const override { return Visible; }
   void setVisible( bool visible ) override { Visible = visible; }

   Box3f const & getBoundingBox() const override { return BoundingBox; }
   void          setBoundingBox( Box3f const & bbox ) override { BoundingBox = bbox; }

   int              getMaterialCount() const;
   Material const & getMaterial( int i ) const;
   Material &       getMaterial( size_t i );

   void addEmptyBuffer();
   void addMeshBuffer( SMeshBuffer const & buf );
   void addMesh( SMesh const & other );

   std::vector< SMeshBuffer > const & getMeshBuffers() const { return Buffers; }
   std::vector< SMeshBuffer > & getMeshBuffers() { return Buffers; }

   int getMeshBufferCount() const;
   SMeshBuffer const & getMeshBuffer( int i ) const;
   SMeshBuffer & getMeshBuffer( int i );

   void removeMeshBuffer( int i );
   //void shiftMeshBuffer( int shifts );
   void recalculateNormals();
   void recalculateBoundingBox( bool fully = false );
   bool isIndex( int i ) const;

   std::string toString( bool printDetails = false ) const;
   size_t getVertexCount() const;
   size_t getIndexCount() const;
   size_t getByteCount() const;

   int getLighting() const;
   int getFogEnable() const;
   int getWireframe() const;

   void setLighting( int illum );
   void setWireframe( bool enable );
   void setFogEnable( bool enable );
   void setCulling( Culling cull );
   void setCulling( bool enable );
   void setDepth( Depth depth );
   void setStencil( Stencil stencil );
   void setBlend( Blend blend );
   void setTexture( int stage, Tex* tex );
   void setTexture( int stage, TexRef ref );

   //std::vector< Triangle3f > createCollisionTriangles() const;
   void flipVertexPosYZ();
   void flipNormals();

   void transformVertices( glm::dmat4 const & m );
   void translateVertices( glm::vec3 const & offset );
   void translateVertices( float x, float y, float z );
   void rotateVertices( float a, float b, float c );
   void colorVertices( uint32_t color );
   void centerVertices();
   void scaleTexture( float u, float v );
   void scaleVertices( float scale = 1.01f );
   void scaleVertices( float sx, float sy, float sz );

//   bool
//   intersectRayWithBoundingBox( Ray3< T > const & ray, MeshIntersectionResult< T > & result,
//                                glm::tmat4x4<T> const & trs ) const;

   bool
   intersectRayWithBoundingBox( Ray3< T > const & ray,
                                MeshIntersectionResult< T > & result ) const;

   bool
   intersectRay( Ray3< T > const & ray,
                 MeshIntersectionResult< T > & result ) const;

//   bool
//   intersectRay( Ray3< T > const & ray,
//                 MeshIntersectionResult< T > & result,
//                 glm::tmat4x4<T> const & modelMat = glm::tmat4x4<T>( T(1) ) ) const;
   /*
   template < typename T >
   bool intersectRay(
         Ray3< T > const & ray,
         glm::tvec3< T >* hitPosition = nullptr,
         uint32_t* bufferIndex = nullptr,
         uint32_t* triangleIndex = nullptr ) const
   {
      auto hitDist = std::numeric_limits< T >::max();
      auto hitPos = glm::tvec3< T >( T(0), T(0), T(0) );

      uint32_t bufIndex = 0;
      uint32_t triIndex = 0;
      bool doesIntersect = false;

      for ( int32_t i = 0; i < getMeshBufferCount(); ++i )
      {
         auto const & p = getMeshBuffer( i );
         if ( !p.isVisible() ) continue; // nothing todo

         glm::tvec3< T > tmpPos;
         uint32_t tmpIndex = 0;
         if ( p.intersectRay( ray, &tmpPos, &tmpIndex ) )
         {
            doesIntersect = true;
            auto dist = glm::length( hitPos - ray.getPos() );
            if ( hitDist > dist )
            {
               hitDist = dist;
               hitPos = hitPos;
               bufIndex = uint32_t( i );
               triIndex = tmpIndex;
            }
         }
      }

      if ( doesIntersect )
      {
         if ( hitPosition ) { *hitPosition = hitPos; }
         if ( bufferIndex ) { *bufferIndex = bufIndex; }
         if ( triangleIndex ) { *triangleIndex = triIndex; }
      }

      return doesIntersect;
   }
*/

   DE_CREATE_LOGGER("de.gpu.SMesh")
   glm::dmat4 ModelMat;
   Box3f BoundingBox;//! Bounding box of this meshbuffer.
   std::string Name;
   std::vector< SMeshBuffer > Buffers;   //! Geometry buffers for this mesh
   bool Visible;

};





} // end namespace gpu.
} // end namespace de.
