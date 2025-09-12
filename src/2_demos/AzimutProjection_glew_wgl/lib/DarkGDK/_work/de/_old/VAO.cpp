#include "VAO.hpp"
#include <de/GL_Validate.hpp>

namespace de {
namespace gpu {

int
ES10::draw( uint32_t & vbo, uint32_t & ibo, uint32_t & vao, PrimitiveType primType,
               void const* vertices, uint32_t vCount, FVF const & fvf,
               void const* indices, uint32_t iCount, IndexType indexType )
{
   if ( !vertices || vCount < 1 ) { DE_ERROR("No vertices") return -1; } // nothing to draw
   if ( !indices && iCount > 0 ) { DE_ERROR("Malformed indices") return -2; } // malformed indices
   bool const useIndices = indices && iCount > 0;

   // +==========+
   // |   Draw   |
   // +==========+

   BufferTools::enableFVF( fvf, (uint8_t const* )vertices );

   if ( useIndices )
   {
      ::glDrawElements( GLenum( BufferTools::fromPrimitiveType( primType ) ),
                        GLsizei( iCount ),
                        GLenum( BufferTools::fromIndexType( indexType ) ),
                        (const void*)indices ); GL_VALIDATE
   }
   else
   {
      ::glDrawArrays( GLenum( BufferTools::fromPrimitiveType( primType ) ),
                      0, GLsizei( vCount ) ); GL_VALIDATE
   }

   BufferTools::disableFVF( fvf );

   //BufferTools::disableFVF( fvf );

   //::glBindVertexArray(old_vao); GL_VALIDATE
   //   ::glBindBuffer( GL_ARRAY_BUFFER, old_vbo );
   //   ::glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, old_ibo );
   return 0;
}

int
ES20::draw( uint32_t & vbo, uint32_t & ibo, uint32_t & vao, PrimitiveType primType,
               void const* vertices, uint32_t vCount, FVF const & fvf,
               void const* indices, uint32_t iCount, IndexType indexType )
{
   if ( !vertices || vCount < 1 ) { DE_ERROR("No vertices") return -1; } // nothing to draw
   if ( !indices && iCount > 0 ) { DE_ERROR("Malformed indices") return -2; } // malformed indices
   bool const useIndices = indices && iCount > 0;

   // RestorePoint
   GLint old_vao = 0; ::glGetIntegerv( GL_VERTEX_ARRAY_BINDING, &old_vao ); GL_VALIDATE;
   GLint old_vbo = 0; ::glGetIntegerv( GL_ARRAY_BUFFER_BINDING, &old_vbo ); GL_VALIDATE;
   GLint old_ibo = 0; ::glGetIntegerv( GL_ELEMENT_ARRAY_BUFFER_BINDING, &old_ibo ); GL_VALIDATE;

   //DE_ERROR("VBO(",vbo,"), IBO(",ibo,"), OLD(",old_vbo,",",old_ibo,",",old_vao,")" )
   GLuint vboId = vbo;
   GLuint iboId = ibo;

   // +================+
   // |   Create VBO   |
   // +================+
   if ( !vboId )
   {
      ::glGenBuffers( 1, &vboId ); GL_VALIDATE
      if ( vboId == 0 ) { DE_ERROR("No vbo created") return -4; }
   }

   // +=================+
   // |  bind + upload  |
   // +=================+
   ::glBindBuffer( GL_ARRAY_BUFFER, vboId); GL_VALIDATE
   if ( !vbo )
   {
      //DE_DEBUG("Upload VBO(",vboId, "), vertexCount(",vCount,")")
      size_t byteCount = size_t(vCount) * fvf.getByteSize();
      ::glBufferData( GL_ARRAY_BUFFER, byteCount, vertices, GL_STATIC_DRAW ); GL_VALIDATE
      vbo = vboId;
   }

   // +================+
   // |   Create IBO   |
   // +================+
   if ( useIndices && !iboId )
   {
      ::glGenBuffers( 1, &iboId ); GL_VALIDATE
      if ( iboId == 0 ) { DE_ERROR("No ibo created") return -5; }
   }

   // Upload only if ibo not existed before
   if ( useIndices )
   {
      ::glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, iboId ); GL_VALIDATE
      if ( !ibo && useIndices )
      {
         //DE_DEBUG("Upload IBO(",iboId, "), indexCount(",iCount,")")
         size_t bs = size_t(iCount) * getIndexTypeSize(indexType);
         ::glBufferData( GL_ELEMENT_ARRAY_BUFFER, bs, indices, GL_STATIC_DRAW ); GL_VALIDATE
         ibo = iboId;
      }
   }

   // +==========+
   // |   Draw   |
   // +==========+

   BufferTools::enableFVF( fvf, (uint8_t const* )vertices );

   if ( useIndices )
   {
      ::glDrawElements( GLenum( BufferTools::fromPrimitiveType( primType ) ),
                        GLsizei( iCount ),
                        GLenum( BufferTools::fromIndexType( indexType ) ),
                        (const void*)0 ); GL_VALIDATE
   }
   else
   {
      ::glDrawArrays( GLenum( BufferTools::fromPrimitiveType( primType ) ),
                      0, GLsizei( vCount ) ); GL_VALIDATE
   }

   BufferTools::disableFVF( fvf );

   //BufferTools::disableFVF( fvf );

   //::glBindVertexArray(old_vao); GL_VALIDATE
   //   ::glBindBuffer( GL_ARRAY_BUFFER, old_vbo );
   //   ::glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, old_ibo );
   return 0;
}

int
ES30::draw( uint32_t & vbo, uint32_t & ibo, uint32_t & vao, PrimitiveType primType,
               void const* vertices, uint32_t vCount, FVF const & fvf,
               void const* indices, uint32_t iCount, IndexType indexType )
{
   if ( !vertices || vCount < 1 ) { DE_ERROR("No vertices") return -1; } // nothing to draw
   if ( !indices && iCount > 0 ) { DE_ERROR("Malformed indices") return -2; } // malformed indices
   bool const useIndices = indices && iCount > 0;

   // RestorePoint
   GLint old_vao = 0; ::glGetIntegerv( GL_VERTEX_ARRAY_BINDING, &old_vao ); GL_VALIDATE;
   GLint old_vbo = 0; ::glGetIntegerv( GL_ARRAY_BUFFER_BINDING, &old_vbo ); GL_VALIDATE;
   GLint old_ibo = 0; ::glGetIntegerv( GL_ELEMENT_ARRAY_BUFFER_BINDING, &old_ibo ); GL_VALIDATE;

   DE_ERROR("VBO(",vbo,"), IBO(",ibo,"), VAO(",vao,"), OLD(",old_vbo,",",old_ibo,",",old_vao,")" )
   GLuint vaoId = vao;
   GLuint vboId = vbo;
   GLuint iboId = ibo;

   // +================+
   // |   Create VBO   |
   // +================+
   if ( !vboId )
   {
      ::glGenBuffers( 1, &vboId ); GL_VALIDATE
      if ( vboId == 0 ) { DE_ERROR("No vbo created") return -4; }
      //DE_DEBUG("Created VBO(",vboId, "), vertexCount(",vCount,")")
   }

   // +================+
   // |   Create IBO   |
   // +================+
   if ( useIndices && !iboId )
   {
      ::glGenBuffers( 1, &iboId ); GL_VALIDATE
      if ( iboId == 0 ) { DE_ERROR("No ibo created") return -5; }
      //DE_DEBUG("Created IBO(",iboId, "), indexCount(",iCount,")")
   }

   // +================+
   // |   Create VAO   |
   // +================+
   if ( !vaoId )
   {
      ::glGenVertexArrays( 1, &vaoId ); GL_VALIDATE
      if ( vaoId == 0 ) { DE_ERROR("No vao created") return -3; }
      //DE_DEBUG("Created VAO(",vaoId, "), vertexCount(",vCount,")")
   }

   // +==========+
   // |   Bind   |
   // +==========+

   ::glBindVertexArray( vaoId ); GL_VALIDATE


   if ( !vao )
   {
      // +=================+
      // |  bind + upload  |
      // +=================+

      // Upload only if vbo not existed before
      if ( !vbo )
      {
         DE_DEBUG("Upload VBO(",vboId, "), vertexCount(",vCount,")")
         ::glBindBuffer( GL_ARRAY_BUFFER, vboId); GL_VALIDATE
         size_t byteCount = size_t(vCount) * fvf.getByteSize();
         ::glBufferData( GL_ARRAY_BUFFER, byteCount, vertices, GL_STATIC_DRAW ); GL_VALIDATE
         vbo = vboId;
      }

      // Upload only if ibo not existed before
      if ( !ibo && useIndices )
      {
         DE_DEBUG("Upload IBO(",iboId, "), indexCount(",iCount,")")
         ::glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, iboId ); GL_VALIDATE
         size_t bs = size_t(iCount) * getIndexTypeSize(indexType);
         ::glBufferData( GL_ELEMENT_ARRAY_BUFFER, bs, indices, GL_STATIC_DRAW ); GL_VALIDATE
         ibo = iboId;
      }

      BufferTools::enableFVF( fvf, (uint8_t const* )vertices );

      ::glBindVertexArray( old_vao ); GL_VALIDATE
      ::glBindBuffer( GL_ARRAY_BUFFER, old_vbo );
      ::glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, old_ibo );

      BufferTools::disableFVF( fvf );

      vao = vaoId;
   }


   // +==========+
   // |   Draw   |
   // +==========+

   ::glBindVertexArray( vaoId ); GL_VALIDATE
   //::glBindBuffer( GL_ARRAY_BUFFER, vboId ); GL_VALIDATE
   ::glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, iboId ); GL_VALIDATE

   //BufferTools::enableFVF( fvf, (uint8_t const* )vertices );

   if ( useIndices )
   {
      ::glDrawElements( GLenum( BufferTools::fromPrimitiveType( primType ) ),
                        GLsizei( iCount ),
                        GLenum( BufferTools::fromIndexType( indexType ) ),
                        (const void*)0 ); GL_VALIDATE
   }
   else
   {
      ::glDrawArrays( GLenum( BufferTools::fromPrimitiveType( primType ) ),
                      0, GLsizei( vCount ) );   GL_VALIDATE
   }

   //BufferTools::disableFVF( fvf );


   //BufferTools::disableFVF( fvf );

   //::glBindVertexArray(old_vao); GL_VALIDATE
   //   ::glBindBuffer( GL_ARRAY_BUFFER, old_vbo );
   //   ::glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, old_ibo );
   return 0;
}
// Setup
// unsigned int cubeVAO, cubeVBO, cubeIBO;
// VBO - Since OpenGL ES 2.0 ( we love it, it means we store a mesh on graphics card memory GDDR5 )
// glGenBuffers(1, &cubeVBO);
// glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
// glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
// IBO - Since OpenGL ES 2.0 ( we love it, it means we store a mesh on graphics card memory GDDR5 )
// glGenBuffers(1, &cubeIBO);
// glBindBuffer(GL_ARRAY_BUFFER, cubeIBO);
// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), &cubeIndices, GL_STATIC_DRAW);
// VAO
// glGenVertexArrays(1, &cubeVAO);
// glBindVertexArray(cubeVAO);
// glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeIBO);
// glEnableVertexAttribArray(0);
// glEnableVertexAttribArray(1);
// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
// glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));


// ===========================================================================
// FVFUtils GLESv3
// ===========================================================================
uint32_t
BufferTools::fromPrimitiveType( PrimitiveType const primType )
{
   switch ( primType )
   {
      case PrimitiveType::Points: return GL_POINTS;
      case PrimitiveType::Lines: return GL_LINES;
      case PrimitiveType::LineStrip: return GL_LINE_STRIP;
      case PrimitiveType::LineLoop: return GL_LINE_LOOP;
      case PrimitiveType::Triangles: return GL_TRIANGLES;
      case PrimitiveType::TriangleStrip: return GL_TRIANGLE_STRIP;
      case PrimitiveType::TriangleFan: return GL_TRIANGLE_FAN;
      default: return GL_POINTS;
   }
}

PrimitiveType
BufferTools::toPrimitiveType( uint32_t const primType )
{
   switch ( primType )
   {
      case GL_POINTS: return PrimitiveType::Points;
      case GL_LINES: return PrimitiveType::Lines;
      case GL_LINE_STRIP: return PrimitiveType::LineStrip;
      case GL_LINE_LOOP: return PrimitiveType::LineLoop;
      case GL_TRIANGLES: return PrimitiveType::Triangles;
      case GL_TRIANGLE_STRIP: return PrimitiveType::TriangleStrip;
      case GL_TRIANGLE_FAN: return PrimitiveType::TriangleFan;
      default: return PrimitiveType::Points;
   }
}
uint32_t
BufferTools::fromIndexType( IndexType const indexType )
{
   switch ( indexType )
   {
      case IndexType::U8: return GL_UNSIGNED_BYTE;
      case IndexType::U16: return GL_UNSIGNED_SHORT;
      case IndexType::U32: return GL_UNSIGNED_INT;
      default: assert( false ); return 0;
   }
}

VertexAttribType
BufferTools::toVertexAttribType( uint32_t type )
{
   switch( type )
   {
      case GL_FLOAT: return VertexAttribType::F32;
      case GL_BYTE: return VertexAttribType::S8;
      case GL_SHORT: return VertexAttribType::S16;
      case GL_INT: return VertexAttribType::S32;
      case GL_UNSIGNED_BYTE: return VertexAttribType::U8;
      case GL_UNSIGNED_SHORT: return VertexAttribType::U16;
      case GL_UNSIGNED_INT: return VertexAttribType::U32;
      default: { assert( false );
         return VertexAttribType::Max;
      }
   }
}

uint32_t
BufferTools::fromVertexAttribType( VertexAttribType type )
{
   switch( type )
   {
      case VertexAttribType::F32: return GL_FLOAT;
      case VertexAttribType::S8: return GL_BYTE;
      case VertexAttribType::S16: return GL_SHORT;
      case VertexAttribType::S32: return GL_INT;
      case VertexAttribType::U8: return GL_UNSIGNED_BYTE;
      case VertexAttribType::U16: return GL_UNSIGNED_SHORT;
      case VertexAttribType::U32: return GL_UNSIGNED_INT;
      default: { assert( false );
         return 0;
      }
   }
}

bool
BufferTools::enableFVF( FVF const & fvf, uint8_t const* vertices )
{
   if ( !vertices ) { return false; }
   GLuint k = 0;
   // loop vertex attributes
   for ( size_t i = 0 ; i < fvf.m_Data.size(); ++i )
   {
      VertexAttrib const & a = fvf.m_Data[ i ];
      if ( !a.m_Enabled ) continue;
      GLint const aCount = GLint( a.m_Count );
      GLenum const aType = fromVertexAttribType( a.m_Type );
      GLsizei const aStride = GLsizei( fvf.m_Stride );
      GLboolean const bAutoNorm = a.m_Normalize ? GL_TRUE : GL_FALSE;
      ::glEnableVertexAttribArray( k ); GL_VALIDATE
      ::glVertexAttribPointer( k, aCount, aType, bAutoNorm, aStride, vertices ); GL_VALIDATE
      vertices += a.m_ByteSize; // next attribute.
      k++;
   }

   return true;
}

bool BufferTools::disableFVF( FVF const & fvf )
{
   GLuint k = 0;
   for ( size_t i = 0 ; i < fvf.m_Data.size(); ++i )
   {
      VertexAttrib const & a = fvf.m_Data[ i ];
      if ( !a.m_Enabled ) continue;
      ::glDisableVertexAttribArray( GLuint( i ) ); GL_VALIDATE
      k++;
   }
   return true;
}

void BufferTools::destroyVBO( uint32_t & vbo )
{
   if ( vbo < 1 ) { return; }
   GLuint id = vbo;
   ::glDeleteBuffers( 1, &id ); GL_VALIDATE
   vbo = 0;
}

bool BufferTools::createVBO( uint32_t & vbo )
{
   if ( vbo > 0 ) { return true; } // Nothing todo, already created.
   GLuint id = vbo;
   ::glGenBuffers( 1, &id ); GL_VALIDATE
   bool ok = id != 0; // Was buffer created?
   if ( ok ) { vbo = id; }
   else      { vbo = 0; }
   return ok;
}

void BufferTools::destroyVAO( uint32_t & vao )
{
   if ( vao < 1 ) { return; }
   GLuint id = vao;
   ::glDeleteVertexArrays( 1, &id ); GL_VALIDATE
   vao = 0;
}

bool BufferTools::createVAO( uint32_t & vao )
{
   if ( vao != 0 ) { return true; }
   GLuint id = 0;
   ::glGenVertexArrays( 1, &id ); GL_VALIDATE
   if ( id == 0 ) { vao = 0; return false; }
   else { vao = id; return true; }
}

void BufferTools::destroy( uint32_t & vbo, uint32_t & ibo, uint32_t & vao )
{
   // Nothing todo, since no gpu buffer was used.
   // Immediate mode uploads CPU side right before rendering
   // and then nothing needs to be cleaned up on GPU side, CPU mesh stays constant.
   // No gpu buffer ids have to managed or carried around.
   destroyVBO( vbo );
   destroyIBO( ibo );
   destroyVAO( vao );
}

bool
BufferTools::upload( uint32_t & vbo, uint32_t & ibo, uint32_t & vao,
   PrimitiveType primType,
   void const* vertices, uint32_t vCount, FVF const & fvf,
   void const* indices, uint32_t iCount, IndexType indexType )
{
   if ( !vertices ) { return false; }
   if ( vCount < 1 ) { return false; }

   // RestorePoint
   GLint vao1 = 0; ::glGetIntegerv( GL_VERTEX_ARRAY_BINDING, &vao1 ); GL_VALIDATE
   GLint vbo1 = 0; ::glGetIntegerv( GL_ARRAY_BUFFER_BINDING, &vbo1 ); GL_VALIDATE
   GLint ibo1 = 0; ::glGetIntegerv( GL_ELEMENT_ARRAY_BUFFER_BINDING, &ibo1 ); GL_VALIDATE

   // ==============================
   // === Upload vertices to VBO ===
   // ==============================
   if ( !vbo ) { createVBO( vbo ); }
   if ( !vbo ) { DE_ERROR("No vbo") return false; }
   ::glBindBuffer( GL_ARRAY_BUFFER, vbo); GL_VALIDATE
   size_t byteCount = size_t(vCount) * fvf.getByteSize();
   ::glBufferData( GL_ARRAY_BUFFER, byteCount, vertices, GL_STATIC_DRAW ); GL_VALIDATE

   // =============================
   // === Upload indices to IBO ===
   // =============================
   bool useIndices = indices && iCount > 0;
   if ( useIndices )
   {
      if ( !ibo ) { createIBO( ibo ); }
      if ( !ibo ) { DE_ERROR("No ibo") return false; }
      ::glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo ); GL_VALIDATE
      size_t bs = size_t(iCount) * getIndexTypeSize(indexType);
      ::glBufferData( GL_ELEMENT_ARRAY_BUFFER, bs, indices, GL_STATIC_DRAW ); GL_VALIDATE
   }

   return true;
}

void
BufferTools::drawPrimitiveList( uint32_t* vbo, uint32_t* ibo, uint32_t* vao,
   PrimitiveType primType, void const* vertices, uint32_t vCount, FVF const & fvf,
   void const* indices, uint32_t iCount, IndexType indexType )
{
   if ( vbo && ibo && vao )
   {
      BufferTools::drawDetached( *vbo, *ibo, *vao,
                        primType, vertices, vCount, fvf,
                        indices, iCount, indexType );
   }
   else
   {
      BufferTools::drawImmediate( primType, vertices, vCount, fvf,
         indices, iCount, indexType );
   }
}

bool
BufferTools::drawImmediate( PrimitiveType primType,
   void const* vertices, uint32_t vCount, FVF const & fvf,
   void const* indices, uint32_t indexCount, IndexType indexType, bool debug )
{
   if ( !vertices ) { return false; } // DE_DEBUG( "No vertices. caller(",caller,")" )
   if ( vCount < 1 ) { return false; } // DE_DEBUG( "Empty vertices. caller(",caller,")" )
   if ( indexCount > 0 && !indices ) { return false; } // DE_ERROR( "Invalid indices. caller(",caller,")" )

   bool ok = enableFVF( fvf, (uint8_t const* )vertices );
   if ( ok )
   {
      if ( indexCount > 0 )
      {
         ::glDrawElements( GLenum( fromPrimitiveType( primType ) ),
                           GLsizei( indexCount ),
                           GLenum( fromIndexType( indexType ) ),
                           indices ); GL_VALIDATE
      }
      else
      {
         ::glDrawArrays( GLenum( fromPrimitiveType( primType ) ),
                         0,
                         GLsizei( vCount ) ); GL_VALIDATE
      }
      disableFVF( fvf );
      return true;
   }
   else
   {
      return false;
   }
}


void
BufferTools::drawDetached( uint32_t & vbo, uint32_t & ibo, uint32_t & vao,
   PrimitiveType primType, void const* vertices, uint32_t vCount, FVF const & fvf,
   void const* indices, uint32_t iCount, IndexType indexType )
{
   if ( !vertices || vCount < 1 )
   {
      return; // nothing to draw
   }
   if ( !indices && iCount > 0 )
   {
      return; // malformed indices
   }

   // RestorePoint
   GLint old_vao = 0;
   GLint old_vbo = 0;
   GLint old_ibo = 0;
   ::glGetIntegerv( GL_VERTEX_ARRAY_BINDING, &old_vao ); GL_VALIDATE;
   ::glGetIntegerv( GL_ARRAY_BUFFER_BINDING, &old_vbo ); GL_VALIDATE;
   ::glGetIntegerv( GL_ELEMENT_ARRAY_BUFFER_BINDING, &old_ibo ); GL_VALIDATE;

   if ( !vao )
   {
      upload( vbo, ibo, vao, primType, vertices, vCount, fvf, indices, iCount, indexType );
    }

   if ( !vao )
   {
      DE_ERROR("No vao")
      return;
   }

   ::glBindVertexArray( vao ); GL_VALIDATE

   bool useIndices = indices && iCount > 0;
   if ( useIndices )
   {
      ::glDrawElements( GLenum( fromPrimitiveType( primType ) ),
                        GLsizei( iCount ),
                        GLenum( fromIndexType( indexType ) ),
                        (const void*)0 ); GL_VALIDATE
   }
   else
   {
      ::glDrawArrays( GLenum( fromPrimitiveType( primType ) ),
                      0, GLsizei( vCount ) );   GL_VALIDATE
   }


   ::glBindVertexArray(old_vao); GL_VALIDATE
   //   ::glBindBuffer( GL_ARRAY_BUFFER, old_vbo );
   //   ::glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, old_ibo );
}



} // end namespace gpu.
} // end namespace de.
