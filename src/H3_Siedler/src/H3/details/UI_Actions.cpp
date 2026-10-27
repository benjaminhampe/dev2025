#include "UI_Actions.h"
#include <H3/H3_Game.h>
#include <de/gui/Label.h>
#include <de/gui/Button.h>

de::gui::Button*
createActionButton(H3_Game* game, H3_Tex::eID id, de::gui::Env* env, de::gui::Widget* parent)
{
    auto btn = new de::gui::Button("", env, parent);
    btn->setBorderColor(dbRGB(255,255,0),1);

    auto tex = game->getTex(id, dbStr("UI_Actions().texId",int(id)));
    btn->setTexture(tex);

    float zoom = env->getZoom();
    int r = zoom * 8;
    btn->setRadius(r);

    btn->setBorderColor(dbRGB(255,255,255,225), de::gui::Widget::eWS_Hovered);
    //btn->setFillColor(dbRGB(255,255,255), de::gui::Widget::eWS_Hovered);
    return btn;
}

// =====================================================================
UI_Actions::UI_Actions( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& pos )
// =====================================================================
    : de::gui::Widget(env, parent, id, pos)
    , m_game(game)
{
    // (1.) DoDice
    m_btnDice = createActionButton(game,H3_Tex::DoDice,env,this);
    m_btnDice->onClicked = [&](){ m_game->doDice(); };

    // (2.) DoBank
    m_btnBanking = createActionButton(game,H3_Tex::DoBank,env,this);
    m_btnBanking->onClicked = [&](){ m_game->enterBanking(); };

    // (3.) DoTrade
    m_btnTrading = createActionButton(game,H3_Tex::DoTrade,env,this);
    m_btnTrading->onClicked = [&](){ m_game->enterTrading(); };

    // (4.) DoEndTurn
    m_btnEndTurn = createActionButton(game,H3_Tex::DoEndTurn,env,this);
    m_btnEndTurn->onClicked = [&](){ m_game->doEndTurn(); };

    // (5.) DoBuyEventCard
    m_btnBuyCard = createActionButton(game,H3_Tex::DoBuyEventCard,env,this);
    m_btnBuyCard->onClicked = [&](){ m_game->doBuyCard(); };

    // (6.) DoCancel
    m_btnCancel = createActionButton(game,H3_Tex::DoCancel,env,this);
    m_btnCancel->onClicked = [&](){ m_game->doPlaceCancel(); };

    // (New.) DoRob
    m_btnPlaceThief = createActionButton(game,H3_Tex::DoRob,env,this);
    //m_btnPlaceThief->onClicked = [&](){ m_game->doPlaceCancel(); };

    // (7.) DoBuyRoad
    m_btnBuyRoad = createActionButton(game,H3_Tex::DoBuyRoad,env,this);
    m_btnBuyRoad->onClicked = [&](){ m_game->doBuyRoad(); };

    // (8.) DoBuyFarm
    m_btnBuyFarm = createActionButton(game,H3_Tex::DoBuyFarm,env,this);
    m_btnBuyFarm->onClicked = [&](){ m_game->doBuyFarm(); };

    // (9.) DoBuyCity
    m_btnBuyCity = createActionButton(game,H3_Tex::DoBuyCity,env,this);
    m_btnBuyCity->onClicked = [&](){ m_game->doBuyCity(); };


    // (20.) Reveal PointCard
    m_btnPointCard = createActionButton(game,H3_Tex::NicePointCard,env,this);
    m_btnPointCard->onClicked = [&](){ m_game->revealPointCard(); };

    // (20.) Reveal KnightCard
    m_btnKnightCard = createActionButton(game,H3_Tex::NiceKnightCard,env,this);
    m_btnKnightCard->onClicked = [&](){ m_game->revealKnightCard(); };

    // (20.) Reveal InventCard
    m_btnInventCard = createActionButton(game,H3_Tex::NiceInventCard,env,this);
    m_btnInventCard->onClicked = [&](){ m_game->revealInventCard(); };

    // (20.) Reveal MonopolCard
    m_btnMonopolCard = createActionButton(game,H3_Tex::NiceMonopolCard,env,this);
    m_btnMonopolCard->onClicked = [&](){ m_game->revealMonopolCard(); };

}

UI_Actions::~UI_Actions()
{
    // DE_WARN("")
}

void UI_Actions::setDebugVisible(bool visible)
{
    setVisible( visible );
    for (auto & child : m_children)
    {
        child->setVisible(visible);
    }

    if (!visible)
    {
        updateVisibility();
    }
}

/*
void UI_Actions::draw()
{
    de::gui::Widget::draw();

    if (!isVisible()) return;

    for (auto & child : m_children)
    {
        child->draw();
    }
}
*/

void UI_Actions::resize(int X, int Y, int W, int H)
{
    m_rect = de::Recti(X,Y,W,H);

    const float zoom = m_env->getZoom();
    const int p = zoom * 5;
    const int s = zoom * 2;
    const int w1 = zoom * 68;
    const int h1 = zoom * 28;
    const int w2 = zoom * 64;
    const int h2 = zoom * 64;
    const int w3 = w2;
    const int h3 = zoom * 96;
    const int x0 = W/2 - 2*w1;
    int x = x0;
    int y = H - 1 - p - h1;
    m_btnDice->resize(x,y,w1,h1);    x += w1 + p;
    m_btnBanking->resize(x,y,w1,h1); x += w1 + p;
    m_btnTrading->resize(x,y,w1,h1); x += w1 + p;
    m_btnEndTurn->resize(x,y,w1,h1); // x += w2 + p;

    x = x0 - w1 - p;
    y = H - 1 - p - h1 - p - h2;
    m_btnCancel->resize(x+s,y,w2,h2); x += w1 + p;
    m_btnPlaceThief->resize(x+s,y,w2,h2); x += w1 + p;
    m_btnBuyRoad->resize(x+s,y,w2,h2); x += w1 + p;
    m_btnBuyFarm->resize(x+s,y,w2,h2); x += w1 + p;
    m_btnBuyCity->resize(x+s,y,w2,h2); x += w1 + p;

    y = H - 1 - p - h3;
    m_btnBuyCard->resize(x,y,w3,h3); x += w2 + p;
    m_btnPointCard->resize(x,y,w3,h3); x += w2 + p;
    m_btnKnightCard->resize(x,y,w3,h3); x += w2 + p;
    m_btnInventCard->resize(x,y,w3,h3); x += w2 + p;
    m_btnMonopolCard->resize(x,y,w3,h3); // x += w2 + p;
}

void UI_Actions::hideActions()
{
    m_btnDice->setVisible( false );
    m_btnEndTurn->setVisible( false );
    m_btnBanking->setVisible( false );
    m_btnTrading->setVisible( false );

    m_btnCancel->setVisible( false );
    m_btnPlaceThief->setVisible( false );
    m_btnBuyRoad->setVisible( false );
    m_btnBuyFarm->setVisible( false );
    m_btnBuyCity->setVisible( false );

    m_btnBuyCard->setVisible( false );
    m_btnPointCard->setVisible( false );
    m_btnKnightCard->setVisible( false );
    m_btnInventCard->setVisible( false );
    m_btnMonopolCard->setVisible( false );

    //UI_get( game, H3_UI::BtnDoRob ).visible = false;
}

void UI_Actions::updateVisibility()
{
    if (!isVisible())
    {
        return;
    }

    hideActions();

    auto& player = m_game->getCurrentPlayer();

    if (m_game->m_state == H3_State::PlaceRoad ||
        m_game->m_state == H3_State::PlaceFarm ||
        m_game->m_state == H3_State::PlaceCity )
    {
        m_btnCancel->setVisible(true);
        return;
    }

    if (m_game->m_state == H3_State::PlaceThief )
    {
        m_btnPlaceThief->setVisible(true);
        return;
    }

    if (m_game->m_state != H3_State::Idle )
    {
        return;
    }

    // Round - 0
    if ( m_game->m_round == 0 )
    {
        DE_ERROR("Invalid Round 0")
        return;
    }
    // Round - 1
    else if ( m_game->m_round == 1 )
    {
        if ( player.state.dice.sum() )
        {
            m_btnEndTurn->setVisible(true);
        }
        else
        {
            m_btnDice->setVisible(true);
        }
    }
    // Round - 2
    else if ( m_game->m_round == 2 )
    {
        if ( player.state.farms.size() < 1 )
        {
            m_btnBuyFarm->setVisible(true);
        }
        else
        {
            if ( player.state.roads.size() < 1 )
            {
                m_btnBuyRoad->setVisible(true);
            }
            else
            {
                m_btnEndTurn->setVisible(true);
            }
        }
    }
    // Round - 3
    else if ( m_game->m_round == 3 )
    {
        if ( player.state.farms.size() < 2 )
        {
            m_btnBuyFarm->setVisible(true);
        }
        else
        {
            if ( player.state.roads.size() < 2 )
            {
                m_btnBuyRoad->setVisible(true);
            }
            else
            {
                m_btnEndTurn->setVisible(true);
            }
        }
    }
    // Round - N
    else
    {
        if ( player.state.dice.sum() )
        {
            m_btnEndTurn->setVisible(true);

            if ( player.state.canBanking() ) { m_btnBanking->setVisible(true); }
            if ( player.state.canTrading() ) { m_btnTrading->setVisible(true); }
            if ( player.state.bank.canBuyRoad() ) { m_btnBuyRoad->setVisible(true); }
            if ( player.state.bank.canBuyFarm() ) { m_btnBuyFarm->setVisible(true); }
            if ( player.state.bank.canBuyCity() ) { m_btnBuyCity->setVisible(true); }
            if ( player.state.bank.canBuyCard() ) { m_btnBuyCard->setVisible(true); }
            if ( player.state.unrevealedPointCards() ) { m_btnPointCard->setVisible(true); }
            if ( player.state.unrevealedKnightCards() ) { m_btnKnightCard->setVisible(true); }
            if ( player.state.unrevealedInventCards() ) { m_btnInventCard->setVisible(true); }
            if ( player.state.unrevealedMonopolCards() ) { m_btnMonopolCard->setVisible(true); }
        }
        else
        {
            m_btnDice->setVisible(true);
        }
    }
}
