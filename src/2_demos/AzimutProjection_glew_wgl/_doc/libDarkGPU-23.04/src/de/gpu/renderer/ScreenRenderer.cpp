#include <de/gpu/renderer/ScreenRenderer.hpp>
#include <de/gpu/VideoDriver.hpp>

namespace de {
namespace gpu {

namespace
{
   constexpr float const CONST_Z_INIT = 0.90f; // At 1.0 it disappears, not inside frustum.
   constexpr float const CONST_Z_STEP = -0.00001f; // enough for 200.000 elements, TODO: test more.
}

ScreenRenderer::ScreenRenderer( VideoDriver* driver )
   : m_driver( driver )
   , m_zIndex( CONST_Z_INIT )
   , m_isDirty( true )
{
   m_shader[ 0 ] = nullptr;
   m_shader[ 1 ] = nullptr;
}

bool
ScreenRenderer::setMaterial( Material const & m )
{
   if ( !m_driver ) { DE_ERROR("No driver") return false; }

   std::string name = createShaderName( m );
   Shader* shader = m_driver->getShader( name );
   if ( !shader )
   {
      shader = m_driver->createShader( name, createVS( m ), createFS( m ) );
      if ( !shader )
      {
         DE_ERROR("No shader created name(", name, ")")
      }
   }

   if ( !shader )
   {
      DE_ERROR("No shader name ",name)
      return false;
   }

   if ( !m_driver->setShader( shader ) )
   {
      DE_ERROR("No shader set, name ",name )
      return false;
   }

   TexRef ref = m.getDiffuseMap();
   if ( ref.m_tex )
   {
//      if ( !m_driver->activateTexture( 0, ref.tex ) )
//      {
//         DE_ERROR("No tex unit activated")
//         return false;
//      }

      auto stage = m_driver->bindTexture( ref.m_tex );
      if ( stage < 0 )
      {
         DE_ERROR("No tex unit activated")
         return false;
      }

      if ( !shader->setUniformi( "u_tex", stage ) )
      {
         DE_ERROR("Sampler u_tex not activated" )
         return false;
      }

      if ( !shader->setUniform4f( "u_texTransform", ref.getTexTransform() ) )
      {
         DE_ERROR("u_texTransform not uploaded" )
         return false;
      }
   }

   int w = m_driver->getScreenWidth();
   int h = m_driver->getScreenHeight();
   //DE_DEBUG("w(",w,"), h(",h,")")
   if ( !shader->setUniform2f( "u_screenSize", glm::vec2( w, h ) ) )
   {
      DE_ERROR("u_screenSize not set" )
      return false;
   }

   auto state = m.state;
   state.depth = Depth::disabled();
   state.culling = Culling::disabled();
   m_driver->setState( m.state );

   return true;
}

void
ScreenRenderer::unsetMaterial( Material const & material )
{

}

std::string
ScreenRenderer::createShaderName( Material const & material )
{
   std::stringstream s; s << "s2d";
   if ( material.hasDiffuseMap() ) s << "+Td";
   if ( material.CloudTransparent ) s << "+txt";
   return s.str();
}

std::string
ScreenRenderer::createVS( Material const & material )
{
   std::stringstream vs;
   vs << R"(
      // Add precision
      precision highp float;

      uniform vec2 u_screenSize;

      // Add attributes
      layout(location = 0) in vec3 a_pos;
      //layout(location = 1) in vec3 a_normal;
      layout(location = 2) in lowp vec4 a_color;
      layout(location = 3) in vec2 a_tex;

      // Add varyings
      out vec3 v_pos;
      //out vec3 v_normal;
      out vec4 v_color;
      out vec2 v_tex;

      void main()
      {
         vec2 ndc = (2.0 * vec2( a_pos.x, u_screenSize.y - 1.0 - a_pos.y ) / u_screenSize ) - 1.0;
         gl_Position = vec4( ndc, -1.0, 1.0 );
         //gl_Position = vec4( ndc, -1.0, 1.0 );
         v_pos = a_pos;
         //v_normal = a_normal;
         v_color = clamp( vec4( a_color ) * (1.0 / 255.0), vec4( 0,0,0,0 ), vec4( 1,1,1,1 ) );
         v_tex = a_tex;
      }
   )";
   return vs.str();
}

std::string
ScreenRenderer::createFS( Material const & material )
{
   std::stringstream fs;
   fs << R"(
      precision highp float;
   )";

   if ( material.hasDiffuseMap() )
   {
      fs << R"(
         uniform sampler2D u_tex;
         uniform vec4 u_texTransform;
      )";
   }

   fs << R"(
      in vec3 v_pos;
      //in vec3 v_normal;
      in vec4 v_color;
      in vec2 v_tex;

      layout(location = 0) out vec4 color;

      void main()
      {
   )";

   if ( material.hasDiffuseMap() )
   {
      fs << R"(
         vec2 atlasPos = u_texTransform.xy;
         vec2 atlasDim = u_texTransform.zw;
         vec2 atlasUV = atlasDim * fract( v_tex ) + atlasPos;
         vec4 Td = texture( u_tex, atlasUV );
      )";
      if ( material.CloudTransparent )
      {
      fs << R"(
         color = v_color * vec4(1,1,1,Td.r);
      )";
      }
      else
      {
      fs << R"(
         color = v_color * Td;
         )";
      }
   }
   else
   {
   fs << R"(
         color = v_color;
   )";
   }

   fs << R"(
         // Final clamp
         color = clamp( color, vec4( 0,0,0,0 ), vec4( 1,1,1,1 ) );
      }
   )";

   return fs.str();
}

} // end namespace gpu.
} // end namespace de.

