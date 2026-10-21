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

int countTypes(const H3_Bank& bank)
{
    int n = 0;
    if (bank.A) n++;
    if (bank.B) n++;
    if (bank.C) n++;
    if (bank.D) n++;
    if (bank.E) n++;
    return n;
}

int getRatio(int tt, const H3_Player & player)
{
    if ( tt == 0 && player.state.hasPortA() )
    {
        return 2;
    }
    if ( tt == 1 && player.state.hasPortB() )
    {
        return 2;
    }
    if ( tt == 2 && player.state.hasPortC() )
    {
        return 2;
    }
    if ( tt == 3 && player.state.hasPortD() )
    {
        return 2;
    }
    if ( tt == 4 && player.state.hasPortE() )
    {
        return 2;
    }

    if ( player.state.hasPort3v1() ) return 3;

    return 4;
}

void UI_Banking::updateUserSide(int lastType)
{
    H3_Player & player = m_game->getPlayer( m_playerId );

    // Update user

    // Clear user bank, except last used resource type...
    // int numTypes = countTypes(m_user);
    // if (numTypes > 1)
    // {
    //     if (lastType != H3_TileType::TileA) m_user.A = 0;
    //     if (lastType != H3_TileType::TileB) m_user.B = 0;
    //     if (lastType != H3_TileType::TileC) m_user.C = 0;
    //     if (lastType != H3_TileType::TileD) m_user.D = 0;
    //     if (lastType != H3_TileType::TileE) m_user.E = 0;
    // }

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

    DE_DEBUG("Ratios "
             "A(",getRatio(0,player),"), "
             "B(",getRatio(1,player),"), "
             "C(",getRatio(2,player),"), "
             "D(",getRatio(3,player),"), "
             "E(",getRatio(4,player),")")

    m_userA.btn->setEnabled(remainA >= getRatio(0,player)); // remain
    m_userB.btn->setEnabled(remainB >= getRatio(1,player)); // remain
    m_userC.btn->setEnabled(remainC >= getRatio(2,player)); // remain
    m_userD.btn->setEnabled(remainD >= getRatio(3,player)); // remain
    m_userE.btn->setEnabled(remainE >= getRatio(4,player)); // remain

    m_userA.ldn->setText(std::to_string(remainA)); // remain
    m_userB.ldn->setText(std::to_string(remainB)); // remain
    m_userC.ldn->setText(std::to_string(remainC)); // remain
    m_userD.ldn->setText(std::to_string(remainD)); // remain
    m_userE.ldn->setText(std::to_string(remainE)); // remain

    // Update bank
    const bool bCanBank =
        (m_user.A >= getRatio(0, player)) ||
        (m_user.B >= getRatio(1, player)) ||
        (m_user.C >= getRatio(2, player)) ||
        (m_user.D >= getRatio(3, player)) ||
        (m_user.E >= getRatio(4, player));

    m_bankA.btn->setEnabled(bCanBank);
    m_bankB.btn->setEnabled(bCanBank);
    m_bankC.btn->setEnabled(bCanBank);
    m_bankD.btn->setEnabled(bCanBank);
    m_bankE.btn->setEnabled(bCanBank);
}

void UI_Banking::updateBankSide()
{

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

    m_btnCancel->onClicked = [&] ()
        {
            DE_DEBUG("Cancel banking.")

            // Restore old Game GUI state.
            m_game->m_uiGame->popState();
            m_game->enterIdle();
        };

    // Accept logic.

    m_btnAccept->onClicked = [&] ()
        {
             DE_DEBUG("Accept banking.")

            H3_Player & player = m_game->getPlayer( m_playerId );

            // Restore old Game GUI state.
            m_game->m_uiGame->popState();
            m_game->enterIdle();
        };

    // User '+' (LeftMouseClick)

    m_userA.btn->onClickedLeft = [&] () {
        if (!isEnabled()) { DE_TRACE("No A") }
        H3_Player & player = m_game->getPlayer( m_playerId );
        if (m_user.A < player.state.bank.A) { m_user.A++; }
        updateUserSide(H3_TileType::TileA);
        updateBankSide();
    };

    m_userB.btn->onClickedLeft = [&] () {
        if (!isEnabled()) { DE_TRACE("No B") }
        H3_Player & player = m_game->getPlayer( m_playerId );
        if (m_user.B < player.state.bank.B) { m_user.B++; }
        updateUserSide(H3_TileType::TileB);
        updateBankSide();
    };

    m_userC.btn->onClickedLeft = [&] () {
        if (!isEnabled()) { DE_TRACE("No C") }
        H3_Player & player = m_game->getPlayer( m_playerId );
        if (m_user.C < player.state.bank.C) { m_user.C++; }
        updateUserSide(H3_TileType::TileC);
        updateBankSide();
    };

    m_userD.btn->onClickedLeft = [&] () {
        if (!isEnabled()) { DE_TRACE("No D") }
        H3_Player & player = m_game->getPlayer( m_playerId );
        if (m_user.D < player.state.bank.D) { m_user.D++; }
        updateUserSide(H3_TileType::TileD);
        updateBankSide();
    };

    m_userE.btn->onClickedLeft = [&] () {
        if (!isEnabled()) { DE_TRACE("No E") }
        H3_Player & player = m_game->getPlayer( m_playerId );
        if (m_user.E < player.state.bank.E) { m_user.E++; }
        updateUserSide(H3_TileType::TileE);
        updateBankSide();
    };

    // User '-' (RightMouseClick)

    m_userA.btn->onClickedRight = [&] () {
        if (m_user.A > 0) { m_user.A--; }
        updateUserSide(H3_TileType::TileA);
        updateBankSide();
    };

    m_userB.btn->onClickedRight = [&] () {
        if (m_user.B > 0) { m_user.B--; }
        updateUserSide(H3_TileType::TileB);
        updateBankSide();
    };

    m_userC.btn->onClickedRight = [&] () {
        if (m_user.C > 0) { m_user.C--; }
        updateUserSide(H3_TileType::TileC);
        updateBankSide();
    };

    m_userD.btn->onClickedRight = [&] () {
        if (m_user.D > 0) { m_user.D--; }
        updateUserSide(H3_TileType::TileD);
        updateBankSide();
    };

    m_userE.btn->onClickedRight = [&] () {
        if (m_user.E > 0) { m_user.E--; }
        updateUserSide(H3_TileType::TileE);
        updateBankSide();
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

    // Bank:

    m_bankA.lup->setText( "4:1" );
    m_bankB.lup->setText( "4:1" );
    m_bankC.lup->setText( "4:1" );
    m_bankD.lup->setText( "4:1" );
    m_bankE.lup->setText( "4:1" );

    if (player.state.hasPort3v1())
    {
        m_bankA.lup->setText( "3:1" );
        m_bankB.lup->setText( "3:1" );
        m_bankC.lup->setText( "3:1" );
        m_bankD.lup->setText( "3:1" );
        m_bankE.lup->setText( "3:1" );
    }

    if (player.state.hasPortA()) { m_bankA.lup->setText( "2:1" ); }
    if (player.state.hasPortB()) { m_bankB.lup->setText( "2:1" ); }
    if (player.state.hasPortC()) { m_bankC.lup->setText( "2:1" ); }
    if (player.state.hasPortD()) { m_bankD.lup->setText( "2:1" ); }
    if (player.state.hasPortE()) { m_bankE.lup->setText( "2:1" ); }

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

    m_userA.btn->setEnabled(player.state.bank.A >= getRatio(0,player));
    m_userB.btn->setEnabled(player.state.bank.B >= getRatio(1,player));
    m_userC.btn->setEnabled(player.state.bank.C >= getRatio(2,player));
    m_userD.btn->setEnabled(player.state.bank.D >= getRatio(3,player));
    m_userE.btn->setEnabled(player.state.bank.E >= getRatio(4,player));

    m_userA.ldn->setText( std::to_string(player.state.bank.A) );
    m_userB.ldn->setText( std::to_string(player.state.bank.B) );
    m_userC.ldn->setText( std::to_string(player.state.bank.C) );
    m_userD.ldn->setText( std::to_string(player.state.bank.D) );
    m_userE.ldn->setText( std::to_string(player.state.bank.E) );

    int m_giveCard = -1; // H3_TileType resource Card type the player gives
    int m_wishCard = -1; // H3_TileType the player wants from bank
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

