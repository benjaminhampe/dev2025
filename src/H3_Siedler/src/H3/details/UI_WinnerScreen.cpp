#include "UI_WinnerScreen.h"
#include <H3/H3_Game.h>
#include <de/gui/Label.h>
#include <de/gui/Button.h>

// =====================================================================
UI_WinnerScreen::UI_WinnerScreen( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& pos )
    // =====================================================================
    : de::gui::Widget(env, parent, id, pos)
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
    DE_WARN("")
}

void UI_WinnerScreen::updateData()
{

}

void UI_WinnerScreen::resizeLayout()
{
    int scr_w = m_env->getScreenWidth();
    int scr_h = m_env->getScreenHeight();

    // ::::: Panel ::::
    int w = scr_w - 200;
    int h = scr_h - 200;
    int x = 100;
    int y = 100;
    setRect( de::Recti(x,y,w,h) );


    m_lblWinner->setRect(de::Recti(100,100,w - 200, 100));
    m_lblWinnerIcon->setRect(de::Recti(100,200,100,100));
    m_lblWinnerName->setRect(de::Recti(200,200,w-200,100));

    // ::::: CloseButton ::::
    auto r_close = de::Recti(100,getRect().h - 100,getRect().w - 200,64);
    m_btnClose->setRect( r_close );
}

// static
de::gui::Button*
UI_WinnerScreen::createButton(std::string txt, de::gpu::TexRef ref, de::Recti pos)
{
    auto btn = new de::gui::Button(txt, m_env, this, 0, pos);
    btn->setRadius( 8 );
    btn->setFillColor(dbRGBA(255,255,255,200),0);
    btn->setTextColor(dbRGBA(255,255,255),1);
    btn->setTexture( ref );
    return btn;
}
