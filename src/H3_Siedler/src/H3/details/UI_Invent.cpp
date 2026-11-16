#include "UI_Invent.h"
#include <H3/H3_Game.h>

void UI_Invent::createDoubleWidget(DoubleWidget& w, int tileType)
{
    w.btn = new de::gui::Button("", m_env, this);
    w.lbl = new de::gui::Label("", m_env, this);

    de::gpu::TexRef tex;
    switch (tileType)
    {
    case H3_TileType::TileA: tex = m_game->getTex(H3_Tex::CardA, __func__); break;
    case H3_TileType::TileB: tex = m_game->getTex(H3_Tex::CardB, __func__); break;
    case H3_TileType::TileC: tex = m_game->getTex(H3_Tex::CardC, __func__); break;
    case H3_TileType::TileD: tex = m_game->getTex(H3_Tex::CardD, __func__); break;
    case H3_TileType::TileE: tex = m_game->getTex(H3_Tex::CardE, __func__); break;
    default: break;
    }

    w.btn->setTexture(tex);
    w.btn->setRadius(8);
}

// =====================================================================
UI_Invent::UI_Invent( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& pos )
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

    // Create widgets
    createDoubleWidget(m_cardA, H3_TileType::TileA);
    createDoubleWidget(m_cardB, H3_TileType::TileB);
    createDoubleWidget(m_cardC, H3_TileType::TileC);
    createDoubleWidget(m_cardD, H3_TileType::TileD);
    createDoubleWidget(m_cardE, H3_TileType::TileE);

    m_btnCancel = new de::gui::Button("Cancel", m_env, this);
    m_btnAccept = new de::gui::Button("Accept", m_env, this);

    auto tex_scroll2 = game->getTex(H3_Tex::Scroll2, __func__);
    m_btnCancel->setTexture( tex_scroll2 );
    m_btnAccept->setTexture( tex_scroll2 );

    // User '+' (LeftMouseClick)

    m_cardA.btn->onClickedLeft = [&] () {
        if (m_bank.sum() >= 2) return;
        m_bank.A++;
        updateWidgetLogic();
    };
    m_cardB.btn->onClickedLeft = [&] () {
        if (m_bank.sum() >= 2) return;
        m_bank.B++;
        updateWidgetLogic();
    };
    m_cardC.btn->onClickedLeft = [&] () {
        if (m_bank.sum() >= 2) return;
        m_bank.C++;
        updateWidgetLogic();
    };
    m_cardD.btn->onClickedLeft = [&] () {
        if (m_bank.sum() >= 2) return;
        m_bank.D++;
        updateWidgetLogic();
    };
    m_cardE.btn->onClickedLeft = [&] () {
        if (m_bank.sum() >= 2) return;
        m_bank.E++;
        updateWidgetLogic();
    };

    // User '-' (RightMouseClick)

    m_cardA.btn->onClickedRight = [&] () {
        if (m_bank.A < 1) return;
        m_bank.A--;
        updateWidgetLogic();
    };
    m_cardB.btn->onClickedRight = [&] () {
        if (m_bank.B < 1) return;
        m_bank.B--;
        updateWidgetLogic();
    };
    m_cardC.btn->onClickedRight = [&] () {
        if (m_bank.C < 1) return;
        m_bank.C--;
        updateWidgetLogic();
    };
    m_cardD.btn->onClickedRight = [&] () {
        if (m_bank.D < 1) return;
        m_bank.D--;
        updateWidgetLogic();
    };
    m_cardE.btn->onClickedRight = [&] () {
        if (m_bank.E < 1) return;
        m_bank.E--;
        updateWidgetLogic();
    };

    // Cancel logic.
    m_btnCancel->onClicked = [this] () { this->onCancel(); };
    m_btnAccept->onClicked = [this] () { this->onAccept(); };

}

UI_Invent::~UI_Invent()
{

}

void UI_Invent::updateWidgetLogic()
{
    H3_Player & player = m_game->getPlayer( m_playerId );

    m_cardA.lbl->setText(std::to_string(m_bank.A));
    m_cardB.lbl->setText(std::to_string(m_bank.B));
    m_cardC.lbl->setText(std::to_string(m_bank.C));
    m_cardD.lbl->setText(std::to_string(m_bank.D));
    m_cardE.lbl->setText(std::to_string(m_bank.E));

    if (m_bank.sum() < 2)
    {
        m_cardA.btn->setEnabled(true);
        m_cardB.btn->setEnabled(true);
        m_cardC.btn->setEnabled(true);
        m_cardD.btn->setEnabled(true);
        m_cardE.btn->setEnabled(true);
        m_btnAccept->setEnabled(false);
    }
    else
    {
        m_cardA.btn->setEnabled(false);
        m_cardB.btn->setEnabled(false);
        m_cardC.btn->setEnabled(false);
        m_cardD.btn->setEnabled(false);
        m_cardE.btn->setEnabled(false);
        m_btnAccept->setEnabled(true);
    }
}

void UI_Invent::startInvent( uint32_t playerId )
{
    m_bank.clear();
    m_playerId = playerId;
    updateWidgetLogic();
}


void UI_Invent::onCancel()
{
    DE_DEBUG("Cancel inventCard.")
    m_game->m_uiGame->popState();
    m_game->enterIdle();
}

void UI_Invent::onAccept()
{
    H3_Player & player = m_game->getPlayer( m_playerId );

    if (player.state.inventCards.empty())
    {
        DE_ERROR("No inventCards!")
        m_game->m_uiGame->popState();
        m_game->enterIdle();
        return;
    }

    DE_DEBUG("=================  Finish inventCard  =================")

    DE_DEBUG("bank = ",m_bank.str())



    for (int i = 0; i < m_bank.sum(); ++i)
    {
        H3_TileType tt = m_bank.getTileTypeFromCardIndex(i);

        // [Animation]
        int scr_w = m_game->getDriver()->getScreenWidth();
        int scr_h = m_game->getDriver()->getScreenHeight();
        int bank_x = scr_w - 50;
        int bank_y = scr_h - 100 - i * 20;

        int play_w = player.screenRect.w / 10;
        int play_h = player.screenRect.h / 2;
        int play_x = player.screenRect.x + play_w + 2*play_w*(tt - H3_TileType::TileA);
        int play_y = player.screenRect.y + play_h;

        auto ani = new H3_Card2DAnimation(m_game,{bank_x,bank_y},{play_x,play_y},tt);
        m_game->m_animations.addAnimation(ani);
    }

    DE_DEBUG("player.before = ", player.state.bank.str())
    player.state.bank.add(m_bank);
    DE_DEBUG("player.after = ", player.state.bank.str())

    u32 cardId = player.state.inventCards.back();
    player.state.inventCards.pop_back();
    player.state.invents.push_back(cardId);

    m_game->m_uiGame->popState();
    m_game->enterIdle();
}

void UI_Invent::resize(int X, int Y, int W, int H)
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
    int bank_h = 2*H / 10;
    int name_h = H / 10;
    int btns_h = H / 10;

    int s = (H - 3*p - head_h - bank_h - name_h - btns_h) / 3;

    int head_w = 8 * W /10;
    int head_x = (W - head_w)/2;
    int head_y = p;
    m_rcHead = de::Recti(head_x, head_y, head_w, head_h);

    int bank_w = 7 * W /10;
    int bank_x = (W - bank_w)/2;
    int bank_y = p + head_h + s;
    m_rcBank = de::Recti(bank_x - fontSize, bank_y, bank_w, bank_h);

    int name_w = 7 * W /10;
    int name_x = (W - name_w)/2;
    int name_y = p + head_h + s + bank_h + s;
    m_rcName = de::Recti(name_x, name_y, name_w, name_h);

    int btns_w = 7 * W /10;
    int btns_x = (W - btns_w)/2;
    int btns_y = p + head_h + s + bank_h + s + name_h + p;
    m_rcBtns = de::Recti(btns_x, btns_y, btns_w, btns_h);

    // 1x Bank Icon + 5x Bank buttons
    {
        int w = (m_rcBank.w - 5*p) / 6;
        int h = m_rcBank.h;
        int x = m_rcBank.x + w + p;
        int y = m_rcBank.y;
        int ln = zoom * 28;
        int p2 = 10;
        m_cardA.resize(x,y,w,h,p2,ln,font); x += w + p;
        m_cardB.resize(x,y,w,h,p2,ln,font); x += w + p;
        m_cardC.resize(x,y,w,h,p2,ln,font); x += w + p;
        m_cardD.resize(x,y,w,h,p2,ln,font); x += w + p;
        m_cardE.resize(x,y,w,h,p2,ln,font);
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

void UI_Invent::draw()
{
    Panel::draw();
    if (!isVisible()) return;

    auto driver = m_env->getVideoDriver();

    // <debug>
    auto red = dbRGB(255,0,0);
    auto pos = getAbsoluteRect().pos();
    driver->getScreenRenderer()->draw2DRectLine(m_rcHead+pos,red);
    driver->getScreenRenderer()->draw2DRectLine(m_rcBank+pos,red);
    driver->getScreenRenderer()->draw2DRectLine(m_rcName+pos,red);
    driver->getScreenRenderer()->draw2DRectLine(m_rcBtns+pos,red);
    // </debug>

    drawTitle();
    // drawBankIconAndName();
    // drawUserIcon();
    // drawUserName();

    for (auto & child : m_children)
    {
        child->draw();
    }
}

void UI_Invent::drawTitle()
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
        dbStr("Invention of ",player.name,", get 2 cards."),
        dbRGB(0,0,0),
        de::Align::Centered,
        de::Font("Postamt", zoom * 24),
        dbRGB(255,255,255,225),
        10);
}

/*
void UI_Invent::drawBankIconAndName()
{
    auto driver = m_env->getVideoDriver();

    auto pos = getAbsoluteRect();

    auto zoom = m_env->getZoom();

    int p = zoom * 10;

    // Icon "Bank"
    int icon_w = (m_rcBank.w - 5*p)/6;
    int icon_h = icon_w;
    int icon_x = pos.x + m_rcBank.x;
    int icon_y = pos.y + m_rcBank.y + (m_rcBank.h - icon_h)/2 - 12;
    auto r_icon = de::Recti(icon_x,icon_y,icon_w,icon_h);
    auto icon = m_game->getTex(H3_Tex::DoBank,__func__);


    driver->getScreenRenderer()->
        draw2DRoundRect(r_icon, {8,8}, icon);

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

    // Text "Bank"
    int text_w = (m_rcBank.w - 5*p)/6;
    int text_h = text_w;
    int text_x = pos.x + m_rcBank.x;
    int text_y = pos.y + m_rcBank.y + (m_rcBank.h - text_h)/2 - 12;
    auto text_rect = de::Recti(text_x, text_y, text_w, text_h);
    driver->draw2DText(
        text_rect.x2() - p,
        text_rect.centerY(),
        "Bank",
        dbRGB(0,0,0),
        de::Align::RightMiddle,
        de::Font("Postamt", zoom * 24),
        dbRGB(255,255,255,225),
        10);

}

void UI_Invent::drawUserIcon()
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

void UI_Invent::drawUserName()
{

}

*/
