#include "UI_WinnerScreen.h"
#include <H3/H3_Game.h>

// =====================================================================
UI_WinnerScreen::UI_WinnerScreen( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& pos )
    // =====================================================================
    : de::gui::Panel(env, parent, id, pos)
    , m_game(game)
    , m_btnClose(nullptr)
{
    auto texPanel = game->getTex(H3_Tex::Scroll, __func__);
    auto texClose = game->getTex(H3_Tex::Scroll1, __func__);

    setTexture( texPanel );
    setFillColor( 0xffffffff );

    m_btnClose = createButton("Return to Menu", texClose, de::Recti(0,0,0,0) );
    m_btnClose->setOnClicked( [&](){ m_game->enterStartMenu(); } );

    setDrawChildren(false);
}

UI_WinnerScreen::~UI_WinnerScreen()
{

}

void UI_WinnerScreen::updateData()
{

}

void UI_WinnerScreen::draw()
{
    Panel::draw();

    if (!isVisible()) return;

    const float zoom = m_env->getZoom();
    auto driver = m_env->getVideoDriver();
    const int x = m_rect.x;
    const int y = m_rect.y;
    const int w = m_rect.w;
    const int h = m_rect.h;

    // --- Determine Winner ---
    auto & players = m_game->m_players;

    uint32_t winnerId = 0;

    int maxPoints = 0;
    for (int i = 0; i < players.size(); ++i)
    {
        const H3_Player & player = players[i];
        if (maxPoints < player.state.victoryPoints)
        {
            maxPoints = player.state.victoryPoints;
            winnerId = player.id;
        }
    }

    const H3_Player & winner = m_game->getPlayer(winnerId);

    // --- Draw Winner Title ---

    int tx = x + w/2;
    int ty = zoom * 50;
    de::Font titleFont("Postamt", zoom * 32);

    auto s = dbStr("Winner is ",winner.name," with ",maxPoints," points.");
    driver->draw2DText(
        tx,ty,
        s,
        winner.color,
        de::Align::Centered,
        titleFont);

    // --- Draw Winner Icon ---
    int py = y + ty + 20;

    auto ico = m_game->getTex((H3_Tex::eID)winner.avatar, "WinnerScreen");
    int ico_w = zoom * ico.w();
    int ico_h = zoom * ico.h();
    driver->getScreenRenderer()->draw2DRect(
        de::Recti(x+(w-ico_w)/2, py, ico_w, ico_h),
        0xFFFFFFFF,
        ico);

    py += ico_h + zoom * 10;

    int cw = zoom * 32;
    int ch = cw;
    int cx = x+(w-cw)/2;
    int cy = py;

    driver->getScreenRenderer()->draw2DCircle(
        de::Recti(cx, cy, cw, ch),
        winner.color
    );

    de::Font pointsFont("Postamt", zoom * 20);
    driver->getFontRenderer()->draw2DText(
        cx + cw/2, cy + ch/2,
        dbStr(maxPoints),
        0xFF000000,
        de::Align::Centered,
        pointsFont,
        winner.color
    );

    m_btnClose->draw();
}

void UI_WinnerScreen::resize(int X, int Y, int W, int H)
{
    m_rect = de::Recti(X,Y,W,H);

    const float zoom = m_env->getZoom();
    setFont( de::Font("Postamt", zoom * 16) );

    // ::::: Panel ::::
    int w = W - 200;
    int h = H - 200;
    int x = 100;
    int y = 100;

    // m_lblWinner->resize(100,100,w - 200,100);
    // m_lblWinnerIcon->resize(100,200,100,100);
    // m_lblWinnerName->resize(200,200,w-200,100);
    m_btnClose->resize(100, y+m_rect.h-100, m_rect.w-200, 64);
}

// static
de::gui::Button*
UI_WinnerScreen::createButton(std::string txt, de::gpu::TexRef ref, de::Recti pos)
{
    auto btn = new de::gui::Button(txt, m_env, this, 0, pos);
    btn->setRadius( 8 );
    // btn->setFillColor(dbRGB(255,255,255,200),0);
    // btn->setTextColor(dbRGB(255,255,255),1);
    btn->setTexture( ref );
    return btn;
}
