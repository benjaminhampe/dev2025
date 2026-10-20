#include "H3_CardAnimation.h"
#include <H3/H3_Game.h>

// =====================================================================
H3_CardAnimation::H3_CardAnimation(
        H3_Game* game,
        glm::vec3 worldPos,
        glm::vec2 mousePos,
        int tileType,
        double duration)
    : m_game(game)
    , m_worldPos(worldPos)
    , m_mousePos(mousePos)
    , m_tileType(tileType)
    , m_duration(duration)
    , m_timeNow(0.0)
{
    de::smesh::SMeshRoundRect::addXZ(m_mesh,{0,0,0},24,40,4,4,23);
    de::smesh::SMeshBufferTool::centerVertices(m_mesh);

    de::gpu::TexRef tex;
    switch(tileType)
    {
        case H3_TileType::TileA: tex = game->getTex(H3_Tex::CardA, __func__); break;
        case H3_TileType::TileB: tex = game->getTex(H3_Tex::CardB, __func__); break;
        case H3_TileType::TileC: tex = game->getTex(H3_Tex::CardC, __func__); break;
        case H3_TileType::TileD: tex = game->getTex(H3_Tex::CardD, __func__); break;
        case H3_TileType::TileE: tex = game->getTex(H3_Tex::CardE, __func__); break;
        default: break;
    }
    m_mesh.setTexture(0,tex);
    m_mesh.upload();
}

H3_CardAnimation::~H3_CardAnimation()
{
    m_mesh.destroy();
}

bool H3_CardAnimation::isAnimating() const
{
    return m_timeNow < m_duration;
}

#if 0
glm::vec3
getPointOnNearPlane(double mouseX, double mouseY,
                            int screenWidth, int screenHeight,
                            const glm::mat4& myViewMatrix,
                            const glm::mat4& myProjMatrix)
{
    // 1. Convert Screen space to NDC Space (-1.0 to 1.0)
    // Flip Y because screen space (0,0) is top-left, NDC (0,0) is center.
    float ndcX = (2.0f * static_cast<float>(mouseX)) / static_cast<float>(screenWidth) - 1.0f;
    float ndcY = 1.0f - (2.0f * static_cast<float>(mouseY)) / static_cast<float>(screenHeight);

    // 2. Set Z to your matrix format's near plane (-1.0f or 0.0f)
    float nearPlaneNDC = -1.0f; // Change to 0.0f for Vulkan/DirectX/Reverse-Z

    // 3. Unproject back to World Space
    glm::mat4 invViewProj = glm::inverse(myProjMatrix * myViewMatrix);
    glm::vec4 destinationWorld4 = invViewProj * glm::vec4(ndcX, ndcY, nearPlaneNDC, 1.0f);

    // 4. Perspective Divide to get a clean 3D World position
    return glm::vec3(destinationWorld4) / destinationWorld4.w;
}

#else

glm::vec3
getPointOnNearPlane(
    float mouseX,
    float mouseY,
    int screenWidth,
    int screenHeight,
    const glm::mat4& myViewMatrix,
    const glm::mat4& myProjMatrix)
{
    float mx = float(mouseX) / float(screenWidth);
    float my = float(mouseY) / float(screenHeight);

    // 1. Convert Screen space to NDC Space (-1.0 to 1.0)
    // Map X from [0, width] to [-1, 1]
    float ndcX = ((2.0f * mx) - 1.0f);

    // Map Y from [0, height] to [-1, 1] WITHOUT flipping it via (1.0f - ...)
    // This perfectly matches your orthoLH(y, y+h) boundary logic.
    float ndcY = ((2.0f * my) - 1.0f);

    // 2. Set Z to 0.0f because you are using Left-Handed (LH) projections
    float nearPlaneNDC = 0.0f;

    // 3. Unproject back to World Space using your inverted VP matrix
    glm::mat4 invViewProj = glm::inverse(myProjMatrix * myViewMatrix);
    glm::vec4 destinationWorld4 = invViewProj * glm::vec4(ndcX, ndcY, nearPlaneNDC, 1.0f);

    // 4. Perspective Divide to get the final 3D position
    return glm::vec3(destinationWorld4) / destinationWorld4.w;
}
#endif

void H3_CardAnimation::draw(double dt)
{
    auto driver = m_game->getDriver();

    double t = m_timeNow / m_duration;

    auto A = m_worldPos;

#if 0
    auto ray = driver->getCamera()->computeRayf(m_mousePos.x, m_mousePos.y);
    auto B = ray.getPos();

#else
    auto camera = driver->getCamera();
    auto B = getPointOnNearPlane(
        m_mousePos.x,
        m_mousePos.y,
        driver->getScreenWidth(),
        driver->getScreenHeight(),
        camera->getViewMatrix(),
        camera->getProjectionMatrix());
#endif

    de::TRSf trs;
    trs.pos = (B - A) * float(t);
    trs.update();

    driver->setModelMatrix(trs.modelMat);

    driver->getSMaterialRenderer()->draw3D(m_mesh);

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
