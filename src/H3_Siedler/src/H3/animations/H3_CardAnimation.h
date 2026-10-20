#pragma once
#include "H3_Animation.h"

// =====================================================================
class H3_CardAnimation : public H3_Animation
// =====================================================================
{
    H3_Game* m_game;
    glm::vec3 m_worldPos;
    glm::vec2 m_mousePos;
    int m_tileType;
    double m_duration;
    double m_timeNow;
    de::smesh::SMeshBuffer m_mesh;

public:
    H3_CardAnimation(
        H3_Game* game,
        glm::vec3 worldPos,
        glm::vec2 mousePos,
        int tileType,
        double duration);

    ~H3_CardAnimation() override;

    bool isAnimating() const override;

    void draw(double dt) override;
};
