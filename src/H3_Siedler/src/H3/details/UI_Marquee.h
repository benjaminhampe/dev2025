#pragma once
#include <H3/H3_Board.h>
#include <de/gui/Widget.h>

#include <stack>

// ===========================================================================
struct UI_MarqueeItem
// ===========================================================================
{
    std::wstring msg;
    uint32_t color = 0xFFFFFFFF;
    int timeDurationInMs = 0; // 0 = display forever until clear()
    std::optional<double> timeStartInSec = 0;
    //bool hasStarted = false;
    de::Font font = de::Font("Garton", 128);
};

// =====================================================================
class UI_Marquee : public de::gui::Widget
// =====================================================================
{
public:
    UI_Marquee( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& r );
    ~UI_Marquee() override;

    void draw() override;

    void clearMarquee();
    void marqueeH1( std::wstring msg, int durationMs, uint32_t color );
    void marqueeH3( std::wstring msg, int durationMs, uint32_t color );

private:
    H3_Game* m_game;

public:

    de::Font fontH1 = de::Font("Postamt", 48);
    de::Font fontH3 = de::Font("Postamt", 24);
    std::deque< UI_MarqueeItem > h1;
    std::deque< UI_MarqueeItem > h3;

};
