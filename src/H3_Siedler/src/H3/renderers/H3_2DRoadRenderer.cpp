#include "H3_2DRoadRenderer.h"
#include <H3/H3_Game.h>
#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>

H3_2DRoadRenderer::H3_2DRoadRenderer()
    : m_driver(nullptr)
    , m_shader(nullptr)
    , m_chipTexture(nullptr)
    , m_vao(0)
    , m_u_screenSize(-1)
    , m_u_tex2DArray(-1)
    , m_u_posTransform(-1)
{}

H3_2DRoadRenderer::~H3_2DRoadRenderer()
{
    if (m_vao || m_chipTexture)
    {
        DE_ERROR("Forgot to call destroy()")
    }
}

void H3_2DRoadRenderer::destroy()
{
    if (m_vao)
    {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo_instancePos);
        glDeleteBuffers(1, &m_vbo_instanceAngle);
        glDeleteBuffers(1, &m_vbo_instanceLayer);

        m_vao = 0;
        m_vbo_instancePos = 0;
        m_vbo_instanceAngle = 0;
        m_vbo_instanceLayer = 0;
    }

    if (m_chipTexture)
    {
        m_chipTexture = nullptr;
    }
}

void H3_2DRoadRenderer::init( H3_Game& game, H3_ChipTexture* chipTexture )
{
    m_driver = game.getDriver();
    m_chipTexture = chipTexture;
    if (!m_driver) { DE_ERROR("No driver") return; }
    if (!m_chipTexture) { DE_ERROR("No chipTexture") return; }
    if (!m_chipTexture->m_tex.id) { DE_ERROR("No chipTexture id") return; }

    initVAO(game);
    initShader(game);
}

void H3_2DRoadRenderer::initVAO( H3_Game& game )
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo_instancePos);
    glGenBuffers(1, &m_vbo_instanceAngle);
    glGenBuffers(1, &m_vbo_instanceLayer);

    glBindVertexArray(m_vao);

    int k = 0;
    // location[0] vbo_instancePos:
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instancePos);
    glVertexAttribPointer(k, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    glVertexAttribDivisor(k, 1); // Update per instance
    glEnableVertexAttribArray(k);
    k++;

    // location[1] vbo_instanceRadius:
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceAngle);
    glVertexAttribPointer(k, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
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


void H3_2DRoadRenderer::initShader( H3_Game& game )
{
    m_driver = game.getDriver();

    if (!m_driver) { DE_ERROR("No driver") return; }

    const std::string shaderName = "road2drend";
    const std::string vs = R"(

        layout(location = 0) in vec4 a_instancePos;
        layout(location = 1) in float a_instanceAngle;
        layout(location = 2) in uint a_instanceLayer;

        const vec2 a_vertices[6] = vec2[](
            vec2(0.0, 1.0), vec2(1.0, 1.0), vec2(1.0, 0.0),
            vec2(0.0, 1.0), vec2(1.0, 0.0), vec2(0.0, 0.0)
        );

        uniform vec2 u_screenSize;
        uniform vec4 u_posTransform;

        out vec2 v_tex;
        flat out uint v_layer;

        // Hilfsfunktion, um die echten Pixelkoordinaten (Y-oben) zu berechnen
        vec2 toPixels(vec2 p0, float angle, vec2 offset1, vec2 scale1, vec2 offset2, vec2 scale2)
        {
            float c = cos(angle);
            float s = sin(angle);

            // Apply 2D rotation matrix:
            // x' = x * cos(theta) - y * sin(theta)
            // y' = x * sin(theta) + y * cos(theta)
            vec2 p1 = vec2(c*p0.x - s*p0.y, s*p0.x + c*p0.y);
            vec2 p2 = offset1 + (scale1 * p1);

            //vec2 p2 = vec2(c*p1.x - s*p1.y, s*p1.x + c*p1.y);

            //vec2 p1 = offset1 + (scale1 * p0);

            vec2 p3 = offset2 + (scale2 * p2);
            return p3; // Das sind die Koordinaten in Pixeln (Y=0 oben)
        }

        vec2 toNDC(vec2 p2, vec2 screenSize) {
            vec2 p3 = vec2(p2.x, screenSize.y - p2.y); // Y-Achse für OpenGL umdrehen
            return (p3 / screenSize) * 2.0 - 1.0;
        }

        void main() {
            vec2 a_tex = a_vertices[gl_VertexID];
            vec2 a_pos = a_tex - vec2(0.5, 0.5);

            vec2 off1 = a_instancePos.xy; vec2 scl1 = a_instancePos.zw;
            vec2 off2 = u_posTransform.xy; vec2 scl2 = u_posTransform.zw;

            float angle = a_instanceAngle;

            // 1. Berechne Position in Pixeln (Y=0 oben)
            vec2 pixelPos = toPixels(a_pos, angle, off1, scl1, off2, scl2);

            // 2. In NDC für gl_Position umwandeln
            gl_Position = vec4(toNDC(pixelPos, u_screenSize), 0.0, 1.0);

            v_tex = a_tex;
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
            // 1. Texture-Farbe holen
            vec3 texCoord = vec3(v_tex.x, v_tex.y, float(v_layer));
            fragColor = texture(u_tex2DArray, texCoord);
        }
    )";

    // // Kreis mit smoothstep für weiche/harte Kanten definieren
    // float circle = 1.0 - smoothstep(radius - edge_softness,
    //                                 radius + edge_softness, dist);

    m_shader = m_driver->createShader( shaderName, vs, fs );
    m_u_screenSize = glGetUniformLocation(m_shader->id, "u_screenSize");
    m_u_tex2DArray = glGetUniformLocation(m_shader->id, "u_tex2DArray");
    m_u_posTransform = glGetUniformLocation(m_shader->id, "u_posTransform");
}

void H3_2DRoadRenderer::render(const glm::vec4& posTransform, const de::Recti& scissor)
{
    if (!m_driver) { DE_ERROR("No driver") return; }
    if (!m_chipTexture) { DE_ERROR("No chipTexture") return; }
    if (!m_shader) { DE_ERROR("No shader") return; }
    if (!m_vao) { DE_ERROR("No vao") return; }

    // [RenderStates]
    de::gpu::State state;
    state.culling = de::gpu::Culling::disabled();
    state.depth = de::gpu::Depth::disabled();
    state.blend = de::gpu::Blend::alphaBlend();
    state.scissor = de::gpu::Scissor::enabled(scissor);
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
    glBindTexture(GL_TEXTURE_2D_ARRAY, m_chipTexture->m_tex.id);
    //glBindTextureUnit(0, m_tex2DArray.id);
    de::gpu::applySamplerOptions(m_chipTexture->m_tex.so);
    glUniform1i(m_u_tex2DArray, 0);

    // [VAO] Quads:
    glBindVertexArray(m_vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, m_instancePos.size());
    glBindVertexArray(0);

    // [CleanUp]
    // glBindTexture( GL_TEXTURE_2D, 0 );
    GL_VALIDATE;
}

void H3_2DRoadRenderer::uploadInstances()
{
    if (m_instancePos.empty()) { return; }

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instancePos);
    glBufferData(GL_ARRAY_BUFFER,
                    m_instancePos.size() * sizeof(glm::vec4),
                    m_instancePos.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceAngle);
    glBufferData(GL_ARRAY_BUFFER,
                    m_instanceAngle.size() * sizeof(float),
                    m_instanceAngle.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceLayer);
    glBufferData(GL_ARRAY_BUFFER,
                    m_instanceLayer.size() * sizeof(uint8_t),
                    m_instanceLayer.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void H3_2DRoadRenderer::collectAndUploadInstances(const H3_Game& game, const H3_Cfg& cfg)
{
    collectInstances( game, cfg );
    uploadInstances();
}

void H3_2DRoadRenderer::collectInstances(const H3_Game& game, const H3_Cfg& cfg)
{
    m_instancePos.clear();
    m_instanceAngle.clear();
    m_instanceLayer.clear();

    const size_t n = cfg.tiles.size();
    m_instancePos.reserve(n);
    m_instanceAngle.reserve(n);
    m_instanceLayer.reserve(n);

    auto tileSize = glm::vec2(100,100);
    auto roadSize = glm::vec2(30, 6);
    auto roadTex = game.getTex(H3_Tex::Wood2, __func__ );

    for (const H3_CfgTile& tile : cfg.tiles)
    {
        if (!tile.tt().isPort()) continue;

        float x = tile.x();
        float y = tile.y();
        glm::vec2 tile_pos = glm::vec2(x,y);

        int ang60 = tile.angle60();

        // Draw bridge A:
        glm::vec2 cornerA_pos = de::smesh::SMeshHexagon::getCorner(ang60, tileSize.x, tileSize.y);
        float cornerA_ang = atan2( cornerA_pos.y, cornerA_pos.x );
        cornerA_ang = de::Math::wrapAngleToRange0_2pi(de::Math::PI - cornerA_ang); // 180.0f -
        glm::vec2 posA = tile_pos + cornerA_pos * 0.5f;

        // Draw bridge B:
        glm::vec2 cornerB_pos = de::smesh::SMeshHexagon::getCorner((ang60 + 1) % 6, tileSize.x, tileSize.y);
        float cornerB_ang = atan2( cornerB_pos.y, cornerB_pos.x );
        cornerB_ang = de::Math::wrapAngleToRange0_2pi(de::Math::PI - cornerB_ang); // 180.0f -
        glm::vec2 posB = tile_pos + cornerB_pos * 0.5f;

        // [Instance] a_instanceLayer
        uint8_t layer = H3_ChipTexture::Chip3v1;
        switch( tile.tt() )
        {
            case H3_TileType::PortA: layer = H3_ChipTexture::ChipA; break;
            case H3_TileType::PortB: layer = H3_ChipTexture::ChipB; break;
            case H3_TileType::PortC: layer = H3_ChipTexture::ChipC; break;
            case H3_TileType::PortD: layer = H3_ChipTexture::ChipD; break;
            case H3_TileType::PortE: layer = H3_ChipTexture::ChipE; break;
            default: break;
        }

        // [Instance] a_instancePos
        // [Instance] a_instanceRadius
        // [Instance] a_instanceLayer
        glm::vec4 posTransformA;
        posTransformA.x = posA.x;
        posTransformA.y = posA.y;
        posTransformA.z = roadSize.x;
        posTransformA.w = roadSize.y;
        m_instancePos.emplace_back( posTransformA );
        m_instanceAngle.emplace_back( cornerA_ang );
        m_instanceLayer.emplace_back( layer );

        // [Instance] a_instancePos
        // [Instance] a_instanceRadius
        // [Instance] a_instanceLayer
        glm::vec4 posTransformB;
        posTransformB.x = posB.x;
        posTransformB.y = posB.y;
        posTransformB.z = roadSize.x;
        posTransformB.w = roadSize.y;
        m_instancePos.emplace_back( posTransformB );
        m_instanceAngle.emplace_back( cornerB_ang );
        m_instanceLayer.emplace_back( layer );
    }

    DE_ERROR("Collected ", m_instancePos.size(), " instances")
}

/*
void H3_Game::drawPortBridges()
{
    auto driver = getDriver();
    driver->beginPerf("PortBridges");

    auto rend = driver->getSMaterialRenderer();

    de::smesh::SMeshBuffer bridge;
    de::smesh::SMeshBox::add( bridge, m_roadSize, {0,0,0}, 0xFFFFFFFF );
    bridge.primitiveType = de::gpu::PrimitiveType::Triangles;
    bridge.material.Lighting = 1;
    bridge.material.setTexture( 0, getTex(H3_Tex::Wood2, "H3_Game::drawPortBridges()") );
    bridge.upload();

    for (H3_Tile & tile : m_tiles)
    {
        if (!tile.tt.isPort()) continue;

        int ang60 = tile.angle60;

        // Draw bridge A:
        auto cornerA_pos2 = de::smesh::SMeshHexagon::getCorner(ang60, m_tileSize.x, m_tileSize.z);
        auto cornerA_pos3 = glm::vec3(cornerA_pos2.x, 0, cornerA_pos2.y);
        auto cornerA_ang = atan2( cornerA_pos2.y, cornerA_pos2.x ) * de::Math::RAD2DEG;
        cornerA_ang = de::Math::wrapAngle0_360(180.0f - cornerA_ang);
        de::TRSf trs;
        trs.pos = tile.pos + cornerA_pos3 * 0.5f;
        trs.rotate = glm::vec3(0,cornerA_ang,0);
        trs.update();
        driver->setModelMatrix( trs.modelMat );
        rend->draw3D(bridge);

        // Draw bridge B:
        auto cornerB_pos2 = de::smesh::SMeshHexagon::getCorner((ang60 + 1) % 6, m_tileSize.x, m_tileSize.z);
        auto cornerB_pos3 = glm::vec3(cornerB_pos2.x, 0, cornerB_pos2.y);
        auto cornerB_ang = atan2( cornerB_pos2.y, cornerB_pos2.x ) * de::Math::RAD2DEG;
        cornerB_ang = de::Math::wrapAngle0_360(180.0f - cornerB_ang);
        trs.reset();
        trs.pos = tile.pos + cornerB_pos3 * 0.5f;
        trs.rotate = glm::vec3(0,cornerB_ang,0);
        trs.update();
        driver->setModelMatrix( trs.modelMat );
        rend->draw3D(bridge);
    }

    bridge.destroy();

    driver->endPerf();
}
*/
