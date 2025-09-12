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
    auto scr_w = m_env->getScreenWidth();
    auto scr_h = m_env->getScreenHeight();

    float fY = 0.9f * scr_w;
    float fX = 0.5f * scr_h;
    float fW = 0.06f * scr_w;
    float fH = 0.06f * scr_h;

    // (1.) DoDice
    {
        fX = 0.3f * scr_w;
        auto btn = new de::gui::Button("", env, this, 0, de::Recti(fX,fY,68,28));
        btn->setTexture(m_game->getTex(H3_Tex::DoDice, "UI_Actions().DoDice"));
        btn->setBorderColor(dbRGBA(255,255,0),1);
        m_btnDice = btn;
        m_btnDice->setOnClicked([&](){ m_game->doDice(); });
    }

    // (2.) DoBank
    {
        fX = 0.4f * scr_w;
        auto btn = new de::gui::Button("", env, this, 0, de::Recti(fX,fY,68,28));
        btn->setTexture(m_game->getTex(H3_Tex::DoBank, "UI_Actions().DoDice"));
        btn->setBorderColor(dbRGBA(255,255,0),1);
        m_btnBank = btn;
        m_btnBank->setOnClicked([&](){ H3_MessageBox("Bank", "DoBanking"); });
    }

    // (3.) DoTrade
    {
        fX = 0.5f * scr_w;
        auto btn = new de::gui::Button("", env, this, 0, de::Recti(fX,fY,68,28));
        btn->setTexture(m_game->getTex(H3_Tex::DoTrade, "UI_Actions().DoTrade"));
        btn->setBorderColor(dbRGBA(255,255,0),1);
        m_btnTrade = btn;
        m_btnTrade->setOnClicked([&](){ H3_MessageBox("Trade", "DoTrade"); });
    }

    // (4.) DoEndTurn
    {
        fX = 0.6f * scr_w;
        auto btn = new de::gui::Button("", env, this, 0, de::Recti(fX,fY,68,28));
        btn->setTexture(m_game->getTex(H3_Tex::DoEndTurn, "UI_Actions().DoEndTurn"));
        btn->setBorderColor(dbRGBA(255,255,0),1);
        m_btnEndTurn = btn;
        m_btnEndTurn->setOnClicked([&](){ m_game->doEndTurn(); });
    }

    // (5.) DoBuyEventCard
    {
        auto tex = m_game->getTex(H3_Tex::DoBuyEventCard, "UI_Actions().DoBuyEventCard");
        fX = 0.7f * scr_w;
        fY = 0.86f * scr_h;
        fH = 0.1f * scr_h;
        auto btn = new de::gui::Button("", env, this, 0, de::Recti(fX,fY,64,64));
        btn->setTexture(tex);
        btn->setBorderColor(dbRGBA(255,255,0),1);
        m_btnBuyCard = btn;
        m_btnBuyCard->setOnClicked([&](){ m_game->doBuyCard(); });
    }

    fY = 0.78f * scr_h;

    // (6.) DoCancel
    {
        auto tex = m_game->getTex(H3_Tex::DoCancel, "UI_Actions().DoCancel");
        fX = 0.3f * scr_w;
        auto btn = new de::gui::Button("", env, this, 0, de::Recti(fX,fY,64,64));
        btn->setTexture(tex);
        btn->setBorderColor(dbRGBA(255,255,0),1);
        m_btnCancel = btn;
        m_btnCancel->setOnClicked([&](){ m_game->doPlaceCancel(); });
    }

    // (7.) DoBuyRoad
    {
        auto tex = m_game->getTex(H3_Tex::DoBuyRoad, "UI_Actions().DoBuyRoad");
        fX = 0.4f * scr_w;
        auto btn = new de::gui::Button("", env, this, 0, de::Recti(fX,fY,64,64));
        btn->setTexture(tex);
        btn->setBorderColor(dbRGBA(255,255,0),1);
        m_btnBuyRoad = btn;
        m_btnBuyRoad->setOnClicked([&](){ m_game->doBuyRoad(); });
    }

    // (8.) DoBuyFarm
    {
        auto tex = m_game->getTex(H3_Tex::DoBuyFarm, "UI_Actions().DoBuyFarm");
        fX = 0.5f * scr_w;
        auto btn = new de::gui::Button("", env, this, 0, de::Recti(fX,fY,64,64));
        btn->setTexture(tex);
        btn->setBorderColor(dbRGBA(255,255,0),1);
        m_btnBuyFarm = btn;
        m_btnBuyFarm->setOnClicked([&](){ m_game->doBuyFarm(); });
    }

    // (9.) DoBuyRoad
    {
        auto tex = m_game->getTex(H3_Tex::DoBuyCity, "UI_Actions().DoBuyCity");
        fX = 0.6f * scr_w;
        auto btn = new de::gui::Button("", env, this, 0, de::Recti(fX,fY,64,64));
        btn->setTexture(tex);
        btn->setBorderColor(dbRGBA(255,255,0),1);
        m_btnBuyCity = btn;
        m_btnBuyCity->setOnClicked([&](){ m_game->doBuyCity(); });
    }

    //UI_addElem( game, H3_UI::BtnDoRob, H3_UIElem::ImageButton, de::Recti( 300, 68,0,0),
    //            12, de::Align::TopCenter, int(H3_Tex::DoRob), "DoRob");

    //   addButton( int(H3_Tex::DoUndo), 0.2, 0.7, 0.3, 0.1, "DoUndo");
    //   addButton( int(H3_Tex::DoRedo), 0.2, 0.7, 0.3, 0.1, "DoRedo");

}

UI_Actions::~UI_Actions()
{
    DE_WARN("")
}

void UI_Actions::resizeLayout()
{
    DE_WARN("")

    const auto fScale = 0.01f * m_env->getScalePc();
    const auto scr_w = m_env->getScreenWidth();
    const auto scr_h = m_env->getScreenHeight();
    setRect(0,0,scr_w,scr_h);

    const int p = fScale * 5;
    const int w1 = fScale * m_btnDice->getMinimumSize().w;
    const int h1 = fScale * m_btnDice->getMinimumSize().h;
    const int w2 = fScale * m_btnBank->getMinimumSize().w;
    const int h2 = fScale * m_btnBank->getMinimumSize().h;

    const int w3 = fScale * m_btnCancel->getMinimumSize().w;
    const int h3 = fScale * m_btnCancel->getMinimumSize().h;

    const int w4 = fScale * m_btnBuyCard->getMinimumSize().w;
    const int h4 = fScale * m_btnBuyCard->getMinimumSize().h;


    int x = scr_w/2; //  - p - w1 - p - w2
    int y = scr_h - 1 - p - std::max(h1,h2);

    m_btnDice->setRect(x,y,w1,h1); x += w1 + p;
    m_btnBank->setRect(x,y,w2,h2); x += w2 + p;
    m_btnTrade->setRect(x,y,w2,h2); x += w2 + p;
    m_btnEndTurn->setRect(x,y,w2,h2); x += w2 + p;

    x = scr_w/2; //  - p - w3 - p - w3
    y = scr_h - 1 - p - std::max(h1,h2) - p - h3;
    m_btnCancel->setRect(x,y,w3,h3); x += w3 + p;
    m_btnBuyRoad->setRect(x,y,w3,h3); x += w3 + p;
    m_btnBuyFarm->setRect(x,y,w3,h3); x += w3 + p;
    m_btnBuyCity->setRect(x,y,w3,h3); x += w3 + p;

    y = scr_h - 1 - p - h4;
    m_btnBuyCard->setRect(x,y,w4,h4);

    DE_DEBUG("this = ", this->getRect().str())
    DE_DEBUG("m_btnDice = ", m_btnDice->getRect().str(), ", visible = ", m_btnDice->isVisible())
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
        if ( player.didDice )
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
        if ( player.didBuyFarm )
        {
            if ( player.didBuyRoad )
            {
                m_btnEndTurn->setVisible(true);
            }
            else
            {
                m_btnBuyRoad->setVisible(true);
            }
        }
        else
        {
            m_btnBuyFarm->setVisible(true);
        }
    }
    // Round - 3
    else if ( m_game->m_round == 3 )
    {
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
    }
    // Round - N
    else
    {
        if ( player.didDice )
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

