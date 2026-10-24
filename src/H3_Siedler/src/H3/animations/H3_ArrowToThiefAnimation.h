#pragma once
#include "H3_Animation.h"

// =====================================================================
class H3_ArrowToThiefAnimation : public H3_Animation
// =====================================================================
{
    H3_Game* m_game;
    glm::vec2 m_posBeg;
    double m_duration;
    double m_timeNow;
    de::smesh::SMeshBuffer m_mesh;

public:
    H3_ArrowToThiefAnimation(H3_Game* game);
    ~H3_ArrowToThiefAnimation() override;

    bool isAnimating() const override;
    void draw(double dt) override;
};
