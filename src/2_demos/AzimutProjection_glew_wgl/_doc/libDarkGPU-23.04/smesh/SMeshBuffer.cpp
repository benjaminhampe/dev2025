#include "SMeshBuffer.hpp"
#include "VAO.hpp"

namespace de {
namespace gpu {

/// @brief Constructor
SMeshBuffer::SMeshBuffer( PrimitiveType primType )
   : TMeshBuffer< S3DVertex >( primType )
{
   // DE_DEBUG(getName())
}

SMeshBuffer::~SMeshBuffer()
{
   //DE_DEBUG(getName())
   destroy();
}

bool
SMeshBuffer::upload( int mode )
{
   DE_ERROR("")
   return true;
//      BufferTools::upload( m_VBO, m_IBO, m_VAO,
//      getPrimitiveType(), getVertices(), getVertexCount(), getFVF(),
//      getIndices(), getIndexCount(), getIndexType() );
}

void
SMeshBuffer::destroy()
{
   //DE_ERROR("")
   BufferTools::destroy( m_VBO, m_IBO, m_VAO );
}

void
SMeshBuffer::draw( int mode )
{
   ES10::draw( m_VBO, m_IBO, m_VAO,
               getPrimitiveType(), getVertices(), getVertexCount(), getFVF(),
               getIndices(), getIndexCount(), getIndexType() );
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

} // end namespace gpu.
} // end namespace de.
