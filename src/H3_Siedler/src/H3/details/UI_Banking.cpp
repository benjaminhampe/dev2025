#include "UI_Banking.h"
#include <H3/H3_Game.h>

void UI_Banking::createLabelButton(UI_Banking::LabelButton& w, int tileType)
{
    w.lbl = new de::gui::Label("", m_env, this);
    w.btn = new de::gui::Button("", m_env, this);

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
UI_Banking::UI_Banking( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& pos )
// =====================================================================
    : de::gui::Panel(env, parent, id, pos)
    , m_game(game)
    , m_playerId(0)
{
    // Compute card aspect ratio
    auto tex_card = game->getTex(H3_Tex::CardA, __func__);
    m_aspect = float(tex_card.w()) / float(tex_card.h());

    // Set dialog panel background texture
    auto tex_scroll = game->getTex(H3_Tex::Scroll, __func__);
    setTexture( tex_scroll );
    setFillColor( 0xffffffff );

    // Create widgets
    createLabelButton(m_bankA, H3_TileType::TileA);
    createLabelButton(m_bankB, H3_TileType::TileB);
    createLabelButton(m_bankC, H3_TileType::TileC);
    createLabelButton(m_bankD, H3_TileType::TileD);
    createLabelButton(m_bankE, H3_TileType::TileE);

    createLabelButton(m_userA, H3_TileType::TileA);
    createLabelButton(m_userB, H3_TileType::TileB);
    createLabelButton(m_userC, H3_TileType::TileC);
    createLabelButton(m_userD, H3_TileType::TileD);
    createLabelButton(m_userE, H3_TileType::TileE);

    m_btnCancel = new de::gui::Button("Cancel", m_env, this);
    m_btnAccept = new de::gui::Button("Accept", m_env, this);

    auto tex_scroll2 = game->getTex(H3_Tex::Scroll2, __func__);
    m_btnAccept->setTexture( tex_scroll2 );


    // Set callback logic.

    m_btnCancel->onClicked = [&] ()
        {
            DE_DEBUG("Cancel banking.")

            // Restore old Game GUI state.
            m_game->m_uiGame->popState();
            m_game->enterIdle();
        };
    m_btnAccept->onClicked = [&] ()
        {
             DE_DEBUG("Accept banking.")

            H3_Player & player = m_game->getPlayer( m_playerId );

            // Restore old Game GUI state.
            m_game->m_uiGame->popState();
            m_game->enterIdle();
        };

/*
    m_bankA.btn->onClicked = [&] (int value)
    {
        if
        H3_Player & player = m_game->getPlayer( m_playerId );

        if (player.state.hasPort3v1())
        m_user.A = value;
        // m_btnAccept->setEnabled(m_bankThief.sum() >= m_cardsToRemove);
    };

    m_btnB->onValueChanged = [&] (int value) {
        m_bankThief.B = value;
        // m_btnAccept->setEnabled(m_bankThief.sum() >= m_cardsToRemove);
    };

    m_btnC->onValueChanged = [&] (int value) {
        m_bankThief.C = value;
        // m_btnAccept->setEnabled(m_bankThief.sum() >= m_cardsToRemove);
    };

    m_btnD->onValueChanged = [&] (int value) {
        m_bankThief.D = value;
        // m_btnAccept->setEnabled(m_bankThief.sum() >= m_cardsToRemove);
    };

    m_btnE->onValueChanged = [&] (int value) {
        m_bankThief.E = value;
        // m_btnAccept->setEnabled(m_bankThief.sum() >= m_cardsToRemove);
    };
*/
    setDrawChildren(false);
}

UI_Banking::~UI_Banking()
{

}


void UI_Banking::startBanking( uint32_t playerId )
{
    m_playerId = playerId;

    const H3_Player & player = m_game->getPlayer( playerId );

    DE_DEBUG("player[",player.id,"] ", player.name)

    m_bankA.setText( "0" );
    m_bankB.setText( "0" );
    m_bankC.setText( "0" );
    m_bankD.setText( "0" );
    m_bankE.setText( "0" );

    m_userA.setText( dbStr(player.state.bank.A) );
    m_userB.setText( dbStr(player.state.bank.B) );
    m_userC.setText( dbStr(player.state.bank.C) );
    m_userD.setText( dbStr(player.state.bank.D) );
    m_userE.setText( dbStr(player.state.bank.E) );

    int m_giveCard = -1; // H3_TileType resource Card type the player gives
    int m_wishCard = -1; // H3_TileType the player wants from bank
}

void UI_Banking::resize(int X, int Y, int W, int H)
{
    m_rect = de::Recti(X,Y,W,H);

    const float zoom = m_env->getZoom();
    const int p = zoom * 10;
    const int fontSize = zoom * 24;

    // for (auto & child : m_children)
    // {
    //     auto font = child->getFont();
    //     font.pixelSize = fontSize;
    //     child->setFont(font);
    // }

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
    m_rcBank = de::Recti(bank_x, bank_y, bank_w, bank_h);

    int user_w = 7 * W /10;
    int user_x = (W - user_w)/2;
    int user_y = p + head_h + s + bank_h + s;
    m_rcUser = de::Recti(user_x, user_y, user_w, user_h);

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
        int ln = zoom * 24;
        m_bankA.resize(x,y,w,h,p,ln,false); x += w + p;
        m_bankB.resize(x,y,w,h,p,ln,false); x += w + p;
        m_bankC.resize(x,y,w,h,p,ln,false); x += w + p;
        m_bankD.resize(x,y,w,h,p,ln,false); x += w + p;
        m_bankE.resize(x,y,w,h,p,ln,false);
    }

    // 1x User Icon + 5x User buttons
    {
        int w = (m_rcUser.w - 5*p) / 6;
        int h = m_rcUser.h;
        int x = m_rcUser.x + w + p;
        int y = m_rcUser.y;
        int ln = zoom * 24;
        m_userA.resize(x,y,w,h,p,ln,false); x += w + p;
        m_userB.resize(x,y,w,h,p,ln,false); x += w + p;
        m_userC.resize(x,y,w,h,p,ln,false); x += w + p;
        m_userD.resize(x,y,w,h,p,ln,false); x += w + p;
        m_userE.resize(x,y,w,h,p,ln,false);
    }

    // Cancel + Accept buttons
    {
        int w = (m_rcBtns.w - p) / 2;
        int h = m_rcBtns.h;
        int x = m_rcBtns.x;
        int y = m_rcBtns.y;
        int ln = zoom * 24;
        m_btnCancel->resize(x,y,w,h); x += w + p;
        m_btnAccept->resize(x,y,w,h);
    }
}

void UI_Banking::drawTitle()
{
    auto driver = m_env->getVideoDriver();
    auto pos = getAbsoluteRect();
    auto zoom = m_env->getZoom();

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
        de::Font("Postamt", zoom * 28),
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

void UI_Banking::draw()
{
    Panel::draw();
    if (!isVisible()) return;



    drawTitle();
    drawBankIconAndName();
    drawUserIcon();
    drawUserName();

    for (auto & child : m_children)
    {
        child->draw();
    }
}

/*
void UI_Banking::updateTexts()
{
    m_btnA1->setText( std::to_string(m_bankThief.A) );
    m_btnB1->setText( std::to_string(m_bankThief.B) );
    m_btnC1->setText( std::to_string(m_bankThief.C) );
    m_btnD1->setText( std::to_string(m_bankThief.D) );
    m_btnE1->setText( std::to_string(m_bankThief.E) );

    m_btnA2->setText( std::to_string(m_bankPlayer.A) );
    m_btnB2->setText( std::to_string(m_bankPlayer.B) );
    m_btnC2->setText( std::to_string(m_bankPlayer.C) );
    m_btnD2->setText( std::to_string(m_bankPlayer.D) );
    m_btnE2->setText( std::to_string(m_bankPlayer.E) );

    m_lblTitle->setText( dbStr("Donate ", m_cardsToRemove, " more cards.") );
}


void UI_Banking::resize(int X, int Y, int W, int H)
{
    // ::::: Panel body ::::
    int panel_w = W/2;
    int panel_h = 2*H/3;
    int panel_x = (W - panel_w)/2;
    int panel_y = (H - panel_h)/2;
    m_rect = de::Recti(panel_x,panel_y,panel_w,panel_h);

    const float zoom = m_env->getZoom();
    const int p = zoom * 5;
    const int fontSize = zoom * 24;

    for (auto & child : m_children)
    {
        auto font = child->getFont();
        font.pixelSize = fontSize;
        child->setFont(font);
    }

    int h1 = m_rect.h / 6 - p;
    int h2 = m_rect.h / 3 - p;

    int w = m_rect.w;
    int h = m_rect.h;
    int x = 0;
    int y = 0;

    m_lblTitle->resize(x+50,y,w-100,h1);
    y += h1 + p;

    resizeLayoutThief(de::Recti(x+50,y,w-100,h2));
    y += h2 + p;

    resizeLayoutPlayer(de::Recti(x+50,y,w-100,h2));
    y += h2 + p;

    m_btnAccept->resize(x+50,y,w-100,h1);
}


void UI_Banking::resizeLayoutThief( de::Recti pos )
{
    const float zoom = m_env->getZoom();
    //H3_drawDebugRect(*m_game, pos, 0xFF0000FF, de::Font5x8(fScale,fScale));

    de::Font5x8 font(zoom,zoom);
    int wName = font.getTextSize("Thief").width + 4;
    int hName = font.getTextSize("Thief").height + 4;
    int hIcon = pos.h - hName; // Line-height of Icon

    m_lblThiefName->resize(pos.x,pos.y,wName,hName);
    m_lblThiefIcon->resize(pos.x,pos.y+hName,hIcon,hIcon);

    int wLeft = std::max(wName,hIcon);

    int wRight = pos.w - wLeft;

    int p = 10;

    int x = pos.x + wLeft + p;
    int y = pos.y;
    int w = (wRight)/5 - p;
    int h = pos.h;
    if (m_btnA1) m_btnA1->resize(x,y,w,h); x += w + p;
    if (m_btnB1) m_btnB1->resize(x,y,w,h); x += w + p;
    if (m_btnC1) m_btnC1->resize(x,y,w,h); x += w + p;
    if (m_btnD1) m_btnD1->resize(x,y,w,h); x += w + p;
    if (m_btnE1) m_btnE1->resize(x,y,w,h);
}

void UI_Banking::resizeLayoutPlayer( de::Recti pos )
{
    const float zoom = m_env->getZoom();

    //H3_drawDebugRect(*m_game, pos, 0xFF0000FF, de::Font5x8(fScale,fScale));

    de::Font5x8 font(zoom,zoom);
    int wName = font.getTextSize("Player").width + 4;
    int hName = font.getTextSize("Player").height + 4;
    int hIcon = pos.h - hName; // Line-height of Icon

    m_lblPlayerName->resize(pos.x,pos.y,wName,hName);
    m_lblPlayerIcon->resize(pos.x,pos.y+hName,hIcon,hIcon);

    int wLeft = std::max(wName,hIcon);

    int wRight = pos.w - wLeft;

    int p = 10;

    int x = pos.x + wLeft + p;
    int y = pos.y;
    int w = (wRight)/5 - p;
    int h = pos.h;
    if (m_btnA2) m_btnA2->resize(x,y,w,h); x += w + p;
    if (m_btnB2) m_btnB2->resize(x,y,w,h); x += w + p;
    if (m_btnC2) m_btnC2->resize(x,y,w,h); x += w + p;
    if (m_btnD2) m_btnD2->resize(x,y,w,h); x += w + p;
    if (m_btnE2) m_btnE2->resize(x,y,w,h);
}

*/


