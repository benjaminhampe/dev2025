#pragma once
#include "H3_Animation.h"

// =====================================================================
class H3_Dice2DAnimation : public H3_Animation
// =====================================================================
{
    H3_Game* m_game;
    glm::vec2 m_posBegDice1;
    glm::vec2 m_posEndDice1;
    glm::vec2 m_posBegDice2;
    glm::vec2 m_posEndDice2;
    float m_rotyDice1;
    float m_rotyDice2;
    float m_sizeAttack;
    float m_sizeSustain;
    double m_durationAttack;
    double m_durationSustain;
    double m_durationRelease;
    double m_timeNow;
    de::smesh::SMeshBuffer m_mesh;

public:
    H3_Dice2DAnimation(H3_Game* game);
    ~H3_Dice2DAnimation() override;

    bool isAnimating() const override;
    void draw(double dt) override;
};
