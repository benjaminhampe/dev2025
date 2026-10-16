#include "UI_Game.h"
#include <H3/H3_Game.h>
#include <de/gui/Label.h>
#include <de/gui/Button.h>

// =====================================================================
UI_Game::UI_Game( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& pos )
// =====================================================================
    : de::gui::Widget(env, parent, id, pos)
    , m_game(game)
    , m_worldCreator(nullptr)
    , m_winnerScreen(nullptr)
    , m_actions(nullptr)
    , m_donateCards(nullptr)
{
    setDrawChildren(true);

    auto r = de::Recti(0,0,0,0);
    m_worldCreator = new UI_WorldCreator( game, env, this, 1, r );
    m_marquee = new UI_Marquee( game, env, this, 2, r );
    m_actions = new UI_Actions( game, env, this, 3, r );
    m_donateCards = new UI_DonateCards( game, env, this, 4, r );
    m_winnerScreen = new UI_WinnerScreen( game, env, this, 5, r );

    m_worldCreator->setVisible(false);
    m_marquee->setVisible(false);
    m_actions->setVisible(false);
    m_donateCards->setVisible(false);
    m_winnerScreen->setVisible(false);

    m_worldCreator->setPreset( H3_Cfg::createStandard() );
}

UI_Game::~UI_Game()
{

}

/*
void UI_Game::draw()
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

void UI_Game::resize(int x, int y, int w, int h)
{
    DE_DEBUG("Resize(",x,",",y,",",w,",",h,")")
    m_rect = de::Recti(x, y, w, h);
    updateLayouts();
}

void UI_Game::pushState(const H3_GuiState& guiState)
{
    m_stack.push(getState());
    setState(guiState);
}

void UI_Game::popState()
{
    setState(m_stack.top());
    m_stack.pop();
}

H3_GuiState UI_Game::getState() const
{
    H3_GuiState guiState;
    guiState.bWorldCreator = m_worldCreator->isVisible();
    guiState.bMarquee = m_marquee->isVisible();
    guiState.bActions = m_actions->isVisible();
    guiState.bDonateCards = m_donateCards->isVisible();
    guiState.bWinnerScreen = m_winnerScreen->isVisible();
    return guiState;
}

void UI_Game::setState(const H3_GuiState& guiState)
{
    if (m_worldCreator){
        m_worldCreator->setVisible(guiState.bWorldCreator);
    }
    if (m_marquee){
        m_marquee->setVisible(guiState.bMarquee);
    }
    if (m_actions){
        m_actions->setVisible(guiState.bActions);
    }
    if (m_donateCards){
        m_donateCards->setVisible(guiState.bDonateCards);
    }
    if (m_winnerScreen){
        m_winnerScreen->setVisible(guiState.bWinnerScreen);
    }

    updateLayouts();
}

void UI_Game::updateLayouts()
{
    int x = m_rect.x;
    int y = m_rect.y;
    int w = m_rect.w;
    int h = m_rect.h;

    if (m_worldCreator && m_worldCreator->isVisible())
    {
        // m_worldCreator->resize(0,0,scr_w,scr_h);
        m_worldCreator->resize(x,y,w,h);
    }

    if (m_winnerScreen && m_winnerScreen->isVisible())
    {
        //m_winnerScreen->resize(0,0,scr_w,scr_h);
        m_winnerScreen->resize(x,y,w,h);
    }

    if (m_actions && m_actions->isVisible())
    {
        //m_actions->resize(0,0,scr_w,scr_h);
        m_actions->resize(x,y,w,h);
    }

    if (m_donateCards && m_donateCards->isVisible())
    {
        int dw = w - 400;
        int dh = h - 300;
        int dx = (w - dw) / 2;
        int dy = (h - dh) / 2;
        m_donateCards->resize(dx,dy,dw,dh);
        // m_donateCards->resize(0,0,scr_w,scr_h);
    }
}


/*
H3_GuiState H3_Game::UI_getState() const
{
    return m_guiState;
}

void H3_Game::UI_setState(const H3_GuiState& guiState)
{
}

void H3_Game::UI_resizeLayouts()
{
    if (!m_driver)
    {
        DE_ERROR("No driver")
        return;
    }

    auto scr_w = m_driver->getScreenWidth();
    auto scr_h = m_driver->getScreenHeight();

    if (m_uiMainmenu && m_uiMainmenu->isVisible())
    {
        m_uiMainmenu->resize(0,0,scr_w / 6, scr_h / 2);
    }

    if (m_uiHelpWindow && m_uiHelpWindow->isVisible())
    {
        m_uiHelpWindow->resize(scr_w / 6, 50, 4*scr_w / 6, scr_h - 100);
    }

    if (m_uiWorldCreator && m_uiWorldCreator->isVisible())
    {
        m_uiWorldCreator->resize(0,0,scr_w,scr_h);
    }

    if (m_uiWinnerScreen && m_uiWinnerScreen->isVisible())
    {
        m_uiWinnerScreen->resize(0,0,scr_w,scr_h);
    }

    if (m_uiActions && m_uiActions->isVisible())
    {
        m_uiActions->resize(0,0,scr_w,scr_h);
    }

    if (m_uiDonateCards && m_uiDonateCards->isVisible())
    {
        int w = scr_w - 400;
        int h = scr_h - 300;
        int x = (scr_w - w) / 2;
        int y = (scr_h - h) / 2;
        m_uiDonateCards->resize(x,y,w,h);
    }
}

*/
