#include <de/gpu/renderer/BillboardRenderer.h>
#include <de/gpu/VideoDriver.h>
#include <de_opengl.h>

namespace de {
namespace gpu {

BillboardRenderer::BillboardRenderer()
    : m_driver(nullptr)
    , m_shader(nullptr)
{

}

BillboardRenderer::~BillboardRenderer()
{

}

void BillboardRenderer::init( VideoDriver* driver )
{
    m_driver = driver;
}
void BillboardRenderer::clear()
{
    m_billboards.clear();
}
void BillboardRenderer::render()
{
    if ( !m_driver ) { DE_ERROR("No driver") return; }
    //m_driver->resetModelMatrix();

    for ( Billboard & bb : m_billboards )
    {
        draw3DBillboard( bb.mesh, bb.transform );
    }
}
std::string
BillboardRenderer::createShaderName( SMaterial const & mtl ) const
{
    std::string name = "billboard";
    if ( mtl.hasDiffuseMap() ) name += "+tex";
    return name;
}

void BillboardRenderer::draw3DBillboard( SMeshBuffer & meshBuffer, const glm::mat4& modelMat )
{   
    if ( !m_driver ) { DE_ERROR("No driver") return; }
    if ( meshBuffer.getVertexCount() < 1 ) { return; } // Nothing to draw.

    // [UseProgram]
    const SMaterial& mtl = meshBuffer.getMaterial();
    const std::string& name = createShaderName( mtl );

    Shader* shader = m_driver->getShader( name );
    if ( !shader )
    {
        std::ostringstream vs;
        vs << R"(
            precision highp float;

            uniform mat4 u_projViewMat;
            uniform mat4 u_modelMat;
            uniform vec2 u_screenSize;

            in vec3 a_pos;
            in vec3 a_normal;
            in lowp vec4 a_color;
            in vec2 a_tex;

            out vec4 v_color;
            out vec2 v_tex;

            void main()
            {
                vec4 pos = (u_projViewMat * u_modelMat) * vec4( a_pos, 1.0 );
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

            out vec4 fragColor;

            in vec4 v_color;
            in vec2 v_tex;
        )";


        if ( mtl.hasDiffuseMap() )
        {
            fs << R"(
                uniform sampler2D u_tex;  // Diffuse tex atlas page
                uniform vec4 u_texTransform; // Diffuse tex atlas patch rect
            )";
        }

        fs << R"(
            void main()
            {
                fragColor = v_color;
        )";

        if ( mtl.hasDiffuseMap() )
        {
            fs << R"(
                vec2 texCoords = u_texTransform.zw * fract(v_tex) + u_texTransform.xy;
                fragColor *= texture( u_tex, texCoords );
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
    if ( !m_driver->useShader( shader ) )
    {
        DE_ERROR("No active shader(",name,")")
        return;
    }

    // [MVP] Common to all shaders
    auto projViewMat = glm::dmat4( 1.0 );
    auto camera = m_driver->getCamera();
    if ( camera )
    {
        projViewMat = camera->getViewProjectionMatrix();
    }
    shader->setMat4f( "u_projViewMat", projViewMat );
    shader->setMat4f( "u_modelMat", modelMat );

    // [ScreenSize]
    int w = m_driver->getScreenWidth();
    int h = m_driver->getScreenHeight();
    shader->setVec2f( "u_screenSize", glm::vec2(w,h) );

    // [TexDiffuse]
    const TexRef& diffuseMap = mtl.getDiffuseMap();
    if ( diffuseMap.tex )
    {
        m_driver->useTexture(diffuseMap.tex,0);
        shader->setInt("u_tex", 0);
        shader->setVec4f("u_texTransform", diffuseMap.coords);
    }

    // [State]
    //State state = vao.getMaterial().state;
    //state.culling = Culling::disabled();
    //state.depth = Depth::disabled();
    //state.blend = Blend::alphaBlend();
    m_driver->setState( mtl.state );

    meshBuffer.draw();
}

void BillboardRenderer::add( int typ, glm::vec2 size, glm::vec3 pos, glm::vec2 radius, uint32_t color, TexRef ref )
{
    uint32_t tess = 33;

    m_billboards.emplace_back();

    Billboard & bb = m_billboards.back();
    bb.typ = typ;
    bb.size = size;
    bb.transform = glm::translate(glm::mat4(1.0f), pos);
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
        S3DVertex M( 0,0,0, 0,0,0, color, .5f, .5f );
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
            S3DVertex P( 0,0,0, nx,ny,0, color, u, v );
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

} // end namespace gpu.
} // end namespace de.









