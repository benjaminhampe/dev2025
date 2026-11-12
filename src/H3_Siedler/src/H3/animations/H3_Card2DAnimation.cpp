#include "H3_Card2DAnimation.h"
#include <H3/H3_Game.h>

// =====================================================================
H3_Card2DAnimation::
H3_Card2DAnimation( H3_Game* game, glm::vec2 posBeg, glm::vec2 posEnd, int tileType)
// =====================================================================
    : m_game(game)
    , m_posBeg(posBeg)
    , m_posEnd(posEnd)
    , m_tileType(tileType)
    , m_duration(2.0)
    , m_timeNow(0.0)
{
    // m_mesh.name = "H3_Card2DAnimation";
    // de::smesh::SMeshRoundRect::addXZ(m_mesh,{0,0,0},24,40,4,4,23);
    // de::smesh::SMeshBufferTool::centerVertices(m_mesh);

    // m_mesh.setTexture(0,m_tex);
    // m_mesh.upload();

    switch (tileType)
    {
        case H3_TileType::TileA: m_tex = game->getTex(H3_Tex::CardA, __func__); break;
        case H3_TileType::TileB: m_tex = game->getTex(H3_Tex::CardB, __func__); break;
        case H3_TileType::TileC: m_tex = game->getTex(H3_Tex::CardC, __func__); break;
        case H3_TileType::TileD: m_tex = game->getTex(H3_Tex::CardD, __func__); break;
        case H3_TileType::TileE: m_tex = game->getTex(H3_Tex::CardE, __func__); break;
        default: break;
    }

}

H3_Card2DAnimation::~H3_Card2DAnimation()
{
    //m_mesh.destroy();
}

bool H3_Card2DAnimation::isAnimating() const
{
    return m_timeNow < m_duration;
}

void H3_Card2DAnimation::draw(double dt)
{
    auto driver = m_game->getDriver();
    auto camera = driver->getCamera();

    double t = m_timeNow / m_duration;

    auto A = m_posBeg;
    auto B = m_posEnd;
    auto C = A + ((B-A) * float(t));
    auto zoom = m_game->getZoom();
    int w = zoom*60;
    int h = zoom*100;
    int r = zoom*6;
    auto rect = de::Recti(C.x - w/2, C.y - h/2, w, h);
    driver->getScreenRenderer()->
        draw2DRoundRect(rect,{r,r},0xFFFFFFFF, m_tex);

    m_timeNow += dt;
}


/*
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
};

Ray getMouseRayDirect(double mouseX, double mouseY,
                     int screenWidth, int screenHeight,
                     const glm::mat4& myViewMatrix,
                     const glm::mat4& myProjMatrix)
{
    // 1. Convert Screen Coordinates to NDC Space (Range -1.0 to 1.0)
    // Flip Y because standard window space is top-left (0,0), but NDC is bottom-left (-1,-1)
    float ndcX = (2.0f * static_cast<float>(mouseX)) / static_cast<float>(screenWidth) - 1.0f;
    float ndcY = 1.0f - (2.0f * static_cast<float>(mouseY)) / static_cast<float>(screenHeight);

    // 2. Combine your custom matrices and invert them to map from Clip space back to World space
    glm::mat4 invViewProj = glm::inverse(myProjMatrix * myViewMatrix);

    // 3. Define homogeneous coordinates for the Near and Far clipping planes
    // Note: If using standard OpenGL depth, near is Z = -1.0 and far is Z = 1.0
    // If using Reverse-Z or Vulkan depth bounds, adjust these accordingly.
    glm::vec4 nearNDC(ndcX, ndcY, -1.0f, 1.0f);
    glm::vec4 farNDC(ndcX, ndcY,  1.0f, 1.0f);

    // 4. Transform back to World Space
    glm::vec4 nearWorld4 = invViewProj * nearNDC;
    glm::vec4 farWorld4  = invViewProj * farNDC;

    // 5. Perspective divide (divide by W) to get concrete 3D positions
    glm::vec3 nearWorld = glm::vec3(nearWorld4) / nearWorld4.w;
    glm::vec3 farWorld  = glm::vec3(farWorld4) / farWorld4.w;

    // 6. Generate the ray
    glm::vec3 rayOrigin    = nearWorld;
    glm::vec3 rayDirection = glm::normalize(farWorld - nearWorld);

    return Ray{ rayOrigin, rayDirection };
}
*/
