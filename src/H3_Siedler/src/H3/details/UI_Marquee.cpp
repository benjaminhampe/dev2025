#include "UI_Marquee.h"
#include <H3/H3_Game.h>

// =====================================================================
UI_Marquee::UI_Marquee( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& pos )
// =====================================================================
    : de::gui::Widget(env, parent, id, pos)
    , m_game(game)
{
    setDrawChildren(false);
}

UI_Marquee::~UI_Marquee()
{

}

void UI_Marquee::clearMarquee()
{
    h1.clear();
    h3.clear();
}

void UI_Marquee::marqueeH1( std::wstring msg, int durationMs, uint32_t color )
{
    UI_MarqueeItem item;
    item.msg = msg;
    item.timeDurationInMs = durationMs;
    item.timeStartInSec = std::nullopt;
    item.color = color;
    item.font = fontH1;
    h1.push_back( std::move( item ) );
}

void UI_Marquee::marqueeH3( std::wstring msg, int durationMs, uint32_t color )
{
    UI_MarqueeItem item;
    item.msg = msg;
    item.timeDurationInMs = durationMs;
    item.timeStartInSec = std::nullopt;
    item.color = color;
    item.font = fontH3;
    h3.push_back( std::move( item ) );
}


void UI_Marquee::draw()
{
    if (!isVisible()) return;

    auto driver = m_env->getVideoDriver();
    if (!driver) { DE_ERROR("No driver") return; }

    const int w = driver->getScreenWidth();
    const int h = driver->getScreenHeight();
    const double t = dbTimeInSeconds();

    const float zoom = m_env->getZoom();
    const auto fontSizeH1 = zoom * fontH1.pixelSize;
    const auto fontSizeH3 = zoom * fontH3.pixelSize;

    const auto align = de::Align::TopCenter;

    // === H1 ===
    if (!h1.empty())
    {
        auto & item = h1.front();

        if (!item.timeStartInSec)
        {
            item.timeStartInSec = t;
        }

        // Draw marquee when duration = 0, or timeout not reached.
        if (item.timeDurationInMs < 1 || ((t - item.timeStartInSec.value()) <= (0.001 * item.timeDurationInMs)))
        {
            auto font = fontH1;
            font.pixelSize = fontSizeH1;
            const int x = w / 2;
            const int y = h / 20;

            driver->draw2DText( x, y, item.msg,item.color, align,font,0xA0000000,10 );
        }
        else
        {
            h1.pop_front();
        }
    }

    // === H3 ===
    if (!h3.empty())
    {
        auto & item = h3.front();

        if (!item.timeStartInSec)
        {
            item.timeStartInSec = t;
        }

        // Draw marquee when duration = 0, or timeout not reached.
        if (item.timeDurationInMs < 1 || ((t - item.timeStartInSec.value()) <= (0.001 * item.timeDurationInMs)))
        {
            de::Font font = fontH3;
            font.pixelSize = fontSizeH3;
            const int x = w / 2;
            const int y = h / 20 + fontSizeH1 + 20;
            driver->draw2DText( x, y, item.msg, item.color, align, font, 0xA0000000, 10 );
        }
        else
        {
            h3.pop_front();
        }
    }
}







/*

void H3_Game::clearMarquee()
{
    m_uiMarquee.h1.clear();
    m_uiMarquee.h3.clear();
}

void H3_Game::marqueeH1( std::wstring msg, int durationMs, uint32_t color )
{
    UI_MarqueeItem item;
    item.msg = msg;
    item.timeDurationInMs = durationMs;
    item.timeStartInSec = std::nullopt;
    item.color = color;
    item.font = m_uiMarquee.fontH1;
    m_uiMarquee.h1.push_back( std::move( item ) );
}

void H3_Game::marqueeH3( std::wstring msg, int durationMs, uint32_t color )
{
    UI_MarqueeItem item;
    item.msg = msg;
    item.timeDurationInMs = durationMs;
    item.timeStartInSec = std::nullopt;
    item.color = color;
    item.font = m_uiMarquee.fontH3;
    m_uiMarquee.h3.push_back( std::move( item ) );
}

void H3_Game::drawMarquee()
{
    auto driver = getDriver();
    if (!driver)
    {
        DE_ERROR("No driver")
        return;
    }
    const int w = driver->getScreenWidth();
    const int h = driver->getScreenHeight();
    const double t = dbTimeInSeconds();

    const auto fontSizeH1 = getZoom() * m_uiMarquee.fontH1.pixelSize;
    const auto fontSizeH3 = getZoom() * m_uiMarquee.fontH3.pixelSize;

    const auto align = de::Align::TopCenter;

    // === H1 ===
    if (!m_uiMarquee.h1.empty())
    {
        auto & item = m_uiMarquee.h1.front();

        if (!item.timeStartInSec)
        {
            item.timeStartInSec = t;
        }

        // Draw marquee when duration = 0, or timeout not reached.
        if (item.timeDurationInMs < 1 || ((t - item.timeStartInSec.value()) <= (0.001 * item.timeDurationInMs)))
        {
            auto font = m_uiMarquee.fontH1;
            font.pixelSize = fontSizeH1;
            const int x = w / 2;
            const int y = h / 20;

            driver->draw2DText( x, y, item.msg,item.color, align,font,0xA0000000,10 );
        }
        else
        {
            m_uiMarquee.h1.pop_front();
        }
    }

    // === H3 ===
    if (!m_uiMarquee.h3.empty())
    {
        auto & item = m_uiMarquee.h3.front();

        if (!item.timeStartInSec)
        {
            item.timeStartInSec = t;
        }

        // Draw marquee when duration = 0, or timeout not reached.
        if (item.timeDurationInMs < 1 || ((t - item.timeStartInSec.value()) <= (0.001 * item.timeDurationInMs)))
        {
            de::Font font = m_uiMarquee.fontH3;
            font.pixelSize = fontSizeH3;
            const int x = w / 2;
            const int y = h / 20 + fontSizeH1 + 20;
            driver->draw2DText( x, y, item.msg, item.color, align, font, 0xA0000000, 10 );
        }
        else
        {
            m_uiMarquee.h3.pop_front();
        }
    }
}

*/
