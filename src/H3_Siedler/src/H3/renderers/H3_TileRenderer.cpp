#include "H3_TileRenderer.h"
#include <H3/H3_Game.h>
#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>

void H3_TileRenderer_initVAO(uint32_t& vao, uint32_t& vboMat, uint32_t& vboAngle60)
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vboMat);
    glGenBuffers(1, &vboAngle60);
    glBindVertexArray(vao);

    // vbo_instanceMat:
    glBindBuffer(GL_ARRAY_BUFFER, vboMat);
    for (int i = 0; i < 4; i++)
    {
        glVertexAttribPointer(i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(float) * i * 4));
        glVertexAttribDivisor(i, 1); // Update per instance
        glEnableVertexAttribArray(i);
    }

    // vbo_instanceAngle60:
    glBindBuffer(GL_ARRAY_BUFFER, vboAngle60);
    glVertexAttribPointer(4, 1, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(uint8_t), (void*)0);
    glVertexAttribDivisor(4, 1); // Per-instance divisor
    glEnableVertexAttribArray(4);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GL_VALIDATE;
}

H3_TileRenderer::H3_TileRenderer()
{
    m_shader = nullptr;
}

H3_TileRenderer::~H3_TileRenderer()
{

}

void H3_TileRenderer::destroy()
{

}

void H3_TileRenderer::init( H3_Game& game )
{
    m_driver = game.getDriver();

    if (!m_driver) { DE_ERROR("No driver") return; }

    de::gpu::SO so;
    so.setAF( 1.0f );
    so.setMin( de::gpu::SO::Minify::LinearMipmapLinear );
    so.setMag( de::gpu::SO::Magnify::Linear );
    so.setWrapR( de::gpu::SO::Wrap::Repeat );
    so.setWrapS( de::gpu::SO::Wrap::Repeat );
    so.setWrapT( de::gpu::SO::Wrap::Repeat );

    const auto imgTileA = H3_getImg( game, H3_Img::TileA );
    const auto imgTileB = H3_getImg( game, H3_Img::TileB );
    const auto imgTileC = H3_getImg( game, H3_Img::TileC );
    const auto imgTileD = H3_getImg( game, H3_Img::TileD );
    const auto imgTileE = H3_getImg( game, H3_Img::TileE );
    const auto imgTileF = H3_getImg( game, H3_Img::TileDesert );

    auto texA = m_driver->createTexture2D( "H3_TTexA", *imgTileA, so);
    auto texB = m_driver->createTexture2D( "H3_TTexB", *imgTileB, so);
    auto texC = m_driver->createTexture2D( "H3_TTexC", *imgTileC, so);
    auto texD = m_driver->createTexture2D( "H3_TTexD", *imgTileD, so);
    auto texE = m_driver->createTexture2D( "H3_TTexE", *imgTileE, so);
    auto texF = m_driver->createTexture2D( "H3_TTexF", *imgTileF, so);
    m_tex[0] = de::gpu::TexRef(texA);
    m_tex[1] = de::gpu::TexRef(texB);
    m_tex[2] = de::gpu::TexRef(texC);
    m_tex[3] = de::gpu::TexRef(texD);
    m_tex[4] = de::gpu::TexRef(texE);
    m_tex[5] = de::gpu::TexRef(texF);

    //initVAO();

    for (int i = 0; i < 6; ++i)
    {
        H3_TileRenderer_initVAO(
                m_vao[i],
                m_vbo_instanceMat[i],
                m_vbo_instanceAngle60[i]);
    }

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
        layout(location = 4) in float a_instanceAngle60; // index into uvcoords

        uniform mat4 u_projViewMat;

        out vec2 v_texCoord;

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

        void main()
        {
            const uint i = a_indices[gl_VertexID];
            const uint k = (i + uint(a_instanceAngle60)) % 6;
            v_texCoord = a_texcoord[k];
            const float x = a_position[i].x;
            const float z = a_position[i].y;
            gl_Position = u_projViewMat * a_instanceMat * vec4(x, 0.0, z, 1.0);
        }
        )";

    // Fragment Shader Source
    const std::string fs = R"(

        out vec4 fragColor;
        in vec2 v_texCoord;

        uniform sampler2D u_tex;

        void main()
        {
            fragColor = texture(u_tex, v_texCoord);
        }
    )";

    m_shader = m_driver->createShader( shaderName, vs, fs );
    m_loc_pvMat = glGetUniformLocation(m_shader->id, "u_projViewMat");
    m_loc_tex = glGetUniformLocation(m_shader->id, "u_tex");

    //m_loc_timeInSec = glGetUniformLocation(m_shader->id, "u_timeInSec");
    //m_loc_gradientMap = glGetUniformLocation(m_shader->id, "u_gradientMap");
    //m_loc_dudvMap = glGetUniformLocation(m_shader->id, "u_dudvMap");
}


void H3_TileRenderer::collectInstances(int i, const H3_Game& game)
{
    m_instanceMat[i].clear();
    m_instanceAngle60[i].clear();
    //m_instanceLayer.clear();

    const size_t n = game.m_tiles.size();
    m_instanceMat[i].reserve(n);
    m_instanceAngle60[i].reserve(n);
    //m_instanceLayer.reserve(n);

    H3_TileType tt = H3_TileType::Desert;
    switch (i)
    {
        case 0: tt = H3_TileType::TileA; break;
        case 1: tt = H3_TileType::TileB; break;
        case 2: tt = H3_TileType::TileC; break;
        case 3: tt = H3_TileType::TileD; break;
        case 4: tt = H3_TileType::TileE; break;
        default: break;
    }

    for (const auto& tile : game.m_tiles)
    {
        if ( tile.tt != tt ) continue; // Skip all non 'tt' tiles

        de::TRSf trs;
        trs.pos = tile.pos;
        trs.rotate = glm::vec3(0,0,0);
        trs.scale = game.m_tileSize;
        trs.update();
        m_instanceMat[i].push_back( trs.modelMat );
        m_instanceAngle60[i].emplace_back( tile.angle60 );
    }
}


void H3_TileRenderer::uploadInstances(int i)
{
    if (m_instanceMat[i].empty()) { return; }

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceMat[i]);
    glBufferData(GL_ARRAY_BUFFER,
                    m_instanceMat[i].size() * sizeof(glm::mat4),
                    m_instanceMat[i].data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceAngle60[i]);
    glBufferData(GL_ARRAY_BUFFER,
                    m_instanceAngle60[i].size() * sizeof(uint8_t),
                    m_instanceAngle60[i].data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void H3_TileRenderer::collectAndUploadInstances(const H3_Game& game)
{
    for (int i = 0; i < 6; i++)
    {
        collectInstances( i, game );
        uploadInstances( i );
    }
}

void H3_TileRenderer::render()
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
    glUniformMatrix4fv(m_loc_pvMat, 1, GL_FALSE, glm::value_ptr(pvMat));

    for (int i = 0; i < 6; i++)
    {
        if (!m_vao[i] || m_instanceMat[i].empty())
        {
            continue; // Nothing to draw.
        }

        // [Uniform] sampler2D
        m_driver->useTexture( m_tex[i].tex, 0);
        glUniform1i(m_loc_tex, 0);

        // [Render]
        glBindVertexArray(m_vao[i]);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 12, m_instanceMat[i].size());
        glBindVertexArray(0);

        glBindTexture( GL_TEXTURE_2D, 0 );
    }

    // [CleanUp]
    GL_VALIDATE;
}

