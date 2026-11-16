#include "UI_Monopol.h"
#include <H3/H3_Game.h>

// =====================================================================
UI_Monopol::UI_Monopol( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& pos )
// =====================================================================
    : de::gui::Panel(env, parent, id, pos)
    , m_game(game)
    , m_playerId(0)
{
    setDrawChildren(false);

    // Compute card aspect ratio
    auto tex_card = game->getTex(H3_Tex::CardA, __func__);
    m_aspect = float(tex_card.w()) / float(tex_card.h());

    // Set dialog panel background texture
    auto tex_scroll = game->getTex(H3_Tex::Scroll, __func__);
    setTexture( tex_scroll );
    setFillColor( 0xffffffff );

    m_btnCardA = new de::gui::Button("", m_env, this);
    m_btnCardA->setTexture( game->getTex(H3_Tex::CardA, __func__) );
    m_btnCardA->setRadius(8);

    m_btnCardB = new de::gui::Button("", m_env, this);
    m_btnCardB->setTexture( game->getTex(H3_Tex::CardB, __func__) );
    m_btnCardB->setRadius(8);

    m_btnCardC = new de::gui::Button("", m_env, this);
    m_btnCardC->setTexture( game->getTex(H3_Tex::CardC, __func__) );
    m_btnCardC->setRadius(8);

    m_btnCardD = new de::gui::Button("", m_env, this);
    m_btnCardD->setTexture( game->getTex(H3_Tex::CardD, __func__) );
    m_btnCardD->setRadius(8);

    m_btnCardE = new de::gui::Button("", m_env, this);
    m_btnCardE->setTexture( game->getTex(H3_Tex::CardE, __func__) );
    m_btnCardE->setRadius(8);

    m_btnCancel = new de::gui::Button("Cancel", m_env, this);
    m_btnCancel->setTexture( game->getTex(H3_Tex::Scroll2, __func__) );

    m_btnAccept = new de::gui::Button("Accept", m_env, this);
    m_btnAccept->setTexture( game->getTex(H3_Tex::Scroll2, __func__) );

    // User '+' (LeftMouseClick)

    m_btnCardA->onClickedLeft = [&] () {
        if (m_bank.sum() > 0) return;
        m_bank.A = 1;
        updateWidgetLogic();
    };

    m_btnCardB->onClickedLeft = [&] () {
        if (m_bank.sum() > 0) return;
        m_bank.B = 1;
        updateWidgetLogic();
    };

    m_btnCardC->onClickedLeft = [&] () {
        if (m_bank.sum() > 0) return;
        m_bank.C = 1;
        updateWidgetLogic();
    };

    m_btnCardD->onClickedLeft = [&] () {
        if (m_bank.sum() > 0) return;
        m_bank.D = 1;
        updateWidgetLogic();
    };

    m_btnCardE->onClickedLeft = [&] () {
        if (m_bank.sum() > 0) return;
        m_bank.E = 1;
        updateWidgetLogic();
    };

    // User '-' (RightMouseClick)

    m_btnCardA->onClickedRight = [&] () {
        m_bank.A = 0;
        updateWidgetLogic();
    };

    m_btnCardB->onClickedRight = [&] () {
        m_bank.B = 0;
        updateWidgetLogic();
    };

    m_btnCardC->onClickedRight = [&] () {
        m_bank.C = 0;
        updateWidgetLogic();
    };

    m_btnCardD->onClickedRight = [&] () {
        m_bank.D = 0;
        updateWidgetLogic();
    };

    m_btnCardE->onClickedRight = [&] () {
        m_bank.E = 0;
        updateWidgetLogic();
    };

    // Cancel logic.
    m_btnCancel->onClicked = [this] () { this->onCancel(); };
    m_btnAccept->onClicked = [this] () { this->onAccept(); };
}

UI_Monopol::~UI_Monopol()
{

}

void UI_Monopol::startMonopol( uint32_t playerId )
{
    m_playerId = playerId;
    m_bank.clear();
    updateWidgetLogic();
}

void UI_Monopol::updateWidgetLogic()
{
    if (m_bank.sum())
    {
        m_btnCardA->setEnabled(false);
        m_btnCardB->setEnabled(false);
        m_btnCardC->setEnabled(false);
        m_btnCardD->setEnabled(false);
        m_btnCardE->setEnabled(false);
        m_btnAccept->setEnabled(true);
    }
    else
    {
        m_btnCardA->setEnabled(true);
        m_btnCardB->setEnabled(true);
        m_btnCardC->setEnabled(true);
        m_btnCardD->setEnabled(true);
        m_btnCardE->setEnabled(true);
        m_btnAccept->setEnabled(false);
    }
}

void UI_Monopol::onCancel()
{
    DE_DEBUG("Cancel monopol.")

    // Restore old Game GUI state.
    m_game->m_uiGame->popState();
    m_game->enterIdle();
}

void UI_Monopol::onAccept()
{
    if (m_bank.sum() < 1) return;

    H3_Player & winner = m_game->getPlayer( m_playerId );

    if (winner.state.monopolCards.empty())
    {
        DE_ERROR("No monopolCards!")
        m_game->m_uiGame->popState();
        m_game->enterIdle();
        return;
    }

    DE_DEBUG("=================  Finish monopol  =================")

    H3_TileType tt = m_bank.getTileTypeFromCardIndex(0);

    int totalAmount = 0;

    for (H3_Player & player : m_game->m_players)
    {
        if (player.id == m_playerId) continue;

        int amount = player.state.bank.getAmount(tt);

        DE_DEBUG("Player[",player.id,"] ",player.name," has ",amount," cards")

        totalAmount += amount;

        for (int i = 0; i < amount; ++i)
        {
            // [Animation]
            int w1 = player.screenRect.w / 10;
            int h1 = player.screenRect.h / 2;
            int x1 = player.screenRect.x + w1 + 2*w1*(tt - H3_TileType::TileA);
            int y1 = player.screenRect.y + h1;

            int w2 = winner.screenRect.w / 10;
            int h2 = winner.screenRect.h / 2;
            int x2 = winner.screenRect.x + w2 + 2*w2*(tt - H3_TileType::TileA);
            int y2 = winner.screenRect.y + h2;

            auto ani = new H3_Card2DAnimation(m_game,{x1,y1},{x1,y1},tt);
            m_game->m_animations.addAnimation(ani);
        }

        player.state.bank.setAmount( tt, 0 );
    }

    DE_DEBUG("Winner[",winner.id,"] ",winner.name," gets ",totalAmount," cards.")

    winner.state.bank.addAmount( tt, totalAmount );


    u32 cardId = winner.state.monopolCards.back();
    winner.state.monopolCards.pop_back();
    winner.state.monopols.push_back(cardId);

    // Restore old Game GUI state.
    m_game->m_uiGame->popState();
    m_game->enterIdle();
}


void UI_Monopol::resize(int X, int Y, int W, int H)
{
    m_rect = de::Recti(X,Y,W,H);

    const float zoom = m_env->getZoom();
    const int p = zoom * 10;
    const int fontSize = zoom * 24;

    auto font = de::Font("Postamt",fontSize);
    setFont(font);

    // m_btnCancel->setFont(font);
    // m_btnAccept->setFont(font);

    int head_h = H / 10;
    int card_h = 2*H / 10;
    int name_h = H / 10;
    int btns_h = H / 10;

    int s = (H - 3*p - head_h - card_h - name_h - btns_h) / 3;

    int head_w = 8 * W /10;
    int head_x = (W - head_w)/2;
    int head_y = p;
    m_rcHead = de::Recti(head_x, head_y, head_w, head_h);

    int card_w = 7 * W /10;
    int card_x = (W - card_w)/2;
    int card_y = p + head_h + s;
    m_rcCard = de::Recti(card_x - fontSize, card_y, card_w, card_h);

    int name_w = 7 * W /10;
    int name_x = (W - name_w)/2;
    int name_y = p + head_h + s + card_h + s;
    m_rcName = de::Recti(name_x, name_y, name_w, name_h);

    int btns_w = 7 * W /10;
    int btns_x = (W - btns_w)/2;
    int btns_y = p + head_h + s + card_h + s + name_h + p;
    m_rcBtns = de::Recti(btns_x, btns_y, btns_w, btns_h);

    // 1x Bank Icon + 5x Bank buttons
    {
        int w = (m_rcCard.w - 5*p) / 6;
        int h = m_rcCard.h;
        int x = m_rcCard.x + w + p;
        int y = m_rcCard.y;
        int ln = zoom * 28;
        int p2 = 10;
        m_btnCardA->resize(x,y,w,h); x += w + p;
        m_btnCardB->resize(x,y,w,h); x += w + p;
        m_btnCardC->resize(x,y,w,h); x += w + p;
        m_btnCardD->resize(x,y,w,h); x += w + p;
        m_btnCardE->resize(x,y,w,h);
    }

    // Cancel + Accept buttons
    {
        int p3 = zoom * 40;
        int w = (m_rcBtns.w - p3) / 2;
        int h = m_rcBtns.h;
        int x = m_rcBtns.x;
        int y = m_rcBtns.y;
        int ln = zoom * 24;
        m_btnCancel->resize(x,y,w,h); x += w + p3;
        m_btnAccept->resize(x,y,w,h);
    }
}

void UI_Monopol::draw()
{
    Panel::draw();
    if (!isVisible()) return;

    auto driver = m_env->getVideoDriver();

    // <debug>
    auto red = dbRGB(255,0,0);
    auto pos = getAbsoluteRect().pos();
    driver->getScreenRenderer()->draw2DRectLine(m_rcHead+pos,red);
    driver->getScreenRenderer()->draw2DRectLine(m_rcCard+pos,red);
    driver->getScreenRenderer()->draw2DRectLine(m_rcName+pos,red);
    driver->getScreenRenderer()->draw2DRectLine(m_rcBtns+pos,red);
    // </debug>

    drawTitle();
    // drawUserIcon();
    // drawUserName();

    for (auto & child : m_children)
    {
        child->draw();
    }
}

void UI_Monopol::drawTitle()
{
    auto driver = m_env->getVideoDriver();
    auto pos = getAbsoluteRect();
    auto zoom = m_env->getZoom();
    int p = zoom * 10;

    int x = pos.x + m_rcHead.x;
    int y = pos.y + m_rcHead.y;
    int w = m_rcHead.w;
    int h = m_rcHead.h;
    auto r_title = de::Recti(x,y,w,h);

    auto bg = m_game->getTex(H3_Tex::Scroll1,__func__);
    driver->getScreenQuadRenderer()->
        draw2DRect(r_title, bg, true);

    // // Thief icon
    // int icon_s = r_title.w / 10;
    // int icon_h = (r_title.h * 8) / 10;
    // int icon_w = icon_h;
    // int icon_x = r_title.x + icon_s;
    // int icon_y = r_title.y + r_title.h / 10;
    // auto r_icon = de::Recti(icon_x, icon_y, icon_w, icon_h);
    // auto t_icon = m_game->getTex(H3_Tex::Benni,__func__);

    // driver->getScreenQuadRenderer()->
    //     draw2DRect(r_icon, t_icon, true);

    auto & player = m_game->getPlayer(m_playerId);

    // Banking
    driver->draw2DText(
        r_title.centerX(),
        r_title.centerY(),
        dbStr("Monopol for ",player.name),
        dbRGB(0,0,0),
        de::Align::Centered,
        de::Font("Postamt", zoom * 24),
        dbRGB(255,255,255,225),
        10);
}

/*
void UI_Monopol::drawUserIcon()
{
    auto driver = m_env->getVideoDriver();

    auto pos = getAbsoluteRect();

    auto zoom = m_env->getZoom();

    auto & player = m_game->getPlayer(m_playerId);

    auto player_icon = m_game->getTex((H3_Tex::eID)player.avatar,__func__);

    // Icon "Player"
    int p = zoom * 10;
    int icon_w = (m_rcUser.w - 5*p)/6;
    int icon_h = icon_w;
    int icon_x = pos.x + m_rcUser.x;
    int icon_y = pos.y + m_rcUser.y + (m_rcUser.h - icon_h)/2;
    auto r_icon = de::Recti(icon_x,icon_y,icon_w,icon_h);

    driver->getScreenRenderer()->
        draw2DRoundRect(r_icon, {8,8}, player_icon);

    // Text "Bank"
    driver->draw2DText(
        r_icon.centerX(),
        r_icon.y2() + p,
        "Bank",
        dbRGB(0,0,0),
        de::Align::Centered,
        de::Font("Postamt", zoom * 24),
        dbRGB(255,255,255,225),
        10);
}

void UI_Monopol::drawUserName()
{

}
*/
