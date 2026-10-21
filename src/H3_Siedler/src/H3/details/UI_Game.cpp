#include "UI_Game.h"
#include <H3/H3_Game.h>
#include <H3/details/UI_CpuPlayer.h>
#include <H3/details/H3_UI.h>

// =====================================================================
UI_Game::UI_Game( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& pos )
// =====================================================================
    : de::gui::Widget(env, parent, id, pos)
    , m_game(game)
    , m_forceResize{ true }
    , m_worldCreator(nullptr)
    , m_winnerScreen(nullptr)
    , m_actions(nullptr)
    , m_donateCards(nullptr)
    , m_banking(nullptr)
{
    setDrawChildren(true);

    auto r = de::Recti(0,0,0,0);
    m_worldCreator = new UI_WorldCreator( game, env, this, 1, r );
    m_marquee = new UI_Marquee( game, env, this, 2, r );
    m_actions = new UI_Actions( game, env, this, 3, r );
    m_donateCards = new UI_DonateCards( game, env, this, 4, r );
    m_winnerScreen = new UI_WinnerScreen( game, env, this, 5, r );
    m_banking = new UI_Banking( game, env, this, 6, r );

    m_worldCreator->setVisible(false);
    m_marquee->setVisible(false);
    m_actions->setVisible(false);
    m_donateCards->setVisible(false);
    m_winnerScreen->setVisible(false);
    m_banking->setVisible(false);
}

UI_Game::~UI_Game()
{

}

void UI_Game::updateVisibleActions()
{
    if (m_actions)
    {
        m_actions->updateVisibility();
    }

    H3_Game & game = *m_game;
    UI_Stats_update( game );
    //UIBanking_update( game );
}

void UI_Game::draw()
{
    if (!isVisible())
    {
        return;
    }

    if ( !m_game )
    {
        DE_ERROR("No game")
        return;
    }

    if ( m_game->isGameRunning() )
    {
        //DE_DEBUG("Draw(",m_rect.str(),")")

        H3_Game & game = *m_game;

        float zoom = game.getZoom();
        int w = m_rect.w; // game.getDriver()->getScreenWidth();
        int h = m_rect.h; // game.getDriver()->getScreenHeight();

        // --- Draw Enemy Statistics ---

        int p = zoom * 10;
        int estat_w = (w - 5*p) / 4;
        int estat_h = zoom * 150;
        int x = p;
        int y = zoom * 10;
        for (int i = 0; i < game.m_players.size(); ++i)
        {
            H3_Player & player = game.m_players[i];
            if (player.typ == H3_PlayerType::HUMAN) continue;

            UI_CpuPlayer_draw( game, player.id, de::Recti(x,y,estat_w,estat_h) );

            x += estat_w + p;
        }

        auto driver = m_env->getVideoDriver();

        driver->beginPerf("UI_Stats");
        UI_Stats_render( game );
        driver->endPerf();

        // driver->beginPerf("UI_PlayerStats");
        // for ( int i = 0; i < m_players.size(); ++i )
        // {
        //     UI_drawPlayerStats( *this, 20, 20+100*i, m_players[ i ].id );
        // }
        // driver->endPerf();

        driver->beginPerf("UI_RoundStats");
        UI_drawRoundStats( game );
        driver->endPerf();

        driver->beginPerf("UI_HoverInfo");
        auto r_panel = de::Recti( 20, 300, zoom * 200, zoom * 300 );
        UI_drawInfoPanelTile( game, r_panel, game.m_hoverTileId );
        UI_drawInfoPanelCorner( game, r_panel, game.m_hoverCornerId );
        UI_drawInfoPanelEdge( game, r_panel, game.m_hoverEdgeId );
        driver->endPerf();

        driver->beginPerf("UI_Log");
        UI_drawLog( game );
        driver->endPerf();


        drawDice();
    }



    if (isDrawChildren())
    {
        for (auto child : m_children)
        {
            child->draw();
        }
    }
}

void UI_Game::resize(int x, int y, int w, int h)
{
    // x = 0;
    // y = 0;
    // w = m_env->getVideoDriver()->getScreenWidth();
    // h = m_env->getVideoDriver()->getScreenHeight();
    auto rect = de::Recti(x, y, w, h);

    if (m_forceResize)
    {
        m_forceResize = false;
    }
    else
    {
        if (rect == m_rect)
        {
            return;
        }
    }


    DE_DEBUG("Resize(",x,",",y,",",w,",",h,")")

    m_rect = de::Recti(x, y, w, h);
    updateLayouts();
}


void UI_Game::updateLayouts()
{
    int x = m_rect.x;
    int y = m_rect.y;
    int w = m_rect.w;
    int h = m_rect.h;
    if (m_worldCreator)
    {
        m_worldCreator->resize(x,y,w,h);
    }
    if (m_actions)
    {
        m_actions->resize(x,y,w,h);
    }
    if (m_donateCards)
    {
        // ::::: Panel body ::::
        int dim = 8 * std::min(w,h) / 10;
        int panel_w = dim;
        int panel_h = dim;
        int panel_x = (w - panel_w)/2;
        int panel_y = (h - panel_h)/2;
        m_donateCards->resize(panel_x, panel_y, panel_w, panel_h);
    }
    if (m_winnerScreen)
    {
        int panel_w = w - 200;
        int panel_h = h - 200;
        int panel_x = 100;
        int panel_y = 100;
        m_winnerScreen->resize(panel_x, panel_y, panel_w, panel_h);
    }
    if (m_banking)
    {
        // ::::: Panel body ::::
        int dim = 8 * std::min(w,h) / 10;
        int panel_w = dim;
        int panel_h = dim;
        int panel_x = (w - panel_w)/2;
        int panel_y = (h - panel_h)/2;
        m_banking->resize(panel_x, panel_y, panel_w, panel_h);
    }
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
    guiState.bGameOverlay = m_actions->isVisible();
    guiState.bDonateCards = m_donateCards->isVisible();
    guiState.bBanking = m_banking->isVisible();
    guiState.bWinnerScreen = m_winnerScreen->isVisible();
    return guiState;
}

void UI_Game::setState(const H3_GuiState& guiState)
{
    if (m_worldCreator){
        m_worldCreator->setVisible(guiState.bWorldCreator);
    }
    if (m_actions){
        m_actions->setVisible(guiState.bGameOverlay);
        m_marquee->setVisible(guiState.bGameOverlay);
    }
    if (m_donateCards){
        m_donateCards->setVisible(guiState.bDonateCards);
    }
    if (m_banking){
        m_banking->setVisible(guiState.bBanking);
    }
    if (m_winnerScreen){
        m_winnerScreen->setVisible(guiState.bWinnerScreen);
    }

    updateLayouts();
}


void UI_Game::drawDice()
{
    const float zoom = m_env->getZoom();

    const int r = zoom * 6;

    auto driver = m_env->getVideoDriver();
    const int w = driver->getScreenWidth();
    const int h = driver->getScreenHeight();

    const auto & dice = m_game->m_dice;
    const de::gpu::TexRef tex_1 = H3_getDiceTex(*m_game,dice.a);
    const de::gpu::TexRef tex_2 = H3_getDiceTex(*m_game,dice.b);

    const int d = zoom * 48;
    const int s = zoom * 12;

    int x = w - 2*(d+s);
    int y = h - 300;

    driver->getScreenRenderer()->
        draw2DRoundRect(de::Recti(x,y,d,d), {r,r}, tex_1);

    driver->getScreenRenderer()->
        draw2DRoundRect(de::Recti(x+d+s,y,d,d), {r,r}, tex_2);
}
