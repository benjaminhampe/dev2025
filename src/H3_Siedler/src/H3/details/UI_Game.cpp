#include "UI_Game.h"
#include <H3/H3_Game.h>
#include <H3/details/H3_Topology.h>
#include <H3/details/UI_WorldCreator.h>
#include <H3/details/UI_Marquee.h>
#include <H3/details/UI_Actions.h>
#include <H3/details/UI_Banking.h>
#include <H3/details/UI_DonateCards.h>
#include <H3/details/UI_WinnerScreen.h>

// =====================================================================
UI_Game::UI_Game( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& pos )
// =====================================================================
    : de::gui::Widget(env, parent, id, pos)
    , m_game(game)
    , m_forceResize{ true }
    , m_worldCreator(nullptr)
    , m_winnerScreen(nullptr)
    , m_actions(nullptr)
    , m_donateCards(nullptr)
    , m_banking(nullptr)
    , m_bDebug(false)
{
    setDrawChildren(true);

    auto r = de::Recti(0,0,0,0);
    m_worldCreator = new UI_WorldCreator( game, env, this, 1, r );
    m_marquee = new UI_Marquee( game, env, this, 2, r );
    m_actions = new UI_Actions( game, env, this, 3, r );
    m_donateCards = new UI_DonateCards( game, env, this, 4, r );
    m_winnerScreen = new UI_WinnerScreen( game, env, this, 5, r );
    m_banking = new UI_Banking( game, env, this, 6, r );

    m_worldCreator->setVisible(false);
    m_marquee->setVisible(false);
    m_actions->setVisible(false);
    m_donateCards->setVisible(false);
    m_winnerScreen->setVisible(false);
    m_banking->setVisible(false);
}

UI_Game::~UI_Game()
{

}

void UI_Game::forceResize()
{
    m_forceResize = true;
}

void UI_Game::setDebug(bool bDebug)
{
    m_bDebug = bDebug;
}

bool UI_Game::isDebug() const
{
    return m_bDebug;
}

void UI_Game::updateVisibleActions()
{
    if (m_actions)
    {
        m_actions->updateVisibility();
    }

    //H3_Game & game = *m_game;
    //UI_Stats_update( game );
    //UIBanking_update( game );
}

void UI_Game::draw()
{
    if (!isVisible())
    {
        return;
    }

    if ( !m_game )
    {
        DE_ERROR("No game")
        return;
    }

    if ( m_game->isGameRunning() )
    {
        //DE_DEBUG("Draw(",m_rect.str(),")")

        H3_Game & game = *m_game;

        float zoom = game.getZoom();
        int w = m_rect.w; // game.getDriver()->getScreenWidth();
        int h = m_rect.h; // game.getDriver()->getScreenHeight();

        // --- Draw Enemy Statistics ---

        int p = 10;
        int estat_w = (w - 5*p) / 4;
        int estat_h = zoom * 150;
        int x = p;
        int y = p;
        for (const H3_Player & player : game.m_players)
        {
            if (player.typ == H3_PlayerType::HUMAN)
                continue;

            de::Recti r_cpuPlayer(x,y,estat_w,estat_h);
            drawCpuPlayer( game, player.id, r_cpuPlayer, m_bDebug );

            x += estat_w + p;
        }

        auto driver = m_env->getVideoDriver();

        driver->beginPerf("UI_HumanPlayer");
        drawHumanPlayer( game );
        driver->endPerf();

        // driver->beginPerf("UI_PlayerStats");
        // for ( int i = 0; i < m_players.size(); ++i )
        // {
        //     UI_drawPlayerStats( *this, 20, 20+100*i, m_players[ i ].id );
        // }
        // driver->endPerf();

        drawDice();

        if (m_bDebug)
        {
            driver->beginPerf("UI_RoundStats");
            drawRoundStats( game, p, estat_h + 3*p );
            driver->endPerf();

            driver->beginPerf("UI_HoverInfo");
            auto r_panel = de::Recti( 20, 400, zoom * 200, zoom * 250 );
            drawInfoPanel_Tile( game, r_panel, game.m_hoverTileId );
            drawInfoPanel_Corner( game, r_panel, game.m_hoverCornerId );
            drawInfoPanel_Edge( game, r_panel, game.m_hoverEdgeId );
            driver->endPerf();

            driver->beginPerf("UI_Log");
            UI_drawLog( game );
            driver->endPerf();
        }
    }

    if (isDrawChildren())
    {
        for (auto child : m_children)
        {
            child->draw();
        }
    }
}


void UI_Game::drawDice()
{
    const float zoom = m_env->getZoom();

    const int r = zoom * 6;

    auto driver = m_env->getVideoDriver();
    const int w = driver->getScreenWidth();
    const int h = driver->getScreenHeight();

    const auto & dice = m_game->m_dice;
    const de::gpu::TexRef tex_1 = H3_getDiceTex(*m_game,dice.a);
    const de::gpu::TexRef tex_2 = H3_getDiceTex(*m_game,dice.b);

    const int d = zoom * 48;
    const int s = zoom * 12;

    int x = w - 2*(d+s);
    int y = h - 300;

    driver->getScreenRenderer()->
        draw2DRoundRect(de::Recti(x,y,d,d), {r,r}, tex_1);

    driver->getScreenRenderer()->
        draw2DRoundRect(de::Recti(x+d+s,y,d,d), {r,r}, tex_2);
}

void UI_Game::resize(int x, int y, int w, int h)
{
    // x = 0;
    // y = 0;
    // w = m_env->getVideoDriver()->getScreenWidth();
    // h = m_env->getVideoDriver()->getScreenHeight();
    auto rect = de::Recti(x, y, w, h);

    if (m_forceResize)
    {
        m_forceResize = false;
    }
    else
    {
        if (rect == m_rect)
        {
            return;
        }
    }


    // DE_DEBUG("Resize(",x,",",y,",",w,",",h,")")

    m_rect = de::Recti(x, y, w, h);
    updateLayouts();
}


void UI_Game::updateLayouts()
{
    int x = m_rect.x;
    int y = m_rect.y;
    int w = m_rect.w;
    int h = m_rect.h;
    if (m_worldCreator)
    {
        m_worldCreator->resize(x,y,w,h);
    }
    if (m_actions)
    {
        m_actions->resize(x,y,w,h);
    }
    if (m_donateCards)
    {
        // ::::: Panel body ::::
        int dim_h = 6 * std::min(w,h) / 10;
        int max_w = 8 * w / 10;
        int panel_w = std::min(max_w, 4*dim_h/3);
        int panel_h = dim_h;
        int panel_x = (w - panel_w)/2;
        int panel_y = (h - panel_h)/2;
        m_donateCards->resize(panel_x, panel_y, panel_w, panel_h);
    }
    if (m_banking)
    {
        // ::::: Panel body ::::
        int dim = 8 * std::min(w,h) / 10;
        int panel_w = dim;
        int panel_h = dim;
        int panel_x = (w - panel_w)/2;
        int panel_y = (h - panel_h)/2;
        m_banking->resize(panel_x, panel_y, panel_w, panel_h);
    }
    if (m_winnerScreen)
    {
        int panel_w = w - 200;
        int panel_h = h - 200;
        int panel_x = 100;
        int panel_y = 100;
        m_winnerScreen->resize(panel_x, panel_y, panel_w, panel_h);
    }
}

void UI_Game::pushState(const H3_GuiState& guiState)
{
    m_stack.push(getState());
    setState(guiState);
}

void UI_Game::popState()
{
    setState(m_stack.top());
    m_stack.pop();
}

H3_GuiState UI_Game::getState() const
{
    H3_GuiState guiState;
    guiState.bWorldCreator = m_worldCreator->isVisible();
    guiState.bMarquee = m_marquee->isVisible();
    guiState.bActions = m_actions->isVisible();
    guiState.bDonateCards = m_donateCards->isVisible();
    guiState.bBanking = m_banking->isVisible();
    guiState.bWinnerScreen = m_winnerScreen->isVisible();
    return guiState;
}

void UI_Game::setState(const H3_GuiState& guiState)
{
    if (m_worldCreator){
        m_worldCreator->setVisible(guiState.bWorldCreator);
    }
    if (m_actions){
        m_actions->setVisible(guiState.bActions);
    }
    if (m_marquee){
        m_marquee->setVisible(guiState.bMarquee);
    }
    if (m_donateCards){
        m_donateCards->setVisible(guiState.bDonateCards);
    }
    if (m_banking){
        m_banking->setVisible(guiState.bBanking);
    }
    if (m_winnerScreen){
        m_winnerScreen->setVisible(guiState.bWinnerScreen);
    }

    updateLayouts();
}





void UI_Game::drawHumanPlayer(H3_Game & game)
{
    auto driver = game.getDriver();
    auto zoom = game.getZoom();

    auto drawStat = [&](int r, int x, int y, int w, int h,
                        int texId, int value, const de::Font5x8& font )
    {
        const auto tex = game.getTex(H3_Tex::eID(texId), __func__);

        if (r > 0)
        {
            driver->getScreenRenderer()->draw2DRoundRect(
                de::Recti(x,y,w,h), glm::ivec2(r,r), 0xFFFFFFFF, tex, 13);
        }
        else
        {
            driver->getScreenRenderer()->draw2DRect(
                de::Recti(x,y,w,h), 0xFFFFFFFF, tex);
        }

        if ( value > -1)
        {
            const auto s = std::to_string( value );
            const auto ts = font.getTextSize( s );

            int tw = ts.width;
            int th = (ts.height / 8) * 7;

            int tx = x + w - 2 - zoom - tw;
            int ty = y + h - 2 - zoom - th;

            driver->draw2DText( tx, ty, s, dbRGB(255,255,255),
                                de::Align::Default, font );
        }
    };

    H3_Player & player = game.getCurrentPlayer();

    const auto scr_w = driver->getScreenWidth();
    const auto scr_h = driver->getScreenHeight();
    de::Font5x8 font(zoom*2,zoom*2,0,0,1,1);

    int r = zoom * 6;
    int s = 1; // spacing
    int p = zoom * 5; // padding
    int w = zoom * 39;

    int hIcon = zoom * 64;
    int hCard = zoom * 64;
    int hStat = zoom * 39;

    int wT = 5 * w + 2*p;
    int hT = p + hIcon + p + hCard + p + hStat + p;

    // [Draw] Background
    de::Recti rcBody(p,scr_h - 1 - p - hT,wT,hT);
    player.screenRect = rcBody; // Used for animations

    driver->getScreenRenderer()->draw2DRoundRect(
        rcBody, glm::ivec2(r,r), player.color);

    // [Draw] Player.icon
    de::Recti rcIcon(rcBody.x + p,rcBody.y + p,hIcon,hIcon);
    auto icon = game.getTex((H3_Tex::eID)player.avatar, __func__ );
    driver->getScreenRenderer()->draw2DRoundRect(
        rcIcon,
        glm::ivec2(r,r),
        0xFFFFFFFF,
        icon,
        13);

    // [Draw] Name Background
    int name_x = rcIcon.x2() + 2*p;
    int name_y = rcIcon.y2();
    driver->draw2DText( name_x, name_y, player.name, dbRGB(255,255,255),
                        de::Align::BottomLeft, de::Font("Postamt",zoom * 28),
                        dbRGB(0,0,0,200), p );

    // [Draw] Dice
    int dice_x = name_x + 2*p;
    int dice_y = rcIcon.centerY() - p - hStat;
    const int dice1 = player.getDice().a;
    const int dice2 = player.getDice().b;
    const int dice1tex = (dice1 > 0) ? H3_Tex::Dice1 + dice1 - 1 : H3_Tex::Unknown;
    const int dice2tex = (dice2 > 0) ? H3_Tex::Dice1 + dice2 - 1 : H3_Tex::Unknown;
    drawStat(r,dice_x,dice_y,hStat,hStat, dice1tex, -1, font); dice_x += hStat + p;
    drawStat(r,dice_x,dice_y,hStat,hStat, dice2tex, -1, font); dice_x += hStat + p;

    int x = rcBody.x + p;
    int y = rcBody.y + p + rcIcon.h + p;
    drawStat(r,x,y,w,hCard, H3_Tex::CardA, player.state.bank.A, font); x += w + s;
    drawStat(r,x,y,w,hCard, H3_Tex::CardB, player.state.bank.B, font); x += w + s;
    drawStat(r,x,y,w,hCard, H3_Tex::CardC, player.state.bank.C, font); x += w + s;
    drawStat(r,x,y,w,hCard, H3_Tex::CardD, player.state.bank.D, font); x += w + s;
    drawStat(r,x,y,w,hCard, H3_Tex::CardE, player.state.bank.E, font); x += w + s;
    drawStat(r,x,y,w,hCard, H3_Tex::EventCardPoint, player.state.points.size(), font); x += w + s;
    drawStat(r,x,y,w,hCard, H3_Tex::EventCardKnight, player.state.knights.size(), font); x += w + s;
    drawStat(r,x,y,w,hCard, H3_Tex::NiceInventCard, player.state.invents.size(), font); x += w + s;
    drawStat(r,x,y,w,hCard, H3_Tex::NiceMonopolCard, player.state.monopols.size(), font); x += w + s;

    x = rcBody.x + p;
    y = rcBody.y2() - p - hStat;
    drawStat(0,x,y,w,hStat, H3_Tex::StatVic, player.state.victoryPoints, font); x += w + s;
    drawStat(0,x,y,w,hStat, H3_Tex::StatRoad, player.state.roads.size(), font); x += w + s;
    drawStat(0,x,y,w,hStat, H3_Tex::StatFarm, player.state.farms.size(), font); x += w + s;
    drawStat(0,x,y,w,hStat, H3_Tex::StatCity, player.state.citys.size(), font); x += w + s;
    const int cardSum = player.state.numEventCards(true);
    drawStat(0,x,y,w,hStat, H3_Tex::StatCard, cardSum, font); x += w + s;
    if (player.state.hasBonusArmy)
    drawStat(0,x,y,w,hStat, H3_Tex::BonusArmy, player.state.hasBonusArmy, font); x += w + s;
    if (player.state.hasBonusRoad)
    drawStat(0,x,y,w,hStat, H3_Tex::BonusRoad, player.state.hasBonusRoad, font); x += w + s;

    // Bonus
    int wB = zoom * 64;
    int hB = zoom * 64;
    x = scr_w - 1 - p - wB - p - wB;
    y = scr_h - 1 - p - hB;
    drawStat(wB/2,x,y,wB,hB, H3_Tex::BonusArmy, player.state.hasBonusArmy, font); x += wB + p;
    drawStat(wB/2,x,y,wB,hB, H3_Tex::BonusRoad, player.state.hasBonusRoad, font); // x += wB + p;
}



void UI_Game::drawCpuPlayer( H3_Game& game, uint32_t playerId, const de::Recti& pos, bool bDebug )
{
    //DE_DEBUG("Draw(",pos.str(),")")

    auto zoom = game.getZoom();
    int p = zoom * 8;

    auto driver = game.getDriver();

    // Debug Outline
    //driver->getScreenRenderer()->
    //    draw2DRectLine( pos, 0xFFFFFFFF );

    // Background
    auto t_scroll = game.getTex(H3_Tex::Scroll2, __func__);
    driver->getScreenRenderer()->
        draw2DRect( pos, 0xFFFFFFFF, t_scroll );

    // Get Player
    if (playerId < 1) { return; }
    auto & player = game.getPlayer(playerId);
    player.screenRect = pos; // Used for animations

    // Icon Background
    int icon_x = pos.x + 2*p;
    int icon_y = pos.y + 2*p;
    int icon_w = 2 * pos.h / 3;
    int icon_h = icon_w;
    auto r_iconBg = de::Recti(icon_x, icon_y, icon_w, icon_h);

    // Icon Background
    driver->getScreenRenderer()->
        draw2DRoundRect( r_iconBg, {p,p}, player.color );

    // Name Background
    int name_x = r_iconBg.x2() - 2*p;
    int name_y = r_iconBg.y;
    int name_w = pos.w - 3*p - icon_w;
    int name_h = pos.h / 3;
    auto r_nameBg = de::Recti( name_x, name_y, name_w, name_h );

    // Name Background
    driver->getScreenRenderer()->
        draw2DRoundRect( r_nameBg, {p,p}, player.color );

    // Icon
    auto icon = game.getTex((H3_Tex::eID)player.avatar, __func__);
    driver->getScreenRenderer()->
        draw2DRoundRect( r_iconBg.inset(p,p+2,p,p), {p,p}, icon );

    // Name
    auto r_name = r_nameBg.inset(p);
    driver->draw2DText( r_name.centerX(),
                        r_name.centerY(),
                        player.name,
                        dbRGB(0,0,0),
                        de::Align::Centered,
                        de::Font("Postamt",zoom * 28),
                        dbRGB(255,255,255,225),
                        p );

    // VictoryPoints - Circle
    {
        // Background Circle with Player color
        int d = zoom * 40;
        auto r0 = de::Recti(pos.x-2,pos.y-2,d,d);
        driver->getScreenRenderer()->
            draw2DCircle( r0, player.color );

        // Foreground Circle in green vic tex
        int r = zoom * 4;
        auto texv = game.getTex( H3_Tex::StatVic, __func__ );
        driver->getScreenRenderer()->
            draw2DCircle( r0.inset(r,r,r,r), 0xFFFFFFFF, texv );

        // Foreground Text in black
        driver->draw2DText(
            r0.centerX(),
            r0.centerY(),
            std::to_string( player.state.victoryPoints ),
            dbRGB(255,255,255),
            de::Align::Centered,
            de::Font5x8(zoom*2,zoom*2,0,0,1,1),
            dbRGB(0,0,0,150),
            zoom );

        // auto f = de::Font("Postamt", 2 * d / 3);
        // auto s = dbStr(player.state.victoryPoints);
        // int cx = r0.centerX();
        // int cy = r0.centerY();
        // driver->getFontRenderer()->
        //     draw2DText(cx,cy,s,dbRGB(0,0,0),de::Align::Centered,f);
    }

    // Dice 32 x 32
    {
        int dice_w = zoom * 36;
        int dice_h = dice_w;
        int dice_x = icon_x + icon_w / 2 - dice_w - 4;
        int dice_y = icon_y + icon_h + 8;
        int d = dice_w + 4;

        auto r = glm::ivec2{5,5};
        // H3_drawDice( game, de::Recti(x,y, 24,24), 5, dice1, dice2 );
        auto tex1 = H3_getDiceTex( game, player.getDice().a );
        auto tex2 = H3_getDiceTex( game, player.getDice().b );
        de::Recti r_dice1(dice_x, dice_y, dice_w, dice_h ); dice_x += d;
        de::Recti r_dice2(dice_x, dice_y, dice_w, dice_h );
        driver->getScreenRenderer()->
            draw2DRoundRect( r_dice1, r, 0xFFFFFFFF, tex1, 12 );
        driver->getScreenRenderer()->
            draw2DRoundRect( r_dice2, r, 0xFFFFFFFF, tex2, 12 );
    }


    auto drawStatNum = [&]( int img, de::Recti pos, int value )
    {
        const auto tex = game.getTex(H3_Tex::eID(img), __func__);

        driver->getScreenQuadRenderer()->
            draw2DRect( pos, tex, true );

        driver->draw2DText(
            pos.x2() - zoom,
            pos.y2() - zoom,
            std::to_string( value ),
            dbRGB(255,255,255),
            de::Align::BottomRight,
            de::Font5x8(zoom*2,zoom*2,0,0,1,1),
            dbRGB(0,0,0,200),
            zoom );
    };


    {
        int s = 4; // spacing
        int x = icon_x + icon_w + s;
        int y = name_y + name_h + s;
        int w = zoom * 30;
        int h = zoom * 50;

        drawStatNum( H3_Tex::EventCardPoint, de::Recti(x,y,w,h), player.state.points.size()); x += w + s;
        drawStatNum( H3_Tex::EventCardKnight, de::Recti(x,y,w,h), player.state.knights.size()); x += w + s;

        w = zoom * 36;
        h = zoom * 36;
        //drawStatNum( H3_Tex::StatVic, de::Recti(x,y,w,h), player.state.victoryPoints, 16); x += w + s;
        drawStatNum( H3_Tex::StatRoad, de::Recti(x,y,w,h), player.state.roads.size()); x += w + s;
        drawStatNum( H3_Tex::StatFarm, de::Recti(x,y,w,h), player.state.farms.size()); x += w + s;
        drawStatNum( H3_Tex::StatCity, de::Recti(x,y,w,h), player.state.citys.size()); x += w + s;

        // u32 n = player.state.numEventCards(true);
        // drawStatNum( H3_Tex::StatCard, de::Recti(x,y,w,h), n, 16);

        if (bDebug)
        {
            w = zoom * 30;
            h = zoom * 50;
            x = icon_x + icon_w + s;
            y = name_y + name_h + h + 2*s;
            drawStatNum( H3_Tex::CardA, de::Recti(x,y,w,h), player.state.bank.A); x += w + s;
            drawStatNum( H3_Tex::CardB, de::Recti(x,y,w,h), player.state.bank.B); x += w + s;
            drawStatNum( H3_Tex::CardC, de::Recti(x,y,w,h), player.state.bank.C); x += w + s;
            drawStatNum( H3_Tex::CardD, de::Recti(x,y,w,h), player.state.bank.D); x += w + s;
            drawStatNum( H3_Tex::CardE, de::Recti(x,y,w,h), player.state.bank.E); x += w + s;
            drawStatNum( H3_Tex::NicePointCard, de::Recti(x,y,w,h), player.state.pointCards.size()); x += w + s;
            drawStatNum( H3_Tex::NiceKnightCard, de::Recti(x,y,w,h), player.state.knightCards.size()); x += w + s;
            drawStatNum( H3_Tex::NiceInventCard, de::Recti(x,y,w,h), player.state.inventCards.size()); x += w + s;
            drawStatNum( H3_Tex::NiceMonopolCard, de::Recti(x,y,w,h), player.state.monopolCards.size()); x += w + s;
        }
    }

}

void UI_Game::drawRoundStats( H3_Game & game, int x, int y, de::Align::EAlign align )
{
    auto driver = game.getDriver();
    if ( !driver ) { DE_ERROR("No driver") return; }

    if (game.m_playerId < 0) return;
    H3_Player & player = game.getPlayer(game.m_playerId);

    // const int mx = game.m_mouseX;
    // const int my = game.m_mouseY;
    // const int fps = driver->getFPS();
    // const int w = driver->getScreenWidth();
    // const int h = driver->getScreenHeight();
    const int p = 10;
    const u32 fg = dbRGB(255,255,255);
    const u32 bg = dbRGB(0,0,0,128);

    de::Font5x8 font1(3,3,0,0,1,1);
    const int ln = font1.getTextSize("W").height + p;

    auto sS = dbStr("State(", H3_State::getString( game.m_state ),")");
    auto sP = dbStr("Player(", player.id,"|",player.name,"), diceOrder(",player.diceOrder,")");
    auto sR = dbStr("Round(", game.m_round, "), Turn(", game.m_turn,")");
    driver->draw2DText(x,y,sS,fg,align,font1,bg,p); y += ln;
    driver->draw2DText(x,y,sP,fg,align,font1,bg,p); y += ln;
    driver->draw2DText(x,y,sR,fg,align,font1,bg,p); y += ln;

/*
    auto sA = dbStr("Dice = ", game.getCurrentPlayer().state.dice.sum());
    auto sB = dbStr("HoverTile = ", game.m_hoverTileId);
    auto sC = dbStr("HoverCorner = ", game.m_hoverCornerId);
    auto sD = dbStr("HoverEdge = ", game.m_hoverEdgeId);
    auto sE = dbStr("HoverRoad = ", game.m_hoverRoadId);
    auto sF = dbStr("HoverFarm = ", game.m_hoverFarmId);

    driver->draw2DText(x,y,sA,fg,align,font1,bg,p); y += ln;
    driver->draw2DText(x,y,sB,fg,align,font1,bg,p); y += ln;
    driver->draw2DText(x,y,sC,fg,align,font1,bg,p); y += ln;
    driver->draw2DText(x,y,sD,fg,align,font1,bg,p); y += ln;
    driver->draw2DText(x,y,sE,fg,align,font1,bg,p); y += ln;
    driver->draw2DText(x,y,sF,fg,align,font1,bg,p); y += ln;
*/
}



void UI_Game::drawEngineStats( H3_Game & game, int x, int y, de::Align::EAlign align)
{
    auto driver = game.getDriver();
    if ( !driver ) { DE_ERROR("No driver") return; }

    const int p = 10;
    de::Font5x8 font1(3,3,0,0,1,1);
    int ln = font1.getTextSize("W").height + p;

    u32 bg = dbRGB(0,0,0,128);
    u32 fg = dbRGB(255,225,101);
    driver->draw2DText(x,y,dbStr("FPS(",driver->getFPS(),")"),fg,align,font1,bg,p); y += ln;

/*
    auto camera = driver->getCamera();
    if ( camera )
    {
        auto s1 = dbStr("Camera-Pos(",glm::ivec3(camera->getPos()),")");
        auto s2 = dbStr("Camera-Target(",glm::ivec3(camera->getTarget()),")");
        auto s3 = dbStr("Camera-Angle(",camera->getAng(),")");

        driver->draw2DText(x,y,s1,dbRGB(101,201,101),align,font1,bg,padd); y += ln1;
        driver->draw2DText(x,y,s2,dbRGB(125,215,125),align,font1,bg,padd); y += ln1;
        driver->draw2DText(x,y,s3,dbRGB(145,225,145),align,font1,bg,padd); y += ln1;
    }


    const int w = driver->getScreenWidth();
    const int h = driver->getScreenHeight();
    const int mx = game.m_mouseX;
    const int my = game.m_mouseY;

    const auto s1 = dbStr("Screen(",w,",",h,")");
    const auto s2 = dbStr("Mouse(",mx,",",my,") L(", game.m_isMouseLeftPressed, "), R(", game.m_isMouseRightPressed,"), M(", game.m_isMouseMiddlePressed,")");
    const auto fx = double(mx) / double(w);
    const auto fy = double(my) / double(h);
    const auto s3 = dbStr("Relative-Mouse(",fx,",",fy,")");


    driver->draw2DText(x,y,s1,dbRGB(255,101,101),align,font1,bg,padd); y += ln1;
    driver->draw2DText(x,y,s2,dbRGB(255,150,101),align,font1,bg,padd); y += ln1;
    driver->draw2DText(x,y,s3,dbRGB(255,200,101),align,font1,bg,padd); y += ln1;

    auto s6 = dbStr("State = ", H3_State::getString( game.m_state ));
    auto s7 = dbStr("Round = ", game.m_round);
    auto s8 = dbStr("Turn = ", game.m_turn);
    auto s9 = dbStr("Player = ", game.getCurrentPlayer().name);
    auto sA = dbStr("Dice = ", game.getCurrentPlayer().state.dice.sum());
    auto sB = dbStr("HoverTile = ", game.m_hoverTileId);
    auto sC = dbStr("HoverCorner = ", game.m_hoverCornerId);
    auto sD = dbStr("HoverEdge = ", game.m_hoverEdgeId);
    auto sE = dbStr("HoverRoad = ", game.m_hoverRoadId);
    auto sF = dbStr("HoverFarm = ", game.m_hoverFarmId);

    u32 white = 0xFFFFFFFF;
    driver->draw2DText(x,y,s6,white,align,font1,bg,padd); y += ln1;
    driver->draw2DText(x,y,s7,white,align,font1,bg,padd); y += ln1;
    driver->draw2DText(x,y,s8,white,align,font1,bg,padd); y += ln1;
    driver->draw2DText(x,y,s9,white,align,font1,bg,padd); y += ln1;
    driver->draw2DText(x,y,sA,white,align,font1,bg,padd); y += ln1;
    driver->draw2DText(x,y,sB,white,align,font1,bg,padd); y += ln1;
    driver->draw2DText(x,y,sC,white,align,font1,bg,padd); y += ln1;
    driver->draw2DText(x,y,sD,white,align,font1,bg,padd); y += ln1;
    driver->draw2DText(x,y,sE,white,align,font1,bg,padd); y += ln1;
    driver->draw2DText(x,y,sF,white,align,font1,bg,padd); y += ln1;
*/

}


void UI_Game::drawInfoPanel_Tile( H3_Game & game, de::Recti pos, const u32 tileId )
{
    if ( !tileId ) { return; }
    auto driver = game.getDriver();
    if ( !driver ) { DE_ERROR("No driver") return; }

    const float zoom = game.getZoom();

    const auto font = de::Font5x8(zoom,zoom);
    const auto align = de::Align::Default;

    auto H3_drawHexagon = [&] ( const de::Recti& pos,
                        const u32 color, const de::gpu::TexRef& tex, int num )
    {
        driver->getScreenRenderer()->draw2DHexagon( pos, color, tex );

        if ( num > -1 )
        {
            driver->draw2DText(
                pos.centerX(),
                pos.centerY(),
                std::to_string(num),
                dbRGB(255,255,255),
                de::Align::Centered,
                font,
                dbRGB(0,0,0,128), 4 );
        }
    };

    H3_Tile& tile = H3_getTile(game, __func__, tileId);

    uint32_t white = 0xFFFFFFFF;

    driver->getScreenRenderer()->
        draw2DRoundRect( pos, glm::ivec2(12, 12), 0xA0000000, de::gpu::TexRef(), 13 );

    int w = pos.w;
    int x = pos.x + 20;
    int y = pos.y + 20;
    int ln = zoom * 10;


    auto s1 = dbStr(H3_TileType::getString( tile.tt )," (",tile.i, ",", tile.j,")");
    auto s3 = dbStr(tile.pos);
    auto s4 = dbStr("angle60 = ",tile.angle60);
    auto s5 = dbStr("hasRobber = ",tile.id == game.m_thief.occupiedTileId);
    auto s6 = dbStr("NextCount = ",tile.next.size());
    auto s7 = dbStr("CornerCount = ",tile.corners.size());
    auto s9 = dbStr("EdgeCount = ",tile.edges.size());

    driver->draw2DText( x,y, s1, white, align, font );
    if ( tile.chipValue > 0 )
    {
        auto s2 = dbStr("Dice = ",tile.chipValue);
        driver->draw2DText( x+w-40,y, s2, white, de::Align::TopRight, font );
    }
    y += ln;
    H3_drawHexagon( de::Recti(x+50,y,50,50), 0xFFFFFFFF, H3_getTileTex( game, tile.tt ), tile.id );
    driver->draw2DText( x+w-40,y+25, s3, white, de::Align::RightMiddle, font ); y += 50;
    driver->draw2DText( x,y, s4, white, align, font ); y += ln;
    driver->draw2DText( x,y, s5, white, align, font ); y += ln;

    // [Info] Next Tiles
    y += 4;
    driver->draw2DText( x,y, s6, white, align, font ); y += ln;
    for ( size_t i = 0; i < tile.next.size(); ++i )
    {
        H3_Tile & n = H3_getTile( game, __func__, tile.next[ i ] );
        H3_drawHexagon( de::Recti(x+28*i,y,24,24), 0xFFFFFFFF, H3_getTileTex( game, n.tt ), n.id );
    }
    y += 28;

    // [Info] Corners

    driver->draw2DText( x,y, s7, white, align, font ); y += ln;
    driver->getScreenRenderer()->draw2DCircle( de::Recti(x,y,24,24), 0xFFFFFFFF, game.getTex(H3_Tex::ChipS, __func__) );
    for ( size_t i = 0; i < tile.corners.size(); ++i )
    {
        H3_Corner & n = H3_getCorner( game, __func__, tile.corners[ i ] );
        auto s8 = dbStr("Corner[",i,"] = ",n.id);
        driver->draw2DText( 30 + x,y, s8, white, align, font );
        y += ln;
    }
    if ( tile.corners.size() < 3 )
    {
        y += ln * (3 - tile.corners.size());
    }
    y += 4;

    // [Info] Edges
    driver->draw2DText( x,y, s9, white, align, font );
    y += ln;
    driver->getScreenRenderer()->draw2DCircle( de::Recti(x,y,24,24), 0xFFFFFFFF, game.getTex(H3_Tex::ChipW, __func__) );
    for ( size_t i = 0; i < tile.edges.size(); ++i )
    {
        H3_Edge & n = H3_getEdge( game, __func__, tile.edges[ i ] );
        driver->draw2DText( 30 + x,y, dbStr("Edge[",i,"] = ",n.id), white, align, font );
        y += ln;
    }
    if ( tile.edges.size() < 3 )
    {
        y += ln * (3 - tile.edges.size());
    }
    y += 4;
}

void UI_Game::drawInfoPanel_Corner( H3_Game & game, de::Recti pos, const u32 cornerId )
{
    auto driver = game.getDriver();
    if ( !driver ) { DE_ERROR("No driver") return; }

    if ( !cornerId ) { return; }
    H3_Corner& corner = H3_getCorner(game, __func__, cornerId);

    const float zoom = game.getZoom();
    auto font = de::Font5x8(zoom, zoom);

    auto H3_drawHexagon = [&] ( const de::Recti& pos,
                        const u32 color, const de::gpu::TexRef& tex, int num )
    {
        driver->getScreenRenderer()->draw2DHexagon( pos, color, tex );

        if ( num > -1 )
        {
            driver->draw2DText(
                pos.centerX(),
                pos.centerY(),
                std::to_string(num),
                dbRGB(255,255,255),
                de::Align::Centered,
                font,
                dbRGB(0,0,0,128), 4 );
        }
    };

    uint32_t white = 0xFFFFFFFF;
    driver->getScreenRenderer()->draw2DRoundRect( pos, glm::ivec2(12, 12), 0xA0000000, de::gpu::TexRef(), 13 );


    int w = pos.w;
    int x = pos.x + 20;
    int y = pos.y + 20;
    int ln = zoom * 10;

    auto s0 = dbStr("cornerId = ",corner.id);
    auto s1 = dbStr(corner.trs.pos);
    auto s2 = dbStr("NextCount = ",corner.next.size());
    driver->draw2DText( x,y, s0, white, de::Align::Default, font );
    y += ln;
    driver->getScreenRenderer()->draw2DCircle( de::Recti(x,y,48,48), 0xFFFFFFFF, game.getTex(H3_Tex::ChipS, __func__) );
    driver->draw2DText( x+w-40,y+24, s1, white, de::Align::RightMiddle, font );
    y += 52;

    driver->draw2DText( x,y, s2, white, de::Align::Default, font );
    y += ln;
    for ( size_t i = 0; i < corner.next.size(); ++i )
    {
        H3_Corner & n = H3_getCorner( game, __func__, corner.next[ i ] );

        auto s3 = dbStr("Next[",i,"] = ",corner.next[ i ]);

        driver->draw2DText( x,y, s3, white, de::Align::Default, font );
        y += ln;
    }
    y += 4;

    auto s4 = dbStr("TileCount = ",corner.tiles.size());
    driver->draw2DText( x,y, s4, white, de::Align::Default, font );
    y += ln;
    for ( size_t i = 0; i < corner.tiles.size(); ++i )
    {
        H3_Tile & n = H3_getTile( game, __func__, corner.tiles[ i ] );
        H3_drawHexagon( de::Recti(x+40*(i+1),y,32,32), 0xFFFFFFFF, H3_getTileTex( game, n.tt ), n.id );
    }
    y += 36;

    auto s5 = dbStr("EdgeCount = ",corner.edges.size());
    driver->draw2DText( x,y, s5, de::Align(), white, font );
    y += ln;
    driver->getScreenRenderer()->draw2DCircle( de::Recti(x,y,32,32), 0xFFFFFFFF, game.getTex(H3_Tex::ChipW, __func__) );

    for ( size_t i = 0; i < corner.edges.size(); ++i )
    {
        H3_Edge & n = H3_getEdge( game, __func__, corner.edges[ i ] );
        auto s6 = dbStr("Edge[",i,"] = ",n.id);
        driver->draw2DText( x+40,y, s6, white, de::Align(), font );
        y += ln;
    }

    if ( corner.edges.size() < 3 )
    {
        y += ln * (3 - corner.edges.size());
    }
}

void UI_Game::drawInfoPanel_Edge( H3_Game & game, de::Recti pos, const u32 edgeId )
{
    auto driver = game.getDriver();
    if ( !driver ) { DE_ERROR("No driver") return; }

    if ( !edgeId ) { return; }
    H3_Edge & edge = H3_getEdge(game, __func__, edgeId);

    const float zoom = game.getZoom();
    auto font = de::Font5x8(zoom, zoom);

    auto H3_drawHexagon = [&] ( const de::Recti& pos,
                        const u32 color, const de::gpu::TexRef& tex, int num )
    {
        driver->getScreenRenderer()->draw2DHexagon( pos, color, tex );

        if ( num > -1 )
        {
            driver->draw2DText(
                pos.centerX(),
                pos.centerY(),
                std::to_string(num),
                dbRGB(255,255,255),
                de::Align::Centered,
                font,
                dbRGB(0,0,0,128), 4 );
        }
    };

    uint32_t white = 0xFFFFFFFF;

    driver->getScreenRenderer()->draw2DRoundRect( pos, glm::ivec2(12, 12), 0xA0000000, de::gpu::TexRef(), 13 );


    int w = pos.w;
    int x = pos.x + 20;
    int y = pos.y + 20;
    int ln = zoom * 10;

    auto s1 = dbStr("edgeId = ",edge.id);
    driver->draw2DText( x,y,s1, white, de::Align(), font );
    y += ln;
    driver->getScreenRenderer()->draw2DCircle( de::Recti(x,y,48,48), 0xFFFFFFFF, game.getTex(H3_Tex::ChipW, __func__) );
    auto s2 = dbStr(edge.trs.pos);
    driver->draw2DText( x+w-40,y+24, s2, white, de::Align::RightMiddle, font );
    y += 52;

    auto s3 = dbStr("NextCount = ",edge.next.size());
    driver->draw2DText( x,y, s3, white, de::Align(), font );
    y += ln;
    for ( size_t i = 0; i < edge.next.size(); ++i )
    {
        H3_Edge & n = H3_getEdge( game, __func__, edge.next[ i ] );
        auto s4 = dbStr("Next[",i,"] = ",edge.next[ i ]);
        driver->draw2DText( x,y, s4, white, de::Align(), font );
        y += ln;
    }
    y += 4;

    auto s4 = dbStr("TileCount = ",edge.tiles.size());
    driver->draw2DText( x,y, s4, white, de::Align(), font );
    y += ln;
    for ( size_t i = 0; i < edge.tiles.size(); ++i )
    {
        H3_Tile & n = H3_getTile( game, __func__, edge.tiles[ i ] );
        int d = zoom*32;
        int k = x + (d + 8)*(i+1);
        H3_drawHexagon( de::Recti(k,y,d,d), 0xFFFFFFFF, H3_getTileTex( game, n.tt ), n.id );
    }
    y += (zoom * 32) + 4;

    auto s5 = dbStr("CornerCount = ",edge.corners.size());
    driver->draw2DText( x,y, s5, white, de::Align(), font );
    y += ln;
    driver->getScreenRenderer()->draw2DCircle( de::Recti(x,y,zoom*32,zoom*32), 0xFFFFFFFF, game.getTex(H3_Tex::ChipS, __func__) );
    for ( size_t i = 0; i < edge.corners.size(); ++i )
    {
        H3_Corner & n = H3_getCorner( game, __func__, edge.corners[ i ] );
        auto s6 = dbStr("Corner[",i,"] = ",n.id);
        driver->draw2DText( x+(zoom*32)+8,y, s6, white, de::Align(), font );
        y += ln;
    }

    // if ( edge.corners.size() < 3 )
    // {
    //     y += ln * (3 - edge.corners.size());
    // }
}
