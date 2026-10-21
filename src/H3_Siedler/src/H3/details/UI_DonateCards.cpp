#include "UI_DonateCards.h"
#include <H3/H3_Game.h>

// =====================================================================
UI_DonateCards::UI_DonateCards( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& pos )
// =====================================================================
    : de::gui::Panel(env, parent, id, pos)
    , m_game(game)
    , m_playerId(-1)
    , m_btnA(nullptr)
    , m_btnB(nullptr)
    , m_btnC(nullptr)
    , m_btnD(nullptr)
    , m_btnE(nullptr)
    , m_btnAccept(nullptr)
{
    auto tex_scroll = game->getTex(H3_Tex::Scroll, __func__);
    setTexture( tex_scroll );
    setFillColor( 0xffffffff );

    m_btnA = new UI_DonateCard(m_game, m_env, this);
    m_btnB = new UI_DonateCard(m_game, m_env, this);
    m_btnC = new UI_DonateCard(m_game, m_env, this);
    m_btnD = new UI_DonateCard(m_game, m_env, this);
    m_btnE = new UI_DonateCard(m_game, m_env, this);

    m_btnAccept = new de::gui::Button("Accept", m_env, this);

    auto tex_scroll2 = game->getTex(H3_Tex::Scroll2, __func__);
    m_btnAccept->setTexture( tex_scroll2 );

    m_btnAccept->onClicked = [&] ()
        {
            DE_DEBUG("Return from donation")

            if (m_cardsToRemove > m_bankThief.sum())
            {
                DE_ERROR("Need (",m_cardsToRemove - m_bankThief.sum(),") more cards.")
                return;
            }

            H3_Player & player = m_game->getPlayer( m_playerId );

            player.state.bank -= m_bankThief;

            // Restore old Game GUI state.
            m_game->m_uiGame->popState();

            if (player.id == m_game->m_playerId)
            {
                DE_DEBUG("IsCurrentPlayer -> Place thief")
                // Enter new state.
                m_game->enterThiefPlacement();
            }
            else
            {
                DE_DEBUG("NotCurrentPlayer -> AI Place thief")
                m_game->enterIdle();
            }
        };

    m_btnA->onValueChanged = [&] (int value) {
        m_bankThief.A = value;
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

    setDrawChildren(true);
}

UI_DonateCards::~UI_DonateCards()
{

}


void UI_DonateCards::startDonation( uint32_t playerId )
{
    const H3_Player & player = m_game->getPlayer( playerId );

    DE_DEBUG("player[",player.id,"] ", player.name)

    m_playerId = playerId;
    m_bankThief.clear();

    m_bankPlayer = player.state.bank;
    m_cardsToRemove = player.state.bank.sum()/2;

    m_btnA->setCardData(0, player.state.bank.A);
    m_btnB->setCardData(1, player.state.bank.B);
    m_btnC->setCardData(2, player.state.bank.C);
    m_btnD->setCardData(3, player.state.bank.D);
    m_btnE->setCardData(4, player.state.bank.E);
}


void UI_DonateCards::resize(int X, int Y, int W, int H)
{
    m_rect = de::Recti(X,Y,W,H);

    const float zoom = m_env->getZoom();
    const int p = zoom * 5;
    const int fontSize = zoom * 24;

    for (auto & child : m_children)
    {
        auto font = child->getFont();
        font.pixelSize = fontSize;
        child->setFont(font);
    }

    {
        int w = m_rect.w / 8;
        int h = m_rect.h / 3;

        int x = (W - 5*w)/2;
        int y = 3*(H - h)/10;

        if (m_btnA) m_btnA->resize(x,y,w,h); x += w + p;
        if (m_btnB) m_btnB->resize(x,y,w,h); x += w + p;
        if (m_btnC) m_btnC->resize(x,y,w,h); x += w + p;
        if (m_btnD) m_btnD->resize(x,y,w,h); x += w + p;
        if (m_btnE) m_btnE->resize(x,y,w,h);
    }

    {
        int w = 6 * m_rect.w / 10;
        int h = m_rect.h / 7;
        int x = (W - w)/2;
        int y = H - h;
        m_btnAccept->resize(x,y,w,h);
    }
}


void UI_DonateCards::draw()
{
    Panel::draw();

    if (!isVisible()) return;

    auto driver = m_env->getVideoDriver();

    if (m_game->findPlayer(m_playerId) < 0)
    {
        DE_ERROR("No playerId = ",m_playerId)
        return;
    }

    auto pos = getAbsoluteRect();

    auto & player = m_game->getPlayer(m_playerId);

    auto zoom = m_env->getZoom();

    auto fontBig = de::Font("Postamt", zoom * 28);

    int donated = m_btnA->getCardData()
                + m_btnB->getCardData()
                + m_btnC->getCardData()
                + m_btnD->getCardData()
                + m_btnE->getCardData();


    int w = 7 * m_rect.w / 11;
    int h = m_rect.h / 8;
    int x = pos.x + (m_rect.w - w)/2;
    int y = pos.y + 20;
    auto r_title = de::Recti(x,y,w,h);
    auto scroll = m_game->getTex(H3_Tex::Scroll1,__func__);

    driver->getScreenQuadRenderer()->
        draw2DRect(r_title,scroll,true);

    // Thief icon
    int icon_s = r_title.w / 10;
    int icon_h = (r_title.h * 8) / 10;
    int icon_w = icon_h;
    int icon_x = r_title.x + icon_s;
    int icon_y = r_title.y + r_title.h / 10;
    auto r_icon = de::Recti(icon_x, icon_y, icon_w, icon_h);
    auto t_icon = m_game->getTex(H3_Tex::Benni,__func__);

    driver->getScreenQuadRenderer()->
        draw2DRect(r_icon, t_icon, true);

    // Thief text
    int text_w = r_title.w - icon_w - 10;
    int text_h = r_title.h - r_title.h / 10;
    int text_x = r_icon.x2() + 10;
    int text_y = r_title.y + r_title.h / 10;
    auto r_text = de::Recti(text_x, text_y, text_w, text_h);
    auto t_text = dbStr("Donate ", m_cardsToRemove - donated," more cards.");
    driver->draw2DText(r_text.x, r_text.centerY(), t_text,
        dbRGB(0,0,0), de::Align::LeftMiddle, fontBig,
        dbRGB(255,255,255,225), 10);


    // Player
    int y1 = m_btnA->getRect().y2();
    int y2 = m_btnAccept->getRect().y1();

    int play_w = 7 * m_rect.w / 11;
    int play_h = std::max(100,(y2-y1)/2);
    int play_x = pos.x + 4*(m_rect.w - play_w) / 10;
    int play_y = pos.y + y1 + ((y2-y1) - play_h)/2;
    auto r_play = de::Recti(play_x, play_y, play_w, play_h);

    driver->getScreenRenderer()->
        draw2DRoundRect(r_play, {10,10}, player.color);

    int pico_s = r_play.h / 10;
    int pico_h = 8 * r_play.h / 10;
    int pico_w = pico_h;
    int pico_x = r_play.x + pico_s;
    int pico_y = r_play.y + pico_s;

    auto r_pico = de::Recti(pico_x, pico_y, pico_w, pico_h);
    auto t_pico = m_game->getTex((H3_Tex::eID)player.avatar,__func__);
    driver->getScreenRenderer()->
        draw2DRoundRect(r_pico, {10,10}, t_pico);

    int ptxt_w = r_play.w - r_pico.w;
    int ptxt_h = r_play.h;
    int ptxt_x = r_play.x + pico_w + 2*pico_s;
    int ptxt_y = r_play.y;
    auto r_ptxt = de::Recti(ptxt_x, ptxt_y, ptxt_w, ptxt_h);
    auto t_ptxt = dbStr(player.name," lost ",donated," "
                    "of ", player.state.bank.sum() , " cards.");

    driver->draw2DText(r_ptxt.x, r_ptxt.centerY(), t_ptxt,
        dbRGB(0,0,0), de::Align::LeftMiddle, fontBig,
        dbRGB(255,255,255,235), 10);

    for (auto & child : m_children)
    {
        child->draw();
    }
}

/*
void UI_DonateCards::updateTexts()
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


void UI_DonateCards::resize(int X, int Y, int W, int H)
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


void UI_DonateCards::resizeLayoutThief( de::Recti pos )
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

void UI_DonateCards::resizeLayoutPlayer( de::Recti pos )
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


