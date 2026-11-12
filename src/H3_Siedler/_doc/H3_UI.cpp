#include "H3_UI.h"
#include <H3/H3_Game.h>
#include <H3/details/H3_Tex.h>
#include <H3/details/H3_Topology.h>

void H3_draw2DRect( H3_Game & game, const de::Recti& pos,
                   const uint32_t color, const de::gpu::TexRef& tex )
{
    auto driver = game.getDriver();
    auto screenRenderer = driver->getScreenRenderer();
    screenRenderer->draw2DRect( pos, color, tex );
}

void H3_drawDebugRect( H3_Game & game, const de::Recti& pos,
                   const uint32_t color, const de::Font5x8& font )
{
    auto driver = game.getDriver();
    driver->getScreenRenderer()->draw2DRectLine( pos, color );

    auto rend = driver->getFontRenderer5x8();

    rend->draw2DText( pos.x + 2, pos.y + 2, dbStr("(",pos.x,",",pos.y,")"), color, de::Align::TopLeft, font);
    rend->draw2DText( pos.centerX(), pos.y - 2, dbStr(pos.w), color, de::Align::BottomCenter, font);
    rend->draw2DText( pos.x2() + 2, pos.centerY(), dbStr(pos.h), color, de::Align::MiddleLeft, font);
}

void H3_draw2DRoundRect( H3_Game & game, const de::Recti& pos,
                        const glm::ivec2& r, const uint32_t color, int tess )
{
    auto driver = game.getDriver();
    auto screenRenderer = driver->getScreenRenderer();
    screenRenderer->draw2DRoundRect( pos, r, color, de::gpu::TexRef(), tess );
}

void H3_draw2DRoundRect( H3_Game & game, const de::Recti& pos,
                        const glm::ivec2& r, const de::gpu::TexRef& tex, const uint32_t color, int tess )
{
    auto driver = game.getDriver();
    auto screenRenderer = driver->getScreenRenderer();
    screenRenderer->draw2DRoundRect( pos, r, color, tex, tess );
}

void H3_drawCircle( H3_Game & game, const de::Recti& pos,
                   const uint32_t color, const de::gpu::TexRef& tex, int num, int tess )
{
    auto driver = game.getDriver();
    auto screenRenderer = driver->getScreenRenderer();
    auto font58Renderer = driver->getFontRenderer5x8();
    screenRenderer->draw2DCircle( pos, color, tex, tess );

    if ( num > -1 )
    {
        const float zoom = game.getZoom();
        auto font = de::Font5x8(zoom,zoom);
        H3_drawText( game, pos.centerX(), pos.centerY(), std::to_string(num), 0xFFFFFFFF, de::Align::Centered, font );
    }
}

void H3_drawHexagon( H3_Game & game, const de::Recti& pos,
                    const uint32_t color, const de::gpu::TexRef& tex, int num )
{
    auto driver = game.getDriver();
    driver->getScreenRenderer()->draw2DHexagon( pos, color, tex );

    if ( num > -1 )
    {
        const float zoom = game.getZoom();
        auto font = de::Font5x8(zoom,zoom);
        H3_drawText( game, pos.centerX(), pos.centerY(), std::to_string(num), 0xFFFFFFFF, de::Align::Centered, font );
    }
}

void H3_drawDice( H3_Game & game, const de::Recti& pos, int r, int dice )
{
    auto driver = game.getDriver();
    auto screenRenderer = driver->getScreenRenderer();
    screenRenderer->draw2DRoundRect( pos, glm::ivec2(r,r), 0xFFFFFFFF, H3_getDiceTex( game, dice ), 13 );
}

void H3_drawDice( H3_Game & game, const de::Recti& pos, int r, int dice1, int dice2 )
{
    auto driver = game.getDriver();
    auto screenRenderer = driver->getScreenRenderer();
    const int x = pos.x;
    const int y = pos.y;
    const int w = pos.w;
    const int h = pos.h;
    const auto k = glm::ivec2(r,r);
    screenRenderer->draw2DRoundRect( de::Recti(x, y, w, h ), k, 0xFFFFFFFF, H3_getDiceTex( game, dice1 ), 11 );
    screenRenderer->draw2DRoundRect( de::Recti(x + w + 10, y, w, h ), k, 0xFFFFFFFF, H3_getDiceTex( game, dice2 ), 11 );
}

void H3_drawText( H3_Game & game, int x, int y, const std::string& msg, const uint32_t color, const de::Align align,
                  const de::Font5x8 & font5x8, uint32_t bgColor, int padding )
{
    auto driver = game.getDriver();
    driver->draw2DText( x,y, msg, color, align, font5x8, bgColor, padding );
}

void H3_drawText( H3_Game & game, int x, int y, const std::string& msg, const uint32_t color, const de::Align align,
                  const de::Font & font, uint32_t bgColor, int padding )
{
    auto driver = game.getDriver();
    driver->draw2DText( x,y, msg, color, align, font, bgColor, padding );
}

/*

void UI_CpuPlayer_draw(
        H3_Game& game,
        uint32_t playerId,
        const de::Recti& pos,
        bool bDebug)
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
        auto tex1 = H3_getDiceTex( game, player.state.dice.a );
        auto tex2 = H3_getDiceTex( game, player.state.dice.b );
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

void UI_Stats_update( H3_Game & game )
{
    // Nothing todo (yet), render method does gathering.

    UI_Stats_show( game );


    H3_Player & player = H3_getCurrentPlayer( game );

    auto &ui = game.m_ui_stats;
    ui.vic.setNumberText( player.victoryPoints );
    ui.card.setNumberText( player.eventCards.size() );
    ui.road.setNumberText( player.roads.size() );
    ui.farm.setNumberText( player.farms.size() );
    ui.city.setNumberText( player.citys.size() );

    ui.a.setNumberText( player.bank.A );
    ui.b.setNumberText( player.bank.B );
    ui.c.setNumberText( player.bank.C );
    ui.d.setNumberText( player.bank.D );
    ui.e.setNumberText( player.bank.E );

    // Stats

    if ( player.knightCards.size() > 0 )
    {
        UI_get( game, H3_UI::BtnStatKnightCard, __func__ ).visible = true;
        UI_get( game, H3_UI::LblStatKnightCard, __func__ ).visible = true;
        UI_get( game, H3_UI::LblStatKnightCard, __func__ ).msg = std::to_string( player.knightCards.size() );

    }
    if ( player.pointCards.size() > 0 )
    {
        UI_get( game, H3_UI::BtnStatPointCard, __func__ ).visible = true;
        UI_get( game, H3_UI::LblStatPointCard, __func__ ).visible = true;
        UI_get( game, H3_UI::LblStatPointCard, __func__ ).msg = std::to_string( player.pointCards.size() );
    }


    H3_UI_Stats_hide( game );

    H3_Player & player = H3_getCurrentPlayer( game );

    // Stats
    if ( player.victoryPoints > 0 )
    {
        UI_get( game, H3_UI::BtnStatVic, __func__ ).visible = true;
        UI_get( game, H3_UI::LblStatVic, __func__ ).visible = true;
        UI_get( game, H3_UI::LblStatVic, __func__ ).msg = std::to_string( player.victoryPoints );
    }

    if ( player.eventCards.size() )
    {
        UI_get( game, H3_UI::BtnStatCard, __func__ ).visible = true;
        UI_get( game, H3_UI::LblStatCard, __func__ ).visible = true;
        UI_get( game, H3_UI::LblStatCard, __func__ ).msg = std::to_string( player.eventCards.size() );
    }
    if ( player.roads.size() )
    {
        UI_get( game, H3_UI::BtnStatRoad, __func__ ).visible = true;
        UI_get( game, H3_UI::LblStatRoad, __func__ ).visible = true;
        UI_get( game, H3_UI::LblStatRoad, __func__ ).msg = std::to_string( player.roads.size() );
    }
    if ( player.farms.size() )
    {
        UI_get( game, H3_UI::BtnStatFarm, __func__ ).visible = true;
        UI_get( game, H3_UI::LblStatFarm, __func__ ).visible = true;
        UI_get( game, H3_UI::LblStatFarm, __func__ ).msg = std::to_string( player.farms.size() );
    }
    if ( player.citys.size() )
    {
        UI_get( game, H3_UI::BtnStatCity, __func__ ).visible = true;
        UI_get( game, H3_UI::LblStatCity, __func__ ).visible = true;
        UI_get( game, H3_UI::LblStatCity, __func__ ).msg = std::to_string( player.citys.size() );
    }
    if ( player.bank.A > 0 )
    {
        UI_get( game, H3_UI::BtnStatA, __func__ ).visible = true;
        UI_get( game, H3_UI::LblStatA, __func__ ).visible = true;
        UI_get( game, H3_UI::LblStatA, __func__ ).msg = std::to_string( player.bank.A );
    }
    if ( player.bank.B > 0 )
    {
        UI_get( game, H3_UI::BtnStatB, __func__ ).visible = true;
        UI_get( game, H3_UI::LblStatB, __func__ ).visible = true;
        UI_get( game, H3_UI::LblStatB, __func__ ).msg = std::to_string( player.bank.B );
    }
    if ( player.bank.C > 0 )
    {
        UI_get( game, H3_UI::BtnStatC, __func__ ).visible = true;
        UI_get( game, H3_UI::LblStatC, __func__ ).visible = true;
        UI_get( game, H3_UI::LblStatC, __func__ ).msg = std::to_string( player.bank.C );
    }
    if ( player.bank.D > 0 )
    {
        UI_get( game, H3_UI::BtnStatD, __func__ ).visible = true;
        UI_get( game, H3_UI::LblStatD, __func__ ).visible = true;
        UI_get( game, H3_UI::LblStatD, __func__ ).msg = std::to_string( player.bank.D );
    }
    if ( player.bank.E > 0 )
    {
        UI_get( game, H3_UI::BtnStatE, __func__ ).visible = true;
        UI_get( game, H3_UI::LblStatE, __func__ ).visible = true;
        UI_get( game, H3_UI::LblStatE, __func__ ).msg = std::to_string( player.bank.E );
    }
    if ( player.knightCards.size() > 0 )
    {
        UI_get( game, H3_UI::BtnStatKnightCard, __func__ ).visible = true;
        UI_get( game, H3_UI::LblStatKnightCard, __func__ ).visible = true;
        UI_get( game, H3_UI::LblStatKnightCard, __func__ ).msg = std::to_string( player.knightCards.size() );

    }
    if ( player.pointCards.size() > 0 )
    {
        UI_get( game, H3_UI::BtnStatPointCard, __func__ ).visible = true;
        UI_get( game, H3_UI::LblStatPointCard, __func__ ).visible = true;
        UI_get( game, H3_UI::LblStatPointCard, __func__ ).msg = std::to_string( player.pointCards.size() );
    }
}

void UI_Stats_setVisible( H3_Game & game, bool bVisible )
{
}
void UI_Stats_hide( H3_Game & game )
{
    UI_Stats_setVisible( game, false );
}
void UI_Stats_show( H3_Game & game )
{
    UI_Stats_setVisible( game, true );
}
void UI_drawDlgLooseCards( H3_Game & game )
{
    auto driver = game.getDriver();
    if ( !driver ) { DE_ERROR("No driver") return; }
}
int UI_computeBankRatio( H3_Game & game, int bankingMode )
{
    H3_Player & player = H3_getCurrentPlayer( game );

    int ratio = 4;
    if ( player.hasPort3v1 ) ratio = 3;

    if ( bankingMode == 0 && player.hasPortA )
    {
        ratio = 2;
    }
    if ( bankingMode == 1 && player.hasPortB )
    {
        ratio = 2;
    }
    if ( bankingMode == 2 && player.hasPortC )
    {
        ratio = 2;
    }
    if ( bankingMode == 3 && player.hasPortD )
    {
        ratio = 2;
    }
    if ( bankingMode == 4 && player.hasPortE )
    {
        ratio = 2;
    }
    return ratio;
}

void UI_doBanking( H3_Game & game, int bankingMode )
{
    if ( game.m_state != H3_State::Idle ) { DE_ERROR("No correct idle state") return; }
    H3_Player & player = H3_getCurrentPlayer( game );

    int ratio = UI_computeBankRatio( game, bankingMode );
    if ( game.m_bankingMode == 0 && ratio > player.bank.A )
    {
        DE_DEBUG("[Bank] Not enough A(",player.bank.A,")")
        return;
    }
    if ( game.m_bankingMode == 1 && ratio > player.bank.B )
    {
        DE_DEBUG("[Bank] Not enough B(",player.bank.B,")")
        return;
    }
    if ( game.m_bankingMode == 2 && ratio > player.bank.C )
    {
        DE_DEBUG("[Bank] Not enough C(",player.bank.C,")")
        return;
    }
    if ( game.m_bankingMode == 3 && ratio > player.bank.D )
    {
        DE_DEBUG("[Bank] Not enough D(",player.bank.D,")")
        return;
    }
    if ( game.m_bankingMode == 4 && ratio > player.bank.E )
    {
        DE_DEBUG("[Bank] Not enough E(",player.bank.E,")")
        return;
    }

    DE_DEBUG("[Bank] playerId(", player.id,"), res(",bankingMode,"), ratio(",ratio,")")

    UI_get( game, H3_UI::BankLblA ).msg = "0";
    UI_get( game, H3_UI::BankLblB ).msg = "0";
    UI_get( game, H3_UI::BankLblC ).msg = "0";
    UI_get( game, H3_UI::BankLblD ).msg = "0";
    UI_get( game, H3_UI::BankLblE ).msg = "0";

    if ( game.m_bankingMode == 0 )
    {
        UI_get( game, H3_UI::BankLblA ).msg = std::to_string( ratio );
    }
    else if ( game.m_bankingMode == 1 )
    {
        UI_get( game, H3_UI::BankLblB ).msg = std::to_string( ratio );
    }
    else if ( game.m_bankingMode == 2 )
    {
        UI_get( game, H3_UI::BankLblC ).msg = std::to_string( ratio );
    }
    else if ( game.m_bankingMode == 3 )
    {
        UI_get( game, H3_UI::BankLblD ).msg = std::to_string( ratio );
    }
    else if ( game.m_bankingMode == 4 )
    {
        UI_get( game, H3_UI::BankLblE ).msg = std::to_string( ratio );
    }

    game.m_state = H3_State::Banking;
    game.m_bankingMode = bankingMode;
    UI_update( game );
}

void UI_cancelBanking( H3_Game & game )
{
    if ( game.m_state == H3_State::Banking )
    {
        DE_DEBUG(__func__)
        game.m_state = H3_State::Idle;
        game.m_bankingMode = -1;
        game.m_bankingWish = -1;
        UI_update( game );
    }
}

void UI_finishBanking( H3_Game & game )
{
    if ( game.m_state != H3_State::Banking )
    {
        DE_ERROR("No correct banking state") return;
    }

    H3_Player & player = H3_getCurrentPlayer( game );
    DE_DEBUG("[EndBank] playerId(", player.id,"), bankingMode(",game.m_bankingMode,")")

    int ratio = UI_computeBankRatio( game, game.m_bankingMode );
    if ( game.m_bankingMode == 0 )
    {
        player.bank.A -= ratio;
    }
    else if ( game.m_bankingMode == 1 )
    {
        player.bank.B -= ratio;
    }
    else if ( game.m_bankingMode == 2 )
    {
        player.bank.C -= ratio;
    }
    else if ( game.m_bankingMode == 3 )
    {
        player.bank.D -= ratio;
    }
    else if ( game.m_bankingMode == 4 )
    {
        player.bank.E -= ratio;
    }

    if ( game.m_bankingWish == 0 )
    {
        player.bank.A++;
    }
    else if ( game.m_bankingWish == 1 )
    {
        player.bank.B++;
    }
    else if ( game.m_bankingWish == 2 )
    {
        player.bank.C++;
    }
    else if ( game.m_bankingWish == 3 )
    {
        player.bank.D++;
    }
    else if ( game.m_bankingWish == 4 )
    {
        player.bank.E++;
    }

    game.m_state = H3_State::Idle;
    game.m_bankingMode = -1;
    game.m_bankingWish = -1;

    UI_update( game );
}


void UI_setBankingVisible( H3_Game & game, bool bVisible )
{
    UI_get( game, H3_UI::BankLblTitle ).visible = bVisible;
    UI_get( game, H3_UI::BankBtnA ).visible = bVisible;
    UI_get( game, H3_UI::BankLblA ).visible = bVisible;
    UI_get( game, H3_UI::BankBtnB ).visible = bVisible;
    UI_get( game, H3_UI::BankLblB ).visible = bVisible;
    UI_get( game, H3_UI::BankBtnC ).visible = bVisible;
    UI_get( game, H3_UI::BankLblC ).visible = bVisible;
    UI_get( game, H3_UI::BankBtnD ).visible = bVisible;
    UI_get( game, H3_UI::BankLblD ).visible = bVisible;
    UI_get( game, H3_UI::BankBtnE ).visible = bVisible;
    UI_get( game, H3_UI::BankLblE ).visible = bVisible;
    UI_get( game, H3_UI::BankBtnOK ).visible = bVisible;
    UI_get( game, H3_UI::BankBtnCancel ).visible = bVisible;
}

void
UI_hideBanking( H3_Game & game ) { UI_setBankingVisible( game, false ); }

void
UI_showBanking( H3_Game & game ) { UI_setBankingVisible( game, true ); }

void
UI_drawStatNum( H3_Game & game, int img, de::Recti pos, int value, int icon_width )
{
    auto driver = game.getDriver();
    if ( !driver ) { DE_ERROR("No driver") return; }

    de::gpu::TexRef tex = game.getTex(H3_Tex::Unknown, __func__);
    if ( img > 0 )
    {
        tex = game.getTex(H3_Tex::eID(img), __func__);
    }

    de::Recti iconPos = pos;
    //   if ( icon_width > 0 )
    //   {
    //      iconPos.w = icon_width;
    //   }
    H3_draw2DRect( game, iconPos, 0xFFFFFFFF, tex );

    auto valueStr = std::to_string( value );

    de::Font5x8 font;
    font.quadWidth = 2;
    font.quadHeight = 2;
    font.quadSpacingX = 0;
    font.quadSpacingY = 0;
    font.glyphSpacingX = 1;
    font.glyphSpacingY = 1;
    auto ts = font.getTextSize( valueStr );

    int msg_x = pos.x + pos.w/2;
    int msg_y = pos.y + pos.h - 1;
    de::Recti r(msg_x - 2 - int(0.5f*ts.width),
                msg_y - 1 - int(ts.height),
                int(ts.width) + 2,
                int(ts.height) + 1 );

    auto font58Renderer = driver->getFontRenderer5x8();

    H3_draw2DRect( game, r, 0x80000000 );
    H3_drawText( game, msg_x, msg_y, valueStr,
    0xFFFFFFFF, de::Align::BottomCenter, font );
}

void
UI_drawPlayerStats( H3_Game & game, int px, int py, int playerId )
{
    auto driver = game.getDriver();
    if ( !driver ) { DE_ERROR("No driver") return; }
    H3_Player & current = game.getCurrentPlayer();
    H3_Player & player = game.getPlayer( playerId );

    const float zoom = game.getZoom();
    //int p = 20;
    //de::Font nameFont( "garton", 26, false, false, true, true, false );
    de::Font5x8 nameFont(zoom * 2, zoom * 2,0,0,1,1);

    // Draw current Player highlight
    de::Recti r_panel(px,py,280,96);
    H3_draw2DRoundRect( game, r_panel, glm::ivec2(12,12), 0x40FFFFFF, 16 );

    // Draw current Player highlight
    de::Recti r_highlight( r_panel.x+2, r_panel.y+2, r_panel.w-4, r_panel.h-4 );
    if ( playerId == current.id )
    {
        H3_draw2DRoundRect( game, r_highlight, glm::ivec2(12,12), 0x40FFFFFF, 16 );
    }

    // Draw Player background...
    {
        de::gpu::TexRef ref;
        int x = r_panel.x+8;
        int y = r_panel.y+8;
        int w = r_panel.w-16;
        int h = r_panel.h-16;
        de::Recti r_bg( x, y, w, h );
        H3_draw2DRoundRect( game, r_bg, glm::ivec2(8,8), ref, player.color, 16 );
    }

    // Draw Player Icon
    {
        de::gpu::TexRef ref = game.getTex(static_cast<H3_Tex::eID>(player.avatar), __func__);
        int x = r_panel.x+16;
        int y = r_panel.y+16;
        int w = 64;
        int h = 64;
        de::Recti r_icon( x, y, w, h );
        H3_draw2DRoundRect( game, r_icon, glm::ivec2(8,8), ref, 0xFFFFFFFF, 16 );
    }

    // Draw Player Name
    de::Align::EAlign align = de::Align::TopLeft;
    {
        int x = r_panel.x+96;
        int y = r_panel.y+8;
        int w = 100;
        int h = 28;
        de::Recti r_name( x, y, w, h );
        int cx = r_name.centerX();
        int cy = r_name.centerY();
        H3_drawText( game, cx,cy, player.name, 0xFFFFFFFF, de::Align::Centered,
                     nameFont, 0x80000000, 5);
        H3_draw2DRoundRect( game, r_name, glm::ivec2(8,8), player.color, 16 );

        x = r_panel.x+102;
        y = r_panel.y+10;
        std::wostringstream s;
        s << player.name.c_str();
    }

    // Draw Player Dice 16 * 16
    {
        int x = r_panel.x+204;
        int y = r_panel.y+10;
        int dice1 = player.state.dice.a;
        int dice2 = player.state.dice.b;
        H3_drawDice( game, de::Recti(x,y, 24,24), 5, dice1, dice2 );
    }

    {
        int x = r_panel.x+96;
        int y = r_panel.y+36;
        int w = 162;
        int h = 52;
        de::Recti r_stats( x, y, w, h );
        H3_draw2DRoundRect( game, r_stats, glm::ivec2(8,8), player.color, 16 );
    }

    int d = 25;
    int w = 24;
    int h = 24;
    int x = r_panel.x+103;
    int y = r_panel.y+38;

    // Draw 24x24 Player Victory Points
    UI_drawStatNum( game, H3_Tex::StatVic, de::Recti(x,y,w,h), player.state.victoryPoints, 0); x += d;
    UI_drawStatNum( game, H3_Tex::StatRoad, de::Recti(x,y,w,h), player.state.roads.size(), 0); x += d;
    UI_drawStatNum( game, H3_Tex::StatFarm, de::Recti(x,y,w,h), player.state.farms.size(), 0); x += d;
    UI_drawStatNum( game, H3_Tex::StatCity, de::Recti(x,y,w,h), player.state.citys.size(), 0); x += d;
    UI_drawStatNum( game, H3_Tex::StatCard, de::Recti(x,y,w,h), player.state.knightCards.size(), 0); x += d;
    UI_drawStatNum( game, H3_Tex::EventCardPoint, de::Recti(x,y,w,h), player.state.pointCards.size(), 16); // x += d;

    // <--> ROW 2 <-->
    x = r_panel.x+103;
    y = r_panel.y+62;
    UI_drawStatNum( game, H3_Tex::CardA, de::Recti(x,y,w,h), player.state.bank.A, 16); x += d;
    UI_drawStatNum( game, H3_Tex::CardB, de::Recti(x,y,w,h), player.state.bank.B, 16); x += d;
    UI_drawStatNum( game, H3_Tex::CardC, de::Recti(x,y,w,h), player.state.bank.C, 16); x += d;
    UI_drawStatNum( game, H3_Tex::CardD, de::Recti(x,y,w,h), player.state.bank.D, 16); x += d;
    UI_drawStatNum( game, H3_Tex::CardE, de::Recti(x,y,w,h), player.state.bank.E, 16); x += d;
    UI_drawStatNum( game, H3_Tex::EventCardKnight, de::Recti(x,y,w,h), player.state.knightCards.size(), 16); // x += d;
}
*/
