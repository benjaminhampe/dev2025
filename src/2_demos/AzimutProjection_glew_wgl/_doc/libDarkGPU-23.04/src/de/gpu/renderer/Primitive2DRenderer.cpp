#include <de/gpu/renderer/Primitive2DRenderer.hpp>
#include <de/gpu/VideoDriver.hpp>

namespace de {
namespace gpu {

namespace
{
   constexpr float const CONST_Z_INIT = 0.90f; // At 1.0 it disappears, not inside frustum.
   constexpr float const CONST_Z_STEP = -0.00001f; // enough for 200.000 elements, TODO: test more.
}

Primitive2DRenderer::Primitive2DRenderer( VideoDriver* driver )
   : m_driver( driver )
   , m_buffer( nullptr )
   , m_zIndex( 0.0f )
{

}

Primitive2DRenderer::~Primitive2DRenderer()
{

}

void
Primitive2DRenderer::clear()
{
   m_mesh.clear();
   m_buffer = nullptr;
}

void
Primitive2DRenderer::setDriver( VideoDriver* driver )
{
   m_driver = driver;
}

void
Primitive2DRenderer::begin( PrimitiveType primType, TexRef const & ref )
{
   // The AI: 3 conditions before adding a new buffer
   if ( !m_buffer // if no current work mesh
      || m_buffer->PrimType != primType // or work mesh differs in primtype
      || m_buffer->getMaterial().getTexture(0).m_tex != ref.m_tex ) // or work mesh differs in Tex
   {
      // ... then add buffer to mesh, STL vector is public, all standards can be used by end-user.
      m_mesh.Buffers.emplace_back();
      m_buffer = &m_mesh.Buffers.back();

      // Prepare buffer...
      m_buffer->Name = "PrimitiveRenderBuf" + std::to_string( m_mesh.Buffers.size() );
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
Primitive2DRenderer::add2DRect( Recti const & pos, uint32_t colorA, uint32_t colorB, uint32_t colorC, uint32_t colorD, TexRef const & ref )
{
   begin( PrimitiveType::Triangles, ref );
   if ( !m_buffer ) { DE_ERROR("No current buffer") return; }
   SMeshRect::add( *m_buffer, pos, colorA, colorB, colorC, colorD, ref );
}

void
Primitive2DRenderer::add2DRectOutline( Recti const & pos, uint32_t colorA, uint32_t colorB, uint32_t colorC, uint32_t colorD, TexRef const & ref, int borderWidth )
{
   begin( PrimitiveType::Lines, ref );
   if ( !m_buffer ) { DE_ERROR("No current buffer") return; }
   SMeshRect::addOutline( *m_buffer, pos, colorA, colorB, colorC, colorD, ref, borderWidth );
}

void
Primitive2DRenderer::add2DRoundRect( Recti const & pos, glm::ivec2 radius, uint32_t oColor, uint32_t iColor, TexRef const & ref, int tess )
{
   begin( PrimitiveType::Triangles, ref );
   if ( !m_buffer ) { DE_ERROR("No current buffer") return; }
   addRoundRect( *m_buffer, pos, m_zIndex, radius, oColor, iColor, ref, tess );
}

void
Primitive2DRenderer::add2DRoundRectOutline( Recti const & pos, glm::ivec2 radius, uint32_t color, TexRef const & ref, int borderWidth, int tess )
{
   begin( PrimitiveType::Lines, ref );
   if ( !m_buffer ) { DE_ERROR("No current buffer") return; }
   addRoundRectOutline( *m_buffer, pos, m_zIndex, radius, color, ref, borderWidth, tess );
}

/*

void
Primitive2DRenderer::add2DText( int x, int y, std::string msg, uint32_t color, Align::EAlign align, Font5x8 const & font, TexRef const & ref )
{
   begin( PrimitiveType::Triangles, ref );
   if ( !m_buffer ) { DE_ERROR("No current buffer") return; }
   getFont5x8( font )->add2DText( *m_buffer, x,y, msg, color, align );
}

void
draw2DShadowText( VideoDriver* driver,
   int x, int y, std::wstring const & msg, uint32_t fg_color, uint32_t bg_color,
   Align::EAlign align, Font const & font )
{
   Font small = font;
   small.pixelSize -= 4;
//   TextSize ts_font = getFontAtlas( font )->getTextSize( msg );
//   TextSize ts_small = getFontAtlas( small )->getTextSize( msg );
   //driver->draw2DText( x, y, msg, bg_color, align, font );
   driver->draw2DText( x-1, y-1, msg, bg_color, align, font );
   driver->draw2DText( x-1, y, msg, bg_color, align, font );
   driver->draw2DText( x-1, y+1, msg, bg_color, align, font );
   driver->draw2DText( x, y-1, msg, bg_color, align, font );
   driver->draw2DText( x, y, msg, bg_color, align, font );
   driver->draw2DText( x, y+1, msg, bg_color, align, font );
   driver->draw2DText( x+1, y-1, msg, bg_color, align, font );
   driver->draw2DText( x+1, y, msg, bg_color, align, font );
   driver->draw2DText( x+1, y+1, msg, bg_color, align, font );
   driver->draw2DText( x, y, msg, fg_color, align, font );
}
void
Primitive2DRenderer::add3DLine( glm::vec3 a, glm::vec3 b, uint32_t colorA, uint32_t colorB )
{
   begin( PrimitiveType::Lines );
   if ( !m_buffer ) { DE_ERROR("No current buffer") return; }
   //add( *m_buffer, pos, colorA, colorB, colorC, colorD, ref );
   m_buffer->addVertex( S3DVertex( a.x, a.y, a.z, 0.f, 0.f, 0.f, colorA, 0,0 ) ); // A
   m_buffer->addVertex( S3DVertex( b.x, b.y, b.z, 0.f, 0.f, 0.f, colorB, 0,0 ) ); // B
   m_buffer->addIndexedLine();
}

void
Primitive2DRenderer::add3DTriangleOutline( glm::vec3 a, glm::vec3 b, glm::vec3 c, uint32_t colorA, uint32_t colorB, uint32_t colorC )
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
}

void
Primitive2DRenderer::add3DRectOutline( glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, uint32_t colorA, uint32_t colorB, uint32_t colorC, uint32_t colorD )
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
}




*/

void
Primitive2DRenderer::render()
{
   if (!m_driver) { DE_ERROR("No driver") return; }

   //DE_DEBUG("m_mesh.Buffers.size() = ", m_mesh.Buffers.size())

   m_driver->draw2D( m_mesh );
}


} // end namespace gpu.
} // end namespace de.
