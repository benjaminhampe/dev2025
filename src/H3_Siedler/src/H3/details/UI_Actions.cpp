#include "UI_Actions.h"
#include <H3/H3_Game.h>
#include <de/gui/Label.h>
#include <de/gui/Button.h>

// =====================================================================
UI_Actions::UI_Actions( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& pos )
// =====================================================================
    : de::gui::Widget(env, parent, id, pos)
    , m_game(game)
{
    // (1.) DoDice
    {
        auto btn = new de::gui::Button("", env, this, 0, de::Recti(0,0,0,0));
        btn->setTexture(m_game->getTex(H3_Tex::DoDice, "UI_Actions().DoDice"));
        btn->setBorderColor(dbRGB(255,255,0),1);
        btn->setOnClicked([&](){ m_game->doDice(); });
        m_btnDice = btn;
    }

    // (2.) DoBank
    {
        auto btn = new de::gui::Button("", env, this, 0, de::Recti(0,0,0,0));
        btn->setTexture(m_game->getTex(H3_Tex::DoBank, "UI_Actions().DoDice"));
        btn->setBorderColor(dbRGB(255,255,0),1);
        btn->setOnClicked([&](){ H3_MessageBox("Bank", "DoBanking"); });
        m_btnBank = btn;
    }

    // (3.) DoTrade
    {
        auto btn = new de::gui::Button("", env, this, 0, de::Recti(0,0,0,0));
        btn->setTexture(m_game->getTex(H3_Tex::DoTrade, "UI_Actions().DoTrade"));
        btn->setBorderColor(dbRGB(255,255,0),1);
        btn->setOnClicked([&](){ H3_MessageBox("Trade", "DoTrade"); });
        m_btnTrade = btn;
    }

    // (4.) DoEndTurn
    {
        auto btn = new de::gui::Button("", env, this, 0, de::Recti(0,0,0,0));
        btn->setTexture(m_game->getTex(H3_Tex::DoEndTurn, "UI_Actions().DoEndTurn"));
        btn->setBorderColor(dbRGB(255,255,0),1);
        btn->setOnClicked([&](){ m_game->doEndTurn(); });
        m_btnEndTurn = btn;
    }

    // (5.) DoBuyEventCard
    {
        auto tex = m_game->getTex(H3_Tex::DoBuyEventCard, "UI_Actions().DoBuyEventCard");
        auto btn = new de::gui::Button("", env, this, 0, de::Recti(0,0,0,0));
        btn->setTexture(tex);
        btn->setBorderColor(dbRGB(255,255,0),1);
        btn->setOnClicked([&](){ m_game->doBuyCard(); });
        m_btnBuyCard = btn;
    }

    // (6.) DoCancel
    {
        auto tex = m_game->getTex(H3_Tex::DoCancel, "UI_Actions().DoCancel");
        auto btn = new de::gui::Button("", env, this, 0, de::Recti(0,0,0,0));
        btn->setTexture(tex);
        btn->setBorderColor(dbRGB(255,255,0),1);
        btn->setOnClicked([&](){ m_game->doPlaceCancel(); });
        m_btnCancel = btn;
    }

    // (7.) DoBuyRoad
    {
        auto tex = m_game->getTex(H3_Tex::DoBuyRoad, "UI_Actions().DoBuyRoad");
        auto btn = new de::gui::Button("", env, this, 0, de::Recti(0,0,0,0));
        btn->setTexture(tex);
        btn->setBorderColor(dbRGB(255,255,0),1);
        btn->setOnClicked([&](){ m_game->doBuyRoad(); });
        m_btnBuyRoad = btn;
    }

    // (8.) DoBuyFarm
    {
        auto tex = m_game->getTex(H3_Tex::DoBuyFarm, "UI_Actions().DoBuyFarm");
        auto btn = new de::gui::Button("", env, this, 0, de::Recti(0,0,0,0));
        btn->setTexture(tex);
        btn->setBorderColor(dbRGB(255,255,0),1);
        btn->setOnClicked([&](){ m_game->doBuyFarm(); });
        m_btnBuyFarm = btn;
    }

    // (9.) DoBuyRoad
    {
        auto tex = m_game->getTex(H3_Tex::DoBuyCity, "UI_Actions().DoBuyCity");
        auto btn = new de::gui::Button("", env, this, 0, de::Recti(0,0,0,0));
        btn->setTexture(tex);
        btn->setBorderColor(dbRGB(255,255,0),1);
        btn->setOnClicked([&](){ m_game->doBuyCity(); });
        m_btnBuyCity = btn;
    }

    //UI_addElem( game, H3_UI::BtnDoRob, H3_UIElem::ImageButton, de::Recti( 300, 68,0,0),
    //            12, de::Align::TopCenter, int(H3_Tex::DoRob), "DoRob");

    //   addButton( int(H3_Tex::DoUndo), 0.2, 0.7, 0.3, 0.1, "DoUndo");
    //   addButton( int(H3_Tex::DoRedo), 0.2, 0.7, 0.3, 0.1, "DoRedo");

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

    // DE_WARN("")

    const float zoom = m_env->getZoom();

    const int p = zoom * 5;
    const int s = zoom * 2; // = zoom * (68-64)/2
    const int w1 = zoom * 68;
    const int h1 = zoom * 28;
    const int w2 = zoom * 64;
    const int h2 = zoom * 64;

    const int w3 = w2;
    const int h3 = zoom * 96;

    int x0 = W/2 - 2*w1;
    int x = x0;
    int y = H - 1 - p - h1;
    m_btnDice->resize(x,y,w1,h1); x += w1 + p;
    m_btnBank->resize(x,y,w1,h1); x += w1 + p;
    m_btnTrade->resize(x,y,w1,h1); x += w1 + p;
    m_btnEndTurn->resize(x,y,w1,h1); // x += w2 + p;

    x = x0;
    y = H - 1 - p - h1 - h2;
    m_btnCancel->resize(x+s,y,w2,h2); x += w1 + p;
    m_btnBuyRoad->resize(x+s,y,w2,h2); x += w1 + p;
    m_btnBuyFarm->resize(x+s,y,w2,h2); x += w1 + p;
    m_btnBuyCity->resize(x+s,y,w2,h2); x += w1 + p;

    y = H - 1 - p - h1 - h3;
    m_btnBuyCard->resize(x,y,w3,h3);

    //DE_DEBUG("this = ", this->getRect().str())
    //DE_DEBUG("m_btnDice = ", m_btnDice->getRect().str(), ", visible = ", m_btnDice->isVisible())
}

void UI_Actions::hideActions()
{
    m_btnDice->setVisible( false );
    m_btnEndTurn->setVisible( false );
    m_btnBank->setVisible( false );
    m_btnTrade->setVisible( false );

    m_btnCancel->setVisible( false );
    m_btnBuyRoad->setVisible( false );
    m_btnBuyFarm->setVisible( false );
    m_btnBuyCity->setVisible( false );

    m_btnBuyCard->setVisible( false );

    //UI_get( game, H3_UI::BtnDoRob ).visible = false;
}

void UI_Actions::updateVisibility()
{
    hideActions();

    auto& player = m_game->getCurrentPlayer();

    if (m_game->m_state == H3_State::PlaceRoad ||
        m_game->m_state == H3_State::PlaceFarm ||
        m_game->m_state == H3_State::PlaceCity )
    {
        m_btnCancel->setVisible(true);
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
        if ( player.state.newFarms.empty() )
        {
            m_btnBuyFarm->setVisible(true);
        }
        else
        {
            if ( player.state.newRoads.empty() )
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
        if ( player.state.newFarms.empty() )
        {
            m_btnBuyFarm->setVisible(true);
        }
        else
        {
            if ( player.state.newRoads.empty() )
            {
                m_btnBuyRoad->setVisible(true);
            }
            else
            {
                m_btnEndTurn->setVisible(true);
            }
        }
        /*
        if ( !player.didBuyRoad )
        {
            m_btnBuyRoad->setVisible(true);
        }
        if ( !player.didBuyFarm )
        {
            m_btnBuyFarm->setVisible(true);
        }
        if ( player.didBuyRoad && player.didBuyFarm )
        {
            m_btnEndTurn->setVisible(true);
        }
        */
    }
    // Round - N
    else
    {
        if ( player.state.dice.sum() )
        {
            m_btnEndTurn->setVisible(true);

            if ( player.state.bank.canBuyRoad() )
            {
                m_btnBuyRoad->setVisible(true);
            }
            if ( player.state.bank.canBuyFarm() )
            {
                m_btnBuyFarm->setVisible(true);
            }
            if ( player.state.bank.canBuyCity() )
            {
                m_btnBuyCity->setVisible(true);
            }
            if ( player.state.bank.canBuyCard() )
            {
                m_btnBuyCard->setVisible(true);
            }
            if ( player.unrevealedPointCards() )
            {
                m_btnPlayPointCard->setVisible(true);
            }

        }
        else
        {
            m_btnDice->setVisible(true);
        }
    }
}

// void UI_Actions::draw()
// {
//     Widget::draw();
// }

