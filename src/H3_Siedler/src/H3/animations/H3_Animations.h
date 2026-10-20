#pragma once
#include "H3_Animation.h"
#include "H3_CardAnimation.h"

// =====================================================================
class H3_Animations
// =====================================================================
{
    H3_Game* m_game;
public:
    H3_Animations();
    ~H3_Animations();

    void init(H3_Game* game);
    void draw();

    void addAnimation(H3_Animation* anim);

    void clearAnimations();

    void restartTimer();
private:
    double m_timeStart;
    double m_timeNow;
    double m_timeLastDraw;

    std::deque<H3_Animation*> m_queue; // FIFO
};
