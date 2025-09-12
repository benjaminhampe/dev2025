#include "UI_MainMenu.h"
#include <H3/H3_Game.h>
#include <de/gui/Label.h>
#include <de/gui/Button.h>

// =====================================================================
UI_MainMenu::UI_MainMenu( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& pos )
// =====================================================================
    : de::gui::Widget(env, parent, id, pos)
    , m_game(game)
{
    auto scr_w = m_env->getScreenWidth();
    auto scr_h = m_env->getScreenHeight();

    int pw = 400;
    int ph = scr_h;
    setRect(0, 0, pw, ph);
    setTexture( m_game->getTex(H3_Tex::Scroll, __func__) );
    setFillColor( 0xffffffff );

    m_btnNewGame = createButton("New Game", 0.23 * pw, 0.13 * ph);
    m_btnRestart = createButton("Restart", 0.21 * pw, 0.28 * ph);
    m_btnRestart->setVisible(false);
    m_btnOptions = createButton("Options", 0.19 * pw, 0.43 * ph);
    m_btnHelp = createButton("Help", 0.17 * pw, 0.58 * ph);
    m_btnExit = createButton("Exit", 0.15 * pw, 0.73 * ph);

    m_btnNewGame->setOnClicked([&](){ m_game->newGame(); });
    m_btnRestart->setOnClicked([&](){ m_game->restartGame(); });
    m_btnOptions->setOnClicked([](){
        H3_MessageBox("Options", "Options are not implemented so yet.\n"
                                 "Try tinkering with xml config files");
    });
    m_btnHelp->setOnClicked([&](){ m_game->toggleHelpWindow(); });
    m_btnExit->setOnClicked([&](){ m_game->exitProgram(); });
}

UI_MainMenu::~UI_MainMenu()
{
    DE_WARN("")
}

// static
de::gui::Button*
UI_MainMenu::createButton(std::string txt, int x, int y )
{
    //const int px = parent->getAbsolutePos().x;
    //const int py = parent->getAbsolutePos().y;
    const int pw = getSize().w;
    const int ph = getSize().h;
    auto btn = new de::gui::Button(txt, m_env, this, 0, de::Recti(x, y, 0.60 * pw, 0.1 * ph));

    btn->setTexture(m_game->getTex(H3_Tex::Scroll1, __func__));
    btn->setFillColor(dbRGBA(255,255,255,200),0);
    btn->setFillColor(dbRGBA(255,255,255,255),1);
    btn->setTextColor(dbRGBA(0,0,0,150),0);
    btn->setTextColor(dbRGBA(0,0,0,255),1);

    // de::Font5x8 menuFont;
    // menuFont.quadWidth = 3;
    // menuFont.quadHeight = 4;
    // menuFont.quadSpacingX = 0;
    // menuFont.quadSpacingY = 0;
    // menuFont.glyphSpacingX = 2;
    // menuFont.glyphSpacingY = 2;
    // btn->setTextFont(menuFont);

    de::Font menuFont("Postamt", 32);
    btn->setFont(menuFont);

    return btn;
}

void UI_MainMenu::resizeLayout()
{

}
