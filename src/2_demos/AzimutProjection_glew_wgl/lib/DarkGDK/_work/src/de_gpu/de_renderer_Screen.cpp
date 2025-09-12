#include <de_gpu/de_renderer_Screen.h>
#include <de_gpu/de_GL_debug_layer.h>
#include <de_gpu/de_FVF_GL.h>

namespace de {

ScreenRendererMeshBuffer::ScreenRendererMeshBuffer()
   // Geometry
   : m_primType(PrimitiveType::Triangles)
   , m_shouldUpload(false)
   , m_shouldBlend(false)
   , m_vao(0)
   , m_vbo(0)
   , m_ibo(0)
   // Material
   , m_texDiffuse(0)
   , m_texTransform(0,0,1,1)
   //, m_color(0xFFFFFFFF)
   //, m_zIndex(0.0f)
{}

ScreenRendererMeshBuffer::~ScreenRendererMeshBuffer()
{
   if ( m_vao )
   {
      std::cout << "Forgot to call ScreenRendererMeshBuffer.destroy() :: "
                "vao(" << m_vao << "), "
                "vbo(" << m_vbo << "), "
                "ibo(" << m_ibo << "), "
                "v(" << m_vertices.size() << "), "
                "i(" << m_indices.size() << ")\n";

      for ( size_t i = 0; i < m_vertices.size(); ++i )
      {
         std::cout << "Vertex["<<i<<"] " << m_vertices[i].toString() << "\n";
      }

      std::cout << std::endl;
   }
}

void
ScreenRendererMeshBuffer::destroy()
{
   if ( m_vao )
   {
      //DE_TRACE("Delete vao = ",m_vao)
      de_glDeleteVertexArrays(1, &m_vao);
      m_vao = 0;
   }
   if ( m_vbo )
   {
      //DE_TRACE("Delete vbo = ",m_vbo)
      de_glDeleteBuffers(1, &m_vbo);
      m_vbo = 0;
   }
   if ( m_ibo )
   {
      //DE_TRACE("Delete ibo = ",m_ibo)
      de_glDeleteBuffers(1, &m_ibo);
      m_ibo = 0;
   }
}

void
ScreenRendererMeshBuffer::render()
{
   if ( m_vertices.empty() )
   {
      std::cout << "ScreenRendererMeshBuffer.render() :: Nothing to draw" << std::endl;
      return; // Nothing to draw
   }

   if ( !m_vao )
   {
      de_glGenVertexArrays(1, &m_vao);
      m_shouldUpload = true;
      //DE_TRACE("Create vao = ",m_vao)
   }
   if ( !m_vbo )
   {
      de_glGenBuffers(1, &m_vbo);
      m_shouldUpload = true;
      //DE_TRACE("Create vbo = ",m_vbo)
   }
   if ( m_indices.size() && !m_ibo )
   {
      de_glGenBuffers(1, &m_ibo);
      //DE_TRACE("Create ibo = ",m_ibo)
   }

   if ( m_shouldUpload )
   {
      de_glBindVertexArray(m_vao);
      de_glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
      de_glBufferData(GL_ARRAY_BUFFER,
                      GLint(m_vertices.size() * sizeof(ScreenRendererVertex)),
                      m_vertices.data(), GL_STATIC_DRAW);

      GLint const stride = GLint(sizeof(ScreenRendererVertex));
      de_glEnableVertexAttribArray(0);
      de_glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride,
                               reinterpret_cast<void*>(0));  // 8b pos
      de_glEnableVertexAttribArray(1);
      de_glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
                               reinterpret_cast<void*>(8)); // 8b texcoords
      de_glEnableVertexAttribArray(2);
      de_glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, stride,
                               reinterpret_cast<void*>(16)); // 16b color
      //         glEnableVertexAttribArray(2);
      //         glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(ScreenRendererVertex), (void*)20); // color
      if ( m_indices.size() )
      {
         de_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
         de_glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         GLint(m_indices.size() * sizeof(uint32_t)),
                         m_indices.data(), GL_STATIC_DRAW);
      }
      de_glBindVertexArray(0);
//      de_glBindBuffer(GL_ARRAY_BUFFER, 0);
//      if ( m_indices.size() )
//      {
//      de_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//      }
      m_shouldUpload = false;

      //DE_TRACE("Uploaded v(",m_vertices.size(),"), i(",m_indices.size(),").")
   }

//   if ( m_texDiffuse )
//   {
//      //de_glActiveTexture(GL_TEXTURE0 + 2);
//      de_glBindTexture(GL_TEXTURE_2D, m_texDiffuse);
////      TexOptions so( 0,
////             TexOptions::Minify::Linear,
////             TexOptions::Magnify::Linear,
////             TexOptions::Wrap::Repeat,
////             TexOptions::Wrap::Repeat );
////      so.apply();
//   }

   de_glBindVertexArray(m_vao);

   GLenum const primType = GL_PrimitiveType::toOpenGL( m_primType );

   if ( m_indices.empty() )
   {
//      std::cout << "Draw vertex only "
//                   "v(" << m_vertices.size() << ")\n";
      de_glDrawArrays(primType, 0, GLsizei(m_vertices.size()));
   }
   else
   {
      //std::cout << "Draw indexed "
      //             "v(" << m_vertices.size() << "), "
      //             "i(" << m_indices.size() << ")\n";
      de_glDrawElements(primType, GLsizei(m_indices.size()),
                        GL_UNSIGNED_INT, nullptr );
   }

   de_glBindVertexArray(0);
//   if ( m_texDiffuse )
//   {
//      de_glBindTexture(GL_TEXTURE_2D, 0);
//   }

//   GL_VALIDATE("...")
}

// ===========================================================================
ScreenRenderer::ScreenRenderer()
// ===========================================================================
   : m_driver( nullptr )
   , m_screenW(1024)
   , m_screenH(768)
{
   std::cout << "ScreenRenderer() :: sizeof(ScreenRendererVertex) = "
             << sizeof(ScreenRendererVertex) << std::endl;
   m_shader[0] = nullptr;
   m_shader[1] = nullptr;
}

ScreenRenderer::~ScreenRenderer()
{
//   if ( m_unitLineRect.m_vao )
//   {
//      std::cout << "Forgot to call ScreenRenderer.destroy()" << std::endl;
//   }
}

void
ScreenRenderer::init( int w, int h, VideoDriver* driver )
{
   m_screenW = w;
   m_screenH = h;
   m_driver = driver;

   std::string const vs = R"(
      // ======== screen_compose_with_zindex.vs ========

      //precision mediump float;

      layout (location = 0) in vec2 a_pos;
      layout (location = 1) in vec2 a_tex;
      layout (location = 2) in vec4 a_color;

      uniform float u_screenW;
      uniform float u_screenH;
      uniform float u_zIndex;
      uniform vec4 u_posTransform;
      out vec2 v_tex;
      out vec4 v_color;

      void main()
      {
         vec2 pos1 = (u_posTransform.zw * a_pos) + u_posTransform.xy;
         vec2 pos2 = vec2( pos1.x + 0.5, u_screenH - 0.5 - pos1.y );
         vec2 ndc = (pos2 * 2.0 / vec2(u_screenW, u_screenH )) - 1.0;
         v_tex = a_tex;
         v_color = a_color;
         gl_Position = vec4( ndc.x, ndc.y, -1.0 + u_zIndex, 1.0 );
      }
   )";

   if ( !m_shader[ 0 ] )
   {
      std::string const fs = R"(
         // ======== colored.fs ========

         //precision mediump float;

         in vec2 v_tex;
         in vec4 v_color;

         uniform vec4 u_color;

         layout (location = 0) out vec4 color;

         void main()
         {
            color = u_color * v_color;
            color = clamp( color, vec4( 0,0,0,0 ), vec4( 1,1,1,1 ) ); // Limiter
         }
      )";
      m_shader[ 0 ] = driver->createShader( "scr2d", vs, fs );
      //   auto loc_screenWidth = glGetUniformLocation( m_colorShader.ID, "u_screenWidth");
      //   auto loc_screenHeight = glGetUniformLocation( m_colorShader.ID, "u_screenHeight");
   }

   if ( !m_shader[ 1 ] )
   {
      std::string const fs =
      R"(
         // ======== textured.fs ========

         //precision mediump float;

         in vec2 v_tex;
         in vec4 v_color;

         uniform vec4 u_color;
         uniform sampler2D u_texDiffuse;
         //uniform vec4 u_texTransform;

         layout (location = 0) out vec4 color;

         void main()
         {
            color = u_color * v_color * texture(u_texDiffuse, v_tex);
            color = clamp( color, vec4( 0,0,0,0 ), vec4( 1,1,1,1 ) ); // Limiter
         }
      )";
      m_shader[ 1 ] = driver->createShader( "scr2d+tex", vs, fs );
   }
}

void
ScreenRenderer::destroy()
{
//   m_unitLineRect.destroy();
//   m_unitSolidRect.destroy();
//   m_unitLineRoundRect2.destroy();
//   m_unitSolidRoundRect2.destroy();
}



void
ScreenRenderer::draw( ScreenRendererMeshBuffer & meshBuffer,
                      glm::vec4 color,
                      float zIndex,
                      glm::vec4 pos_transform )
{
   if ( !m_driver )
   {
      DE_ERROR("No driver")
      return;
   }

   bool const textured = meshBuffer.m_texDiffuse != 0;
   Shader* shader = m_shader[ textured ? 1 : 0 ];
   if ( !shader )
   {
      DE_ERROR("No shader")
      return;
   }

   m_driver->useShader( shader );
   //shader->use();
   shader->setVec1f( "u_screenW", m_screenW );
   shader->setVec1f( "u_screenH", m_screenH );
   shader->setVec4f( "u_color", color ); // RGBAf(meshBuffer.m_color)
   shader->setVec1f( "u_zIndex", zIndex );
   shader->setVec4f( "u_posTransform", pos_transform );

   de_glDisable( GL_CULL_FACE );
   de_glDisable( GL_DEPTH_TEST );

   if ( meshBuffer.m_shouldBlend ||
        color.a < 1.0f - (10.0f * std::numeric_limits< float >::epsilon()) )
   {
      //std::cout << "m_shouldBlend" << std::endl;
      de_glEnable( GL_BLEND );
      //   GL_FUNC_ADD, GL_FUNC_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT, GL_MIN, GL_MAX
      de_glBlendEquation( GL_FUNC_ADD );
      //   GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ZERO, GL_ONE, GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA,
      //   GL_SRC_ALPHA_SATURATE, GL_CONSTANT_ALPHA, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_DST_COLOR,
      //   GL_ONE_MINUS_DST_COLOR, GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR;
      //   GL_BLEND_COLOR, GL_SRC1_ALPHA_EXT, GL_ONE_MINUS_SRC1_ALPHA_EXT, GL_SRC1_COLOR_EXT;
      //   GL_ONE_MINUS_SRC1_COLOR_EXT, GL_ZERO
      de_glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
      //   glBlendEquationSeparate( equation, equation ) ); GL_VALIDATE
      //   glBlendFuncSeparate( src_rgb, dst_rgb, src_a, dst_a );
   }
   else
   {
      de_glDisable( GL_BLEND );
   }

   if ( textured )
   {
      de_glActiveTexture(GL_TEXTURE0);
      de_glBindTexture(GL_TEXTURE_2D, meshBuffer.m_texDiffuse);
      //      TexOptions so( 0,
      //             TexOptions::Minify::Linear,
      //             TexOptions::Magnify::Linear,
      //             TexOptions::Wrap::Repeat,
      //             TexOptions::Wrap::Repeat );
      //      so.apply();
      shader->setVec1i( "u_texDiffuse",0 );
   }

   meshBuffer.render();

   if ( textured )
   {
      de_glBindTexture(GL_TEXTURE_2D, 0);
   }

   //GL_VALIDATE("...")
}

} // end namespace GL.


