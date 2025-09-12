#include <de/gpu/renderer/BillboardRenderer.hpp>
#include <de/gpu/VideoDriver.hpp>

namespace de {
namespace gpu {

BillboardRenderer::BillboardRenderer( VideoDriver* driver )
   : m_driver(driver)
{

}

BillboardRenderer::~BillboardRenderer()
{

}

void
BillboardRenderer::add( int typ, glm::vec2 size, glm::vec3 pos,
                           glm::vec2 radius, uint32_t color, TexRef ref )
{
   uint32_t tess = 33;

   m_billboards.emplace_back();

   Billboard & bb = m_billboards.back();
   bb.typ = typ;
   bb.size = size;
   bb.pos = pos;
   bb.radius = radius;
   bb.tex = ref;
   bb.color = color;

   //bb.mesh.clear( PrimitiveType::Triangles );
   bb.mesh.clear();
   bb.mesh.setPrimitiveType( PrimitiveType::Triangles );
   bb.mesh.setLighting( 0 );
   bb.mesh.setTexture( 0, ref );
   bb.mesh.getMaterial().state.blend = Blend::alphaBlend();

   if ( typ == Billboard::Rect )
   {
      float rx = 0.5f * size.x;
      float ry = 0.5f * size.y;
      S3DVertex A( 0,0,0, -rx, -ry, 0.0f, color, 0, 1 );
      S3DVertex B( 0,0,0, -rx,  ry, 0.0f, color, 0, 0 );
      S3DVertex C( 0,0,0,  rx,  ry, 0.0f, color, 1, 0 );
      S3DVertex D( 0,0,0,  rx, -ry, 0.0f, color, 1, 1 );
      bb.mesh.addQuad( A,B,C,D );
   }

   else
   {
      float rx = 0.5f * size.x;
      float ry = 0.5f * size.y;
      de::gpu::S3DVertex M( 0,0,0, 0,0,0, color, .5f, .5f );
      bb.mesh.addVertex( M );

      // Add circle outline points
      float const angleStep = float( ( 2.0 * M_PI ) / double( tess ) );

      for ( uint32_t i = 0; i <= tess; ++i )
      {
         float const phi = angleStep * float( i );
         float const s = ::sinf( phi );
         float const c = ::cosf( phi );
         float const nx = rx * c;
         float const ny = ry * s;
         float const u = std::clamp( 0.5f + 0.5f * c, 0.0001f, 0.9999f );
         float const v = std::clamp( 0.5f - 0.5f * s, 0.0001f, 0.9999f );
         de::gpu::S3DVertex P( 0,0,0, nx,ny,0, color, u, v );
         bb.mesh.addVertex( P );
      }

      for ( uint32_t i = 0; i < tess; ++i )
      {
         uint32_t m = 0;
         uint32_t a = 1 + i;
         uint32_t b = 1 + i + 1;
         bb.mesh.addIndexedTriangle( m, a, b );
      }
   }

   SMeshBufferTool::translateVertices( bb.mesh, pos );
}

void
BillboardRenderer::render()
{
   if ( !m_driver ) { DE_ERROR("No driver") return; }
   m_driver->resetModelMatrix();

   for ( Billboard & bb : m_billboards )
   {
      draw3DBillboard( bb.mesh );
   }
}

void
BillboardRenderer::draw3DBillboard( IMeshBuffer & vao )
{
   if ( !m_driver ) { DE_ERROR("No driver") return; }
   if ( vao.getVertexCount() < 1 ) { return; } // Nothing to draw.

   //DE_DEBUG("mesh.Vertices = ",mesh.getVertexCount())

   // [UseProgram]
   std::string name = "billboard";
   if ( vao.getMaterial().hasDiffuseMap() ) name += "+tex";

   Shader* shader = m_driver->getShader( name );
   if ( !shader )
   {
      std::ostringstream vs;
      vs << R"(
         precision highp float;

         uniform mat4 u_mvp;
         uniform vec2 u_screenSize;

         in vec3 a_pos;
         in vec3 a_normal;
         in lowp vec4 a_color;
         in vec2 a_tex;

         out vec4 v_color;
         out vec2 v_tex;

         void main()
         {
            vec4 pos = u_mvp * vec4( a_pos, 1.0 );
            // after mvp ( incl. Projection ) coords are in screen, not clipSpace
            pos.x += a_normal.x * 0.5;
            pos.y += a_normal.y * 0.5 * (u_screenSize.x / u_screenSize.y);
            pos.z += a_normal.z;
            gl_Position = pos;
            v_color = clamp( vec4( a_color ) * (1.0 / 255.0), vec4( 0,0,0,0 ), vec4( 1,1,1,1 ) );
            v_tex = a_tex;
         }
      )";

      std::ostringstream fs;
      fs << R"(
         precision highp float;

         in vec4 v_color;
         in vec2 v_tex;

         out vec4 color;
      )";


      if ( vao.getMaterial().hasDiffuseMap() )
      {
      fs << R"(
         uniform sampler2D u_texDiffuse;  // tex atlas page
         uniform vec4 u_texTransform;     // tex atlas sub rectf xywh, all in range [0,1]
         )";
      }

      fs << R"(
         void main()
         {
      )";

      if ( vao.getMaterial().hasDiffuseMap() )
      {
      fs << R"(
            vec2 atlas_pos = u_texTransform.xy;
            vec2 atlas_siz = u_texTransform.zw;
            color = v_color * texture( u_texDiffuse, v_tex * atlas_siz + atlas_pos );
         )";
      }
      else
      {
      fs << R"(
            color = v_color;
         )";
      }

      fs << R"(
         }
         )";

      shader = m_driver->createShader( name, vs.str(), fs.str() );
   }

   if ( !shader )
   {
      //DE_ERROR("No shader with Id ",id,".")
      std::ostringstream s; s << "No shader with name " << name << ".";
      throw std::runtime_error( s.str() );
   }

   // [UseProgram]
   if ( !m_driver->setShader( shader ) )
   {
      DE_ERROR("No active shader(",shader->getName(),")")
      return;
   }

   // [MVP] Common to all shaders
   auto const & modelMatrix = m_driver->getModelMatrix();
   auto vp = glm::dmat4( 1.0 );
   auto camera = m_driver->getCamera();
   if ( camera )
   {
      vp = camera->getViewProjectionMatrix();
      shader->setUniformM4f( "u_mvp", vp * modelMatrix );
   }
   else
   {
      shader->setUniformM4f( "u_mvp", modelMatrix );
   }

   // [ScreenSize]
   int w = m_driver->getScreenWidth();
   int h = m_driver->getScreenHeight();
   shader->setUniform2f( "u_screenSize", glm::vec2(w,h) );
   //shader->setUniform2f( "u_bilborSize", billboardSize );

   // [TexDiffuse]
   TexRef const & texDiffuse = vao.getMaterial().getDiffuseMap();
   if ( texDiffuse.m_tex )
   {
      int stage = m_driver->bindTexture( texDiffuse.m_tex );
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

   // [State]
   //State state = vao.getMaterial().state;
   //state.culling = Culling::disabled();
   //state.depth = Depth::disabled();
   //state.blend = Blend::alphaBlend();
   m_driver->setState( vao.getMaterial().state );

   m_driver->drawRaw( vao );

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









