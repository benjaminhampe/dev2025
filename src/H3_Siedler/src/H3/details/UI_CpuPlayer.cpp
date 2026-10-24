#include "UI_CpuPlayer.h"
#include <H3/H3_Game.h>
#include <H3/details/H3_UI.h>

void UI_CpuPlayer_draw(
        H3_Game& game,
        uint32_t playerId,
        const de::Recti& pos)
{
    //DE_DEBUG("Draw(",pos.str(),")")

    auto zoom = game.getZoom();
    int p = zoom * 8;

    auto driver = game.getDriver();

    // Debug Outline
    driver->getScreenRenderer()->
        draw2DRectLine( pos, 0xFFFFFFFF );

    // Background
    auto t_scroll = game.getTex(H3_Tex::Scroll2, __func__);
    driver->getScreenRenderer()->
        draw2DRect( pos, 0xFFFFFFFF, t_scroll );

    // Get Player
    if (playerId < 1) { return; }
    auto & player = game.getPlayer(playerId);

    // Calc Icon Background
    int icon_x = pos.x + 2*p;
    int icon_y = pos.y + 2*p;
    int icon_w = 2 * pos.h / 3;
    int icon_h = icon_w;
    auto r_iconBg = de::Recti(icon_x, icon_y, icon_w, icon_h);

    // Calc Name Background
    int name_x = r_iconBg.x2() - 2*p;
    int name_y = r_iconBg.y + p;
    int name_w = pos.w - 3*p - icon_w;
    int name_h = pos.h / 3;
    auto r_nameBg = de::Recti( name_x, name_y, name_w, name_h );

    // Draw Icon Background
    driver->getScreenRenderer()->
        draw2DRoundRect( r_iconBg, {p,p}, player.color );

    // Draw Name Background
    driver->getScreenRenderer()->
        draw2DRoundRect( r_nameBg, {p,p}, player.color );

    // Draw Icon
    auto icon = game.getTex((H3_Tex::eID)player.avatar, __func__);
    driver->getScreenRenderer()->
        draw2DRoundRect( r_iconBg.inset(p,p+2,p,p), {p,p}, icon );

    // Draw Name
    auto r_name = r_nameBg.inset(p);
    driver->draw2DText( r_name.centerX(),
                        r_name.centerY(),
                        player.name,
                        dbRGB(0,0,0),
                        de::Align::Centered,
                        de::Font("Postamt",zoom * 28),
                        dbRGB(255,255,255,225),
                        p );


/*
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
*/

    // Dice 16 * 16
    {
        int dice_x = pos.x+20;
        int dice_y = pos.y+20 + icon_h;
        int dice_w = zoom * 24;
        int dice_h = dice_w;
        auto r = glm::ivec2{5,5};
        // H3_drawDice( game, de::Recti(x,y, 24,24), 5, dice1, dice2 );
        auto tex1 = H3_getDiceTex( game, player.state.dice.a );
        auto tex2 = H3_getDiceTex( game, player.state.dice.b );
        de::Recti r_dice1(dice_x, dice_y, dice_w, dice_h ); dice_x += 2 + dice_w;
        de::Recti r_dice2(dice_x, dice_y, dice_w, dice_h );
        driver->getScreenRenderer()->
            draw2DRoundRect( r_dice1, r, 0xFFFFFFFF, tex1, 12 );
        driver->getScreenRenderer()->
            draw2DRoundRect( r_dice2, r, 0xFFFFFFFF, tex2, 12 );
    }


    auto drawStatNum = [&]( int img, de::Recti pos, int value, int icon_width )
    {
        const auto tex = game.getTex(H3_Tex::eID(img), __func__);

        driver->getScreenQuadRenderer()->
            draw2DRect( pos, tex, false );

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
        int x = icon_x + icon_w + 10;
        int y = name_y + name_h + 10;
        int w = zoom * 39;
        int h = zoom * 39;
        int s = 1; // spacing
        drawStatNum( H3_Tex::StatVic, de::Recti(x,y,w,h), player.state.victoryPoints, 16); x += w + s;
        drawStatNum( H3_Tex::StatRoad, de::Recti(x,y,w,h), player.state.roads.size(), 16); x += w + s;
        drawStatNum( H3_Tex::StatFarm, de::Recti(x,y,w,h), player.state.farms.size(), 16); x += w + s;
        drawStatNum( H3_Tex::StatCity, de::Recti(x,y,w,h), player.state.citys.size(), 16); x += w + s;

        u32 n = player.state.numEventCards(true);
        drawStatNum( H3_Tex::StatCard, de::Recti(x,y,w,h), n, 16);
    }

        //     drawStatNum( H3_Tex::CardA, de::Recti(x,y,w,h), player.state.bank.A, 16); x += w + s;
        // drawStatNum( H3_Tex::CardB, de::Recti(x,y,w,h), player.state.bank.B, 16); x += w + s;
        // drawStatNum( H3_Tex::CardC, de::Recti(x,y,w,h), player.state.bank.C, 16); x += w + s;
        // drawStatNum( H3_Tex::CardD, de::Recti(x,y,w,h), player.state.bank.D, 16); x += w + s;
        // drawStatNum( H3_Tex::CardE, de::Recti(x,y,w,h), player.state.bank.E, 16); x += w + s;
        // drawStatNum( H3_Tex::EventCardKnight, de::Recti(x,y,w,h), player.state.knightCards.size(), 16); // x += d;


}

#if 0

void UI_Stats_drawItem( H3_Game & game, int x, int y, int w, int h,
                       int texId, int value, de::Font5x8 font )
{
    auto driver = game.getDriver();
    if ( !driver ) { DE_ERROR("No driver") return; }

    const auto tex = game.getTex(H3_Tex::eID(texId), __func__);
    const auto str = std::to_string( value );

    auto r = glm::ivec2(8,8);
    H3_draw2DRoundRect( game, de::Recti(x,y,w,h), r, tex );

    const float zoom = game.getZoom();

    auto ts = font.getTextSize( str );

    int s = zoom;
    int tw = ts.width;
    int th = (ts.height / 8) * 7;

    auto r_num = de::Recti(x + w - 3 - s - tw,
                           y + h - 3 - s - th,
                           tw + 2*s,
                           th + 2*s);

    H3_draw2DRect( game, r_num, dbRGB(0,0,0,128) );

    H3_drawText( game, r_num.x + 1, r_num.y + 1, str, dbRGB(255,255,255), de::Align::Default, font);
}

void UI_Stats_render( H3_Game & game )
{
    const float zoom = game.getZoom();

    H3_Player & player = game.getCurrentPlayer();

    auto driver = game.getDriver();
    //auto screenRenderer = driver->getScreenRenderer();
    //auto font58Renderer = driver->getFontRenderer5x8();
    const auto scr_w = driver->getScreenWidth();
    const auto scr_h = driver->getScreenHeight();

    de::Font5x8 font;
    font.quadWidth = zoom*2;
    font.quadHeight = zoom*2;
    font.quadSpacingX = 0;
    font.quadSpacingY = 0;
    font.glyphSpacingX = 1;
    font.glyphSpacingY = 1;

    const auto playerTex = game.getTex((H3_Tex::eID)player.avatar, __func__ );
    const auto playerName = player.name;
    const auto playerColor = player.color;

    int s = 1; // spacing
    int p = zoom * 5; // padding
    int w = zoom * 39;

    int h0 = zoom * 64;
    int h1 = zoom * 64;
    int h2 = zoom * 39;

    int wT = 5 * w + 2*p;
    int hT = h0 + h1 + h2 + 2*p;

    int x = 0;
    int y = scr_h - 1 - p - h2 - s - h1 - p - h0 - p;

    x = p;
    y = scr_h - 1 - hT;

    auto r = glm::ivec2(8,8);

    H3_draw2DRoundRect( game, de::Recti(x,y,wT,hT), r, playerColor );
    H3_draw2DRoundRect( game, de::Recti(x + p, y + p, h0, h0), r, playerTex );

    x = p;
    y = scr_h - 1 - p - h2 - s - h1;
    UI_Stats_drawItem( game, x,y,w,h1, H3_Tex::CardA, player.state.bank.A, font); x += w + s;
    UI_Stats_drawItem( game, x,y,w,h1, H3_Tex::CardB, player.state.bank.B, font); x += w + s;
    UI_Stats_drawItem( game, x,y,w,h1, H3_Tex::CardC, player.state.bank.C, font); x += w + s;
    UI_Stats_drawItem( game, x,y,w,h1, H3_Tex::CardD, player.state.bank.D, font); x += w + s;
    UI_Stats_drawItem( game, x,y,w,h1, H3_Tex::CardE, player.state.bank.E, font); // x += w + s;

    x = p;
    y = scr_h - 1 - p - h2;
    UI_Stats_drawItem( game, x,y,w,h2, H3_Tex::StatVic, player.state.victoryPoints, font); x += w + s;
    UI_Stats_drawItem( game, x,y,w,h2, H3_Tex::StatRoad, player.state.roads.size(), font); x += w + s;
    UI_Stats_drawItem( game, x,y,w,h2, H3_Tex::StatFarm, player.state.farms.size(), font); x += w + s;
    UI_Stats_drawItem( game, x,y,w,h2, H3_Tex::StatCity, player.state.citys.size(), font); x += w + s;
    UI_Stats_drawItem( game, x,y,w,h2, H3_Tex::StatCard,
                       player.state.knightCards.size(), font); x += w + s;

    // Cards
    int wC = zoom * 39;
    int hC = zoom * 64;
    x += p;
    y = scr_h - 1 - p - hC;
    UI_Stats_drawItem( game, x,y,wC,hC, H3_Tex::EventCardKnight, player.state.knightCards.size(), font); x += wC + s;
    UI_Stats_drawItem( game, x,y,wC,hC, H3_Tex::EventCardPoint, player.state.pointCards.size(), font); // x += wC + s;

    // Bonus
    int wB = zoom * 64;
    int hB = zoom * 64;
    x = scr_w - 1 - p - wB - p - wB;
    y = scr_h - 1 - p - hB;
    UI_Stats_drawItem( game, x,y,wB,hB, H3_Tex::BonusArmy, player.state.hasBonusArmy, font); x += wB + p;
    UI_Stats_drawItem( game, x,y,wB,hB, H3_Tex::BonusRoad, player.state.hasBonusRoad, font); // x += wB + p;
}

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
