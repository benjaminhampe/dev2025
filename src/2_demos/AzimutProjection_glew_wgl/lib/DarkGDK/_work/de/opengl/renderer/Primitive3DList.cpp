#include <de/gpu/renderer/Primitive3DList.hpp>
#include <de/gpu/VideoDriver.hpp>

namespace de {
namespace gpu {

namespace
{
   constexpr float const CONST_Z_INIT = 0.90f; // At 1.0 it disappears, not inside frustum.
   constexpr float const CONST_Z_STEP = -0.00001f; // enough for 200.000 elements, TODO: test more.
}

Primitive3DList::Primitive3DList( VideoDriver* driver )
   : m_driver( driver )
   , m_buffer( nullptr )
{

}

Primitive3DList::~Primitive3DList()
{

}

void
Primitive3DList::clear()
{
   m_Mesh.clear();
   m_buffer = nullptr;
}

void
Primitive3DList::setDriver( VideoDriver* driver )
{
   m_driver = driver;
}

void
Primitive3DList::begin( PrimitiveType primType, TexRef const & ref )
{
   // The AI: 3 conditions before adding a new buffer
   if ( !m_buffer // if no current work mesh
      || m_buffer->PrimType != primType // or work mesh differs in primtype
      || m_buffer->getMaterial().getTexture(0).m_tex != ref.m_tex ) // or work mesh differs in Tex
   {
      // ... then add buffer to mesh, STL vector is public, all standards can be used by end-user.
      m_Mesh.Buffers.emplace_back();
      m_buffer = &m_Mesh.Buffers.back();

      // Prepare buffer...
      m_buffer->Name = "PrimitiveRenderBuf" + std::to_string( m_Mesh.Buffers.size() );
      m_buffer->PrimType = primType;
      if ( ref.m_tex )
      {
         m_buffer->getMaterial().setTexture( 0, ref );
      }

      if ( primType >= PrimitiveType::LineLoop )
      {
         m_buffer->getMaterial().setLighting( 1 );
      }
      else
      {
         m_buffer->getMaterial().setLighting( 0 );
      }
   }
}

void
Primitive3DList::end()
{
   if ( m_buffer )
   {
      m_buffer->upload();
   }
}

void
Primitive3DList::add3DLine( glm::vec3 a, glm::vec3 b, uint32_t colorA, uint32_t colorB )
{
   begin( PrimitiveType::Lines );
   if ( !m_buffer ) { DE_ERROR("No current buffer") return; }
   //add( *m_buffer, pos, colorA, colorB, colorC, colorD, ref );
   m_buffer->addVertex( S3DVertex( a.x, a.y, a.z, 0.f, 0.f, 0.f, colorA, 0,0 ) ); // A
   m_buffer->addVertex( S3DVertex( b.x, b.y, b.z, 0.f, 0.f, 0.f, colorB, 0,0 ) ); // B
   m_buffer->addIndexedLine();
   end();
}

void
Primitive3DList::add3DTriangleOutline( glm::vec3 a, glm::vec3 b, glm::vec3 c, uint32_t colorA, uint32_t colorB, uint32_t colorC )
{
   begin( PrimitiveType::Lines );
   if ( !m_buffer ) { DE_ERROR("No current buffer") return; }
   //add( *m_buffer, pos, colorA, colorB, colorC, colorD, ref );
   S3DVertex A( a.x, a.y, a.z, 0.f, 0.f, 0.f, colorA, 0,0 );
   S3DVertex B( b.x, b.y, b.z, 0.f, 0.f, 0.f, colorB, 0,0 );
   S3DVertex C( c.x, c.y, c.z, 0.f, 0.f, 0.f, colorC, 0,0 );
   m_buffer->addVertex( A );
   m_buffer->addVertex( B );
   m_buffer->addVertex( C );
   m_buffer->addIndexedLineTriangle();
   end();
}

void
Primitive3DList::add3DRectOutline( glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, uint32_t colorA, uint32_t colorB, uint32_t colorC, uint32_t colorD )
{
   begin( PrimitiveType::Lines );
   if ( !m_buffer ) { DE_ERROR("No current buffer") return; }
   S3DVertex A( a.x, a.y, a.z, 0.f, 0.f, 0.f, colorA, 0,0 );
   S3DVertex B( b.x, b.y, b.z, 0.f, 0.f, 0.f, colorB, 0,0 );
   S3DVertex C( c.x, c.y, c.z, 0.f, 0.f, 0.f, colorC, 0,0 );
   S3DVertex D( d.x, d.y, d.z, 0.f, 0.f, 0.f, colorD, 0,0 );
   m_buffer->addVertex( A );
   m_buffer->addVertex( B );
   m_buffer->addVertex( C );
   m_buffer->addVertex( D );
   m_buffer->addIndexedLineQuad();
   end();
}

void
Primitive3DList::render()
{
   if (!m_driver) { DE_ERROR("No driver") return; }

   //DE_DEBUG("m_Mesh.Buffers.size() = ", m_Mesh.Buffers.size())
   m_driver->resetModelMatrix();
   m_driver->draw3D( m_Mesh );
}


} // end namespace gpu.
} // end namespace de.
