#include "H3_WaterTileRenderer.h"
#include <H3/H3_Game.h>
#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>

H3_WaterTileRenderer::H3_WaterTileRenderer()
{
    m_shader = nullptr;
    m_vao = 0;
    m_vbo_instanceMat = 0;
    m_vbo_instanceAngle60 = 0;
}

H3_WaterTileRenderer::~H3_WaterTileRenderer()
{

}

void H3_WaterTileRenderer::destroy()
{

}

void H3_WaterTileRenderer::init( H3_Game& game )
{
    m_driver = game.getDriver();

    initShader();
    initTexture( game );
    initVAO();
}

void H3_WaterTileRenderer::initTexture( H3_Game& game )
{
    if (!m_driver) { DE_ERROR("No driver") return; }
    const auto imgDiffuse = H3_getImg( game, H3_Img::TileWater );
    de::gpu::SO soDiffuse;
    soDiffuse.setAF( 1.0f );
    soDiffuse.setMin( de::gpu::SO::Minify::LinearMipmapLinear );
    soDiffuse.setMag( de::gpu::SO::Magnify::Linear );
    soDiffuse.setWrapR( de::gpu::SO::Wrap::Repeat );
    soDiffuse.setWrapS( de::gpu::SO::Wrap::Repeat );
    soDiffuse.setWrapT( de::gpu::SO::Wrap::Repeat );
    auto texDiffuse = m_driver->createTexture2D( "H3_WaterTileRendTex", *imgDiffuse, soDiffuse);
    m_diffuseMap = de::gpu::TexRef(texDiffuse);
}

void H3_WaterTileRenderer::initVAO()
{
    if (!m_vao) { glGenVertexArrays(1, &m_vao); }
    if (!m_vbo_instanceMat) { glGenBuffers(1, &m_vbo_instanceMat); }
    if (!m_vbo_instanceAngle60) { glGenBuffers(1, &m_vbo_instanceAngle60); }

    glBindVertexArray(m_vao);

    // vbo_instanceMat:
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceMat);
  //glBufferData(GL_ARRAY_BUFFER, m_instanceMat.size() * sizeof(glm::mat4), m_instanceMat.data(), GL_STATIC_DRAW);
    for (int i = 0; i < 4; i++)
    {
        glVertexAttribPointer(i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(float) * i * 4));
        glVertexAttribDivisor(i, 1); // Update per instance
        glEnableVertexAttribArray(i);
    }

    // vbo_instanceAngle60:
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceAngle60);
  //glBufferData(GL_ARRAY_BUFFER, m_instanceAngle60.size() * sizeof(uint8_t), m_instanceAngle60.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(4, 1, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(uint8_t), (void*)0);
    glVertexAttribDivisor(4, 1); // Per-instance divisor
    glEnableVertexAttribArray(4);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GL_VALIDATE;
}

void H3_WaterTileRenderer::uploadInstances()
{
    if (m_instanceMat.empty()) { return; }

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceMat);
    glBufferData(GL_ARRAY_BUFFER, m_instanceMat.size() * sizeof(glm::mat4), m_instanceMat.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceAngle60);
    glBufferData(GL_ARRAY_BUFFER, m_instanceAngle60.size() * sizeof(uint8_t), m_instanceAngle60.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void H3_WaterTileRenderer::collectAndUploadInstances(const H3_Game& game)
{
    collectInstances( game );
    uploadInstances();
}

void H3_WaterTileRenderer::collectInstances(const H3_Game& game)
{
    m_instanceMat.clear();
    //m_instanceLayer.clear();
    m_instanceAngle60.clear();

    const size_t n = game.m_tiles.size();
    m_instanceMat.reserve(n);
    //m_instanceLayer.reserve(n);
    m_instanceAngle60.reserve(n);

    for (const auto& tile : game.m_tiles)
    {
        if ( !tile.tileType.isWaterOrPort() ) continue;

        de::TRSf trs;
        trs.pos = tile.pos;
        trs.rotate = glm::vec3(0,0,0);
        trs.scale = game.m_tileSize;
        trs.update();
        m_instanceMat.push_back( trs.modelMat );

        m_instanceAngle60.emplace_back( tile.angle60 );
    }
}


void H3_WaterTileRenderer::render()
{
    if (!m_driver) { DE_ERROR("No driver") return; }

    if (!m_vao || m_instanceMat.empty())
    {
        return; // Nothing to draw.
    }

    m_driver->useShader(m_shader);

    // [Uniform] timeInSec
    glUniform1f(m_loc_timeInSec, m_driver->getTime());

    // [Uniform] pvMat
    glm::mat4 pvMat(1.0f);
    auto camera = m_driver->getCamera();
    if (camera) { pvMat = camera->getViewProjectionMatrix(); }
    glUniformMatrix4fv(m_loc_pvMat, 1, GL_FALSE, glm::value_ptr(pvMat));

    // [Uniform] sampler2D
    m_driver->useTexture( m_diffuseMap.tex, 0);
    glUniform1i(m_loc_diffuseMap, 0);

    // [RenderStates]
    de::gpu::State state;
    state.blend = de::gpu::Blend::alphaBlend();
    m_driver->setState( state );

    // [Render]
    glBindVertexArray(m_vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 12, m_instanceMat.size());

    // [CleanUp]
    glBindVertexArray(0);
    glBindTexture( GL_TEXTURE_2D, 0 );
    GL_VALIDATE;
}

void H3_WaterTileRenderer::initShader()
{
    if (!m_driver) { DE_ERROR("No driver") return; }
    if (m_shader) { return; } // Nothing todo.

    const std::string shaderName = "h2oTil";
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

        uniform float u_timeInSec;
        uniform sampler2D u_diffuseMap;

        void main()
        {
            vec2 texCoord = v_texCoord;
            texCoord.x = fract( texCoord.x + u_timeInSec * 0.01);
            texCoord.y = fract( texCoord.y + u_timeInSec * 0.02 );
            fragColor = texture(u_diffuseMap, texCoord);
        }
    )";

    m_shader = m_driver->createShader( shaderName, vs, fs );

    m_loc_timeInSec = glGetUniformLocation(m_shader->id, "u_timeInSec");
    m_loc_pvMat = glGetUniformLocation(m_shader->id, "u_projViewMat");
    m_loc_diffuseMap = glGetUniformLocation(m_shader->id, "u_diffuseMap");
    m_loc_gradientMap = glGetUniformLocation(m_shader->id, "u_gradientMap");
    m_loc_dudvMap = glGetUniformLocation(m_shader->id, "u_dudvMap");
}
