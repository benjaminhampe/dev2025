#pragma once
#include <de_gpu/de_IVideoDriver.h>
#include <de_gpu/de_GL_debug_layer.h>

namespace de {

struct GL_ScreenQuadRenderer
{
   IVideoDriver* m_driver;
   IShader* m_shader;
   uint32_t m_emptyVAO;

   GL_ScreenQuadRenderer()
      : m_driver(nullptr)
      , m_shader(nullptr)
      , m_emptyVAO( 0 )
   {

   }

   void init( IVideoDriver* driver )
   {
      m_driver = driver;

      if ( !m_driver )
      {
         printf( "No driver\n" );
         return;
      }

      if ( !m_shader )
      {
         m_shader = m_driver->createShader( "GL_ScreenQuadRenderer",

            // ===================================
            // Builtin vertex shader variables:
            // ===================================
            // • vec4  gl_Position;
            // • float gl_PointSize
            // ===================================
            R"(
            out vec2 v_texCoord;

            // Draws a screen covering quad without any VAO, VBO or IBO!
            void main()
            {
               vec4 g_vertices[4] = vec4[4](
                                    vec4(-1.0, -1.0, 0.0, 1.0),
                                    vec4( 1.0, -1.0, 0.0, 1.0),
                                    vec4(-1.0,  1.0, 0.0, 1.0),
                                    vec4( 1.0,  1.0, 0.0, 1.0) );

               vec2 g_texCoord[4] = vec2[4](
                                    vec2(0.0, 1.0),
                                    vec2(1.0, 1.0),
                                    vec2(0.0, 0.0),
                                    vec2(1.0, 0.0) );

               v_texCoord = g_texCoord[ gl_VertexID ];

               gl_Position = g_vertices[ gl_VertexID ];

            }

            )",

            // ===================================

            R"(

            in vec2 v_texCoord;

            uniform sampler2D u_tex;

            //uniform vec2 u_screenResolution;    // used to convert fragCoord

            out vec4 fragColor;

            void main()
            {
               fragColor = texture( u_tex, v_texCoord );
            }

            )",

            true );
      }

      if ( !m_emptyVAO )
      {
         de_glGenVertexArrays( 1, &m_emptyVAO );
      }
   }

   void render( int u_tex )
   {
      if ( !m_driver )
      {
         printf( "No driver\n" );
         return;
      }

      if ( !m_shader )
      {
         printf( "No shader\n" );
         return;
      }

      m_driver->useShader( m_shader );

      m_shader->setInt( "u_tex", u_tex );
      //m_shader->setVec2( "u_screenResolution", glm::vec2(w,h) );

      de_glBindVertexArray( m_emptyVAO );

      de_glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

      de_glBindVertexArray( 0 );
   }

};

} // end namespace de
