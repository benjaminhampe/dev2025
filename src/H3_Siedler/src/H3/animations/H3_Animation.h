#pragma once
#include <H3/H3_Board.h>

class H3_Game;

// =====================================================================
class H3_Animation
// =====================================================================
{
public:
    virtual ~H3_Animation() {}

    virtual bool isAnimating() const = 0;

    virtual void draw(double dt) = 0;

    //virtual init(H3_Game* game) = 0;

    //virtual draw(double dt) = 0;
};
