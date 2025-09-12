#pragma once
#include <de_gpu/de_FVF.h>
#include <de_gpu/de_SMaterial.h>
#include <de_gpu/de_Math3D.h>

namespace de {

// =======================================================================
struct IMeshBuffer
// =======================================================================
{
   virtual ~IMeshBuffer() = default;
   virtual uint32_t &            getVAO() = 0; // VAO - VertexArrayObject
   virtual uint32_t &            getVBO() = 0; // VBO - VertexBufferObject
   virtual uint32_t &            getIBO() = 0; // IBO - IndexBufferObject

   virtual void                  setDirty() = 0;
   virtual void                  destroy() = 0;
   virtual void                  upload() = 0;
   virtual void                  draw() = 0;
   virtual size_t                computeMemoryConsumption() const = 0;

   virtual FVF const &           getFVF() const = 0;
   virtual FVF &                 getFVF() = 0;
   virtual void                  setFVF( FVF const & fvf ) = 0;
   virtual PrimitiveType         getPrimitiveType() const = 0;
   virtual void                  setPrimitiveType( PrimitiveType type ) = 0;
   virtual uint32_t              getPrimitiveCount() const { return PrimitiveType::getPrimitiveCount( getPrimitiveType(), getVertexCount(), getIndexCount() ); }
   virtual std::string           getPrimitiveTypeStr() const { return PrimitiveType::getString( getPrimitiveType() ); }
   virtual uint32_t              getFaceCount() const { return getPrimitiveCount(); }
   virtual uint32_t              getEdgeCount() const { return 0; } // PrimitiveType::getEdgeCount( getPrimitiveType(), getIndexCount() == 0 ? getVertexCount() : getIndexCount() ); }

   virtual size_t                getVertexCount() const = 0;
   virtual size_t                getVertexSize() const = 0;
   virtual size_t                getVerticesByteCount() const { return getVertexCount() * getVertexSize(); }
   virtual void const *          getVertices() const = 0;
   virtual void *                getVertices() = 0;
   virtual size_t                getVertexCapacity() const = 0;
   //virtual glm::vec3           getVertexPos( size_t i ) const = 0;
//   virtual glm::vec3             getPos( uint32_t i ) const = 0;
//   virtual void                  setPos( uint32_t i, glm::vec3 const & v ) = 0;

//   virtual glm::vec3             getPos( uint32_t i ) const = 0;
//   virtual glm::vec3 &           getPos( uint32_t i ) = 0;
//   virtual void                  setPos( uint32_t i, glm::vec3 const & v ) = 0;
   virtual void                  clearVertices() = 0;
   virtual void                  moreVertices( size_t n_additional ) = 0;
// virtual void                  translateVertices( glm::vec3 const & offsetPos ) = 0;
// virtual void                  normalizeNormals() = 0; // needs a_pos + a_normal

   virtual IndexType             getIndexType() const = 0;
   virtual std::string           getIndexTypeStr() const { return getIndexTypeString( getIndexType() ); }
   virtual uint32_t              getIndexSize() const = 0;
   virtual void const *          getIndices() const = 0;
   virtual void*                 getIndices() = 0;
   virtual uint32_t              getIndexCount() const = 0;
   virtual uint32_t              getIndexCapacity() const = 0;
   virtual uint32_t              getIndex( uint32_t i ) const = 0;
   virtual uint32_t &            getIndex( uint32_t i ) = 0;
   virtual void                  clearIndices() = 0;
   virtual void                  moreIndices( size_t n_additional ) = 0;
   virtual void                  addIndex( uint32_t index ) = 0;
// virtual void                  addIndexedLine( uint32_t a, uint32_t b ) = 0;
// virtual void                  addIndexedLineTriangle( uint32_t a, uint32_t b, uint32_t c ) = 0;
// virtual void                  addIndexedLineQuad( uint32_t a, uint32_t b, uint32_t c, uint32_t d ) = 0;
// virtual void                  addIndexedLineBox( uint32_t a, uint32_t b, uint32_t c, uint32_t d,
//                                                  uint32_t e, uint32_t f, uint32_t g, uint32_t h ) = 0;
// virtual void                  addIndexedTriangle( uint32_t a, uint32_t b, uint32_t c ) = 0;
// virtual void                  addIndexedQuad( uint32_t a, uint32_t b, uint32_t c, uint32_t d ) = 0;

   virtual glm::dmat4 const &    getModelMatrix() const = 0;
   virtual void                  setModelMatrix( glm::dmat4 const & modelMat ) = 0;

   virtual Box3f const &         getBoundingBox() const = 0;
   virtual Box3f &               getBoundingBox() = 0;
   virtual void                  setBoundingBox( Box3f const & bbox ) = 0;

   /// @brief Recalculates the bounding box. Should be called if the mesh changed.
   virtual void                  recalculateBoundingBox() = 0;

   virtual SMaterial const &     getMaterial() const = 0;
   virtual SMaterial &           getMaterial() = 0;
   virtual void                  setMaterial( SMaterial const & material ) = 0;
   virtual void                  setLighting( int illum ) { getMaterial().setLighting( illum ); }
   virtual void                  setFogEnable( bool enable ) { getMaterial().setFog( enable ); }
   virtual void                  setWireframe( bool enable ) { getMaterial().setWireframe( enable ); }
   virtual void                  setCulling( bool enable ) { getMaterial().setCulling( enable ); }
   virtual void                  setDepth( bool enable ) { getMaterial().setDepth( enable ); }
   virtual void                  setStencil( bool enable ) { getMaterial().setStencil( enable ); }
   virtual void                  setBlend( bool enable ) { getMaterial().setBlend( enable ); }
   virtual void                  setCulling( Culling cull ) { getMaterial().setCulling( cull ); }
   virtual void                  setDepth( Depth depth ) { getMaterial().setDepth( depth ); }
   virtual void                  setStencil( Stencil blend ) { getMaterial().setStencil( blend ); }
   virtual void                  setBlend( Blend blend ) { getMaterial().setBlend( blend ); }
   virtual void                  setPointSize( float ps ) { getMaterial().state.pointSize.m_Now = ps; }
   virtual void                  setLineWidth( float lw ) { getMaterial().state.lineWidth.m_Now = lw; }
   virtual void                  setTexture( int stage, Tex* tex ) { getMaterial().setTexture( stage, TexRef( tex ) ); }
   virtual void                  setTexture( int stage, TexRef const & ref ) { getMaterial().setTexture( stage, ref ); }

   virtual bool                  isVisible() const = 0;
   virtual void                  setVisible( bool visible ) = 0;

   virtual std::string           toString( bool withFVF = false ) const = 0;
   virtual std::string const &   getName() const = 0;
   virtual void                  setName( std::string const & name ) = 0;

   virtual void                  clear() = 0;
   virtual void                  clear( PrimitiveType primType )
   {
      clear();
      setPrimitiveType( primType );
   }


   virtual uint32_t countTriangles() const
   {
      if ( getIndexCount() > 0 )
      {
         switch( getPrimitiveType() )
         {
            case PrimitiveType::Triangles: return getIndexCount() / 3;     // #num triangles in Triangles
            case PrimitiveType::TriangleStrip: return getIndexCount() - 2; // #num triangles in Strip
            case PrimitiveType::TriangleFan: return getIndexCount() - 2;   // #num triangles in Fan
            case PrimitiveType::Quads: return 2*(getIndexCount() / 4);         // #num quads in Quads
            default: return 0;
         }
      }
      else
      {
         switch( getPrimitiveType() )
         {
            case PrimitiveType::Triangles: return getVertexCount() / 3;     // #num triangles in Triangles
            case PrimitiveType::TriangleStrip: return getVertexCount() - 2; // #num triangles in Strip
            case PrimitiveType::TriangleFan: return getVertexCount() - 2;   // #num triangles in Fan
            case PrimitiveType::Quads: return 2*(getVertexCount() / 4);     // #quads = #v / 4, #tris = 2 / quad.
            default: return 0;
         }
      }
   }

   virtual std::vector< TexRef > getTextures() const
   {
      return getMaterial().getTextures();
   }
};




} // end namespace de.
