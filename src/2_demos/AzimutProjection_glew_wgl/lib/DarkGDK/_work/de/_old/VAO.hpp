#pragma once
#include "SMeshBuffer.hpp"

namespace de {
namespace gpu {

enum EGpuBufferUsage
{
   GBU_Static = 0,
   GBU_Dynamic
};

// ===========================================================================
struct ES10
// ===========================================================================
{
   DE_CREATE_LOGGER("de.gpu.ES10")

   static int draw( uint32_t & vbo, uint32_t & ibo, uint32_t & vao,
      PrimitiveType primType, void const* vertices, uint32_t vCount, FVF const & fvf,
      void const* indices, uint32_t iCount, IndexType indexType );

};

// ===========================================================================
struct ES20
// ===========================================================================
{
   DE_CREATE_LOGGER("de.gpu.ES20")

   static int draw( uint32_t & vbo, uint32_t & ibo, uint32_t & vao,
      PrimitiveType primType, void const* vertices, uint32_t vCount, FVF const & fvf,
      void const* indices, uint32_t iCount, IndexType indexType );

};

// ===========================================================================
struct ES30
// ===========================================================================
{
   DE_CREATE_LOGGER("de.gpu.ES30")

   static int draw( uint32_t & vbo, uint32_t & ibo, uint32_t & vao,
      PrimitiveType primType, void const* vertices, uint32_t vCount, FVF const & fvf,
      void const* indices, uint32_t iCount, IndexType indexType );

};

// Manages the GpuSide of a high level mesh.
// Internally creates a VAO with VBO and IBO if needed.
// VAO is supported since ES 3.0
// VBO and IBO supported since ES2.0
// Immediate drawing supported since ES 1.0 ( no gpu buffer, but can be fast for dynamic things )
// ===========================================================================
struct BufferTools
// ===========================================================================
{
   DE_CREATE_LOGGER("de.gpu.BufferTools")
   // Only user of #include <de/gpu/smesh/SMeshBuffer.hpp>
//   static bool drawPrimitiveList( IMeshBuffer const & buf )
//   {
//      return drawPrimitiveList( buf.getPrimitiveType(), buf.getVertices(), buf.getVertexCount(),
//         buf.getFVF(), buf.getIndices(), buf.getIndexCount(), buf.getIndexType() );
//   }

   // ES 2.0 Advanced user:
   static bool createVBO( uint32_t & vbo );
   static void destroyVBO( uint32_t & vbo );

   // ES 2.0 Advanced user semantic != implementation details
   static bool createIBO( uint32_t & ibo ) { return createVBO( ibo ); }
   static void destroyIBO( uint32_t & ibo ) { return destroyVBO( ibo ); }

   // ES 3.0 Advanced user:
   static bool createVAO( uint32_t & vao );
   static void destroyVAO( uint32_t & vao ); // ES 3.0

   // Advanced users use their own custom shader
   // Surpasses the StandardMaterial renderer
   // Does only setup geometry (state/vao/vbo/ibo).

   static void drawDetached( uint32_t & vbo, uint32_t & ibo, uint32_t & vao,
      PrimitiveType primType, void const* vertices, uint32_t vCount, FVF const & fvf,
      void const* indices, uint32_t iCount, IndexType indexType );

   static bool
   drawImmediate( PrimitiveType primType,
      void const* vertices, uint32_t vertexCount, FVF const & fvf,
      void const* indices, uint32_t indexCount, IndexType indexType, bool debug = false );

   static void
   drawPrimitiveList( uint32_t* vbo, uint32_t* ibo, uint32_t* vao,
      PrimitiveType primType, void const* vertices, uint32_t vCount, FVF const & fvf,
      void const* indices, uint32_t iCount, IndexType indexType );

   // Pro draw users

   static bool disableFVF( FVF const & fvf );
   static bool enableFVF( FVF const & fvf, uint8_t const * vertices );

   static uint32_t fromVertexAttribType( VertexAttribType type );
   static VertexAttribType toVertexAttribType( uint32_t type );
   static uint32_t fromIndexType( IndexType const indexType );
   static PrimitiveType toPrimitiveType( uint32_t const primitiveType );
   static uint32_t fromPrimitiveType( PrimitiveType const primitiveType );

   // ===========================================================================
   // VAO_Utils
   // ===========================================================================

   static void destroy( uint32_t & vbo, uint32_t & ibo, uint32_t & vao );
   static bool upload( uint32_t & vbo, uint32_t & ibo, uint32_t & vao,
      PrimitiveType primType, void const* vertices, uint32_t vCount,
      FVF const & fvf, void const* indices, uint32_t iCount, IndexType indexType );


};

} // end namespace gpu.
} // end namespace de.
