#pragma once
#include <de/shader/IShaderManager.hpp>
#include <de/GL_debug_layer.hpp>

namespace de {

struct WallpaperShaderArt_F07
{
   IShaderManager* m_driver;
   IShader* m_shader;

   WallpaperShaderArt_F07()
      : m_driver(nullptr)
      , m_shader(nullptr)
   {

   }

   void init( IShaderManager* shaderManager )
   {
      m_driver = shaderManager;

      if ( !m_driver )
      {
         printf( "No driver\n" );
         return;
      }

      if ( !m_shader )
      {
         m_shader = m_driver->createShader( "shaderArt_F07",

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
                                    vec2(0.0, 0.0),
                                    vec2(1.0, 0.0),
                                    vec2(0.0, 1.0),
                                    vec2(1.0, 1.0) );

               v_texCoord = g_texCoord[ gl_VertexID ];

               gl_Position = g_vertices[ gl_VertexID ];

            }

            )",

            // ===================================
            // Builtin fragment shader variables:
            // ===================================
            // • vec4  gl_FragCoord;
            // • bool  gl_FrontFacing;
            // • vec2  gl_PointCoord;
            // • float gl_FragColor;
            /*
             *             vec3 paletteA( float t )
            {
               vec3 a = vec3( 0.5, 0.5, 0.5 );
               vec3 b = vec3( 0.5, 0.5, 0.5 );
               vec3 c = vec3( 1.0, 1.0, 1.0 );
               vec3 d = vec3( 0.00, 0.33, 0.67 );
               return generic_palette( t, a, b, c, d);
            }

            vec3 paletteB( float t )
            {
               vec3 a = vec3( 0.5, 0.5, 0.5 );
               vec3 b = vec3( 0.5, 0.5, 0.5 );
               vec3 c = vec3( 1.0, 1.0, 1.0 );
               vec3 d = vec3( 0.00, 0.10, 0.20 );
               return generic_palette( t, a, b, c, d);
            }

            vec3 paletteC( float t )
            {
               vec3 a = vec3( 0.5, 0.5, 0.5 );
               vec3 b = vec3( 0.5, 0.5, 0.5 );
               vec3 c = vec3( 1.0, 1.0, 1.0 );
               vec3 d = vec3( 0.30, 0.20, 0.20 );
               return generic_palette( t, a, b, c, d);
            }

            vec3 paletteD( float t )
            {
               vec3 a = vec3( 0.5, 0.5, 0.5 );
               vec3 b = vec3( 0.5, 0.5, 0.5 );
               vec3 c = vec3( 1.0, 1.0, 0.5 );
               vec3 d = vec3( 0.80, 0.90, 0.30 );
               return generic_palette( t, a, b, c, d);
            }

            vec3 paletteE( float t )
            {
               vec3 a = vec3( 0.5, 0.5, 0.5 );
               vec3 b = vec3( 0.5, 0.5, 0.5 );
               vec3 c = vec3( 1.0, 0.7, 0.4 );
               vec3 d = vec3( 0.00, 0.15, 0.20 );
               return generic_palette( t, a, b, c, d);
            }

            vec3 paletteF( float t )
            {
               vec3 a = vec3( 0.5, 0.5, 0.5 );
               vec3 b = vec3( 0.5, 0.5, 0.5 );
               vec3 c = vec3( 2.0, 1.0, 0.0 );
               vec3 d = vec3( 0.50, 0.20, 0.25 );
               return generic_palette( t, a, b, c, d);
            }

            vec3 paletteG( float t )
            {
               vec3 a = vec3( 0.8, 0.5, 0.4 );
               vec3 b = vec3( 0.2, 0.4, 0.2 );
               vec3 c = vec3( 2.0, 1.0, 1.0 );
               vec3 d = vec3( 0.00, 0.25, 0.25 );
               return generic_palette( t, a, b, c, d);
            }
            */
            // ===================================

            R"(

            uniform vec2 u_screenResolution;    // used to convert fragCoord
            uniform float u_timeInSeconds;      // for animating stuff

            out vec4 fragColor;

            vec3 generic_palette( float t, vec3 a, vec3 b, vec3 c, vec3 d )
            {
               return a + b * cos( 6.28318 * ( c * t + d ) );
            }

            vec3 paletteH( float t )
            {
               vec3 a = vec3( 0.5, 0.5, 0.5 );
               vec3 b = vec3( 0.5, 0.5, 0.5 );
               vec3 c = vec3( 1.0, 1.0, 1.0 );
               vec3 d = vec3( 0.263, 0.416, 0.557 );
               return generic_palette( t, a, b, c, d );
            }

            void main()
            {
               vec2 uv = ( gl_FragCoord.xy * 2.0 - u_screenResolution.xy ) / u_screenResolution.y;
               vec2 uv0 = uv;
               vec3 finalColor = vec3( 0.0, 0.0, 0.0 );

               for ( float i = 0.0; i < 3.0; i++ )
               {
                  uv = fract( uv * 1.5 ) - 0.5;

                  float d = length(uv) * exp(-length(uv0));

                  vec3 color = paletteH( length(uv0) + u_timeInSeconds * 0.4 );

                  d = sin( d * 8.0 + u_timeInSeconds ) / 8.0;
                  d = abs( d );

                  d = 0.02 / d;

                  finalColor += color * d;
               }

               fragColor = vec4( finalColor, 1.0 );
            }

            )",

            false );
      }
   }

   void render( int w, int h, float timeInSeconds )
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

      m_shader->setVec1( "u_timeInSeconds", timeInSeconds );
      m_shader->setVec2( "u_screenResolution", glm::vec2(w,h) );

      de_glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

   }

};

} // end namespace de
