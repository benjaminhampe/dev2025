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

    auto r_close = de::Recti(100,
                             getRect().h - 100,
                             getRect().w - 200,
                             64);

    m_lblWinner = new de::gui::Label("Winner", env, this, 0, de::Recti());
    m_lblWinnerIcon = new de::gui::Label("Icon", env, this, 0, de::Recti());
    m_lblWinnerName = new de::gui::Label("Name", env, this, 0, de::Recti());

    m_btnClose = createButton("Close", texClose, r_close );
    m_btnClose->setOnClicked( [&](){ m_game->enterStartMenu(); } );
}

UI_WinnerScreen::~UI_WinnerScreen()
{

}


void UI_WinnerScreen::draw()
{
    Panel::draw();

    if (!isVisible()) return;

    for (auto & child : m_children)
    {
        child->draw();
    }
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

    m_lblWinner->resize(100,100,w - 200,100);
    m_lblWinnerIcon->resize(100,200,100,100);
    m_lblWinnerName->resize(200,200,w-200,100);
    m_btnClose->resize(100,m_rect.h - 100,m_rect.w - 200,64);
}

// static
de::gui::Button*
UI_WinnerScreen::createButton(std::string txt, de::gpu::TexRef ref, de::Recti pos)
{
    auto btn = new de::gui::Button(txt, m_env, this, 0, pos);
    btn->setRadius( 8 );
    btn->setFillColor(dbRGB(255,255,255,200),0);
    btn->setTextColor(dbRGB(255,255,255),1);
    btn->setTexture( ref );
    return btn;
}
