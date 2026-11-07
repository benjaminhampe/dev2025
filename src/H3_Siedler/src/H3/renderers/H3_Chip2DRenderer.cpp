#include "H3_Chip2DRenderer.h"
#include <H3/H3_Game.h>
#include <de_opengl.h>
#include <de/gpu/GL_debug_layer.h>

H3_Chip2DRenderer::H3_Chip2DRenderer()
    : m_driver(nullptr)
    , m_shader(nullptr)
    , m_chipTexture(nullptr)
    , m_vao(0)
    , m_u_screenSize(-1)
    , m_u_tex2DArray(-1)
    , m_u_posTransform(-1)
{}

H3_Chip2DRenderer::~H3_Chip2DRenderer()
{
    if (m_vao)
    {
        DE_ERROR("Forgot to call destroy()")
    }
}

void H3_Chip2DRenderer::destroy()
{
    if (m_vao)
    {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo_instancePos);
        glDeleteBuffers(1, &m_vbo_instanceRadius);
        glDeleteBuffers(1, &m_vbo_instanceLayer);

        m_vao = 0;
        m_vbo_instancePos = 0;
        m_vbo_instanceRadius = 0;
        m_vbo_instanceLayer = 0;
    }
}

void H3_Chip2DRenderer::init( H3_Game& game)
{
    m_driver = game.getDriver();
    if (!m_driver) { DE_ERROR("No driver") return; }

    m_chipTexture = &game.m_tex.m_chipTexture;

    initVAO(game);
    initShader(game);
}

void H3_Chip2DRenderer::initVAO( H3_Game& game )
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo_instancePos);
    glGenBuffers(1, &m_vbo_instanceRadius);
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
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceRadius);
    glVertexAttribPointer(k, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
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


void H3_Chip2DRenderer::initShader( H3_Game& game )
{
    m_driver = game.getDriver();

    if (!m_driver) { DE_ERROR("No driver") return; }

    const std::string shaderName = "chip2drend";
    const std::string vs = R"(

        layout(location = 0) in vec4 a_instancePos;
        layout(location = 1) in vec2 a_instanceRadius; // Wir nehmen an, .x ist der Radius in Pixeln
        layout(location = 2) in uint a_instanceLayer;

        const vec2 a_vertices[6] = vec2[](
            vec2(0.0, 1.0), vec2(1.0, 1.0), vec2(1.0, 0.0),
            vec2(0.0, 1.0), vec2(1.0, 0.0), vec2(0.0, 0.0)
        );

        uniform vec2 u_screenSize;
        uniform vec4 u_posTransform;

        out vec2 v_tex;
        flat out uint v_layer;

        out vec2 v_centerPixels;  // NEU: Zentrum in echten Pixeln
        out float v_radiusPixels; // NEU: Radius als einzelner float in Pixeln

        // Hilfsfunktion, um die echten Pixelkoordinaten (Y-oben) zu berechnen
        vec2 toPixels(vec2 p0, vec2 offset1, vec2 scale1, vec2 offset2, vec2 scale2) {
            vec2 p1 = offset1 + (scale1 * p0);
            vec2 p2 = offset2 + (scale2 * p1);
            return p2; // Das sind die Koordinaten in Pixeln (Y=0 oben)
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

            // 1. Berechne Position in Pixeln (Y=0 oben)
            vec2 pixelPos = toPixels(a_pos, off1, scl1, off2, scl2);

            // 2. In NDC für gl_Position umwandeln
            gl_Position = vec4(toNDC(pixelPos, u_screenSize), 0.0, 1.0);

            v_tex = a_tex;
            v_layer = a_instanceLayer;

            // 3. Berechne das Zentrum in Pixeln (Y=0 oben)
            v_centerPixels = toPixels(vec2(0.0, 0.0), off1, scl1, off2, scl2);

            // 4. Skaliere den Radius mit deiner globalen Transformation (scl2.x)
            v_radiusPixels = a_instanceRadius.x * scl2.x;
        }
    )";

    // Fragment Shader Source
    const std::string fs = R"(

        out vec4 fragColor;

        uniform vec2 u_screenSize;
        uniform sampler2DArray u_tex2DArray;

        in vec2 v_tex;
        flat in uint v_layer;

        in vec2 v_centerPixels;  // Zentrum in Pixeln (Y=0 oben)
        in float v_radiusPixels; // Radius in Pixeln

        void main() {
            // 1. Texture-Farbe holen
            vec3 texCoord = vec3(v_tex.x, v_tex.y, float(v_layer));
            fragColor = texture(u_tex2DArray, texCoord);

            // 2. gl_FragCoord anpassen (Y umdrehen, da gl_FragCoord Y=0 unten hat)
            vec2 currentPixel = vec2(gl_FragCoord.x,
                                      u_screenSize.y - gl_FragCoord.y);

            // 3. Distanz im Pixelraum messen
            float dist = length(currentPixel - v_centerPixels);

            // 4. Antialiasing (1 Pixel weiche Kante, damit der Kreis perfekt glatt aussieht)
            float edge = 1.0;
            float alpha = 1.0 - smoothstep(v_radiusPixels - edge, v_radiusPixels, dist);

            // Alpha-Wert multiplizieren statt 'discard' zu nutzen (bessere Performance)
            fragColor.a *= alpha;
        }

    )";

    // // Kreis mit smoothstep für weiche/harte Kanten definieren
    // float circle = 1.0 - smoothstep(radius - edge_softness,
    //                                 radius + edge_softness, dist);

    // de::gpu::State state;
    // state.blend = de::gpu::Blend::alphaBlend();
    // //state.scissor = de::gpu::Scissor::enabled(scissor);
    // m_driver->setState(state);

    m_shader = m_driver->createShader( shaderName, vs, fs );
    m_u_screenSize = glGetUniformLocation(m_shader->id, "u_screenSize");
    m_u_tex2DArray = glGetUniformLocation(m_shader->id, "u_tex2DArray");
    m_u_posTransform = glGetUniformLocation(m_shader->id, "u_posTransform");
}

void H3_Chip2DRenderer::render(const glm::vec4& posTransform, const de::Recti& scissor)
{
    if (!m_driver) { DE_ERROR("No driver") return; }
    if (!m_shader) { DE_ERROR("No shader") return; }
    if (!m_chipTexture) { DE_ERROR("No chipTexture") return; }
    if (!m_vao) { DE_ERROR("No vao") return; }
    if (m_instancePos.empty()) { /* DE_ERROR("No instances") */ return; }

    // [RenderStates]
    de::gpu::State state;
    state.culling = de::gpu::Culling::disabled();
    state.depth = de::gpu::Depth::disabled();
    state.blend = de::gpu::Blend::alphaBlend();
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
    // int unit = m_driver->useTexture( diffuseMap.tex, 0 );
    glBindTextureUnit(0, m_chipTexture->id);
    glUniform1i(m_u_tex2DArray, 0);

    // [VAO] Quads:
    glBindVertexArray(m_vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, m_instancePos.size());
    glBindVertexArray(0);

    // [CleanUp]
    // glBindTexture( GL_TEXTURE_2D, 0 );
    GL_VALIDATE;
}

void
H3_Chip2DRenderer::collectAndUploadInstances(const H3_Game& game, const H3_Cfg& cfg)
{
    collectInstances( game, cfg );
    uploadInstances();
}

// glm::vec2 tileSize(100,100);
void
H3_Chip2DRenderer::collectInstances(const H3_Game& game, const H3_Cfg& cfg)
{
    m_instancePos.clear();
    m_instanceRadius.clear();
    m_instanceLayer.clear();

    const size_t n = cfg.tiles.size();
    m_instancePos.reserve(n);
    m_instanceRadius.reserve(n);
    m_instanceLayer.reserve(n);

    for (const H3_CfgTile& tile : cfg.tiles)
    {
        // [Instance] a_instanceLayer
        uint8_t layer = H3_ChipTexture::eTypeCount;

        if (tile.tt().isResource())
        {
            switch( tile.diceValue() )
            {
            case 2:  layer = H3_ChipTexture::Chip2; break;
            case 3:  layer = H3_ChipTexture::Chip3; break;
            case 4:  layer = H3_ChipTexture::Chip4; break;
            case 5:  layer = H3_ChipTexture::Chip5; break;
            case 6:  layer = H3_ChipTexture::Chip6; break;
            case 8:  layer = H3_ChipTexture::Chip8; break;
            case 9:  layer = H3_ChipTexture::Chip9; break;
            case 10: layer = H3_ChipTexture::Chip10; break;
            case 11: layer = H3_ChipTexture::Chip11; break;
            case 12: layer = H3_ChipTexture::Chip12; break;
            default: break;
            }
        }
        else if (tile.tt().isPort())
        {
            switch( tile.tt() )
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

        if (layer == H3_ChipTexture::eTypeCount)
        {
            continue; // Skip
        }

        // [Instance] a_instancePos
        glm::vec4 pos;
        pos.x = tile.x();
        pos.y = tile.y();
        pos.z = H3_chipSize.x;
        pos.w = H3_chipSize.y;
        m_instancePos.emplace_back( pos );

        // [Instance] a_instanceRadius
        m_instanceRadius.emplace_back( glm::vec2(H3_chipSize) * 0.5f );

        // [Instance] a_instanceLayer
        m_instanceLayer.emplace_back( layer );

    }

    //DE_ERROR("Collected ", m_instancePos.size(), " instances")
}

void H3_Chip2DRenderer::uploadInstances()
{
    if (m_instancePos.empty()) { return; }

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instancePos);
    glBufferData(GL_ARRAY_BUFFER,
                    m_instancePos.size() * sizeof(glm::vec4),
                    m_instancePos.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceRadius);
    glBufferData(GL_ARRAY_BUFFER,
                    m_instanceRadius.size() * sizeof(glm::vec2),
                    m_instanceRadius.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_instanceLayer);
    glBufferData(GL_ARRAY_BUFFER,
                    m_instanceLayer.size() * sizeof(uint8_t),
                    m_instanceLayer.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
