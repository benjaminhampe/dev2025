#include <de/gpu/renderer/RayRenderer.hpp>
#include <de/gpu/VideoDriver.hpp>

namespace de {
namespace gpu {

namespace {

Image createRayImage( int opacity = 205 )
{
   // the texture is repeated in x dir, so it could be just 1px wide actually.
   int h = 128;
   Image img( 8, h );
   img.fill( 0xFFFFFFFF );

   int radius = 16; // ry

   for ( int y = 0; y < h; ++y )
   {
      uint32_t a = 255;
      float t = float( y ) / float( h-1 );

      if ( y <= radius )
      {
         a = uint32_t( float( opacity ) * sin( float(0.5*M_PI) * t ) );
      }
      else if ( y < h - 1 - radius )
      {
         a = opacity;
      }
      else
      {
         a = uint32_t( float( opacity ) * sin( float(0.5*M_PI) * t ) );
      }

      uint32_t color = RGBA( 255, 255, 255, a );
      ImagePainter::drawLineH( img, 0, img.getWidth()-1 , y, color, false );
   }

   dbSaveImage( img, "benni_mask_ray.png");

   return img;
}


} // end namespace

void
addWave( std::vector< glm::vec3 > & points, float amp, float freq, float phase, float t )
{
   for ( size_t i = 0; i < points.size(); ++i )
   {
      auto & pos = points[ i ];
      //auto t = float(i) / float( points.size() );
      auto phi = fmod( float(2.0*M_PI) * freq * (t+float(i)) + phase, float(2.0*M_PI) );
      pos.y += amp * sin( phi );
   }
}


RayRenderer::RayRenderer()
   : m_Driver(nullptr)
   , m_TimeStart(dbSeconds())
   , m_Time(0.0)
{}

//RayRenderer::~RayRenderer() {}
void
RayRenderer::setDriver( VideoDriver* driver ) { m_Driver = driver; }
void
RayRenderer::clear() { m_Items.clear(); }
void
RayRenderer::add( glm::vec3 const & pos, glm::vec3 const & dir )
{
   m_Items.emplace_back();
   Item & item = m_Items.back();
   item.pos = pos;
   item.dir = dir;
   item.t = dbSeconds();
}

void
RayRenderer::start()
{
   m_TimeStart = dbSeconds();
}

void
RayRenderer::render()
{
   if ( !m_Driver ) { DE_ERROR("No driver") return; }

//   ITexture* tex = m_Driver->getTexture( "rayr" );
//   if ( !tex ) { DE_ERROR("No tex") return; }
//   {
//      tex = m_Driver->addTexture2D( "rayr", createRayImage() );
//   }

   m_Time = dbSeconds() - m_TimeStart;

   auto camera = m_Driver->getCamera();
   if (!camera) { DE_ERROR("No camera") return; }

   int mx = m_Driver->getMouseX();
   int my = m_Driver->getMouseY();
   auto ray = camera->computeRay( mx, my );
   //auto rp = glm::vec3( ray.getPos() );
   //auto rd = glm::vec3( ray.getDir() );
   auto rp = glm::vec3( 0,400,-300 );
   auto rd = glm::vec3( 1,0,0 );

   std::vector< glm::vec3 > points( 10000 );

   for ( size_t i = 0; i < points.size(); ++i )
   {
      auto t = float(i) / float( points.size() );
      auto x = 0.0f + t * 300.0f;
      auto y = 400.0f;
      auto z = -300.0f;
      points[ i ] = glm::vec3( x,y,z );
   }

   addWave( points, 10, 30, 0, 0.01*m_Time );
   addWave( points, 5, 2, 0, 0.01*m_Time );
   addWave( points, 2, 7, 0, 0.01*m_Time );

   // Add dir lines
   SMeshBuffer lines ( PrimitiveType::Lines );
   lines.setLighting( false );
   lines.setBlend( Blend::alphaBlend() );

   auto s = points.front();
   auto e = points.back();
   lines.Vertices.reserve( 2*(1+points.size()) );
   lines.Indices.reserve( 2*(1+points.size()) );
   lines.addVertex( S3DVertex( s.x,s.y,s.z, 0,0,-1, 0xFFFFFFFF,0,0 ) );
   lines.addVertex( S3DVertex( e.x,e.y,e.z, 0,0,-1, 0xFFFFFFFF,0,0 ) );
   lines.addIndex( 0 );
   lines.addIndex( 1 );

   for ( size_t i = 1; i < points.size(); ++i )
   {
      int k1 = i - 1;
      int k2 = i;
      auto const & a = points[ k1 ];
      auto const & b = points[ k2 ];
      S3DVertex A( a.x,a.y,a.z, 0,0,-1, 0xFF00FFFF,0,0 );
      S3DVertex B( b.x,b.y,b.z, 0,0,-1, 0xFFA0FFFF,0,0 );
      int k = lines.getVertexCount();
      lines.addVertex( A );
      lines.addVertex( B );
      lines.addIndex( k );
      lines.addIndex( k+1 );
   }
   m_Driver->draw3D( lines );

}


} // end namespace gpu.
} // end namespace de.









