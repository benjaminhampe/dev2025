#include <de/gpu/renderer/FontRenderer5x8.h>
#include <de/gpu/VideoDriver.h>
#include <de_opengl.h>

namespace de {
namespace gpu {
// ===========================================================================
FontRenderer5x8::FontRenderer5x8()
// ===========================================================================
    : m_driver(nullptr)
    , m_shader(nullptr)
    , m_vao(0)
    , m_vbo(0)
{
   m_shouldUpload = true;
   m_screenWidthLoc = -1;
   m_screenHeightLoc = -1;
}

//FontRenderer5x8::~Font5x8Renderer(){}

void
FontRenderer5x8::init( VideoDriver* driver )
{
    m_driver = driver;

    if ( !m_shader )
    {
        std::string vs = R"(

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

        layout(location = 0) out vec4 fragColor;

        in vec4 v_color;

        void main()
        {
            fragColor = v_color;
        }
        )";

        m_shader = m_driver->createShader( "FontRenderer5x8", vs, fs );

        m_screenWidthLoc = glGetUniformLocation( m_shader->id, "u_screenW");
        m_screenHeightLoc = glGetUniformLocation( m_shader->id, "u_screenH");

        // std::cout << "Create: " << std::endl;
        // std::cout << "m_shader.ID = " << id << std::endl;
        // std::cout << "m_screenWidth = " << m_screenWidth << std::endl;
        // std::cout << "m_screenHeight = " << m_screenHeight << std::endl;
        // std::cout << "m_screenWidthLoc = " << m_screenWidthLoc << std::endl;
        // std::cout << "m_screenHeightLoc = " << m_screenHeightLoc << std::endl;
   }
}

void
FontRenderer5x8::destroy()
{
    if ( m_vao )
    {
        glDeleteVertexArrays(1,&m_vao);
        m_vao = 0;
    }

    if ( m_vbo )
    {
        glDeleteBuffers(1,&m_vbo);
        m_vbo = 0;
    }
}

void
FontRenderer5x8::draw2DText( int x, int y, std::string const & msg, uint32_t color, Align align,
                            Font5x8 const & font, uint32_t bgColor, int padding )
{
    if ( !m_driver ) { DE_ERROR("No driver.") return; }
    if ( !m_shader ) { DE_ERROR("No shader.") return; }

    const auto face = getFontFace5x8();
    if (!face) { DE_ERROR("No font5x8 face, ", font.toString()) return; }

    int32_t bbox_x1 = std::numeric_limits<int32_t>::max();      //
    int32_t bbox_y1 = std::numeric_limits<int32_t>::max();
    int32_t bbox_x2 = std::numeric_limits<int32_t>::lowest();
    int32_t bbox_y2 = std::numeric_limits<int32_t>::lowest();

    const int cx = font.quadWidth + font.quadSpacingX;
    const int cy = font.quadHeight + font.quadSpacingY;
    const int gw = 5 * cx + font.glyphSpacingX;
    const int gh = 8 * cy + font.glyphSpacingY;

    // Align: default is TopLeft
    int aligned_x = x;
    int aligned_y = y;

    // Apply Align:
    if ( align != Align::TopLeft )
    {
        // Optimize here for single lines, default: multiline but slower.
        auto ts = font.getTextSize( msg );
        if ( align & Align::Center ){ aligned_x -= ts.width/2; }
        else if ( align & Align::Right ) { aligned_x -= ts.width;   }
        else {}
        if ( align & Align::Middle ){ aligned_y -= ts.height/2; }
        else if ( align & Align::Bottom ){ aligned_y -= ts.height;   }
        else {}
    }

    m_vertices.clear();

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

        const Font5x8Glyph & glyph = face->getGlyph( ch );

        // Add dot matrix 5mal8 as max 49 quads
        int x1 = glyph_x;
        int y1 = glyph_y;
        for ( int j = 0; j < 8; ++j )
        {
            int y2 = y1 + font.quadHeight;
            for ( int i = 0; i < 5; ++i )
            {
                int x2 = x1 + font.quadWidth;

                bbox_x1 = std::min( bbox_x1, x1 );
                bbox_y1 = std::min( bbox_y1, y1 );
                bbox_x2 = std::max( bbox_x2, x2 );
                bbox_y2 = std::max( bbox_y2, y2 );

                if ( glyph.get( i,j ) )
                {
                    FontRenderer5x8_Vertex A( float(x1), float(y1), -1, color );
                    FontRenderer5x8_Vertex B( float(x1), float(y2), -1, color );
                    FontRenderer5x8_Vertex C( float(x2), float(y2), -1, color );
                    FontRenderer5x8_Vertex D( float(x2), float(y1), -1, color );
                    m_vertices.push_back( A );
                    m_vertices.push_back( C );
                    m_vertices.push_back( B );
                    m_vertices.push_back( A );
                    m_vertices.push_back( D );
                    m_vertices.push_back( C );
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

    // Draw background rect
    if (dbRGBA_A(bgColor) > 0)
    {
        bbox_x1 -= padding;
        bbox_y1 -= padding;
        bbox_x2 += padding;
        bbox_y2 += padding;
        auto bbox_r = Recti(bbox_x1, bbox_y1, bbox_x2 - bbox_x1, bbox_y2 - bbox_y1);
        m_driver->getScreenRenderer()->draw2DRect( bbox_r, bgColor );
    }

    m_shouldUpload = true;

    if ( !m_vao )
    {
        glGenVertexArrays(1, &m_vao);
        m_shouldUpload = true;
    }

    if ( !m_vbo )
    {
        glGenBuffers(1, &m_vbo);
        m_shouldUpload = true;
    }

    if ( m_shouldUpload )
    {
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER,
            m_vertices.size() * sizeof(FontRenderer5x8_Vertex),
            m_vertices.data(), GL_STATIC_DRAW);

        // a_pos:
        glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE,
            sizeof(FontRenderer5x8_Vertex), reinterpret_cast<void*>(0) );
        glEnableVertexAttribArray(0);

        // a_color:
        glVertexAttribPointer( 1, 4, GL_UNSIGNED_BYTE, GL_FALSE,
            sizeof(FontRenderer5x8_Vertex), reinterpret_cast<void*>(12) );
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
        // GL_VALIDATE

        //std::cout << "Upload " << vertexCount << " vertices" << std::endl;
        //std::cout << "Upload " << byteCount << " bytes" << std::endl;
        //std::cout << "Upload " << stride << " stride" << std::endl;
        m_shouldUpload = false;
    }

    // quads.setDepth( Depth::disabled() );
    // quads.setBlend( Blend::alphaBlend() );
    // quads.setCulling( Culling::disabled() );
    // quads.setTexture( 0, tex );
    // //quads.getMaterial().CloudTransparent = true;


    State state;
    state.culling = Culling::disabled();
    state.depth = Depth::disabled();
    state.blend = Blend::alphaBlend();
    m_driver->setState( state );

    const int screenWidth = m_driver->getScreenWidth();
    const int screenHeight = m_driver->getScreenHeight();

    m_driver->useShader( m_shader );
    glUniform1f(m_screenWidthLoc, screenWidth);
    glUniform1f(m_screenHeightLoc, screenHeight);
    //glUseProgram(m_shader->id);
    //m_shader->setVec1f( "u_screenW", screenWidth );
    //m_shader->setVec1f( "u_screenH", screenHeight );

    glBindVertexArray(m_vao);
    glDrawArrays( GL_TRIANGLES, 0, GLsizei( m_vertices.size() ) );
    glBindVertexArray(0);
    // GL_VALIDATE
}

} // end namespace gpu.
} // end namespace de.
