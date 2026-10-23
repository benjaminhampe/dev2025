#include "UI_Banking.h"
#include <H3/H3_Game.h>

void UI_Banking::createTripleWidget(TripleWidget& w, int tileType)
{
    w.lup = new de::gui::Label("", m_env, this);
    w.btn = new de::gui::Button("", m_env, this);
    w.ldn = new de::gui::Label("", m_env, this);

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

void UI_Banking::updateWidgetLogic()
{
    H3_Player & player = m_game->getPlayer( m_playerId );

    // Update user
    m_userA.lup->setText(std::to_string(m_user.A)); // given
    m_userB.lup->setText(std::to_string(m_user.B)); // given
    m_userC.lup->setText(std::to_string(m_user.C)); // given
    m_userD.lup->setText(std::to_string(m_user.D)); // given
    m_userE.lup->setText(std::to_string(m_user.E)); // given

    const int remainA = player.state.bank.A - m_user.A;
    const int remainB = player.state.bank.B - m_user.B;
    const int remainC = player.state.bank.C - m_user.C;
    const int remainD = player.state.bank.D - m_user.D;
    const int remainE = player.state.bank.E - m_user.E;

    m_userA.btn->setEnabled(remainA >= m_ratio.A); // remain
    m_userB.btn->setEnabled(remainB >= m_ratio.B); // remain
    m_userC.btn->setEnabled(remainC >= m_ratio.C); // remain
    m_userD.btn->setEnabled(remainD >= m_ratio.D); // remain
    m_userE.btn->setEnabled(remainE >= m_ratio.E); // remain

    m_userA.ldn->setText(std::to_string(remainA)); // remain
    m_userB.ldn->setText(std::to_string(remainB)); // remain
    m_userC.ldn->setText(std::to_string(remainC)); // remain
    m_userD.ldn->setText(std::to_string(remainD)); // remain
    m_userE.ldn->setText(std::to_string(remainE)); // remain

    m_bankA.ldn->setText(std::to_string(m_bank.A));
    m_bankB.ldn->setText(std::to_string(m_bank.B));
    m_bankC.ldn->setText(std::to_string(m_bank.C));
    m_bankD.ldn->setText(std::to_string(m_bank.D));
    m_bankE.ldn->setText(std::to_string(m_bank.E));

    // Update bank

    int sumBank = m_bank.sum();
    int sumUser = (m_user.A / m_ratio.A)
                + (m_user.B / m_ratio.B)
                + (m_user.C / m_ratio.C)
                + (m_user.D / m_ratio.D)
                + (m_user.E / m_ratio.E);

    // Prevent cheating (yet), clear bank if user removed some cards...
    if (sumBank > sumUser)
    {
        m_bank.clear();
        m_bankA.ldn->setText("0");
        m_bankB.ldn->setText("0");
        m_bankC.ldn->setText("0");
        m_bankD.ldn->setText("0");
        m_bankE.ldn->setText("0");
        sumBank = 0;
    }

    const bool bCanBank = (sumUser > 0) && (sumUser > sumBank);
/*
    const bool bCanBank =
        (m_user.A >= m_ratio.A) ||
        (m_user.B >= m_ratio.B) ||
        (m_user.C >= m_ratio.C) ||
        (m_user.D >= m_ratio.D) ||
        (m_user.E >= m_ratio.E);
*/

    m_bankA.btn->setEnabled(bCanBank);
    m_bankB.btn->setEnabled(bCanBank);
    m_bankC.btn->setEnabled(bCanBank);
    m_bankD.btn->setEnabled(bCanBank);
    m_bankE.btn->setEnabled(bCanBank);

    m_btnAccept->setEnabled( m_bank.sum() > 0 );
}

void UI_Banking::onCancel()
{
    DE_DEBUG("Cancel banking.")

    // Restore old Game GUI state.
    m_game->m_uiGame->popState();
    m_game->enterIdle();
}

void UI_Banking::onAccept()
{
    DE_DEBUG("=================  Finish banking  =================")

    H3_Player & player = m_game->getPlayer( m_playerId );

    DE_DEBUG("player.ratio ", m_ratio.str())
    DE_DEBUG("player.bank.old = ",player.state.bank.str())
    DE_DEBUG("banking.user = ",m_user.str())
    DE_DEBUG("banking.bank = ",m_bank.str())

    // Check
    const int sumBank = m_bank.sum();
    const int sumUser = (m_user.A / m_ratio.A)
                      + (m_user.B / m_ratio.B)
                      + (m_user.C / m_ratio.C)
                      + (m_user.D / m_ratio.D)
                      + (m_user.E / m_ratio.E);
    const bool bAccept = (sumUser >= sumBank);
    if (bAccept)
    {
        player.state.bank.sub(m_user);
        player.state.bank.add(m_bank);
        DE_DEBUG("player.bank.new = ",player.state.bank.str())
    }

    // Restore old Game GUI state.
    m_game->m_uiGame->popState();
    m_game->enterIdle();
}

// =====================================================================
UI_Banking::UI_Banking( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& pos )
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
    createTripleWidget(m_bankA, H3_TileType::TileA);
    createTripleWidget(m_bankB, H3_TileType::TileB);
    createTripleWidget(m_bankC, H3_TileType::TileC);
    createTripleWidget(m_bankD, H3_TileType::TileD);
    createTripleWidget(m_bankE, H3_TileType::TileE);

    createTripleWidget(m_userA, H3_TileType::TileA);
    createTripleWidget(m_userB, H3_TileType::TileB);
    createTripleWidget(m_userC, H3_TileType::TileC);
    createTripleWidget(m_userD, H3_TileType::TileD);
    createTripleWidget(m_userE, H3_TileType::TileE);

    m_btnCancel = new de::gui::Button("Cancel", m_env, this);
    m_btnAccept = new de::gui::Button("Accept", m_env, this);

    auto tex_scroll2 = game->getTex(H3_Tex::Scroll2, __func__);
    m_btnCancel->setTexture( tex_scroll2 );
    m_btnAccept->setTexture( tex_scroll2 );

    // Cancel logic.
    m_btnCancel->onClicked = [this] () { this->onCancel(); };
    m_btnAccept->onClicked = [this] () { this->onAccept(); };

    // User '+' (LeftMouseClick)

    m_userA.btn->onClickedLeft = [&] () {
        //if (!m_bankA.btn->isEnabled()) { return; }
        H3_Player & player = m_game->getPlayer( m_playerId );
        if (m_user.A + m_ratio.A <= player.state.bank.A) { m_user.A += m_ratio.A; }
        updateWidgetLogic();
    };

    m_userB.btn->onClickedLeft = [&] () {
        //if (!m_bankB.btn->isEnabled()) { return; }
        H3_Player & player = m_game->getPlayer( m_playerId );
        if (m_user.B + m_ratio.B <= player.state.bank.B) { m_user.B += m_ratio.B; }
        updateWidgetLogic();
    };

    m_userC.btn->onClickedLeft = [&] () {
        //if (!m_bankC.btn->isEnabled()) { return; }
        H3_Player & player = m_game->getPlayer( m_playerId );
        if (m_user.C + m_ratio.C <= player.state.bank.C) { m_user.C += m_ratio.C; }
        updateWidgetLogic();
    };

    m_userD.btn->onClickedLeft = [&] () {
        //if (!m_bankD.btn->isEnabled()) { return; }
        H3_Player & player = m_game->getPlayer( m_playerId );
        if (m_user.D + m_ratio.D <= player.state.bank.D) { m_user.D += m_ratio.D; }
        updateWidgetLogic();
    };

    m_userE.btn->onClickedLeft = [&] () {
        //if (!m_bankE.btn->isEnabled()) { return; }
        H3_Player & player = m_game->getPlayer( m_playerId );
        if (m_user.E + m_ratio.E <= player.state.bank.E) { m_user.E += m_ratio.E; }
        updateWidgetLogic();
    };

    // User '-' (RightMouseClick)

    m_userA.btn->onClickedRight = [&] () {
        if (m_user.A >= m_ratio.A) { m_user.A -= m_ratio.A; }
        updateWidgetLogic();
    };

    m_userB.btn->onClickedRight = [&] () {
        if (m_user.B >= m_ratio.B) { m_user.B -= m_ratio.B; }
        updateWidgetLogic();
    };

    m_userC.btn->onClickedRight = [&] () {
        if (m_user.C >= m_ratio.C) { m_user.C -= m_ratio.C; }
        updateWidgetLogic();
    };

    m_userD.btn->onClickedRight = [&] () {
        if (m_user.D >= m_ratio.D) { m_user.D -= m_ratio.D; }
        updateWidgetLogic();
    };

    m_userE.btn->onClickedRight = [&] () {
        if (m_user.E >= m_ratio.E) { m_user.E -= m_ratio.E; }
        updateWidgetLogic();
    };


   // Bank '+' (LeftMouseClick)

    m_bankA.btn->onClickedLeft = [&] () {
        if (!m_bankA.btn->isEnabled()) { return; }
        m_bank.A++;
        updateWidgetLogic();
    };

    m_bankB.btn->onClickedLeft = [&] () {
        if (!m_bankB.btn->isEnabled()) { return; }
        m_bank.B++;
        updateWidgetLogic();
    };

    m_bankC.btn->onClickedLeft = [&] () {
        if (!m_bankC.btn->isEnabled()) { return; }
        m_bank.C++;
        updateWidgetLogic();
    };

    m_bankD.btn->onClickedLeft = [&] () {
        if (!m_bankD.btn->isEnabled()) { return; }
        m_bank.D++;
        updateWidgetLogic();
    };

    m_bankE.btn->onClickedLeft = [&] () {
        if (!m_bankE.btn->isEnabled()) { return; }
        m_bank.E++;
        updateWidgetLogic();
    };


   // Bank '-' (RightMouseClick)

    m_bankA.btn->onClickedRight = [&] () {
        if (m_bank.A <= 0) return;
        m_bank.A--;
        updateWidgetLogic();
    };

    m_bankB.btn->onClickedRight = [&] () {
        if (m_bank.B <= 0) return;
        m_bank.B--;
        updateWidgetLogic();
    };

    m_bankC.btn->onClickedRight = [&] () {
        if (m_bank.C <= 0) return;
        m_bank.C--;
        updateWidgetLogic();
    };

    m_bankD.btn->onClickedRight = [&] () {
        if (m_bank.D <= 0) return;
        m_bank.D--;
        updateWidgetLogic();
    };

    m_bankE.btn->onClickedRight = [&] () {
        if (m_bank.E <= 0) return;
        m_bank.E--;
        updateWidgetLogic();
    };
}

UI_Banking::~UI_Banking()
{

}

void UI_Banking::startBanking( uint32_t playerId )
{
    m_playerId = playerId;

    const H3_Player & player = m_game->getPlayer( playerId );

    DE_DEBUG("player[",player.id,"] ", player.name)

    m_ratio = player.getBankingRatios();
    m_bank.clear();
    m_user.clear();

    // Bank:

    m_bankA.lup->setText( dbStr(m_ratio.A,":1") );
    m_bankB.lup->setText( dbStr(m_ratio.B,":1") );
    m_bankC.lup->setText( dbStr(m_ratio.C,":1") );
    m_bankD.lup->setText( dbStr(m_ratio.D,":1") );
    m_bankE.lup->setText( dbStr(m_ratio.E,":1") );

    m_bankA.btn->setEnabled(false);
    m_bankB.btn->setEnabled(false);
    m_bankC.btn->setEnabled(false);
    m_bankD.btn->setEnabled(false);
    m_bankE.btn->setEnabled(false);

    m_bankA.ldn->setText( "0" );
    m_bankB.ldn->setText( "0" );
    m_bankC.ldn->setText( "0" );
    m_bankD.ldn->setText( "0" );
    m_bankE.ldn->setText( "0" );

    // User:

    m_userA.lup->setText( "0" );
    m_userB.ldn->setText( "0" );
    m_userC.ldn->setText( "0" );
    m_userD.ldn->setText( "0" );
    m_userE.ldn->setText( "0" );

    m_userA.btn->setEnabled(player.state.bank.A >= m_ratio.A);
    m_userB.btn->setEnabled(player.state.bank.B >= m_ratio.B);
    m_userC.btn->setEnabled(player.state.bank.C >= m_ratio.C);
    m_userD.btn->setEnabled(player.state.bank.D >= m_ratio.D);
    m_userE.btn->setEnabled(player.state.bank.E >= m_ratio.E);

    m_userA.ldn->setText( std::to_string(player.state.bank.A) );
    m_userB.ldn->setText( std::to_string(player.state.bank.B) );
    m_userC.ldn->setText( std::to_string(player.state.bank.C) );
    m_userD.ldn->setText( std::to_string(player.state.bank.D) );
    m_userE.ldn->setText( std::to_string(player.state.bank.E) );

    updateWidgetLogic();
}

void UI_Banking::resize(int X, int Y, int W, int H)
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
    int user_h = 2*H / 10;
    int name_h = H / 10;
    int btns_h = H / 10;

    int s = (H - 3*p - head_h - bank_h - user_h - name_h - btns_h) / 3;

    int head_w = 8 * W /10;
    int head_x = (W - head_w)/2;
    int head_y = p;
    m_rcHead = de::Recti(head_x, head_y, head_w, head_h);

    int bank_w = 7 * W /10;
    int bank_x = (W - bank_w)/2;
    int bank_y = p + head_h + s;
    m_rcBank = de::Recti(bank_x - fontSize, bank_y, bank_w, bank_h);

    int user_w = 7 * W /10;
    int user_x = (W - user_w)/2;
    int user_y = p + head_h + s + bank_h + s;
    m_rcUser = de::Recti(user_x - fontSize, user_y, user_w, user_h);

    int name_w = 7 * W /10;
    int name_x = (W - name_w)/2;
    int name_y = p + head_h + s + bank_h + s + user_h + s;
    m_rcName = de::Recti(name_x, name_y, name_w, name_h);

    int btns_w = 7 * W /10;
    int btns_x = (W - btns_w)/2;
    int btns_y = p + head_h + s + bank_h + s + user_h + s + name_h + p;
    m_rcBtns = de::Recti(btns_x, btns_y, btns_w, btns_h);

    // 1x Bank Icon + 5x Bank buttons
    {
        int w = (m_rcBank.w - 5*p) / 6;
        int h = m_rcBank.h;
        int x = m_rcBank.x + w + p;
        int y = m_rcBank.y;
        int ln = zoom * 28;
        int p2 = 10;
        m_bankA.resize(x,y,w,h,p2,ln,font); x += w + p;
        m_bankB.resize(x,y,w,h,p2,ln,font); x += w + p;
        m_bankC.resize(x,y,w,h,p2,ln,font); x += w + p;
        m_bankD.resize(x,y,w,h,p2,ln,font); x += w + p;
        m_bankE.resize(x,y,w,h,p2,ln,font);
    }

    // 1x User Icon + 5x User buttons
    {
        int w = (m_rcUser.w - 5*p) / 6;
        int h = m_rcUser.h;
        int x = m_rcUser.x + w + p;
        int y = m_rcUser.y;
        int ln = zoom * 24;
        int p2 = 10;
        m_userA.resize(x,y,w,h,p2,ln,font); x += w + p;
        m_userB.resize(x,y,w,h,p2,ln,font); x += w + p;
        m_userC.resize(x,y,w,h,p2,ln,font); x += w + p;
        m_userD.resize(x,y,w,h,p2,ln,font); x += w + p;
        m_userE.resize(x,y,w,h,p2,ln,font);
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

void UI_Banking::draw()
{
    Panel::draw();
    if (!isVisible()) return;

    auto driver = m_env->getVideoDriver();

    // <debug>
    auto red = dbRGB(255,0,0);
    auto pos = getAbsoluteRect().pos();
    driver->getScreenRenderer()->draw2DRectLine(m_rcHead+pos,red);
    driver->getScreenRenderer()->draw2DRectLine(m_rcBank+pos,red);
    driver->getScreenRenderer()->draw2DRectLine(m_rcUser+pos,red);
    driver->getScreenRenderer()->draw2DRectLine(m_rcName+pos,red);
    driver->getScreenRenderer()->draw2DRectLine(m_rcBtns+pos,red);
    // </debug>

    drawTitle();
    drawBankIconAndName();
    drawUserIcon();
    drawUserName();

    for (auto & child : m_children)
    {
        child->draw();
    }
}

void UI_Banking::drawTitle()
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
        dbStr("Banking for ",player.name),
        dbRGB(0,0,0),
        de::Align::Centered,
        de::Font("Postamt", zoom * 24),
        dbRGB(255,255,255,225),
        10);
}

void UI_Banking::drawBankIconAndName()
{
    auto driver = m_env->getVideoDriver();

    auto pos = getAbsoluteRect();

    auto zoom = m_env->getZoom();

    int p = zoom * 10;
/*
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
*/
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

void UI_Banking::drawUserIcon()
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

/*
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
*/
}

void UI_Banking::drawUserName()
{

}

