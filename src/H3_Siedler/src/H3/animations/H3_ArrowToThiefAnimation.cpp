#include "H3_ArrowToThiefAnimation.h"
#include <H3/H3_Game.h>

// =====================================================================
H3_ArrowToThiefAnimation::H3_ArrowToThiefAnimation(H3_Game* game)
    : m_game(game)
    , m_posBeg(0.35,0.8)
    , m_duration(2.0)
    , m_timeNow(0.0)
{

}

H3_ArrowToThiefAnimation::~H3_ArrowToThiefAnimation()
{
    m_mesh.destroy();
}

bool H3_ArrowToThiefAnimation::isAnimating() const
{
    return m_timeNow < m_duration;
}

void H3_ArrowToThiefAnimation::draw(double dt)
{
    auto driver = m_game->getDriver();
    const int scr_w = driver->getScreenWidth();
    const int scr_h = driver->getScreenHeight();


    const auto & m = m_game->m_thief.trs.modelMat;
    glm::dvec3 screenPos;

    bool bVisible = driver->getCamera()->
                        projectToScreen(screenPos, m, glm::dvec3(0,30,0));




/*
 // Fetch Matrices
    const auto & m = m_game->m_thief.trs.modelMat;
    const auto & v = driver->getCamera()->getViewMatrix();
    const auto & p = driver->getCamera()->getProjectionMatrix();

    // 1. Project to homogeneous clip coordinates
    // Optional: If (0,0,0) represents the feet, change to a local offset (e.g., glm::dvec4(0, 2.0, 0, 1.0)) to elevate it over their head.
    auto clipPos = (p * (v * m)) * glm::dvec4(0.0, 0.0, 0.0, 1.0);

    // 2. Prevent rendering if the thief is behind the camera viewport
    DE_DEBUG("clipPos.w = ",clipPos.w)
    // if (clipPos.w <= 0.0)
    // {
    //     return;
    // }

    // 3. Perspective Divide to Normalized Device Coordinates [-1.0, 1.0]
    glm::dvec3 ndcSpace = glm::dvec3(clipPos) / clipPos.w;

    // 4. Map from [-1, 1] to a [0, 1] UV texture/viewport scale
    auto screenUV = (glm::dvec2(ndcSpace.x, ndcSpace.y) + 1.0) * 0.5;

    // 5. Flip Y axis to match screen-space orientation (0 at top)
    screenUV.y = 1.0 - screenUV.y;

    // 6. Scale up to match physical pixel layout boundaries
    auto screenPos = screenUV * glm::dvec2(scr_w, scr_h);

    // 7. Establish Bounding Rectangle Layout
    const float zoom = m_game->getZoom();
    const int r = zoom * 10;
    auto r_thief = de::Recti(screenPos.x - r, screenPos.y - r, 2 * r, 2 * r);
*/
/*
    const auto & m = m_game->m_thief.trs.modelMat;
    const auto & v = driver->getCamera()->getViewMatrix();
    const auto & p = driver->getCamera()->getProjectionMatrix();

    auto clipPos = (p * (v * m)) * glm::dvec4(0,100,0,1);

    clipPos = glm::dvec4( glm::dvec3(clipPos) / clipPos.w, 1.0);
    auto ndcPos = glm::dvec2( clipPos.x, clipPos.y ) * 2.0 + 1.0;
    ndcPos.y = 1.0 - ndcPos.y;
    auto screenPos = ndcPos * glm::dvec2(scr_w, scr_h);

    const float zoom = m_game->getZoom();
    const int r = zoom * 10;
    auto r_thief = de::Recti(screenPos.x - r, screenPos.y - r, 2*r, 2*r);
*/

    if (bVisible)
    {
        // Draw circle above thief
        const float zoom = m_game->getZoom();
        const int r = zoom * 10;
        auto r_thief = de::Recti(screenPos.x - r, screenPos.y - r, 2 * r, 2 * r);

        driver->getScreenRenderer()->draw2DCircle(
                    r_thief,
                    dbRGB(255,128,20,225),
                    de::gpu::TexRef(),
                    36);


        // Draw line from 2d placement icon to 3d thief
        int x1 = m_posBeg.x * scr_w;
        int y1 = m_posBeg.y * scr_h;
        int x2 = screenPos.x;
        int y2 = screenPos.y;

        driver->getScreenRenderer()->draw2DLine(
                    x1,y1,x2,y2,
                    dbRGB(255,20,20,225),
                    dbRGB(255,128,20,225));


    }



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
