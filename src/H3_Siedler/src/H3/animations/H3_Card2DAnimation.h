#pragma once
#include "H3_Animation.h"

// =====================================================================
class H3_Card2DAnimation : public H3_Animation
// =====================================================================
{
    H3_Game* m_game;
    glm::vec2 m_posBeg;
    glm::vec2 m_posEnd;
    int m_tileType;
    double m_duration;
    double m_timeNow;
    de::gpu::TexRef m_tex;
    de::smesh::SMeshBuffer m_mesh;

public:
    H3_Card2DAnimation( H3_Game* game, glm::vec2 posBeg, glm::vec2 posEnd, int tileType);

    ~H3_Card2DAnimation() override;

    bool isAnimating() const override;

    void draw(double dt) override;
};
