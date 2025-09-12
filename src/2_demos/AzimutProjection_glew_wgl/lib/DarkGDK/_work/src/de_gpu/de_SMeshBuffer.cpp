#include "de_SMeshBuffer.h"
#include <de_gpu/de_GL_debug_layer.h>
#include <de_gpu/de_FVF_GL.h>

//#include "VAO.hpp"

namespace de {

/// @brief Constructor
SMeshBuffer::SMeshBuffer( PrimitiveType primType )
   : TMeshBuffer< S3DVertex >( primType )
{
   // DE_DEBUG(getName())
}

SMeshBuffer::~SMeshBuffer()
{
   if ( m_vao )
   {
      DE_ERROR("Forgot to call SMeshBuffer.destroy()")
   }
}

void
SMeshBuffer::destroy()
{
   if ( m_vao )
   {
      de_glDeleteVertexArrays(1, &m_vao);
      m_vao = 0;
   }

   if ( m_vbo )
   {
      de_glDeleteBuffers(1, &m_vbo);
      m_vbo = 0;
   }

   if ( m_ibo )
   {
      de_glDeleteBuffers(1, &m_ibo);
      m_ibo = 0;
   }
}

void
SMeshBuffer::upload()
{
   if ( !m_vao )
   {
      de_glGenVertexArrays(1, &m_vao);
      m_shouldUpload = true;
   }

   if ( !m_vbo )
   {
      de_glGenBuffers(1, &m_vbo);
      m_shouldUpload = true;
   }

   if ( !m_ibo && m_indices.size() > 0 )
   {
      de_glGenBuffers(1, &m_ibo);
      m_shouldUpload = true;
   }

   if ( m_shouldUpload )
   {
      m_shouldUpload = false;

      de_glBindVertexArray(m_vao);

      if ( m_vbo )
      {
         size_t const vertexCount = m_vertices.size();
         size_t const vertexSize = sizeof(S3DVertex);
         size_t const vertexBytes = vertexCount * vertexSize;
         uint8_t* vertices = reinterpret_cast< uint8_t* >( m_vertices.data() );

         de_glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
         de_glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertexBytes), vertices, GL_STATIC_DRAW);

         // VertexAttribute[0] = Position3D (vec3f)
         de_glEnableVertexAttribArray( 0 );
         de_glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(0) );

         // VertexAttribute[1] = Normal3D (vec3f)
         de_glEnableVertexAttribArray( 1 );
         de_glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(12) );

         // VertexAttribute[2] = ColorRGBA (vec4ub)
         de_glEnableVertexAttribArray( 2 );
         de_glVertexAttribPointer( 2, 4, GL_UNSIGNED_BYTE, GL_FALSE, vertexSize, reinterpret_cast<void*>(24) );

         // VertexAttribute[3] = Tex2DCoord0 (vec2f)
         de_glEnableVertexAttribArray( 3 );
         de_glVertexAttribPointer( 3, 2, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<void*>(28) );

         //std::cout << "Upload " << vertexCount << " vertices" << std::endl;
         //std::cout << "Upload " << vertexBytes << " bytes" << std::endl;
         //std::cout << "Upload " << vertexSize << " stride" << std::endl;
      }

      if ( m_ibo )
      {
         size_t const indexCount = m_indices.size();
         size_t const indexSize = sizeof(uint32_t);
         size_t const indexBytes = indexCount * indexSize;
         uint8_t* indices = reinterpret_cast< uint8_t* >( m_indices.data() );

         de_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
         de_glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(indexBytes), indices, GL_STATIC_DRAW);

         //std::cout << "Upload " << vertexCount << " vertices" << std::endl;
         //std::cout << "Upload " << vertexBytes << " bytes" << std::endl;
         //std::cout << "Upload " << vertexSize << " stride" << std::endl;
      }

      de_glBindVertexArray(0);
   }

   // glDisable(GL_CULL_FACE); //GL_VALIDATE
   // //glDisable(GL_TEXTURE_2D); GL_VALIDATE
   // glDisable(GL_DEPTH_TEST); //GL_VALIDATE


//      BufferTools::upload( m_VBO, m_IBO, m_VAO,
//      getPrimitiveType(), getVertices(), getVertexCount(), getFVF(),
//      getIndices(), getIndexCount(), getIndexType() );
}



void
SMeshBuffer::draw()
{
   upload();

   // Draw call

   de_glBindVertexArray(m_vao);

   GLenum const primType = GL_PrimitiveType::toOpenGL( PrimType );
   if ( m_ibo )
   {
      de_glDrawElements( primType, GLint(m_indices.size()), GL_UNSIGNED_INT, nullptr );
   }
   else
   {
      de_glDrawArrays( primType, 0, GLint(m_vertices.size()) );
   }

   de_glBindVertexArray(0);
   //  GL_VALIDATE

   //ES10::draw( m_VBO, m_IBO, m_VAO,
   //            getPrimitiveType(), getVertices(), getVertexCount(), getFVF(),
   //            getIndices(), getIndexCount(), getIndexType() );
//   if ( mode == 0 )
//   {
//      BufferTools::drawPrimitiveList( nullptr, nullptr, nullptr, getPrimitiveType(), getVertices(), getVertexCount(), getFVF(),
//         getIndices(), getIndexCount(), getIndexType() );
//   }
//   else
//   {
//      BufferTools::drawPrimitiveList( &m_VBO, &m_IBO, &m_VAO, getPrimitiveType(), getVertices(), getVertexCount(), getFVF(),
//         getIndices(), getIndexCount(), getIndexType() );
//   }
}

} // end namespace de.
