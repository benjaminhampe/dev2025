#include "UI_MainMenu.h"
#include <H3/H3_Game.h>
#include <de/gui/Label.h>
#include <de/gui/Button.h>

// =====================================================================
UI_MainMenu::UI_MainMenu( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& pos )
// =====================================================================
    : de::gui::Panel(env, parent, id, pos)
    , m_game(game)
{
    auto scr_w = m_env->getScreenWidth();
    auto scr_h = m_env->getScreenHeight();
    auto h3tex = m_game->getTex(H3_Tex::Scroll, __func__);
    if (h3tex.tex)
    {
        DE_OK("Got tex ", h3tex.str())
    }
    else
    {
        DE_ERROR("No tex")
    }
    int pw = 400;
    int ph = scr_h;
    m_rect = de::Recti(0, 0, pw, ph);
    setTexture( h3tex );
    setFillColor( 0xffffffff );

    int bw = 0.60 * pw;
    int bh = 0.1 * ph;
    m_btnNewGame = createButton("New Game", 0.23 * pw, 0.13 * ph, bw, bh);
    m_btnRestart = createButton("Restart", 0.21 * pw, 0.28 * ph, bw, bh);
    m_btnRestart->setVisible(false);
    m_btnOptions = createButton("Options", 0.19 * pw, 0.43 * ph, bw, bh);
    m_btnHelp = createButton("Help", 0.17 * pw, 0.58 * ph, bw, bh);
    m_btnExit = createButton("Exit", 0.15 * pw, 0.73 * ph, bw, bh);

    m_btnNewGame->setOnClicked([&](){ m_game->newGame(); });
    m_btnRestart->setOnClicked([&](){ m_game->restartGame(); });
    m_btnOptions->setOnClicked([](){
        H3_MessageBox("Options", "Options are not implemented so yet.\n"
                                 "Try tinkering with xml config files");
    });
    m_btnHelp->setOnClicked([&](){ m_game->toggleHelpWindow(); });
    m_btnExit->setOnClicked([&](){ m_game->exitProgram(); });

    setDrawChildren(true);
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

void UI_MainMenu::resize(int x, int y, int w, int h)
{
    DE_DEBUG("Resize(",x,",",y,",",w,",",h,")")
    m_rect = de::Recti(x, y, w, h);

    int bw = 0.60 * w;
    int bh = 0.1 * h;
    m_btnNewGame->resize(0.23 * w, 0.13 * h, bw, bh);
    m_btnRestart->resize(0.21 * w, 0.28 * h, bw, bh);
    m_btnOptions->resize(0.19 * w, 0.43 * h, bw, bh);
    m_btnHelp->resize(0.17 * w, 0.58 * h, bw, bh);
    m_btnExit->resize(0.15 * w, 0.73 * h, bw, bh);
}

// static
de::gui::Button*
UI_MainMenu::createButton(std::string txt, int x, int y, int w, int h )
{
    // const int px = parent->getAbsolutePos().x;
    // const int py = parent->getAbsolutePos().y;
    // const int pw = getSize().w;
    // const int ph = getSize().h;
    de::Recti rect(x, y, w, h);
    auto btn = new de::gui::Button(txt, m_env, this, 0, rect);

    auto h3tex = m_game->getTex(H3_Tex::Scroll1, __func__);
    if (h3tex.tex)
    {
        DE_OK("Got tex ", h3tex.str())
    }
    else
    {
        DE_ERROR("No tex")
    }

    btn->setTexture(h3tex);
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

    de::Font menuFont("Postamt", 32);
    btn->setFont(menuFont);
    return btn;
}
