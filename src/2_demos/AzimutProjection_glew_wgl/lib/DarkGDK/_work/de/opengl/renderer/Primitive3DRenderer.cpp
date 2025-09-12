#include <de/gpu/renderer/Primitive3DRenderer.hpp>
#include <de/gpu/VideoDriver.hpp>

namespace de {
namespace gpu {

Primitive3DRenderer::Primitive3DRenderer( VideoDriver* driver ) : m_driver( driver ) {}
Primitive3DRenderer::~Primitive3DRenderer() { clear(); }
void Primitive3DRenderer::setDriver( VideoDriver* driver ) { m_driver = driver; }

void Primitive3DRenderer::clear()
{
   m_lines.clear();
   m_points.clear();
   m_triangles.clear();
}

void Primitive3DRenderer::render()
{
   if (!m_driver) { DE_ERROR("No driver") return; }
   m_driver->resetModelMatrix();
   m_driver->draw3D( m_triangles );
   m_driver->draw3D( m_lines );
   m_driver->draw3D( m_points );
}

void
Primitive3DRenderer::addLine(
   glm::vec3 a, glm::vec3 b,
   uint32_t colorA, uint32_t colorB )
{
   m_lines.addVertex( S3DVertex( a.x, a.y, a.z, 0.f, 0.f, 0.f, colorA, 0,0 ) ); // A
   m_lines.addVertex( S3DVertex( b.x, b.y, b.z, 0.f, 0.f, 0.f, colorB, 0,0 ) ); // B
   m_lines.addIndexedLine();
}

void
Primitive3DRenderer::addLine(
   glm::vec3 a, glm::vec3 b,
   uint32_t color )
{
   addLine( a,b, color, color );
}

void
Primitive3DRenderer::addLineTriangle(
   glm::vec3 a, glm::vec3 b, glm::vec3 c,
   uint32_t colorA, uint32_t colorB, uint32_t colorC )
{
   S3DVertex A( a.x, a.y, a.z, 0.f, 0.f, 0.f, colorA, 0,0 );
   S3DVertex B( b.x, b.y, b.z, 0.f, 0.f, 0.f, colorB, 0,0 );
   S3DVertex C( c.x, c.y, c.z, 0.f, 0.f, 0.f, colorC, 0,0 );
   m_lines.addVertex( A );
   m_lines.addVertex( B );
   m_lines.addVertex( C );
   m_lines.addIndexedLineTriangle();
}

void
Primitive3DRenderer::addLineTriangle(
   glm::vec3 a, glm::vec3 b, glm::vec3 c,
   uint32_t color )
{
   addLineTriangle( a,b,c, color, color, color );
}

void
Primitive3DRenderer::addLineRect(
   glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d,
   uint32_t colorA, uint32_t colorB, uint32_t colorC, uint32_t colorD )
{
   S3DVertex A( a.x, a.y, a.z, 0.f, 0.f, 0.f, colorA, 0,0 );
   S3DVertex B( b.x, b.y, b.z, 0.f, 0.f, 0.f, colorB, 0,0 );
   S3DVertex C( c.x, c.y, c.z, 0.f, 0.f, 0.f, colorC, 0,0 );
   S3DVertex D( d.x, d.y, d.z, 0.f, 0.f, 0.f, colorD, 0,0 );
   m_lines.addVertex( A );
   m_lines.addVertex( B );
   m_lines.addVertex( C );
   m_lines.addVertex( D );
   m_lines.addIndexedLineQuad();
}

void
Primitive3DRenderer::addLineRect(
   glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d,
   uint32_t color )
{
   addLineRect( a,b,c,d, color,color,color,color );
}


} // end namespace gpu.
} // end namespace de.
