#include "H3_WaterTileRenderer2.h"
#include <H3/H3_Game.h>
#include <H3/details/H3_Img.h>
#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>


//===========================================================
H3_WaterTileRenderer2::H3_WaterTileRenderer2()
//===========================================================
{
    m_shader = nullptr;
    m_u_timePosition = -1;
    m_u_timeDuration = -1;
    m_u_animationFrames = -1;
    m_u_pvMat = -1;
    m_u_tex2DArray = -1;
    m_vao = 0;
    m_vbo_instanceMat = 0;
    m_vbo_instanceAngle60 = 0;

}

H3_WaterTileRenderer2::~H3_WaterTileRenderer2()
{

}

void H3_WaterTileRenderer2::destroy()
{

}

void H3_WaterTileRenderer2::init( H3_Game& game )
{
    m_driver = game.getDriver();

    initShader();
    initVAO();
}

void H3_WaterTileRenderer2::initVAO()
{
    if (!m_vao) { glGenVertexArrays(1, &m_vao); }
    if (!m_vbo_instanceMat) { glGenBuffers(1, &m_vbo_instanceMat); }
    if (!m_vbo_instanceAngle60) { glGenBuffers(1, &m_vbo_instanceAngle60); }

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
    glVertexAttribPointer(4, 1, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(uint8_t), (void*)0);
    glVertexAttribDivisor(4, 1); // Per-instance divisor
    glEnableVertexAttribArray(4);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    GL_VALIDATE;
}

void H3_WaterTileRenderer2::uploadInstances()
{
    if (m_instanceMat.empty()) { return; }

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceMat);
    glBufferData(GL_ARRAY_BUFFER, m_instanceMat.size() * sizeof(glm::mat4), m_instanceMat.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceAngle60);
    glBufferData(GL_ARRAY_BUFFER, m_instanceAngle60.size() * sizeof(uint8_t), m_instanceAngle60.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void H3_WaterTileRenderer2::collectAndUploadInstances(const H3_Game& game)
{
    collectInstances( game );
    uploadInstances();
}

void H3_WaterTileRenderer2::collectInstances(const H3_Game& game)
{
    m_instanceMat.clear();
    m_instanceAngle60.clear();

    const size_t n = game.m_tiles.size();
    m_instanceMat.reserve(n);
    m_instanceAngle60.reserve(n);

    for (const auto& tile : game.m_tiles)
    {
        if ( !tile.tt.isWaterOrPort() ) continue;

        de::TRSf trs;
        trs.pos = tile.pos;
        trs.rotate = glm::vec3(0,0,0);
        trs.scale.x = H3_tileSize.x;
        trs.scale.y = H3_tileSize.z;
        trs.scale.z = H3_tileSize.y;
        trs.update();
        m_instanceMat.push_back( trs.modelMat );
        m_instanceAngle60.emplace_back( tile.angle60 );
    }
}


void H3_WaterTileRenderer2::render(de::gpu::Tex2DArray& tex2DArray, float duration)
{
    if (!m_driver) { DE_ERROR("No driver") return; }

    if (!m_vao || m_instanceMat.empty())
    {
        return; // Nothing to draw.
    }

    m_driver->useShader(m_shader);

    // [Uniform] pvMat
    glm::mat4 pvMat(1.0f);
    auto camera = m_driver->getCamera();
    if (camera) { pvMat = camera->getViewProjectionMatrix(); }
    glUniformMatrix4fv(m_u_pvMat, 1, GL_FALSE, glm::value_ptr(pvMat));

    // [Uniform] time + duration
    glUniform1f(m_u_timePosition, m_driver->getTime());
    glUniform1f(m_u_timeDuration, duration);
    glUniform1f(m_u_animationFrames, tex2DArray.layers);

    // [Uniform] sampler2DArray
    //m_driver->useTexture( m_tex.tex, 0);
    glBindTextureUnit(0,tex2DArray.id);
    de::gpu::applySamplerOptions(tex2DArray.so);
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D_ARRAY, m_tex2DArray.id); // Binde die native GL-id deines Arrays
    //glBindTextureUnit(0, m_tex2DArray.id);
    glUniform1i(m_u_tex2DArray, 0);

    // [RenderStates]
    de::gpu::State state;
    m_driver->setState( state );

    // [Render]
    glBindVertexArray(m_vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 12, m_instanceMat.size());
    glBindVertexArray(0);
}

void H3_WaterTileRenderer2::initShader()
{
    if (!m_driver) { DE_ERROR("No driver") return; }
    if (m_shader) { return; } // Nothing todo.

    const std::string shaderName = "ani_h2o_3D_rend2";
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

        uniform float u_timePosition;
        uniform float u_timeDuration;
        uniform float u_animationFrames;
        uniform sampler2DArray u_tex2DArray;

        void main()
        {
            // 1. Get the 0.0 to 1.0 animation progress
            float t = mod(u_timePosition, u_timeDuration) / u_timeDuration;

            float v_texLayer = t * u_animationFrames;

            vec3 texCoord = vec3(v_texCoord.x, v_texCoord.y, v_texLayer);
            vec3 diffuseColor = texture(u_tex2DArray, texCoord).rgb;
            diffuseColor *= 1.5;
            // diffuseColor = clamp(diffuseColor * 2.0, 0.0, 1.0);

            // Ein sattes, leuchtendes Cyan-Blau (wirkt extrem hell und lebendig)
            vec3 vibrantBlue = vec3(0.1, 0.6, 1.0);
            diffuseColor *= vibrantBlue;
            diffuseColor = clamp(diffuseColor, 0.0, 1.0);

            // Gamma-Aufhellung (Standardwert 2.2)
            float gamma = 2.0;
            diffuseColor = pow(diffuseColor, vec3(1.0 / gamma));

            fragColor = vec4(diffuseColor,1.0);

            fragColor = clamp(fragColor, vec4(0.0,0.0,0.0,0.0), vec4(1.0,1.0,1.0,1.0));
        }
    )";

    m_shader = m_driver->createShader( shaderName, vs, fs );
    m_u_pvMat = glGetUniformLocation(m_shader->id, "u_projViewMat");
    m_u_timePosition = glGetUniformLocation(m_shader->id, "u_timePosition");
    m_u_timeDuration = glGetUniformLocation(m_shader->id, "u_timeDuration");
    m_u_animationFrames = glGetUniformLocation(m_shader->id, "u_animationFrames");
    m_u_tex2DArray = glGetUniformLocation(m_shader->id, "u_tex2DArray");
}
