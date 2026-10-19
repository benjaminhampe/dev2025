#include "UI_MainMenu.h"
#include <H3/H3_Game.h>
#include <de/gui/Label.h>
#include <de/gui/Button.h>

// =====================================================================
UI_MainMenu::UI_MainMenu( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& pos )
// =====================================================================
    : de::gui::Panel(env, parent, id, pos)
    , m_game(game)
    , m_forceResize{ true }
    , m_uiHelpWindow(nullptr)
{
    setDrawChildren(true);

    auto tex = m_game->getTex(H3_Tex::Scroll, __func__);

    setTexture( tex );
    setFillColor( 0xffffffff );

    m_btnNewGame = createButton("New Game");
    m_btnRestart = createButton("Restart");
    m_btnRestart->setVisible(false);
    m_btnOptions = createButton("Options");
    m_btnHelp = createButton("Help");
    m_btnExit = createButton("Exit");

    m_btnNewGame->onClicked = [&](){ m_game->newGame(); };
    m_btnRestart->onClicked = [&](){ m_game->restartGame(); };
    m_btnOptions->onClicked = [](){
        H3_MessageBox("Options", "Options are not implemented so yet.\n"
                                 "Try tinkering with xml config files");
    };
    m_btnHelp->onClicked = [&](){ m_game->toggleHelpWindow(); };
    m_btnExit->onClicked = [&](){ m_game->exitProgram(); };
}

UI_MainMenu::~UI_MainMenu()
{

}

/*
void UI_MainMenu::draw()
{
    Panel::draw();

    if (!isVisible()) return;
    m_btnNewGame->draw();
    m_btnRestart->draw();
    m_btnOptions->draw();
    m_btnHelp->draw();
    m_btnExit->draw();
}
*/

void UI_MainMenu::resize(int X, int Y, int W, int H)
{
    // DE_DEBUG("Resize(",X,",",Y,",",W,",",H,")")
    // DE_DEBUG("Resize(",x,",",y,",",w,",",h,")")

    float zoom = m_env->getZoom();
    int x = X + zoom * 50;
    int y = Y + zoom * 50;
    int w = std::min<int>(W, zoom * 400);
    int h = std::min<int>(H, H/2 + (zoom * 100));
    m_rect = de::Recti(x, y, w, h);

    int bw = 0.60 * w;
    int bh = 0.1 * h;
    m_btnNewGame->resize(0.23 * w, 0.13 * h, bw, bh);
    m_btnRestart->resize(0.21 * w, 0.28 * h, bw, bh);
    m_btnOptions->resize(0.19 * w, 0.43 * h, bw, bh);
    m_btnHelp->resize(0.17 * w, 0.58 * h, bw, bh);
    m_btnExit->resize(0.15 * w, 0.73 * h, bw, bh);

    // if (m_uiHelpWindow && m_uiHelpWindow->isVisible())
    // {
    //     m_uiHelpWindow->resize(scr_w / 6, 50, 4*scr_w / 6, scr_h - 100);
    // }
}

// static
de::gui::Button*
UI_MainMenu::createButton(std::string txt)
{
    auto r = de::Recti(0,0,0,0);
    auto btn = new de::gui::Button(txt, m_env, this, 0, r);

    auto tex = m_game->getTex(H3_Tex::Scroll1, __func__);
    btn->setTexture(tex);

    auto fnt = de::Font("Postamt", 32);
    btn->setFont(fnt);

    btn->setFillColor(dbRGB(255,255,255,200),eWS_Idle);
    btn->setFillColor(dbRGB(255,255,255,255),eWS_Hovered);
    btn->setFillColor(dbRGB(255,255,255,255), eWS_Pressed);
    btn->setFillColor(dbRGB(255,255,255,255), eWS_Released);

    btn->setTextColor(dbRGB(0,0,0,150), dbRGB(255,255,255,100), eWS_Idle);
    btn->setTextColor(dbRGB(0,0,0,255), dbRGB(255,255,255,150), eWS_Hovered);
    btn->setTextColor(dbRGB(0,0,0,255), dbRGB(255,255,255,200), eWS_Pressed);
    btn->setTextColor(dbRGB(0,0,0,255), dbRGB(255,255,255,200), eWS_Released);

    btn->setBorderColor(dbRGB(0,0,0,255), eWS_Idle);
    btn->setBorderColor(dbRGB(0,0,0,255), eWS_Hovered);
    btn->setBorderColor(dbRGB(0,0,0,255), eWS_Pressed);
    btn->setBorderColor(dbRGB(0,0,0,255), eWS_Released);


    return btn;
}
