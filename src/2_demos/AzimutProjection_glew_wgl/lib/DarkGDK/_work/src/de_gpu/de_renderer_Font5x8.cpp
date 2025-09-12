#include <de_gpu/de_renderer_Font5x8.h>
#include <de_gpu/de_GL_debug_layer.h>

namespace de {

// ===========================================================================
Font5x8Renderer2D::Font5x8Renderer2D()
// ===========================================================================
   : m_driver( nullptr )
   , m_shader( nullptr )
   , m_vao(0)
   , m_vbo(0)
{
   m_shouldUpload = true;
   m_screenWidthLoc = -1;
   m_screenHeightLoc = -1;
   setScreenSize(1024,768);
}

//Font5x8Renderer2D::~Font5x8Renderer(){}

void
Font5x8Renderer2D::init( int w, int h, VideoDriver* driver )
{
   m_driver = driver;
   m_screenWidth = w;
   m_screenHeight = h;

   if ( !m_shader )
   {
      std::string vs = R"(
         //precision highp float;

         layout(location = 0) in vec3 a_pos;
         layout(location = 1) in lowp vec4 a_color;

         uniform float u_screenW;
         uniform float u_screenH;

         out vec4 v_color;

         void main()
         {
            float ndc_x = ((2.0*a_pos.x) / u_screenW) - 1.0;
            float ndc_y = ((2.0*(u_screenH - 1.0 - a_pos.y)) / u_screenH) - 1.0;
            gl_Position = vec4( ndc_x, ndc_y, -1.0, 1.0 );
            v_color = clamp( vec4( a_color ) * (1.0 / 255.0), vec4( 0,0,0,0 ), vec4( 1,1,1,1 ) );
         }
      )";

      std::string fs = R"(
         //precision highp float;

         in vec4 v_color;

         layout(location = 0) out vec4 color;

         void main()
         {
            color = v_color;
         }
      )";

      m_shader = driver->createShader( "Font5x8Renderer2D", vs, fs );

      //driver->useShader( m_shader );
      //m_shader->use();
      //u32 id = m_shader->ID;
//      m_screenWidthLoc = glGetUniformLocation( id, "u_screenW");
//      m_screenHeightLoc = glGetUniformLocation( id, "u_screenH");

      //std::cout << "Create: " << std::endl;
      //std::cout << "m_shader.ID = " << id << std::endl;
//      std::cout << "m_screenWidth = " << m_screenWidth << std::endl;
//      std::cout << "m_screenHeight = " << m_screenHeight << std::endl;
//      std::cout << "m_screenWidthLoc = " << m_screenWidthLoc << std::endl;
//      std::cout << "m_screenHeightLoc = " << m_screenHeightLoc << std::endl;
   }

}

void
Font5x8Renderer2D::destroy()
{
   if ( m_vao )
   {
      de_glDeleteVertexArrays(1,&m_vao);
      m_vao = 0;
   }

   if ( m_vbo )
   {
      de_glDeleteBuffers(1,&m_vbo);
      m_vbo = 0;
   }
}

void
Font5x8Renderer2D::setScreenSize( float w, float h )
{
   m_screenWidth = w;
   m_screenHeight = h;
}

void
Font5x8Renderer2D::draw2DText( int x, int y, std::string const & msg,
            uint32_t color, de::Align align, de::Font5x8 const & font )
{
   if ( !m_driver )
   {
      std::cout << "Font5x8Renderer2D :: No driver." << std::endl;
      return;
   }

   if ( !m_shader )
   {
      std::cout << "Font5x8Renderer2D :: No shader." << std::endl;
      return;
   }

   m_vertices.clear();
   addGeometry2DText( m_vertices, x, y, msg, color, align, font );
   m_shouldUpload = true;

   //glUniform1f( m_screenWidthLoc, m_screenWidth );
   //glUniform1f( m_screenHeightLoc, m_screenHeight );

//   std::cout << "m_colorShader.ID = " << m_colorShader.ID << std::endl;
//   std::cout << "m_screenWidth = " << m_screenWidth << std::endl;
//   std::cout << "m_screenHeight = " << m_screenHeight << std::endl;
//   std::cout << "loc_screenWidth = " << loc_screenWidth << std::endl;
//   std::cout << "loc_screenHeight = " << loc_screenHeight << std::endl;

   //
   if ( !m_vao )
   {
      de_glGenVertexArrays(1, &m_vao);
      m_shouldUpload = true;
   }

   if ( !m_vbo )
   {
      de_glGenBuffers(1, &m_vbo);
      m_shouldUpload = true;
   }

   if ( m_shouldUpload )
   {
      de_glBindVertexArray(m_vao);
      de_glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
      de_glBufferData(GL_ARRAY_BUFFER,
                      m_vertices.size() * sizeof(Font5x8Renderer2D_Vertex),
                      m_vertices.data(), GL_STATIC_DRAW);

      // a_pos:
      de_glEnableVertexAttribArray(0);
      de_glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE,
                                sizeof(Font5x8Renderer2D_Vertex),
                                reinterpret_cast<void*>(0) );
      // a_color:
      de_glEnableVertexAttribArray(1);
      de_glVertexAttribPointer( 1, 4, GL_UNSIGNED_BYTE, GL_FALSE,
                                sizeof(Font5x8Renderer2D_Vertex),
                                reinterpret_cast<void*>(12) );

      de_glBindVertexArray(0); //    GL_VALIDATE

      //glDisableVertexAttribArray( 0 );       GL_VALIDATE
      //glDisable(GL_TEXTURE_2D);      GL_VALIDATE

      m_shouldUpload = false;


      //std::cout << "Upload " << vertexCount << " vertices" << std::endl;
      //std::cout << "Upload " << byteCount << " bytes" << std::endl;
      //std::cout << "Upload " << stride << " stride" << std::endl;
   }

   m_driver->useShader( m_shader );
   //m_shader->use();
//   auto loc_screenWidth = glGetUniformLocation( m_colorShader.ID, "u_screenWidth");
//   auto loc_screenHeight = glGetUniformLocation( m_colorShader.ID, "u_screenHeight");
   m_shader->setVec1f( "u_screenW", m_screenWidth );
   m_shader->setVec1f( "u_screenH", m_screenHeight );

   de_glDisable(GL_CULL_FACE); //GL_VALIDATE
   //glDisable(GL_TEXTURE_2D); GL_VALIDATE
   de_glDisable(GL_DEPTH_TEST); //GL_VALIDATE

   de_glBindVertexArray(m_vao); //GL_VALIDATE
   de_glDrawArrays( GL_TRIANGLES, 0, GLsizei( m_vertices.size() ) ); // GL_VALIDATE
   de_glBindVertexArray(0); //  GL_VALIDATE
}

void
Font5x8Renderer2D::addGeometry2DText(
   std::vector< Font5x8Renderer2D_Vertex > & vertices,
   int x, int y, std::string const & msg,
   uint32_t color, de::Align align, de::Font5x8 const & font )
{
   auto face = de::getFontFace5x8( font );

   //auto ts = face->getTextSize( msg ); // Optimize here for single lines, default: multiline but slower.
   //std::cout << "ts = " << ts.toString() << std::endl;

   int cx = font.quadWidth + font.quadSpacingX;
   int cy = font.quadHeight + font.quadSpacingY;
   int gw = 5 * cx + font.glyphSpacingX;
   int gh = 8 * cy + font.glyphSpacingY;

   // Align: default is TopLeft
   int aligned_x = x;
   int aligned_y = y;

   // Apply Align:
   if ( align != de::Align::TopLeft )
   {
      auto ts = face->getTextSize( msg ); // Optimize here for single lines, default: multiline but slower.
           if ( align & de::Align::Center ){ aligned_x -= ts.width/2; }
      else if ( align & de::Align::Right ) { aligned_x -= ts.width;   }
      else {}
           if ( align & de::Align::Middle ){ aligned_y -= ts.height/2; }
      else if ( align & de::Align::Bottom ){ aligned_y -= ts.height;   }
      else {}
   }

   int glyph_x = aligned_x;
   int glyph_y = aligned_y;


   for ( size_t u = 0; u < msg.size(); ++u )
   {
      uint32_t ch = msg[ u ];

      if ( ch == '\r' )  // Mac or Windows line breaks.
      {
         glyph_x = aligned_x;
         glyph_y += gh + font.glyphSpacingY;
         continue;
      }
      if ( ch == '\n' )	// Mac or Windows line breaks.
      {
         glyph_x = aligned_x;
         glyph_y += gh + font.glyphSpacingY;
         continue;
      }

      //char ch = char( std::clamp( unicode, uint32_t(30), uint32_t(127) ) );
      de::Font5x8Glyph & glyph = face->getGlyph( ch );

      // Add dot matrix 5mal8 as max 49 quads
      int x1 = glyph_x;
      int y1 = glyph_y;
      for ( int j = 0; j < 8; ++j )
      {
         int y2 = y1 + font.quadHeight;
         for ( int i = 0; i < 5; ++i )
         {
            int x2 = x1 + font.quadWidth;

            if ( glyph.get( i,j ) )
            {
               Font5x8Renderer2D_Vertex A( float(x1), float(y1), -1, color, 0,1 );
               Font5x8Renderer2D_Vertex B( float(x1), float(y2), -1, color, 0,0 );
               Font5x8Renderer2D_Vertex C( float(x2), float(y2), -1, color, 1,0 );
               Font5x8Renderer2D_Vertex D( float(x2), float(y1), -1, color, 1,1 );
               vertices.push_back( A );
               vertices.push_back( C );
               vertices.push_back( B );
               vertices.push_back( A );
               vertices.push_back( D );
               vertices.push_back( C );
            }
            x2 += font.quadSpacingX;
            x1 = x2;
         }
         y2 += font.quadSpacingY;
         y1 = y2;
         x1 = glyph_x;
      }
      glyph_x += gw;
   }
}


void
Font5x8Renderer2D::add2DText( int x, int y,
         std::string const & msg,
         uint32_t color,
         de::Align align,
         de::Font5x8 const & font )
{
   addGeometry2DText( m_vertices, x, y, msg, color, align, font );
}

} // end namespace de.

