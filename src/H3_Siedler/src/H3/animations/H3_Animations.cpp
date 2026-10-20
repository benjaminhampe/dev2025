#include "H3_Animations.h"
#include <H3/H3_Game.h>

// =====================================================================
H3_Animations::H3_Animations()
// =====================================================================
    : m_game{ nullptr }
{
}

H3_Animations::~H3_Animations()
{
    clearAnimations();
}

void H3_Animations::init(H3_Game* game)
{
    m_game = game;
    restartTimer();
}

void H3_Animations::restartTimer()
{
    m_timeStart = dbTimeInSeconds();
    m_timeNow = 0.0;
    m_timeLastDraw = 0.0;
}

void H3_Animations::draw()
{
    if ( !m_game )
    {
        DE_ERROR("No game")
        return;
    }

    m_timeNow = dbTimeInSeconds() - m_timeStart;

    double dt = m_timeNow - m_timeLastDraw;

    for (H3_Animation* ani : m_queue)
    {
        if (ani) ani->draw(dt);
    }

    // This shifts valid items left, then erases the rest from the back
    m_queue.erase( std::remove_if(m_queue.begin(), m_queue.end(),
        [](H3_Animation* ani)
        {
            if (!ani) return true;
            return !ani->isAnimating();
        }
    ), m_queue.end());

    m_timeLastDraw = m_timeNow;
}

void H3_Animations::addAnimation(H3_Animation* ani)
{
    m_queue.push_back(ani);
}

void H3_Animations::clearAnimations()
{
    for (H3_Animation* ani : m_queue)
    {
        if (ani) delete ani;
    }
    m_queue.clear();
}
