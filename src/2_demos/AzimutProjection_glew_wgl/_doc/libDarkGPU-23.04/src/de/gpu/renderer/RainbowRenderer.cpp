#include <de/gpu/renderer/RainbowRenderer.hpp>
#include <de/gpu/VideoDriver.hpp>

namespace de {
namespace gpu {
namespace {

Image createRainbowImage()
{
   // the texture is repeated in x dir, so it could be just 1px wide actually.
   int h = 1024;
   Image img( 8, h );
   img.fill( 0x00000000 );

   int radius = 32; // ry
   int opacity = 105;

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

      uint32_t color = RainbowColor::computeColor32( t );
      uint32_t r = RGBA_R( color );
      uint32_t g = RGBA_G( color );
      uint32_t b = RGBA_B( color );
      color = RGBA( r, g, b, a );
      ImagePainter::drawLineH( img, 0, img.getWidth()-1 , y, color, false );
   }

   dbSaveImage( img, "benni_mask_lightfiber2.png");

   return img;
}

} // end namespace

RainbowRenderer::RainbowRenderer()
   : m_ScreenGamma( 1.15f )
{
   clear();
}

RainbowRenderer::~RainbowRenderer()
{

}

void
RainbowRenderer::clear()
{
   m_Fibers.clear();

   std::vector< glm::vec3 > points;

   int n = 100;
   float rx = 2000.0f;
   float ry = 1500.0f;
   float phi_start = 0.05 * M_PI;
   float phi_delta = 0.9 * M_PI;
   float phi_step = phi_delta / float( n-1 );
   float phi = phi_start;
   for ( int i = 0; i < n; ++i )
   {
      float x = 1000 + rx * cos( phi );
      float y = ry * sin( phi );
      float z = 2000.0f;
      points.emplace_back( x, y, z );
      phi += phi_step;
   }

   SMeshBufferTool::rotatePoints( points, 0,75, 0);

   addTriangleStrip( points, 100.0f );
}

void
RainbowRenderer::addRainbow( float dx, float dy, glm::dmat4 const & modelMat,
   float lineWidth, int nSegments )
{
   std::vector< glm::vec3 > points;

   float rx = 0.5f * dx;
   float ry = 0.5f * dy;
   float phi_start = 0.05 * M_PI;
   float phi_delta = 0.9 * M_PI;
   float phi_step = phi_delta / float( nSegments-1 );
   float phi = phi_start;
   for ( int i = 0; i < nSegments; ++i )
   {
      float x = rx * cos( phi );
      float y = ry * sin( phi );
      points.emplace_back( x, y, 0.f );
      phi += phi_step;
   }

   //SMeshBufferTool::rotatePoints( points, 0,75, 0);
   //SMeshBufferTool::transformPoints( points, modelMat );
   addTriangleStrip( points, lineWidth );

   SMeshBufferTool::transformVertices( m_Fibers.back().mesh, modelMat );
}

void
RainbowRenderer::addTriangleStrip( std::vector< glm::vec3 > points, float lineWidth )
{
   SRainbow fiber;
   fiber.points = points;
   fiber.mesh.setPrimitiveType( PrimitiveType::TriangleStrip );
   fiber.mesh.setLineWidth( lineWidth );
   fiber.mesh.setLighting( 0 );
   fiber.mesh.setBlend( Blend::alphaBlend() );
   fiber.mesh.setCulling( Culling::disabled() );

   float y0 = -300.0f;
   if ( points.size() < 3 ) return;

   glm::vec3 a = points[ 0 ];
   glm::vec3 b = points[ 1 ];
   glm::vec3 c = points[ 2 ];

   glm::vec3 nab = Math::getNormal3D( a, b );
   glm::vec3 nbc = Math::getNormal3D( b, c );
   glm::vec3 nabc = (nab + nbc) * 0.5f;
   S3DVertex AL( a.x, y0 + a.y, a.z, -nab.x,-nab.y,-nab.z, 0xFFFFFFFF, 0, 0 );
   S3DVertex AR( a.x, y0 + a.y, a.z, nab.x,nab.y,nab.z,0xFFFFFFFF, 0, 1 );
   S3DVertex BL( b.x, y0 + b.y, b.z, -nabc.x,-nabc.y,-nabc.z, 0xFFFFFFFF, 1, 0 );
   S3DVertex BR( b.x, y0 + b.y, b.z, nabc.x,nabc.y,nabc.z, 0xFFFFFFFF, 1, 1 );
   fiber.mesh.addVertex( AL );
   fiber.mesh.addVertex( AR );
   fiber.mesh.addVertex( BL );
   fiber.mesh.addVertex( BR );

   for ( size_t i = 2; i < points.size(); ++i )
   {
      glm::vec3 a = points[ i - 2 ];
      glm::vec3 b = points[ i - 1 ];
      glm::vec3 c = points[ i ];

      glm::vec3 nab = Math::getNormal3D( a, b );
      glm::vec3 nbc = Math::getNormal3D( b, c );
      glm::vec3 nabc = (nab + nbc) * 0.5f;
      S3DVertex CL( c.x, y0 + c.y, c.z, -nabc.x,-nabc.y,-nabc.z, 0xFFFFFFFF, 1, 0 );
      S3DVertex CR( c.x, y0 + c.y, c.z, nabc.x,nabc.y,nabc.z, 0xFFFFFFFF, 1, 1 );

      fiber.mesh.addVertex( CL );
      fiber.mesh.addVertex( CR );
   }

   m_Fibers.emplace_back( std::move( fiber ) );
}

void
RainbowRenderer::render( VideoDriver* driver )
{
   if ( !driver ) { DE_ERROR("No driver") return; }

   if ( !m_rainbowTexture.m_tex )
   {
      m_rainbowTexture = driver->createTexture( "rainbow", createRainbowImage() );
   }

   for ( size_t i = 0; i < m_Fibers.size(); ++i )
   {
      m_Fibers[ i ].mesh.setTexture( 0, m_rainbowTexture );
      draw3D( m_Fibers[ i ].mesh, driver );
   }
}

void
RainbowRenderer::draw3D( IMeshBuffer & vao, VideoDriver* driver )
{
   if ( vao.getVertexCount() < 1 ) { return; } // Nothing to draw.

   // [UseProgram]
   std::string const name = "lfiber";
   Shader* shader = driver->getShader( name );
   if ( !shader )
   {
      std::ostringstream vs;
      vs << R"(
         precision highp float;

         in vec3 a_pos;
         in vec3 a_normal;
         in lowp vec4 a_color;
         in vec2 a_tex;

         uniform mat4 u_mvp;
         uniform float u_lineWidth;

         out vec3 v_pos;
         out vec3 v_normal;
         out vec4 v_color;
         out vec2 v_tex;

         void main()
         {
            gl_PointSize = 5.0;
            vec3 normal = normalize(a_normal);
            gl_Position = u_mvp * vec4( a_pos + a_normal * u_lineWidth, 1.0 );
            v_pos = a_pos;
            v_normal = a_normal;
            v_color = clamp( vec4( a_color ) * (1.0 / 255.0), vec4( 0,0,0,0 ), vec4( 1,1,1,1 ) );
            v_tex = a_tex;
         }
      )";

      std::ostringstream fs;
      fs << R"(
         precision highp float;

         in vec3 v_pos;
         in vec3 v_normal;
         in vec4 v_color;
         in vec2 v_tex;

         out vec4 color;

         uniform float u_screenGamma;

         uniform sampler2D u_texDiffuse;  // tex atlas page
         uniform vec4 u_texTransform;     // tex atlas sub rectf xywh, all in range [0,1]

         void main()
         {
            vec2 atlas_pos = u_texTransform.xy;
            vec2 atlas_siz = u_texTransform.zw;
            vec4 diffuseColor = v_color * texture( u_texDiffuse, v_tex * atlas_siz + atlas_pos );
            vec3 colorGammaCorrected = pow(vec3(diffuseColor), vec3(1.0 / u_screenGamma));
            color = vec4(colorGammaCorrected, diffuseColor.a );
         }
      )";
      shader = driver->createShader( name, vs.str(), fs.str() );
   }

   if ( !shader )
   {
      //DE_ERROR("No shader with Id ",id,".")
      std::ostringstream s; s << "No shader with name " << name << ".";
      throw std::runtime_error( s.str() );
   }

   // [UseProgram]
   if ( !driver->setShader( shader ) )
   {
      DE_ERROR("No active shader(",shader->getName(),")")
      return;
   }

   // [MVP] Common to all shaders
   auto const & modelMatrix = driver->getModelMatrix();
   auto vp = glm::dmat4( 1.0 );
   auto camera = driver->getCamera();
   if ( camera )
   {
      vp = camera->getViewProjectionMatrix();
      shader->setUniformM4f( "u_mvp", vp * modelMatrix );
   }
   else
   {
      shader->setUniformM4f( "u_mvp", modelMatrix );
   }

   // [Gamma] Common to all shaders
   shader->setUniformf( "u_screenGamma", m_ScreenGamma );

   // [TexDiffuse]
   TexRef const & texDiffuse = vao.getMaterial().getDiffuseMap();
   if ( texDiffuse.m_tex )
   {
      int stage = driver->bindTexture( texDiffuse.m_tex );
      if ( stage < 0 )
      {
         DE_ERROR("texDiffuse not activated")
         return;
      }

      if ( !shader->setUniformi( "u_texDiffuse", stage ) )
      {
         DE_ERROR("u_texDiffuse uniform not set" )
         return;
      }

      if ( !shader->setUniform4f( "u_texTransform", texDiffuse.getTexTransform() ) )
      {
         DE_ERROR("u_texTransform uniform not set" )
         return;
      }
   }

   // [State] Cull, Depth, Stencil, Blend, etc...

   State state = vao.getMaterial().state;
   float lineWidth = state.lineWidth.m_Now;
   state.lineWidth = LineWidth();
   driver->setState( state );

   // [LineWidth]
   shader->setUniformf( "u_lineWidth", lineWidth );

   driver->drawRaw( vao );

//   for ( size_t i = 0; i < material.getTextureCount(); ++i )
//   {
//      ITexture* tex = material.getTexture( i );
//      if ( tex )
//      {
//         driver->deactivateTexture( tex );
//      }
//   }
}



} // end namespace gpu.
} // end namespace de.









