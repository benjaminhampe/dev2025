#include "UI_ThiefIntro.h"
#include <H3/H3_Game.h>
#include <de/gui/Label.h>
#include <de/gui/Button.h>

// =====================================================================
UI_ThiefIntro::UI_ThiefIntro( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& pos )
// =====================================================================
    : de::gui::Widget(env, parent, id, pos)
    , m_game(game)
    //, m_panel(nullptr)
    , m_btnA1(nullptr)
    , m_btnB1(nullptr)
    , m_btnC1(nullptr)
    , m_btnD1(nullptr)
    , m_btnE1(nullptr)
    , m_btnA2(nullptr)
    , m_btnB2(nullptr)
    , m_btnC2(nullptr)
    , m_btnD2(nullptr)
    , m_btnE2(nullptr)
    , m_btnAccept(nullptr)
{
    auto texM = game->getTex(H3_Tex::Scroll, __func__);
    auto texN = game->getTex(H3_Tex::Scroll1, __func__);
    auto texA = game->getTex(H3_Tex::TileA, __func__);
    auto texB = game->getTex(H3_Tex::TileB, __func__);
    auto texC = game->getTex(H3_Tex::TileC, __func__);
    auto texD = game->getTex(H3_Tex::TileD, __func__);
    auto texE = game->getTex(H3_Tex::TileE, __func__);
    auto texT = game->getTex(H3_Tex::Benni, __func__); // Thief icon
    auto texP = game->getTex(H3_Tex::Play1, __func__); // Player icon

    setTexture( texM );
    setFillColor( 0xffffffff );

    int w = pos.w;
    //int h = pos.h;
    int p = 20;

    int bw = (w - (6 * p)) / 5;
    int bh = pos.h / 4;

    int x = p;
    int y = p;

    auto r1 = de::Recti(x,y,w-2*p,bh);
    auto r2 = de::Recti(x,y,w-2*p,bh);
    auto r3 = de::Recti(x,y,w-2*p,bh);
    auto r4 = de::Recti(x,y,w-2*p,bh);
    auto r5 = de::Recti(x,y,w-2*p,bh);
    m_lblTitle = new de::gui::Label("Title", env, this, 0, r1);
    m_lblThiefName = new de::gui::Label("Thief", env, this, 0, r3);
    m_lblThiefIcon = new de::gui::Label("", env, this, 0, r2);
    m_lblPlayerName = new de::gui::Label("Player", env, this, 0, r5);
    m_lblPlayerIcon = new de::gui::Label("", env, this, 0, r4);

    m_lblTitle->setRadius(8);
    m_lblThiefName->setRadius(8);
    m_lblThiefIcon->setRadius(8);
    m_lblPlayerName->setRadius(8);
    m_lblPlayerIcon->setRadius(8);

    m_lblTitle->setTexture( texN );
    m_lblThiefName->setTexture( texN );
    m_lblThiefIcon->setTexture( texT );
    m_lblPlayerName->setTexture( texN );
    m_lblPlayerIcon->setTexture( texP );

    x += bh;
    y += bh;

    m_btnA1 = createButton("T_A", texA, de::Recti(x,y,bw,bh)); x += bw + p;
    m_btnB1 = createButton("T_B", texB, de::Recti(x,y,bw,bh)); x += bw + p;
    m_btnC1 = createButton("T_C", texC, de::Recti(x,y,bw,bh)); x += bw + p;
    m_btnD1 = createButton("T_D", texD, de::Recti(x,y,bw,bh)); x += bw + p;
    m_btnE1 = createButton("T_E", texE, de::Recti(x,y,bw,bh)); //x += bw + p;

    x = p;
    y += p + bh;
    m_btnA2 = createButton("P_A", texA, de::Recti(x,y,bw,bh)); x += bw + p;
    m_btnB2 = createButton("P_B", texB, de::Recti(x,y,bw,bh)); x += bw + p;
    m_btnC2 = createButton("P_C", texC, de::Recti(x,y,bw,bh)); x += bw + p;
    m_btnD2 = createButton("P_D", texD, de::Recti(x,y,bw,bh)); x += bw + p;
    m_btnE2 = createButton("P_E", texE, de::Recti(x,y,bw,bh)); //x += bw + p;

    x = p;
    y += p + bh;
    m_btnAccept = createButton("Accept", texN, de::Recti(x,y,5*bw + 4*p,bh));

    m_btnA1->setOnClicked(
        [&] ()
        {
            if (m_bankThief.A > 0) { m_bankThief.A--; m_bankPlayer.A++; }
            updateTexts();
        });
    m_btnB1->setOnClicked(
        [&] ()
        {
            if (m_bankThief.B > 0) { m_bankThief.B--; m_bankPlayer.B++; }
            updateTexts();
        });
    m_btnC1->setOnClicked(
        [&] ()
        {
            if (m_bankThief.C > 0) { m_bankThief.C--; m_bankPlayer.C++; }
            updateTexts();
        });
    m_btnD1->setOnClicked(
        [&] ()
        {
            if (m_bankThief.D > 0) { m_bankThief.D--; m_bankPlayer.D++; }
            updateTexts();
        });
    m_btnE1->setOnClicked(
        [&] ()
        {
            if (m_bankThief.E > 0) { m_bankThief.E--; m_bankPlayer.E++; }
            updateTexts();
        });

    m_btnA2->setOnClicked(
        [&] ()
        {
            if (m_bankPlayer.A > 0) { m_bankThief.A++; m_bankPlayer.A--; }
            updateTexts();
        });
    m_btnB2->setOnClicked(
        [&] ()
        {
            if (m_bankPlayer.B > 0) { m_bankThief.B++; m_bankPlayer.B--; }
            updateTexts();
        });
    m_btnC2->setOnClicked(
        [&] ()
        {
            if (m_bankPlayer.C > 0) { m_bankThief.C++; m_bankPlayer.C--; }
            updateTexts();
        });
    m_btnD2->setOnClicked(
        [&] ()
        {
            if (m_bankPlayer.D > 0) { m_bankThief.D++; m_bankPlayer.D--; }
            updateTexts();
        });
    m_btnE2->setOnClicked(
        [&] ()
        {
            if (m_bankPlayer.E > 0) { m_bankThief.E++; m_bankPlayer.E--; }
            updateTexts();
        });

    m_btnAccept->setOnClicked(
        [&] () {
            if (m_cardsToRemove < 1)
            {
                setVisible(false);

                H3_Player & player = m_game->getPlayer( m_playerId );
                player.state.bank = m_bankPlayer;

                m_game->enterThiefPlacement();
            }
        });
}

UI_ThiefIntro::~UI_ThiefIntro()
{
    DE_WARN("")
}

void UI_ThiefIntro::updateTexts()
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

    m_cardsToRemove = m_bankPlayer.sum() - m_game->m_cfg.cardLimitNoSteal;

    m_lblTitle->setText( dbStr("Donate ", m_cardsToRemove, " more cards.") );
}

void UI_ThiefIntro::resizeLayout()
{
    const int scr_w = m_env->getScreenWidth();
    const int scr_h = m_env->getScreenHeight();

    // ::::: Panel ::::
    int w = scr_w - 400;
    int h = scr_h - 300;
    int x = (scr_w - w) / 2;
    int y = (scr_h - h) / 2;
    setRect( x,y,w,h );

    // ::::: Panel body ::::
    w = getRect().w - 200;
    h = getRect().h - 150;
    x = 100;
    y = 50;
    auto fScale = 0.01f * m_game->getScalePc();
    int p = fScale * 5;
    int h1 = h / 6 - p;
    int h2 = h / 3 - p;
    m_lblTitle->setRect(x,y,w+20,h1);
    y += h1 + p;
    resizeLayoutThief(de::Recti(x+50,y,w-100,h2));
    y += h2 + p;
    resizeLayoutPlayer(de::Recti(x+50,y,w-100,h2));
    y += h2 + p;
    m_btnAccept->setRect(x+50,y,w-150,h1);
}

void UI_ThiefIntro::resizeLayoutThief( de::Recti pos )
{
    auto fScale = 0.01f * m_game->getScalePc();
    //H3_drawDebugRect(*m_game, pos, 0xFF0000FF, de::Font5x8(fScale,fScale));

    de::Font5x8 font(fScale,fScale);
    int wName = font.getTextSize("Thief").width + 4;
    int hName = font.getTextSize("Thief").height + 4;
    int hIcon = pos.h - hName; // Line-height of Icon

    m_lblThiefName->setRect(de::Recti(pos.x,pos.y,wName,hName));
    m_lblThiefIcon->setRect(de::Recti(pos.x,pos.y+hName,hIcon,hIcon));

    int wLeft = std::max(wName,hIcon);

    int wRight = pos.w - wLeft;

    int p = 10;

    int x = pos.x + wLeft + p;
    int y = pos.y;
    int w = (wRight)/5 - p;
    int h = pos.h;
    if (m_btnA1) m_btnA1->setRect( de::Recti(x,y,w,h) ); x += w + p;
    if (m_btnB1) m_btnB1->setRect( de::Recti(x,y,w,h) ); x += w + p;
    if (m_btnC1) m_btnC1->setRect( de::Recti(x,y,w,h) ); x += w + p;
    if (m_btnD1) m_btnD1->setRect( de::Recti(x,y,w,h) ); x += w + p;
    if (m_btnE1) m_btnE1->setRect( de::Recti(x,y,w,h) );
}

void UI_ThiefIntro::resizeLayoutPlayer( de::Recti pos )
{
    auto fScale = 0.01f * m_game->getScalePc();
    //H3_drawDebugRect(*m_game, pos, 0xFF0000FF, de::Font5x8(fScale,fScale));

    de::Font5x8 font(fScale,fScale);
    int wName = font.getTextSize("Player").width + 4;
    int hName = font.getTextSize("Player").height + 4;
    int hIcon = pos.h - hName; // Line-height of Icon

    m_lblPlayerName->setRect(de::Recti(pos.x,pos.y,wName,hName));
    m_lblPlayerIcon->setRect(de::Recti(pos.x,pos.y+hName,hIcon,hIcon));

    int wLeft = std::max(wName,hIcon);

    int wRight = pos.w - wLeft;

    int p = 10;

    int x = pos.x + wLeft + p;
    int y = pos.y;
    int w = (wRight)/5 - p;
    int h = pos.h;
    if (m_btnA2) m_btnA2->setRect( de::Recti(x,y,w,h) ); x += w + p;
    if (m_btnB2) m_btnB2->setRect( de::Recti(x,y,w,h) ); x += w + p;
    if (m_btnC2) m_btnC2->setRect( de::Recti(x,y,w,h) ); x += w + p;
    if (m_btnD2) m_btnD2->setRect( de::Recti(x,y,w,h) ); x += w + p;
    if (m_btnE2) m_btnE2->setRect( de::Recti(x,y,w,h) );
}

void UI_ThiefIntro::setPlayer( uint32_t playerId )
{
    m_playerId = playerId;
    H3_Player & player = m_game->getPlayer( playerId );

    m_bankThief.reset();

    m_bankPlayer = player.state.bank;

    m_lblPlayerName->setText( player.name );
    m_lblPlayerIcon->setTexture( m_game->getTex(H3_Tex::Play2, __func__) );

    updateTexts();
}

// static
de::gui::Button*
UI_ThiefIntro::createButton(std::string txt, de::gpu::TexRef ref, de::Recti pos)
{
    auto btn = new de::gui::Button(txt, m_env, this, 0, pos);
    btn->setRadius( 8 );
    btn->setFillColor(dbRGBA(255,255,255,200),0);
    btn->setTextColor(dbRGBA(255,255,255),1);
    btn->setTexture( ref );
    return btn;
}
