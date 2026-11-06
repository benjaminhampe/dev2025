#include "H3_Tile3DRenderer.h"
#include <H3/H3_Game.h>
#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>

H3_Tile3DRenderer::H3_Tile3DRenderer()
    : m_driver(nullptr)
    , m_shader(nullptr)
    , m_tileTexture(nullptr)
    , m_u_pvMat(-1)
    , m_u_tex(-1)
{}

H3_Tile3DRenderer::~H3_Tile3DRenderer()
{
    if (m_vao)
    {
        DE_ERROR("Forgot to call destroy")
    }
}

void H3_Tile3DRenderer::destroy()
{
    if (m_vao)
    {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo_instanceMat);
        glDeleteBuffers(1, &m_vbo_instanceAngle60);
        glDeleteBuffers(1, &m_vbo_instanceLayer);

        m_vao = 0;
        m_vbo_instanceMat = 0;
        m_vbo_instanceAngle60 = 0;
        m_vbo_instanceLayer = 0;
    }
}

void H3_Tile3DRenderer::init( H3_Game& game )
{
    m_driver = game.getDriver();

    if (!m_driver) { DE_ERROR("No driver") return; }

    m_tileTexture = &game.m_tex.m_tileTexture;

    initVAO();

    const std::string shaderName = "h3flatTil";
    const std::string vs = R"(

        // ===============================================
        /// @brief The Hexagon
        // ===============================================
        ///
        ///               M| x=0 | y=0 | z=0 | u=.5| v=.5|
        ///       D       -|-----|-----|-----|-----|-----|
        ///      / \      A|   0 |  0  | -.5 | .5  | 0   |
        ///   C /   \ E   B| -.5 |  0  |-.25 |  0  | 0.25|
        ///    |-----|    C| -.5 |  0  | .25 |  0  | 0.75|
        ///    |  M  |    D| 0.0 |  0  |  .5 |  .5 | 1   |
        ///    |-----|    E| 0.5 |  0  | .25 |  1  | 0.75|
        ///   B \   / F   F| 0.5 |  0  |-.25 |  1  | 0.25|
        ///      \ /
        ///       A       triangles: BFA, BEF, BCE, CDE
        ///

        layout(location = 0) in mat4 a_instanceMat; // (mat4 spans 4 attribute locations)
        layout(location = 4) in uint a_instanceAngle60;
        layout(location = 5) in uint a_instanceLayer;

        const vec2 a_position[6] = vec2[](
            vec2( 0.00, -0.50), // A=0
            vec2(-0.50, -0.25), // B=1
            vec2(-0.50,  0.25), // C=2
            vec2( 0.00,  0.50), // D=3
            vec2( 0.50,  0.25), // E=4
            vec2( 0.50, -0.25)  // F=5
        );

        const vec2 a_texcoord[6] = vec2[](
            vec2( 0.50, 1.00),  // A=0
            vec2( 0.00, 0.75),  // B=1
            vec2( 0.00, 0.25),  // C=2
            vec2( 0.50, 0.00),  // D=3
            vec2( 1.00, 0.25),  // E=4
            vec2( 1.00, 0.75)   // F=5
        );

        // GL_TRIANGLES
        const uint a_indices[12] = uint[](
            0, 1, 5, // ABF
            1, 4, 5, // BEF
            1, 2, 4, // BCE
            2, 3, 4  // CDE
        );

        uniform mat4 u_projViewMat;

        out vec2 v_texCoord;
        flat out uint v_texLayer;

        void main()
        {
            const uint i = a_indices[gl_VertexID];
            const uint k = (i + uint(a_instanceAngle60)) % 6;

            const float x = a_position[i].x;
            const float z = a_position[i].y;
            gl_Position = u_projViewMat * a_instanceMat * vec4(x, 0.0, z, 1.0);

            v_texCoord = a_texcoord[k];
            v_texLayer = a_instanceLayer;
        }
        )";

    // Fragment Shader Source
    const std::string fs = R"(

        out vec4 fragColor;

        uniform sampler2DArray u_tex;

        in vec2 v_texCoord;
        flat in uint v_texLayer;

        void main()
        {
            vec3 texCoord = vec3(v_texCoord.x, v_texCoord.y, float(v_texLayer));
            fragColor = texture(u_tex, texCoord);
        }
    )";

    m_shader = m_driver->createShader( shaderName, vs, fs );
    m_u_pvMat = glGetUniformLocation(m_shader->id, "u_projViewMat");
    m_u_tex = glGetUniformLocation(m_shader->id, "u_tex");
}

void H3_Tile3DRenderer::initVAO()
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo_instanceMat);
    glGenBuffers(1, &m_vbo_instanceAngle60);
    glGenBuffers(1, &m_vbo_instanceLayer);
    glBindVertexArray(m_vao);

    // vbo_instanceMat:
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceMat);
    for (int i = 0; i < 4; i++)
    {
        glVertexAttribPointer(i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(float) * i * 4));
        glVertexAttribDivisor(i, 1); // Update per instance
        glEnableVertexAttribArray(i);
    }

    // vbo_instanceAngle60:
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceAngle60);
    glVertexAttribIPointer(4, 1, GL_UNSIGNED_BYTE, sizeof(uint8_t), (void*)0);
    glVertexAttribDivisor(4, 1); // Per-instance divisor
    glEnableVertexAttribArray(4);

    // vbo_instanceLayer:
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceLayer);
    glVertexAttribIPointer(5, 1, GL_UNSIGNED_BYTE, sizeof(uint8_t), (void*)0);
    glVertexAttribDivisor(5, 1); // Per-instance divisor
    glEnableVertexAttribArray(5);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GL_VALIDATE;
}

void H3_Tile3DRenderer::collectInstances(const H3_Game& game)
{
    m_instanceMat.clear();
    m_instanceAngle60.clear();
    m_instanceLayer.clear();

    const size_t n = game.m_tiles.size();
    m_instanceMat.reserve(n);
    m_instanceAngle60.reserve(n);
    m_instanceLayer.reserve(n);

    for (const auto& tile : game.m_tiles)
    {
        // [Instance] a_instanceLayer
        uint8_t layer = H3_TileTexture::eTypeCount;
        switch (tile.tt)
        {
            // case H3_TileType::Unknown: layer = Unknown; break;
            // case H3_TileType::Water: layer = H3_TileTexture::Water; break;
            case H3_TileType::Desert: layer = H3_TileTexture::Desert; break;
            case H3_TileType::TileA: layer = H3_TileTexture::TileA; break;
            case H3_TileType::TileB: layer = H3_TileTexture::TileB; break;
            case H3_TileType::TileC: layer = H3_TileTexture::TileC; break;
            case H3_TileType::TileD: layer = H3_TileTexture::TileD; break;
            case H3_TileType::TileE: layer = H3_TileTexture::TileE; break;
            default: break;
        }

        if (layer == H3_TileTexture::eTypeCount)
        {
            continue; // Skip all else, we draw Water separately
        }

        m_instanceLayer.emplace_back( layer );

        // [Instance] a_instanceAngle60
        m_instanceAngle60.emplace_back( tile.angle60 );

        // [Instance] a_instanceMat
        de::TRSf trs;
        trs.pos = tile.pos;
        trs.rotate = glm::vec3(0,0,0);
        trs.scale = game.m_tileSize;
        trs.update();
        m_instanceMat.emplace_back( trs.modelMat );
    }
}

void H3_Tile3DRenderer::uploadInstances()
{
    if (m_instanceMat.empty()) { return; }

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceMat);
    glBufferData(GL_ARRAY_BUFFER,
                    m_instanceMat.size() * sizeof(glm::mat4),
                    m_instanceMat.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceAngle60);
    glBufferData(GL_ARRAY_BUFFER,
                    m_instanceAngle60.size() * sizeof(uint8_t),
                    m_instanceAngle60.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceLayer);
    glBufferData(GL_ARRAY_BUFFER,
                    m_instanceLayer.size() * sizeof(uint8_t),
                    m_instanceLayer.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void H3_Tile3DRenderer::collectAndUploadInstances(const H3_Game& game)
{
    collectInstances(game );
    uploadInstances();
}

void H3_Tile3DRenderer::render()
{
    if (!m_driver) { DE_ERROR("No driver") return; }

    // [RenderStates]
    de::gpu::State state;
    m_driver->setState( state );

    m_driver->useShader(m_shader);

    // [Uniform] pvMat
    glm::mat4 pvMat(1.0f);
    auto camera = m_driver->getCamera();
    if (camera) { pvMat = camera->getViewProjectionMatrix(); }
    glUniformMatrix4fv(m_u_pvMat, 1, GL_FALSE, glm::value_ptr(pvMat));

    // [Uniform] Tex2DArray
    // int unit = m_driver->useTexture( m_tileTexture.tex, 0);
    glBindTextureUnit(0, m_tileTexture->id);
    glUniform1i(m_u_tex, 0);

    // [Render]
    glBindVertexArray(m_vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 12, m_instanceMat.size());
    glBindVertexArray(0);
}
