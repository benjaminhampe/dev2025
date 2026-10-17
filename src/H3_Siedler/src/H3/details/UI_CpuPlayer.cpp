#include "UI_CpuPlayer.h"
#include <H3/H3_Game.h>
#include <H3/details/H3_UI.h>

void UI_CpuPlayer_draw(
        H3_Game& game,
        uint32_t playerId,
        const de::Recti& pos)
{
    auto zoom = game.getZoom();
    auto driver = game.getDriver();
    int p = zoom * 10;

    // Debug Outline
    driver->getScreenRenderer()->
        draw2DRectLine( pos, 0xFFFFFFFF );

    // Background
    auto t_scroll = game.getTex(H3_Tex::Scroll2, __func__);
    driver->getScreenRenderer()->
        draw2DRect( pos.inset(0,p,0,p), 0xFFFFFFFF, t_scroll );

    // Player Icon

    if (playerId < 1)
    {
        return;
    }

    auto & player = game.getPlayer(playerId);

    // Icon Background
    // auto t_wood = game.getTex(H3_Tex::Wood, __func__);
    int icon_h = pos.h / 2;
    auto r_icon = de::Recti(pos.x,pos.y,icon_h,icon_h);
    driver->getScreenRenderer()->
        draw2DRoundRect( r_icon, {p,p}, player.color );

    // Icon Avatar
    auto avatar = game.getTex((H3_Tex::eID)player.avatar, __func__);
    driver->getScreenRenderer()->
        draw2DRoundRect( r_icon.inset(p), {p,p}, 0xFFFFFFFF, avatar );

    // Name Text
    driver->draw2DText( r_icon.x2() + 2*p,
                        r_icon.y + 2*p,
                        player.name,
                        0xFFFFFFFF,
                        de::Align::TopLeft,
                        de::Font("Postamt",zoom * 32),
                        player.color,
                        p );

    // VictoryPoints - Circle
    {
        // Background Circle with Player color
        int r = zoom * 8;
        int d = zoom * 32;
        auto r0 = de::Recti(pos.x,pos.y,d,d);
        driver->getScreenRenderer()->
            draw2DCircle( r0, player.color );

        // Foreground Circle in white
        driver->getScreenRenderer()->
            draw2DCircle( r0.inset(r,r,r,r), 0xFFFFFFFF );

        // Foreground Text in black
        auto f = de::Font("Postamt", 2 * d / 3);
        auto s = dbStr(player.state.victoryPoints);
        int cx = r0.centerX();
        int cy = r0.centerY();
        driver->getFontRenderer()->
            draw2DText(cx,cy,s,dbRGB(0,0,0),de::Align::Centered,f);
    }

    // Num Resource Cards
    {
        // Background Circle with Player color
        int r = zoom * 8;
        int d = zoom * 32;
        auto r0 = de::Recti(pos.x, pos.y, d, 2*d);
        driver->getScreenRenderer()->
            draw2DRoundRect( r0, {r,r}, player.color );

        // Foreground Circle in white
        driver->getScreenRenderer()->
            draw2DRoundRect( r0.inset(r,r,r,r), {r,r}, 0xFFFFFFFF );

        // Foreground Text in black
        de::Font vicFont("Postamt", 2 * d / 3);
        int cx = r0.centerX();
        int cy = r0.centerY();
        driver->getFontRenderer()->draw2DText(cx,cy,
            dbStr(player.state.victoryPoints),
            dbRGB(0,0,0), de::Align::Centered, vicFont );
    }
    //
}

#if 0
/*
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

#endif
