#include "H3_Tile2DRenderer.h"
#include <H3/H3_Game.h>
#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>

H3_Tile2DRenderer::H3_Tile2DRenderer()
    : m_driver(nullptr)
    , m_shader(nullptr)
    , m_tileTexture(nullptr)
    , m_vao(0)
    , m_u_screenSize(-1)
    , m_u_tex2DArray(-1)
    , m_u_posTransform(-1)
{}

H3_Tile2DRenderer::~H3_Tile2DRenderer()
{
    if (m_vao)
    {
        DE_ERROR("Forgot to call destroy()")
    }
}

void H3_Tile2DRenderer::destroy()
{
    if (m_vao)
    {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo_instancePos);
        glDeleteBuffers(1, &m_vbo_instanceAngle60);
        glDeleteBuffers(1, &m_vbo_instanceLayer);

        m_vao = 0;
        m_vbo_instancePos = 0;
        m_vbo_instanceAngle60 = 0;
        m_vbo_instanceLayer = 0;
    }
}

void H3_Tile2DRenderer::init( H3_Game& game )
{
    m_driver = game.getDriver();

    if (!m_driver) { DE_ERROR("No driver") return; }

    m_tileTexture = &game.m_tex.m_tileTexture;
    initVAO(game);
    initShader(game);
}

void H3_Tile2DRenderer::initVAO( H3_Game& game )
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo_instancePos);
    glGenBuffers(1, &m_vbo_instanceAngle60);
    glGenBuffers(1, &m_vbo_instanceLayer);

    glBindVertexArray(m_vao);

    int k = 0;
    // location[0] vbo_instancePos:
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instancePos);
    glVertexAttribPointer(k, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    glVertexAttribDivisor(k, 1); // Update per instance
    glEnableVertexAttribArray(k);
    k++;

    // location[1] vbo_instanceAngle60:
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceAngle60);
    glVertexAttribIPointer(k, 1, GL_UNSIGNED_BYTE, sizeof(uint8_t), (void*)0);
    glVertexAttribDivisor(k, 1); // Per-instance divisor
    glEnableVertexAttribArray(k);
    k++;

    // location[2] vbo_instanceLayer:
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceLayer);
    glVertexAttribIPointer(k, 1, GL_UNSIGNED_BYTE, sizeof(uint8_t), (void*)0);
    glVertexAttribDivisor(k, 1); // Per-instance divisor
    glEnableVertexAttribArray(k);
    k++;

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GL_VALIDATE;
}


void H3_Tile2DRenderer::initShader( H3_Game& game )
{
    m_driver = game.getDriver();

    if (!m_driver) { DE_ERROR("No driver") return; }

    const std::string shaderName = "hxbrd2";
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

        layout(location = 0) in vec4 a_instancePos; // (vec4 xy=offset_xy, wz=scale_xy)
        layout(location = 1) in uint a_instanceAngle60; // index into uvcoords
        layout(location = 2) in uint a_instanceLayer; // index into tex2DArray

        const vec2 a_vertices[6] = vec2[](
            vec2( 0.00,  0.50), // A=0
            vec2(-0.50,  0.25), // B=1
            vec2(-0.50, -0.25), // C=2
            vec2( 0.00, -0.50), // D=3
            vec2( 0.50, -0.25), // E=4
            vec2( 0.50,  0.25)  // F=5
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

        uniform vec2 u_screenSize;
        uniform vec4 u_posTransform;

        out vec2 v_tex;
        flat out uint v_layer;

        void main()
        {
            const uint i = a_indices[gl_VertexID];
            const uint k = (i + a_instanceAngle60) % 6u;

            // Symmetric Model:
            vec2 pos0 = a_vertices[i];

            // Model:
            vec2 offset1 = a_instancePos.xy;
            vec2 scale1 = a_instancePos.zw;
            vec2 pos1 = offset1 + (scale1 * pos0);

            // World2D:
            vec2 offset2 = u_posTransform.xy;
            vec2 scale2 = u_posTransform.zw;
            vec2 pos = offset2 + (scale2 * pos1);

            // ClipSpace
            float ndc_x = (pos.x / u_screenSize.x) * 2.0 - 1.0;
            float ndc_y = ((u_screenSize.y - pos.y) / u_screenSize.y) * 2.0 - 1.0;
            gl_Position = vec4(ndc_x, ndc_y, 0.0, 1.0);

            // vec2 ndc = (pos / u_screenSize) * 2.0 - 1.0;
            // //ndc.y = -ndc.y;
            // gl_Position = vec4(ndc, 0.0, 1.0);

            v_tex = a_texcoord[k];
            v_layer = a_instanceLayer;
        }
        )";

    // Fragment Shader Source
    const std::string fs = R"(

        out vec4 fragColor;

        uniform sampler2DArray u_tex2DArray;

        in vec2 v_tex;
        flat in uint v_layer;

        void main()
        {
            vec3 texCoord = vec3(v_tex.x, v_tex.y, float(v_layer));
            fragColor = texture(u_tex2DArray, texCoord);
        }
    )";

    m_shader = m_driver->createShader( shaderName, vs, fs );
    m_u_screenSize = glGetUniformLocation(m_shader->id, "u_screenSize");
    m_u_tex2DArray = glGetUniformLocation(m_shader->id, "u_tex2DArray");
    m_u_posTransform = glGetUniformLocation(m_shader->id, "u_posTransform");
}



void H3_Tile2DRenderer::render(const glm::vec4& posTransform, const de::Recti& scissor)
{
    if (!m_driver) { DE_ERROR("No driver") return; }
    if (!m_tileTexture) { DE_ERROR("No tileTexture") return; }
    if (!m_shader) { DE_ERROR("No shader") return; }
    if (!m_vao) { DE_ERROR("No vao") return; }

    // [RenderStates]
    de::gpu::State state;
    state.culling = de::gpu::Culling::disabled();
    state.depth = de::gpu::Depth::disabled();
    state.blend = de::gpu::Blend::disabled();
    //state.scissor = de::gpu::Scissor::enabled(scissor);
    m_driver->setState(state);

    // [Shader]
    m_driver->useShader(m_shader);

    // [Shader-Uniform] u_screenSize
    const int w = m_driver->getScreenWidth();
    const int h = m_driver->getScreenHeight();
    glUniform2f(m_u_screenSize, (float)w, (float)h);

    // [Shader-Uniform] u_posTransform
    glUniform4fv(m_u_posTransform, 1, glm::value_ptr(posTransform));

    // [Shader-Uniform] u_tex2DArray
    //  m_driver->useTexture( m_tex2DArray, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_tileTexture->id); // Binde die native GL-id deines Arrays
    //glBindTextureUnit(0, m_tileTexture->id);
    glUniform1i(m_u_tex2DArray, 0);

    // [VAO]:
    glBindVertexArray(m_vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 12, m_instancePos.size());
    glBindVertexArray(0);

    // [CleanUp]
    // glBindTexture( GL_TEXTURE_2D, 0 );
    GL_VALIDATE;
}

void
H3_Tile2DRenderer::collectAndUploadInstances(const H3_Game& game, const H3_Cfg& cfg)
{
    collectInstances( game, cfg );
    uploadInstances();
}

// glm::vec2 tileSize(100,100);
void
H3_Tile2DRenderer::collectInstances(const H3_Game& game, const H3_Cfg& cfg)
{
    m_instancePos.clear();
    m_instanceAngle60.clear();
    m_instanceLayer.clear();

    const size_t n = cfg.tiles.size();
    m_instancePos.reserve(n);
    m_instanceAngle60.reserve(n);
    m_instanceLayer.reserve(n);

    for (const H3_CfgTile& tile : cfg.tiles)
    {
        // [Instance] a_instanceLayer
        uint8_t layer = H3_TileTexture::eTypeCount;
        switch (tile.tt())
        {
            // case H3_TileType::Unknown: layer = Unknown; break;
            case H3_TileType::Water: layer = H3_TileTexture::Water; break;
            case H3_TileType::Desert: layer = H3_TileTexture::Desert; break;
            case H3_TileType::TileA: layer = H3_TileTexture::TileA; break;
            case H3_TileType::TileB: layer = H3_TileTexture::TileB; break;
            case H3_TileType::TileC: layer = H3_TileTexture::TileC; break;
            case H3_TileType::TileD: layer = H3_TileTexture::TileD; break;
            case H3_TileType::TileE: layer = H3_TileTexture::TileE; break;
            case H3_TileType::Port3v1: layer = H3_TileTexture::Water; break;
            case H3_TileType::PortA: layer = H3_TileTexture::Water; break;
            case H3_TileType::PortB: layer = H3_TileTexture::Water; break;
            case H3_TileType::PortC: layer = H3_TileTexture::Water; break;
            case H3_TileType::PortD: layer = H3_TileTexture::Water; break;
            case H3_TileType::PortE: layer = H3_TileTexture::Water; break;
            default: break;
        }

        if (layer == H3_TileTexture::eTypeCount)
        {
            continue;
        }

        m_instanceLayer.emplace_back( layer );

        // [Instance] a_instancePos
        m_instancePos.emplace_back();
        glm::vec4 & pos = m_instancePos.back();
        pos.x = tile.x();
        pos.y = tile.y();
        pos.z = cfg.tileSize.x;
        pos.w = cfg.tileSize.y;

        // [Instance] a_instanceAngle60
        m_instanceAngle60.emplace_back( tile.angle60() );
    }
}


void H3_Tile2DRenderer::uploadInstances()
{
    if (m_instancePos.empty()) { return; }

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instancePos);
    glBufferData(GL_ARRAY_BUFFER,
                    m_instancePos.size() * sizeof(glm::vec4),
                    m_instancePos.data(), GL_STATIC_DRAW);

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

