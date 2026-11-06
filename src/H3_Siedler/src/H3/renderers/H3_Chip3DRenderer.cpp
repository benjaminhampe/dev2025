#include "H3_Chip3DRenderer.h"
#include <H3/H3_Game.h>
#include <H3/details/H3_Topology.h>
#include <de/image/Utils.h>
#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>

H3_Chip3DMeshBuffer::H3_Chip3DMeshBuffer()
    : m_smesh{ nullptr }
    , m_vao{ 0 }
    , m_vbo_vertices{ 0 }
    , m_vbo_indices{ 0 }
    , m_vbo_instanceMat{ 0 }
    , m_vbo_instanceLayer{ 0 }
{}

void H3_Chip3DMeshBuffer::render() const
{
    if (!m_smesh) { DE_ERROR("No smesh") return; }

    if (m_instanceMat.empty()) { return; }

    glBindVertexArray(m_vao);

    const GLenum primType = de::gpu::PrimitiveType::toOpenGL( m_smesh->primitiveType );

    if (m_smesh->indices.size() > 0)
    {
        glDrawElementsInstanced(
            primType,
            GLsizei(m_smesh->indices.size()),
            GL_UNSIGNED_INT,
            nullptr,
            m_instanceMat.size());
    }
    else
    {
        glDrawArraysInstanced(
            primType,
            0,
            GLsizei(m_smesh->indices.size()),
            m_instanceMat.size());
    }

    glBindVertexArray(0);
}

void H3_Chip3DMeshBuffer::initVAO()
{
    if (!m_smesh) { DE_ERROR("No smesh") return; }

    if (!m_vbo_vertices) { glGenBuffers(1, &m_vbo_vertices); }
    if (!m_vbo_indices && m_smesh->indices.size()) { glGenBuffers(1, &m_vbo_indices); }
    if (!m_vbo_instanceMat) { glGenBuffers(1, &m_vbo_instanceMat); }
    if (!m_vbo_instanceLayer) { glGenBuffers(1, &m_vbo_instanceLayer); }
    if (!m_vao) { glGenVertexArrays(1, &m_vao); }

    glBindVertexArray(m_vao);

    // vbo_vertices
    const auto & vertices = m_smesh->vertices;
    const size_t vertexCount = vertices.size();
    const size_t vertexSize = sizeof(de::smesh::S3DVertex);
    const size_t vertexBytes = vertexCount * vertexSize;
    const uint8_t* pVertices = reinterpret_cast< const uint8_t* >( vertices.data() );
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertexBytes), pVertices, GL_STATIC_DRAW);
    // a_pos
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(de::smesh::S3DVertex), (void*)(0) );
    glEnableVertexAttribArray( 0 );
    // a_normal
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(de::smesh::S3DVertex), (void*)(12) );
    glEnableVertexAttribArray( 1 );
    // a_color
    glVertexAttribPointer( 2, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(de::smesh::S3DVertex), (void*)(24) );
    glEnableVertexAttribArray( 2 );
    // a_tex
    glVertexAttribPointer( 3, 2, GL_FLOAT, GL_FALSE, sizeof(de::smesh::S3DVertex), (void*)(28) );
    glEnableVertexAttribArray( 3 );

    // vbo_instanceMat:
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceMat);
    //glBufferData(GL_ARRAY_BUFFER, m_instanceMat.size() * sizeof(glm::mat4), m_instanceMat.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(0));
    glVertexAttribDivisor(4, 1);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(16));
    glVertexAttribDivisor(5, 1);
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(32));
    glVertexAttribDivisor(6, 1);
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(48));
    glVertexAttribDivisor(7, 1);
    glEnableVertexAttribArray(7);

    // vbo_instanceLayer:
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceLayer);
    //glBufferData(GL_ARRAY_BUFFER, m_instanceLayer.size() * sizeof(uint8_t), m_instanceLayer.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(8, 1, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(uint8_t), (void*)0);
    glVertexAttribDivisor(8, 1); // Per-instance divisor
    glEnableVertexAttribArray(8);

    if ( m_vbo_indices )
    {
        const auto nI = m_smesh->indices.size() * sizeof(uint32_t);
        const auto pI = reinterpret_cast< const uint8_t* >( m_smesh->indices.data() );
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo_indices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(nI), pI, GL_STATIC_DRAW);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (m_vbo_indices)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    GL_VALIDATE;
}

void H3_Chip3DMeshBuffer::uploadInstances()
{
    if (m_instanceMat.empty())
    {
        return; // Nothing to upload.
    }

    // DE_DEBUG("Upload ",m_instanceMat.size())

    // vbo_instanceMat:
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceMat);
    glBufferData(GL_ARRAY_BUFFER, m_instanceMat.size() * sizeof(glm::mat4),
                                  m_instanceMat.data(), GL_STATIC_DRAW);

    // vbo_instanceLayer:
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceLayer);
    glBufferData(GL_ARRAY_BUFFER, m_instanceLayer.size() * sizeof(uint8_t),
                                  m_instanceLayer.data(), GL_STATIC_DRAW);

    GL_VALIDATE;
}

void H3_Chip3DMeshBuffer::collectEdges(H3_Game& game, float phi)
{
    if (!game.m_bUpdateDrawCall_Edges)
    {
        return;
    }

    // Reset
    m_instanceMat.clear();
    m_instanceLayer.clear();
    m_instanceAngle60.clear();

    // Reserve
    const auto n = game.m_visibleEdges.size();
    m_instanceMat.reserve(n);
    m_instanceLayer.reserve(n);
    m_instanceAngle60.reserve(n);

    // Collect
    for (u32 edgeId : game.m_visibleEdges)
    {
        const H3_Edge& edge = H3_getEdge(game, __func__, edgeId);

        de::TRSf trs;
        trs.pos = edge.trs.pos;
        trs.rotate.y = phi;
        trs.update();

        m_instanceMat.push_back( trs.modelMat );
        m_instanceLayer.push_back( H3_ChipTexture::ChipW );
        m_instanceAngle60.push_back( 0 );
    }

    game.m_bUpdateDrawCall_Edges = false;
}

void H3_Chip3DMeshBuffer::collectCorners(H3_Game& game, float phi)
{
    if (!game.m_bUpdateDrawCall_Corners)
    {
        return;
    }

    // Reset
    m_instanceMat.clear();
    m_instanceLayer.clear();
    m_instanceAngle60.clear();

    // Reserve
    const auto n = game.m_visibleCorners.size();
    m_instanceMat.reserve(n);
    m_instanceLayer.reserve(n);
    m_instanceAngle60.reserve(n);

    // Collect
    for (u32 cornerId : game.m_visibleCorners)
    {
        const H3_Corner& corner = H3_getCorner(game, __func__, cornerId);

        de::TRSf trs;
        trs.pos = corner.trs.pos;
        trs.rotate.y = phi;
        trs.update();

        m_instanceMat.push_back( trs.modelMat );
        m_instanceLayer.push_back( H3_ChipTexture::ChipS );
        m_instanceAngle60.push_back( 0 );
    }

    game.m_bUpdateDrawCall_Corners = false;
}

void H3_Chip3DMeshBuffer::collectTiles(H3_Game& game, float phi)
{
    if (!game.m_bUpdateDrawCall_Chips)
    {
        return;
    }

    // Reset
    m_instanceMat.clear();
    m_instanceLayer.clear();
    m_instanceAngle60.clear();

    // Reserve
    const auto n = game.m_tiles.size();
    m_instanceMat.reserve(n);
    m_instanceLayer.reserve(n);
    m_instanceAngle60.reserve(n);

    // Collect
    for (const H3_Tile& tile : game.m_tiles)
    {
        uint8_t layer = H3_ChipTexture::eTypeCount;

        if (tile.tt.isResource())
        {
            switch( tile.chipValue )
            {
            case 2: layer = H3_ChipTexture::Chip2; break;
            case 3: layer = H3_ChipTexture::Chip3; break;
            case 4: layer = H3_ChipTexture::Chip4; break;
            case 5: layer = H3_ChipTexture::Chip5; break;
            case 6: layer = H3_ChipTexture::Chip6; break;
            case 8: layer = H3_ChipTexture::Chip8; break;
            case 9: layer = H3_ChipTexture::Chip9; break;
            case 10: layer = H3_ChipTexture::Chip10; break;
            case 11: layer = H3_ChipTexture::Chip11; break;
            case 12: layer = H3_ChipTexture::Chip12; break;
            default: break;
            }
        }
        else if (tile.tt.isPort())
        {
            switch( tile.tt.value )
            {
            case H3_TileType::Port3v1: layer = H3_ChipTexture::Chip3v1; break;
            case H3_TileType::PortA: layer = H3_ChipTexture::ChipA; break;
            case H3_TileType::PortB: layer = H3_ChipTexture::ChipB; break;
            case H3_TileType::PortC: layer = H3_ChipTexture::ChipC; break;
            case H3_TileType::PortD: layer = H3_ChipTexture::ChipD; break;
            case H3_TileType::PortE: layer = H3_ChipTexture::ChipE; break;
            default: break;
            }
        }

        if ( layer == H3_ChipTexture::eTypeCount ) continue;

        de::TRSf trs;
        trs.pos = tile.trsChip.pos;
        trs.rotate.y = phi;
        trs.update();

        m_instanceMat.push_back( trs.modelMat );
        m_instanceLayer.push_back( layer );
        m_instanceAngle60.push_back( 0 );
    }

    game.m_bUpdateDrawCall_Chips = false;
}

// =================================================================
H3_Chip3DRenderer::H3_Chip3DRenderer()
// =================================================================
    : m_driver{ nullptr }
    , m_shader{ nullptr }
    , m_chipTexture{ nullptr }
{}

H3_Chip3DRenderer::~H3_Chip3DRenderer()
{}

void H3_Chip3DRenderer::destroy()
{

}

void H3_Chip3DRenderer::render( const H3_Chip3DMeshBuffer & drawcall )
{
    if (drawcall.m_instanceMat.empty())
    {
        return; // Nothing to draw.
    }

    if (!m_driver) { DE_ERROR("No driver") return; }

    // ==== Shader: u_projViewMat  ====
    m_driver->useShader(m_shader);

    // ==== Shader: u_projViewMat  ====
    glm::mat4 projViewMat(1.0f);
    auto camera = m_driver->getCamera();
    if (camera)
    {
        projViewMat = camera->getViewProjectionMatrix();
    }
    glUniformMatrix4fv(m_locViewProjMat, 1, GL_FALSE, glm::value_ptr( projViewMat ));

    // ==== Shader: u_tex  ====
    glBindTextureUnit(0, m_chipTexture->id);
    glUniform1i(m_locTex, 0);

    // ==== RenderStates  ====
    de::gpu::State state;
    state.culling = de::gpu::Culling::disabled();
    state.blend = de::gpu::Blend::disabled();
    m_driver->setState( state );

    drawcall.render();

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
    GL_VALIDATE;
}

void
H3_Chip3DRenderer::init( H3_Game & game )
{
    m_driver = game.getDriver();
    m_chipTexture = &game.m_tex.m_chipTexture;
    initShader();
}

void H3_Chip3DRenderer::initShader()
{
    if (!m_driver) { DE_ERROR("No driver") return; }

    if (m_shader) { return; } // Nothing todo.

    const std::string shaderName = "Chip3DRend";

    const std::string vs = R"(
        layout(location = 0) in vec3 a_pos;
        layout(location = 1) in vec3 a_normal;
        layout(location = 2) in lowp vec4 a_color;
        layout(location = 3) in vec2 a_texCoord;
        layout(location = 4) in mat4 a_instanceMat; // (mat4 spans 4 attribute locations)
        layout(location = 8) in float a_instanceLayer; // index into texture2DArray

        uniform mat4 u_projViewMat;

        out vec3 v_pos;
        out vec3 v_normal;
        out vec2 v_texCoord;
        out vec4 v_color;
        flat out int v_texLayer;

        void main()
        {
            v_pos = a_pos;
            v_normal = a_normal;
            v_color = clamp( vec4( a_color ) / 255.0, vec4( 0,0,0,0 ), vec4( 1,1,1,1 ) );
            v_texCoord = a_texCoord;
            v_texLayer = int(a_instanceLayer);
            gl_Position = (u_projViewMat * a_instanceMat) * vec4(a_pos, 1.0);
        }
    )";

    const std::string fs = R"(

        out vec4 fragColor;

        in vec3 v_pos;
        in vec3 v_normal;
        in vec4 v_color;
        in vec2 v_texCoord;
        flat in int v_texLayer;

        uniform sampler2DArray u_tex; // Array of 2D diffuseMaps

        void main()
        {
            fragColor = texture(u_tex, vec3(v_texCoord, v_texLayer)) * v_color;
        }
    )";

    m_shader = m_driver->createShader( shaderName, vs, fs );

    m_locViewProjMat = glGetUniformLocation(m_shader->id, "u_projViewMat");
    m_locTex = glGetUniformLocation(m_shader->id, "u_tex");
}
