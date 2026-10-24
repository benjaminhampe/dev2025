#include "H3_Dice2DAnimation.h"
#include <H3/H3_Game.h>

// =====================================================================
H3_Dice2DAnimation::H3_Dice2DAnimation(H3_Game* game)
    : m_game(game)
    , m_posBegDice1(0.8,0.8)
    , m_posBegDice2(0.9,0.8)
    , m_posEndDice1(0.45,0.5)
    , m_posEndDice2(0.50,0.48)
    , m_rotyDice1(715.0f)
    , m_rotyDice2(730.0f)
    , m_sizeAttack(1.0f)
    , m_sizeSustain(3.0f)
    , m_durationAttack(0.5)
    , m_durationSustain(1.0)
    , m_durationRelease(0.5)
    , m_timeNow(0.0)
{

}

H3_Dice2DAnimation::~H3_Dice2DAnimation()
{
    m_mesh.destroy();
}

bool H3_Dice2DAnimation::isAnimating() const
{
    return m_timeNow <
        (m_durationAttack + m_durationSustain + m_durationRelease);
}

void H3_Dice2DAnimation::draw(double dt)
{
    auto driver = m_game->getDriver();
    const int scr_w = driver->getScreenWidth();
    const int scr_h = driver->getScreenHeight();

    //const float zoom = m_env->getZoom();

    //double t = m_timeNow / m_duration;

    //auto A = m_worldPos;

    //const int r = zoom * 6;

    const auto & dice = m_game->m_dice;
    const de::gpu::TexRef tex_1 = H3_getDiceTex(*m_game,dice.a);
    const de::gpu::TexRef tex_2 = H3_getDiceTex(*m_game,dice.b);

    const int tex_w = tex_1.w(); // 45px
    //DE_DEBUG("tex_w = ",tex_w)

    // int state = 0;
    float t = 0.0f;
    float s = 1.0f;
     auto r = glm::ivec2{6,6};
    float r1 = 0.0f;
    float r2 = 0.0f;
    float dx = 0.0f;
    float dy = 0.0f;
    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;
    int w = 0;

    de::TRSf trs1;
    de::TRSf trs2;

    if (m_timeNow < m_durationAttack)
    {
        // state = 0; // Attack
        t = m_timeNow / m_durationAttack;   // Relative time
        s = m_sizeAttack + (m_sizeSustain - m_sizeAttack) * t; // scale
        r = glm::vec2{6,6} * s;
        w = s * tex_w;

        dx = (m_posEndDice1.x - m_posBegDice1.x) * t;
        dy = (m_posEndDice1.y - m_posBegDice1.y) * t;
        x1 = (m_posBegDice1.x + dx) * scr_w;
        y1 = (m_posBegDice1.y + dy) * scr_h;

        dx = (m_posEndDice2.x - m_posBegDice2.x) * t;
        dy = (m_posEndDice2.y - m_posBegDice2.y) * t;
        x2 = (m_posBegDice2.x + dx) * scr_w;
        y2 = (m_posBegDice2.y + dy) * scr_h;

        //dx = (m_sizeSustain - m_sizeAttack) * t;
        //w1 = (m_sizeAttack + dx) * tex_w;


        r1 = de::Math::wrapAngle0_360(m_rotyDice1 * t * de::Math::DEG2RAD);
        r2 = de::Math::wrapAngle0_360(m_rotyDice2 * t * de::Math::DEG2RAD);
    }
    else if (m_timeNow < m_durationAttack + m_durationSustain)
    {
        // state = 1; // Sustain
        // t = (m_timeNow - m_durationAttack) / m_durationSustain;
        s = m_sizeSustain;
        r = glm::vec2{6,6} * s;
        w = s * tex_w;

        x1 = m_posEndDice1.x * scr_w;
        y1 = m_posEndDice1.y * scr_h;
        x2 = m_posEndDice2.x * scr_w;
        y2 = m_posEndDice2.y * scr_h;
        //w1 = m_sizeSustain * tex_w;
        //r0 = glm::vec2{6,6} * m_sizeSustain;

        r1 = de::Math::wrapAngle0_360(m_rotyDice1 * de::Math::DEG2RAD);
        r2 = de::Math::wrapAngle0_360(m_rotyDice2 * de::Math::DEG2RAD);
    }
    else // if (m_timeNow < m_durationAttack + m_durationSustain + m_durationRelease)
    {
        // state = 2; // Release
        t = (m_timeNow - m_durationAttack - m_durationSustain) / m_durationRelease;
        s = m_sizeSustain + (m_sizeAttack - m_sizeSustain) * t;
        r = glm::vec2{6,6} * s;
        w = s * tex_w;

        dx = (m_posBegDice1.x - m_posEndDice1.x) * t;
        dy = (m_posBegDice1.y - m_posEndDice1.y) * t;
        x1 = (m_posEndDice1.x + dx) * scr_w;
        y1 = (m_posEndDice1.y + dy) * scr_h;

        dx = (m_posBegDice2.x - m_posEndDice2.x) * t;
        dy = (m_posBegDice2.y - m_posEndDice2.y) * t;
        x2 = (m_posEndDice2.x + dx) * scr_w;
        y2 = (m_posEndDice2.y + dy) * scr_h;

        //dx = (m_sizeAttack - m_sizeSustain) * t;
        //w1 = (m_sizeSustain + dx) * tex_w;
        //r0 = glm::vec2{6,6} * (m_sizeSustain + dx);

        r1 = de::Math::wrapAngle0_360(m_rotyDice1 * (1.0f - t) * de::Math::DEG2RAD);
        r2 = de::Math::wrapAngle0_360(m_rotyDice2 * (1.0f - t) * de::Math::DEG2RAD);
    }

    trs1.reset();
    trs1.rotate.y = r1;
    trs1.update();

    trs2.reset();
    trs2.rotate.y = r2;
    trs2.update();

    driver->setModelMatrix(trs1.modelMat);

    driver->getScreenRenderer()->
        draw2DRoundRect(de::Recti(x1,y1,w,w), r, tex_1);

    driver->setModelMatrix(trs2.modelMat);

    driver->getScreenRenderer()->
        draw2DRoundRect(de::Recti(x2,y2,w,w), r, tex_2);

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
