#include <de/gpu/renderer/PostFxRenderer.h>
#include <de/gpu/VideoDriver.h>
#include <de_opengl.h>

namespace de {
namespace gpu {
namespace
{
   constexpr float const CONST_Z_INIT = 0.90f; // At 1.0 it disappears, not inside frustum.
   constexpr float const CONST_Z_STEP = -0.00001f; // enough for 200.000 elements, TODO: test more.
}

void
PostFx_Mesh::destroy()
{
    if ( m_vao )
    {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }

    if ( m_vbo_vertices )
    {
        glDeleteBuffers(1, &m_vbo_vertices);
        m_vbo_vertices = 0;
    }

    // if ( m_vbo_indices )
    // {
    //     glDeleteBuffers(1, &m_vbo_indices);
    //     m_vbo_indices = 0;
    // }
}

void
PostFx_Mesh::upload( bool bForceUpload )
{
    bool bNeedUpload = bForceUpload | m_needUpload;
    if ( !m_vao ) { bNeedUpload = true; }
    if ( !m_vbo_vertices ) { bNeedUpload = true; }

    m_needUpload = false;

    if ( !bNeedUpload ) { return; }

    if ( !m_vao ) { glGenVertexArrays(1, &m_vao); }

    if ( !m_vbo_vertices ) { glGenBuffers(1, &m_vbo_vertices); }

    const auto nV = m_vertices.size();
    const auto pV = reinterpret_cast< const uint8_t* >( m_vertices.data() );

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(nV * sizeof(PostFx_Vertex)), pV, GL_STATIC_DRAW);
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(PostFx_Vertex), reinterpret_cast<void*>(0) );
    glEnableVertexAttribArray( 0 );
    glVertexAttribIPointer( 1, 1, GL_UNSIGNED_INT, sizeof(PostFx_Vertex), reinterpret_cast<void*>(12) );
    glEnableVertexAttribArray( 1 );
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof(PostFx_Vertex), reinterpret_cast<void*>(16) );
    glEnableVertexAttribArray( 2 );
    glBindVertexArray(0);
}

void
PostFx_Mesh::draw()
{
    //upload( meshBuffer );

    if (m_vao)
    {
        glBindVertexArray( m_vao );
    }

    if (m_vbo_vertices)
    {
        const GLenum primType = PrimitiveType::toOpenGL( m_primType );
        // if ( m_vbo_indices )
        // {
        //     //glDrawElements( primType, GLint(m_indices.size()), GL_UNSIGNED_INT, nullptr );
        // }
        // else
        // {
        glDrawArrays(primType, 0, GLint(m_vertices.size()) );
        // }
    }

    if (m_vao)
    {
        glBindVertexArray(0);
    }
}

/*
// ===========================================================================
void Line3D_Renderer::initShader()
// ===========================================================================
{
    if (!m_driver) { DE_ERROR("No driver!") return; }

    if (m_shader)
    {
        return;
    }

    std::string vs = R"(
        //precision highp float;

        layout(location = 0) in vec3 a_pos; // XYZ 3D position attribute
        layout(location = 1) in uint a_color; // RGBA 32-bit | in lowp vec4 a_color;

        uniform mat4 u_modelMat;
        uniform mat4 u_viewMat;
        uniform mat4 u_projMat;

        out vec4 v_color;

        void main()
        {
            v_color = unpackUnorm4x8(a_color); // = clamp( vec4( a_color ) * (1.0 / 255.0), vec4(0), vec4(1) );
            gl_PointSize = 5.0;
            gl_Position = u_projMat * (u_viewMat * (u_modelMat * vec4(a_pos, 1.0)));
        }
    )";

    std::string fs = R"(
        //precision highp float;

        out vec4 fragColor;

        in vec4 v_color;

        void main()
        {
            fragColor = clamp( v_color, vec4( 0,0,0,0 ), vec4( 1,1,1,1 ) );
        }
    )";

    m_shader = m_driver->createShader( "Line3D_Shd", vs, fs );

    // int modelLoc = glGetUniformLocation(m_shaderId, "u_modelMat");
    // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m_modelMat));

    // glm::mat4 m_viewMat = pCamera->getViewMatrix();
    // int viewLoc = glGetUniformLocation(m_shaderId, "u_viewMat");
    // glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(m_viewMat));

    // glm::mat4 m_projMat = pCamera->getProjectionMatrix();
    // int projLoc = glGetUniformLocation(m_shaderId, "u_projMat");

    // m_u_modelMat =
    // m_modelMat = glm::mat4(1.0f);

    // m_viewMat = glm::translate(glm::mat4(1.0f), glm::vec3(500.0f, 500.0f, 1000.0f));

    // m_projMat = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 38000.0f);
}

// ===========================================================================
void Line3D_Renderer::init( VideoDriver* driver )
// ===========================================================================
{
    m_driver = driver;

    if (!m_driver) { DE_ERROR("No driver!") return; }

    initShader();
}

void Line3D_Renderer::setMaterial( bool bBlend, const glm::mat4 & modelMat)
{
    // =========================== bind ===========================
    m_driver->useShader(m_shader);

    // =========================== uniforms ===========================
    m_shader->setMat4f("u_modelMat",modelMat);
    glm::mat4 viewMat(1.0f);
    glm::mat4 projMat(1.0f);
    auto camera = m_driver->getCamera();
    if (camera)
    {
        viewMat = camera->getViewMatrix();
        projMat = camera->getProjectionMatrix();
    }
    m_shader->setMat4f("u_viewMat",viewMat);
    m_shader->setMat4f("u_modelMat",projMat);

    // =========================== state ===========================
    auto state = m_driver->getState();
    if (bBlend)
    {
        state.blend = Blend::alphaBlend();
    }
    else
    {
        state.blend = Blend::disabled();
    }

    m_driver->setState(state);
}

void Line3D_Renderer::draw3DLine( const glm::vec3& A, const glm::vec3& B, const uint32_t cA, const uint32_t cB )
{
    Line3D_MeshBuffer o;
    o.m_vertices.emplace_back( A.x, A.y, A.z, cA );
    o.m_vertices.emplace_back( B.x, B.y, B.z, cB );
    o.upload();

    setMaterial(true);
    o.draw();
    o.destroy();
}

void Line3D_Renderer::draw3DLineBox( const glm::vec3& minPos, const glm::vec3& maxPos, const uint32_t color )
{
    Line3D_MeshBuffer o;
    o.m_primType = PrimitiveType::Lines;
    o.addLineBox(minPos,maxPos, color, false);
    o.upload();

    setMaterial(true);
    o.draw();
    o.destroy();
}
*/

PostFxRenderer::PostFxRenderer()
   : m_driver( nullptr )
   , m_zIndex( CONST_Z_INIT )
   //, m_isDirty( true )
{
}

//int PostFxRenderer::getScreenWidth() const { return m_driver->getScreenWidth(); }
//int PostFxRenderer::getScreenHeight() const { return m_driver->getScreenHeight(); }

void
PostFxRenderer::draw2DRect( const Recti& pos, const uint32_t color, const TexRef& tex )
{
    PostFx_Mesh mesh;
    mesh.m_primType = PrimitiveType::Triangles;

    const int x1 = pos.x1();
    const int x2 = pos.x2();
    const int y1 = pos.y1();
    const int y2 = pos.y2();

    const PostFx_Vertex A( x1, y1, 0, color, 0, 0 );
    const PostFx_Vertex B( x2, y1, 0, color, 1, 0 );
    const PostFx_Vertex C( x2, y2, 0, color, 1, 1 );
    const PostFx_Vertex D( x1, y2, 0, color, 0, 1 );

    mesh.addVertex( A );
    mesh.addVertex( B );
    mesh.addVertex( C );
    mesh.addVertex( A );
    mesh.addVertex( C );
    mesh.addVertex( D );
    mesh.upload();

    PostFx_Material mtl;
    mtl.state.culling = Culling::disabled();
    mtl.state.blend = Blend::alphaBlend();
    mtl.diffuseMap = TexRef( tex );
    setMaterial( mtl );
    mesh.draw();
    mesh.destroy();
}

/*
void
PostFxRenderer::draw2DRoundRect( const Recti& pos, const glm::ivec2& r, const uint32_t color, const TexRef& tex, int tess )
{
    SMeshBuffer m( PrimitiveType::Triangles );
    SMeshRoundRect::addXY( m,
                           glm::vec3(pos.x + 0.5f*pos.w, pos.y + 0.5f*pos.h, 0),
                           pos.w,
                           pos.h,
                           r.x,
                           r.y,
                           tess,
                           color, color);
    m.upload();

    m.setCulling( Culling::disabled() );
    m.setBlend( Blend::alphaBlend() );
    m.setTexture( 0, tex );
    setMaterial( m.material );

    m.draw();
    m.destroy();
}

void
PostFxRenderer::draw2DHexagon( const Recti& pos, const uint32_t color, const TexRef& tex )
{
    const float x0 = pos.x;
    const float x1 = pos.x + pos.w/2;
    const float x2 = pos.x + pos.w;

    const float y0 = pos.y;
    const float y1 = pos.y + pos.h/4;
    const float y2 = pos.y + 3*pos.h/4;
    const float y3 = pos.y + pos.h;

    const S3DVertex A( x1,y3,-1, 0,0,-1, color, 0.5f,  1.0f );
    const S3DVertex B( x0,y2,-1, 0,0,-1, color, 0.0f, 0.75f );
    const S3DVertex C( x0,y1,-1, 0,0,-1, color, 0.0f, 0.25f );
    const S3DVertex D( x1,y0,-1, 0,0,-1, color, 0.5f,  0.0f );
    const S3DVertex E( x2,y1,-1, 0,0,-1, color, 1.0f, 0.25f );
    const S3DVertex F( x2,y2,-1, 0,0,-1, color, 1.0f, 0.75f );

    SMeshBuffer m( PrimitiveType::Triangles );
    m.addTriangle( C,D,E ); // Top
    m.addQuad( B,C,E,F ); // Body
    m.addTriangle( A,B,F ); // Bottom
    m.upload();

    m.setCulling( Culling::disabled() );
    m.setBlend( Blend::alphaBlend() );
    m.setTexture( 0, tex );
    setMaterial( m.material );

    m.draw();
    m.destroy();
}

void
PostFxRenderer::draw2DCircle( const Recti& pos, const uint32_t color, const TexRef& tex, int tess )
{
    SMeshBuffer m( PrimitiveType::Triangles );

    // Add center point
    const float rx = 0.5f * pos.w;
    const float ry = 0.5f * pos.h;
    const float cx = float( pos.x ) + 0.5f*pos.w;
    const float cy = float( pos.y ) + 0.5f*pos.h;
    const S3DVertex M( cx,cy,0, 0,0,-1, color, .5f, .5f );
    m.addVertex( M );

    const float angleStep = float( ( 2.0 * M_PI ) / double( tess ) );

    for ( uint32_t i = 0; i <= tess; ++i )
    {
        const float phi = angleStep * float( i );
        const float s = ::sinf( phi );
        const float c = ::cosf( phi );
        const float x = cx + (rx * c);
        const float y = cy + (ry * s);
        const float u = std::clamp( 0.5f + 0.5f * c, 0.0001f, 0.9999f );
        const float v = std::clamp( 0.5f + 0.5f * s, 0.0001f, 0.9999f );
        const S3DVertex P( x,y,0, 0,0,-1, color, u, v );
        m.addVertex( P );
    }

    for ( uint32_t i = 0; i < tess; ++i )
    {
        const uint32_t c = 0;
        const uint32_t a = 1 + i;
        const uint32_t b = 1 + i + 1;
        m.addIndexedTriangle( c, a, b );
    }

    m.upload();

    m.setCulling( Culling::disabled() );
    m.setBlend( Blend::alphaBlend() );
    m.setTexture( 0, tex );
    setMaterial( m.material );

    m.draw();
    m.destroy();
}
*/

bool
PostFxRenderer::setMaterial( PostFx_Material const & mtl, const Recti& pos )
{
    if ( !m_driver ) { DE_ERROR("No driver") return false; }

    const std::string name = createShaderName( mtl );

    auto shader = m_driver->getShader( name );
    if (!shader)
    {
        shader = m_driver->createShader( name, createVS( mtl ), createFS( mtl ) );
        if (!shader)
        {
            DE_ERROR("No shader created name(", name, ")")
            return false;
        }
    }

    if ( !m_driver->useShader( shader ) )
    {
        DE_ERROR("No bound shader ",name )
        return false;
    }

    const int w = m_driver->getScreenWidth();
    const int h = m_driver->getScreenHeight();
    shader->setVec2f( "u_screenSize", glm::vec2( w, h ) );

    int stage = 0;
    if (mtl.diffuseMap.tex)
    {
        m_driver->useTexture(mtl.diffuseMap.tex, stage);
        shader->setInt( "u_tex", stage );
        shader->setVec4f( "u_texTransform", mtl.diffuseMap.coords );
    }

    State state = mtl.state;
    state.depth = Depth::disabled();
    state.culling = Culling::disabled();
    m_driver->setState( state );
    return true;
}

void
PostFxRenderer::unsetMaterial( const PostFx_Material& mtl )
{

}

std::string
PostFxRenderer::createShaderName( const PostFx_Material& mtl )
{
   std::stringstream s; s << "postfx";
   if ( mtl.diffuseMap.tex ) s << "+Td";
   return s.str();
}

std::string
PostFxRenderer::createVS( const PostFx_Material& /* mtl */ )
{
    std::stringstream vs; vs <<
    R"(
        precision highp float;
      //precision mediump float;

        layout(location = 0) in vec3 a_pos;
        layout(location = 1) in vec3 a_normal;
        layout(location = 2) in lowp vec4 a_color;
        layout(location = 3) in vec2 a_tex;

        uniform vec2 u_screenSize;

      //uniform vec2 u_pixelOffset;
      //uniform vec2 u_pixelScale;

      //out vec3 v_pos;
      //out vec3 v_normal;
        out vec4 v_color;
        out vec2 v_tex;

        void main()
        {
          //vec2 dim = u_pixelScale * a_pos.xy;
          //vec2 pos = u_pixelOffset + dim;
            vec2 ndc = (2.0 * vec2( a_pos.x, u_screenSize.y - 1.0 - a_pos.y ) / u_screenSize ) - 1.0;
          //v_pos = pos;
          //v_normal = a_normal;
            v_color = clamp( vec4( a_color ) * (1.0 / 255.0), vec4( 0,0,0,0 ), vec4( 1,1,1,1 ) );
            v_tex = a_tex;
          //vec2 ndc = (2.0 * vec2( pos.x, u_screenSize.y - 1.0 - pos.y ) / u_screenSize ) - 1.0;
            gl_Position = vec4( ndc, 0.0, 1.0 );
        }
    )";
    return vs.str();
}

std::string
PostFxRenderer::createFS( const PostFx_Material& mtl )
{
    std::ostringstream fs;
    fs << R"(
        precision highp float;
      //precision mediump float;

        layout(location = 0) out vec4 fragColor;
    )";

    if ( mtl.diffuseMap.tex )
    {
        fs << R"(
        uniform sampler2D u_tex;        // diffuseMap
        uniform vec4 u_texTransform;    // diffuseMapTransform
        )";
    }

    fs << R"(
        in vec4 v_color;
        in vec2 v_tex;

        void main()
        {
            fragColor = v_color;
    )";

    // Multiply with texture color from diffuseMap:
    if ( mtl.diffuseMap.tex )
    {
        fs << R"(
            vec2 texCoords = u_texTransform.zw * fract( v_tex ) + u_texTransform.xy;
            vec4 Td = texture( u_diffuseMap, texCoords );
            fragColor *= Td;
        )";
    }

    // Limiter:
    fs << R"(
            // Limiter: final clamp.
            fragColor = clamp( fragColor, vec4( 0,0,0,0 ), vec4( 1,1,1,1 ) );
        }
    )";

    return fs.str();
}

} // end namespace gpu.
} // end namespace de.

