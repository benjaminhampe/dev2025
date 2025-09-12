#include "UI_ActionLogger.h"
#include <H3/H3_Game.h>
#include <H3/details/H3_Topology.h>
#include <H3/details/H3_UI.h>

void
UI_resetLog( H3_Game & game )
{
    game.m_uiActionLogger.history.clear();
}

void
UI_logCancel( H3_Game & game )
{
    H3_Player & player = game.getCurrentPlayer();

    UI_Action action;
    action.pts = dbTimeInSeconds();
    action.playerId = player.id;
    action.action = int(H3_Tex::DoCancel);

    game.m_uiActionLogger.history.emplace_back( std::move( action ));
}

void
UI_logEndTurn( H3_Game & game )
{
    H3_Player & player = game.getCurrentPlayer();

    UI_Action action;
    action.pts = dbTimeInSeconds();
    action.playerId = player.id;
    action.action = int(H3_Tex::DoEndTurn);

    game.m_uiActionLogger.history.emplace_back( std::move( action ));
}

void
UI_logDice( H3_Game & game )
{
    H3_Player & player = game.getCurrentPlayer();

    UI_Action action;
    action.pts = dbTimeInSeconds();
    action.playerId = player.id;
    action.action = int(H3_Tex::DoDice);
    action.object = player.state.dice.a;
    action.amount = player.state.dice.b;

    game.m_uiActionLogger.history.emplace_back( std::move( action ));
}

void
UI_logBank( H3_Game & game, H3_Bank in, H3_Bank out )
{
    H3_Player & player = game.getCurrentPlayer();

    UI_Action action;
    action.pts = dbTimeInSeconds();
    action.playerId = player.id;
    action.action = int(H3_Tex::DoDice);
    action.bankIn = in;
    action.bankOut = out;

    game.m_uiActionLogger.history.emplace_back( std::move( action ));
}

void
UI_logBuyRoad( H3_Game & game, int edgeId )
{
    H3_Player & player = game.getCurrentPlayer();

    UI_Action action;
    action.pts = dbTimeInSeconds();
    action.playerId = player.id;
    action.action = int(H3_Tex::DoBuyRoad);
    action.object = edgeId;

    game.m_uiActionLogger.history.emplace_back( std::move( action ));
}

void
UI_logBuyFarm( H3_Game & game, int cornerId )
{
    H3_Player & player = game.getCurrentPlayer();

    UI_Action action;
    action.pts = dbTimeInSeconds();
    action.playerId = player.id;
    action.action = int(H3_Tex::DoBuyFarm);
    action.object = cornerId;

    game.m_uiActionLogger.history.emplace_back( std::move( action ));
}

void
UI_logBuyCity( H3_Game & game, int cornerId )
{
    H3_Player & player = game.getCurrentPlayer();

    UI_Action action;
    action.pts = dbTimeInSeconds();
    action.playerId = player.id;
    action.action = int(H3_Tex::DoBuyCity);
    action.object = cornerId;

    game.m_uiActionLogger.history.emplace_back( std::move( action ));
}

void
UI_logBuyCard( H3_Game & game, int cardType )
{
    H3_Player & player = game.getCurrentPlayer();

    UI_Action action;
    action.pts = dbTimeInSeconds();
    action.playerId = player.id;
    action.action = int(H3_Tex::DoBuyEventCard);
    //action.object = edgeId;

    game.m_uiActionLogger.history.emplace_back( std::move( action ));
}

void
UI_drawLogEndTurn( H3_Game & game, const UI_Action& act, int x, int y )
{
    H3_Player & player = game.getPlayer( act.playerId );

    H3_draw2DRect( game, de::Recti( x,y, 10,10), player.color );
    x += 12;

    auto msg = dbStr("[EndTurn] for ",player.name);

    auto driver = game.getDriver();
    //auto screenRenderer = driver->getScreenRenderer();
    auto font58Renderer = driver->getFontRenderer5x8();

    const auto& font = game.m_uiActionLogger.font;
    //auto ts = font.getTextSize( msg );
    font58Renderer->draw2DText( x,y, msg, 0xFFFFFFFF, de::Align::Default, font );
    //x += ts.width + 5;
}

void
UI_drawLogCancel( H3_Game & game, const UI_Action& act, int x, int y )
{
//   H3_Player & player = game.getPlayer( act.player );

//   auto driver = game.driver;
//   driver->draw2DRect( de::Recti( x,y, 10,10), player.color );
//   x += 12;

//   std::ostringstream s;
//   s << "[Cancel] from " << player.name;
//   std::string msg = s.str();

//   auto ts = font58Renderer->getTextSize( msg );
//   font58Renderer->draw2DText( x,y, msg, 0xFFFFFFFF );
//   x += ts.x + 5;

}

void
UI_drawLogDice( H3_Game & game, const UI_Action& act, int x, int y )
{
    H3_Player & player = game.getPlayer( act.playerId );

    H3_draw2DRect( game, de::Recti( x,y, 10,10), player.color );
    x += 12;

    std::ostringstream o;
    o << "[Dice] for " << player.name << " with eyes "
      << act.object << " + " << act.amount;
    std::string msg = o.str();

    auto driver = game.getDriver();
    //auto screenRenderer = driver->getScreenRenderer();
    auto font58Renderer = driver->getFontRenderer5x8();

    const auto& font = game.m_uiActionLogger.font;
    //const auto ts = font.getTextSize( msg );
    font58Renderer->draw2DText( x,y, msg, 0xFFFFFFFF, de::Align::Default, font );
    //x += ts.width + 5;
}

void
UI_drawLogBank( H3_Game & game, const UI_Action& act, int x, int y )
{
    H3_Player & player = game.getPlayer( act.playerId );

    H3_draw2DRect( game, de::Recti( x,y, 10,10), player.color );
    x += 12;

    std::ostringstream s;
    s << "[Banking] for " << player.name << " with in "
    << act.bankIn.str() << " + " << act.bankOut.str();

    std::string msg = s.str();

    auto driver = game.getDriver();
    //auto screenRenderer = driver->getScreenRenderer();
    auto font58Renderer = driver->getFontRenderer5x8();

    const auto& font = game.m_uiActionLogger.font;
    //const auto ts = font.getTextSize( msg );
    font58Renderer->draw2DText( x,y, msg, 0xFFFFFFFF, de::Align::Default, font );
    //x += ts.width + 5;
}

void
UI_drawLogBuyCard( H3_Game & game, const UI_Action& act, int x, int y )
{
    H3_Player & player = game.getPlayer( act.playerId );

    H3_draw2DRect( game, de::Recti( x,y, 10,10), player.color );
    x += 12;

    auto s = dbStr("[BuyCard] for ",player.name," with bank ",player.state.bank.str());

    auto driver = game.getDriver();
    //auto screenRenderer = driver->getScreenRenderer();
    auto font58Renderer = driver->getFontRenderer5x8();

    const auto& font = game.m_uiActionLogger.font;
    //const auto ts = font.getTextSize( msg );
    font58Renderer->draw2DText(x,y,s,0xFFFFFFFF,de::Align::TopLeft,font);
    //x += ts.width + 5;

    //driver->draw2DRoundRect( de::Recti( x,y, 10,10), 1,1,15, player.color );
}
void
UI_drawLogBuyRoad( H3_Game & game, const UI_Action& act, int x, int y )
{
    H3_Player & player = game.getPlayer( act.playerId );

    H3_draw2DRect( game, de::Recti( x,y, 10,10), player.color );
    x += 12;

    auto s = dbStr("[BuyRoad] for ",player.name," with bank ",player.state.bank.str());

    auto driver = game.getDriver();
    //auto screenRenderer = driver->getScreenRenderer();
    auto font58Renderer = driver->getFontRenderer5x8();

    const auto& font = game.m_uiActionLogger.font;
    //const auto ts = font.getTextSize( msg );
    font58Renderer->draw2DText(x,y,s,0xFFFFFFFF,de::Align::TopLeft,font);
    //x += ts.width + 5;
}

void
UI_drawLogBuyFarm( H3_Game & game, const UI_Action& act, int x, int y )
{
    H3_Player & player = game.getPlayer( act.playerId );

    H3_draw2DRect( game, de::Recti( x,y, 10,10), player.color );
    x += 12;

    auto s = dbStr("[BuyFarm] for ",player.name," with bank ",player.state.bank.str());

    auto driver = game.getDriver();
    //auto screenRenderer = driver->getScreenRenderer();
    auto rendFont58 = driver->getFontRenderer5x8();

    const auto& font = game.m_uiActionLogger.font;
    //const auto ts = font.getTextSize( s );
    rendFont58->draw2DText(x,y,s,0xFFFFFFFF,de::Align::TopLeft,font);
    //x += ts.width + 5;
}

void
UI_drawLogBuyCity( H3_Game & game, const UI_Action& act, int x, int y )
{
    H3_Player & player = game.getPlayer( act.playerId );

    H3_draw2DRect( game, de::Recti( x,y, 10,10), player.color );
    x += 12;

    auto s = dbStr("[BuyCity] for ",player.name," with bank ",player.state.bank.str());

    auto driver = game.getDriver();
    //auto screenRenderer = driver->getScreenRenderer();
    auto font58Renderer = driver->getFontRenderer5x8();

    const auto& font = game.m_uiActionLogger.font;
    //const auto ts = font.getTextSize( msg );
    font58Renderer->draw2DText( x,y, s, 0xFFFFFFFF, de::Align::Default, font );
    //x += ts.width + 5;
}

void
UI_drawLog( H3_Game & game )
{
    auto driver = game.getDriver();
    int w = driver->getScreenWidth();
    int h = driver->getScreenHeight();
    int dx = 300;
    int dy = 450;
    int x = w - 1 - dx;
    int y = (h - dy)/2;
    int ln = 11;
    auto r = glm::ivec2(10,10);
    H3_draw2DRoundRect( game, de::Recti(x,y,dx,dy), r, 0x80000000, 15 );
    x += 10;
    y += 5;
    int act_start = std::max( 0, int(game.m_uiActionLogger.history.size()) - 45 );
    for ( int i = act_start; i < int(game.m_uiActionLogger.history.size()); ++i )
    {
        UI_Action const & act = game.m_uiActionLogger.history[ i ];
        if ( act.action == int(H3_Tex::DoEndTurn) )
        {
            UI_drawLogEndTurn( game, act, x, y );
        }
        else if ( act.action == int(H3_Tex::DoCancel) )
        {
            UI_drawLogCancel( game, act, x, y );
        }
        else if ( act.action == int(H3_Tex::DoDice) )
        {
            UI_drawLogDice( game, act, x, y );
        }
        else if ( act.action == int(H3_Tex::DoBank) )
        {
            UI_drawLogBank( game, act, x, y );
        }
        else if ( act.action == int(H3_Tex::DoBuyEventCard) )
        {
            UI_drawLogBuyCard( game, act, x, y );
        }
        else if ( act.action == int(H3_Tex::DoBuyRoad) )
        {
            UI_drawLogBuyRoad( game, act, x, y );
        }
        else if ( act.action == int(H3_Tex::DoBuyFarm) )
        {
            UI_drawLogBuyFarm( game, act, x, y );
        }
        else if ( act.action == int(H3_Tex::DoBuyCity) )
        {
            UI_drawLogBuyCity( game, act, x, y );
        }
        y += ln;
    }
}


/*

void
UI_drawStatNum( H3_Game & game, int img, de::Recti pos, int value, int icon_width )
{
    auto driver = game.getDriver();
    if ( !driver ) { DE_ERROR("No driver") return; }

    de::TexRef tex = H3_getTex( game, H3_Tex::Unknown );
    if ( img > 0 )
    {
        tex = H3_getTex( game, H3_Tex::eID(img) );
    }

    de::Recti iconPos = pos;
    //   if ( icon_width > 0 )
    //   {
    //      iconPos.w = icon_width;
    //   }
    H3_draw2DRect( game, iconPos, 0xFFFFFFFF, tex );

    auto valueStr = std::to_string( value );

    de::Font5x8 font;
    font.quadWidth = 1.0f;
    font.quadHeight = 1.0f;
    font.quadSpacingX = 0.0f;
    font.quadSpacingY = 0.0f;
    font.glyphSpacingX = 1.0f;
    font.glyphSpacingY = 1.0f;
    auto ts = font.getTextSize( valueStr );

    int msg_x = pos.x + pos.w/2;
    int msg_y = pos.y + pos.h - 1;
    de::Recti r_msg( msg_x-2-int(0.5f*ts.width), msg_y-1-int(ts.height), int(ts.width)+2, int(ts.height)+1 );

    auto font58Renderer = driver->getFontRenderer5x8();

    H3_draw2DRect( game, r_msg, 0x80000000 );
    font58Renderer->draw2DText( msg_x, msg_y, valueStr,
    0xFFFFFFFF, de::Align::BottomCenter, font );
}

void
UI_drawPlayerStats( H3_Game & game, int px, int py, int playerId )
{
    auto driver = &game.m_videoDriver;
    if ( !driver ) { DE_ERROR("No driver") return; }
    H3_Player & current = H3_getCurrentPlayer( game );
    H3_Player & player = game.getPlayer( playerId );

    //int p = 20;
    de::Font fontName( "garton", 26, false, false, true, true, false );
    //de::Font font( "garton", 12, false, false, true, true, false );

    de::TexRef noTex;
    // Draw current Player highlight
    de::Recti r_panel(px,py,280,96);
    H3_draw2DRoundRect( game, r_panel, glm::ivec2(12,12), 0x40FFFFFF, noTex, 16 );

    // Draw current Player highlight
    de::Recti r_highlight( r_panel.x+2, r_panel.y+2, r_panel.w-4, r_panel.h-4 );
    if ( playerId == current.id )
    {
        H3_draw2DRoundRect( game, r_highlight, glm::ivec2(12,12), 0x40FFFFFF, noTex, 16 );
    }

    // Draw Player background...
    {
        de::TexRef ref;
        int x = r_panel.x+8;
        int y = r_panel.y+8;
        int w = r_panel.w-16;
        int h = r_panel.h-16;
        de::Recti r_bg( x, y, w, h );
        H3_draw2DRoundRect( game, r_bg, glm::ivec2(8,8), player.color, ref,16 );
    }

    // Draw Player Icon
    {
        de::TexRef ref = H3_getTex( game, static_cast<H3_Tex::eID>(player.avatar) );
        int x = r_panel.x+16;
        int y = r_panel.y+16;
        int w = 64;
        int h = 64;
        de::Recti r_icon( x, y, w, h );
        H3_draw2DRoundRect( game, r_icon, glm::ivec2(8,8), 0xFFFFFFFF, ref, 16 );
    }

    // Draw Player Name
    de::Align::EAlign align = de::Align::TopLeft;
    {
        int x = r_panel.x+96;
        int y = r_panel.y+8;
        int w = 100;
        int h = 28;
        de::Recti r_name( x, y, w, h );
        H3_draw2DRoundRect( game, r_name, glm::ivec2(8,8), player.color, noTex, 16 );


        x = r_panel.x+102;
        y = r_panel.y+10;
        std::wostringstream s;
        s << player.name.c_str();
        //driver->draw2DText( x, y, s.str(), 0xFFFFFFFF, align, fontName );
    }

    // Draw Player Dice 16 * 16
    {
        int x = r_panel.x+204;
        int y = r_panel.y+10;
        int dice1 = player.dice.a;
        int dice2 = player.dice.b;
        H3_drawDice( game, de::Recti(x,y, 24,24), 5, dice1, dice2 );
    }

    {
        int x = r_panel.x+96;
        int y = r_panel.y+36;
        int w = 162;
        int h = 52;
        de::Recti r_stats( x, y, w, h );
        H3_draw2DRoundRect( game, r_stats, glm::ivec2(8,8), player.color, noTex, 16 );
    }

    int d = 25;
    int w = 24;
    int h = 24;
    int x = r_panel.x+103;
    int y = r_panel.y+38;

    align = de::Align::BottomRight;

    // Draw 24x24 Player Victory Points
    UI_drawStatNum( game, H3_Tex::StatVic, de::Recti(x,y,w,h), player.victoryPoints, 0); x += d;
    UI_drawStatNum( game, H3_Tex::StatRoad, de::Recti(x,y,w,h), player.roads.size(), 0); x += d;
    UI_drawStatNum( game, H3_Tex::StatFarm, de::Recti(x,y,w,h), player.farms.size(), 0); x += d;
    UI_drawStatNum( game, H3_Tex::StatCity, de::Recti(x,y,w,h), player.citys.size(), 0); x += d;
    UI_drawStatNum( game, H3_Tex::StatCard, de::Recti(x,y,w,h), player.eventCards.size(), 0); x += d;
    UI_drawStatNum( game, H3_Tex::EventCardPoint, de::Recti(x,y,w,h), player.pointCards.size(), 16); x += d;

    // <--> ROW 2 <-->
    x = r_panel.x+103;
    y = r_panel.y+62;
    UI_drawStatNum( game, H3_Tex::CardA, de::Recti(x,y,w,h), player.bank.A, 16); x += d;
    UI_drawStatNum( game, H3_Tex::CardB, de::Recti(x,y,w,h), player.bank.B, 16); x += d;
    UI_drawStatNum( game, H3_Tex::CardC, de::Recti(x,y,w,h), player.bank.C, 16); x += d;
    UI_drawStatNum( game, H3_Tex::CardD, de::Recti(x,y,w,h), player.bank.D, 16); x += d;
    UI_drawStatNum( game, H3_Tex::CardE, de::Recti(x,y,w,h), player.bank.E, 16); x += d;
    UI_drawStatNum( game, H3_Tex::EventCardKnight, de::Recti(x,y,w,h), player.knightCards.size(), 16); x += d;
}



void
UI_drawInfoPanelTile( H3_Game & game, de::Recti pos, const u32 tileId )
{
    if ( !tileId ) { return; }
    //auto driver = &game.m_videoDriver;
    //if ( !driver ) { DE_ERROR("No driver") return; }

    H3_Tile& tile = H3_getTile(game, __func__, tileId);

    uint32_t txtColor = 0xFFFFFFFF;

    de::TexRef noTex;
    H3_draw2DRoundRect( game, pos, glm::ivec2(12, 12), 0xA0000000, noTex, 13 );

    int w = pos.w;
    int x = pos.x + 20;
    int y = pos.y + 20;
    int ln = 10;

    auto driver = game.getDriver();
    auto screenRenderer = driver->getScreenRenderer();
    auto font58Renderer = driver->getFontRenderer5x8();

    font58Renderer->draw2DText( x,y, dbStr(H3_TileType::getString( tile.tileType )," (",tile.i, ",", tile.j,")"), txtColor );
    if ( tile.chipValue > 0 )
    {
        font58Renderer->draw2DText( x+w-40,y, dbStr("Dice = ",tile.chipValue), txtColor, de::Align::TopRight );
    }
    y += ln;
    H3_drawHexagon( game, de::Recti(x+50,y,50,50), 0xFFFFFFFF, H3_getTileTex( game, tile.tileType ), tile.id );
    font58Renderer->draw2DText( x+w-40,y+25, dbStr(tile.pos), txtColor, de::Align::RightMiddle );
    y += 50;
    font58Renderer->draw2DText( x,y, dbStr("angle60 = ",tile.angle60), txtColor );
    y += ln;
    font58Renderer->draw2DText( x,y, dbStr("hasRobber = ",tile.hasRobber), txtColor );
    y += ln;

    // [Info] Next Tiles
    y += 4;
    font58Renderer->draw2DText( x,y, dbStr("NextCount = ",tile.next.size()), txtColor );
    y += ln;
    for ( size_t i = 0; i < tile.next.size(); ++i )
    {
        H3_Tile & n = H3_getTile( game, __func__, tile.next[ i ] );
        H3_drawHexagon( game, de::Recti(x+28*i,y,24,24), 0xFFFFFFFF, H3_getTileTex( game, n.tileType ), n.id );
    }
    y += 28;

    // [Info] Corners
    font58Renderer->draw2DText( x,y, dbStr("CornerCount = ",tile.corners.size()), txtColor );
    y += ln;
    H3_drawCircle( game, de::Recti(x,y,24,24), 0xFFFFFFFF, H3_getTex( game, H3_Tex::ChipS ) );
    for ( size_t i = 0; i < tile.corners.size(); ++i )
    {
        H3_Corner & n = H3_getCorner( game, tile.corners[ i ] );
        font58Renderer->draw2DText( 30 + x,y, dbStr("Corner[",i,"] = ",n.id), txtColor );
        y += ln;
    }
    if ( tile.corners.size() < 3 )
    {
        y += ln * (3 - tile.corners.size());
    }
    y += 4;

    // [Info] Edges
    font58Renderer->draw2DText( x,y, dbStr("EdgeCount = ",tile.edges.size()), txtColor );
    y += ln;
    H3_drawCircle( game, de::Recti(x,y,24,24), 0xFFFFFFFF, H3_getTex( game, H3_Tex::ChipW ) );
    for ( size_t i = 0; i < tile.edges.size(); ++i )
    {
        H3_Edge & n = H3_getEdge( game, tile.edges[ i ] );
        font58Renderer->draw2DText( 30 + x,y, dbStr("Edge[",i,"] = ",n.id), txtColor );
        y += ln;
    }
    if ( tile.edges.size() < 3 )
    {
        y += ln * (3 - tile.edges.size());
    }
    y += 4;
}


void
UI_drawInfoPanelCorner( H3_Game & game, de::Recti pos, const u32 cornerId )
{
    if ( !cornerId ) { return; }
    //auto driver = &game.m_videoDriver;
    //if ( !driver ) { DE_ERROR("No driver") return; }

    H3_Corner& corner = H3_getCorner(game, cornerId);

    uint32_t txtColor = 0xFFFFFFFF;
    de::TexRef noTex;
    H3_draw2DRoundRect( game, pos, glm::ivec2(12, 12), 0xA0000000, noTex, 13 );

    int w = pos.w;
    int x = pos.x + 20;
    int y = pos.y + 20;
    int ln = 10;

    auto driver = game.getDriver();
    auto screenRenderer = driver->getScreenRenderer();
    auto font58Renderer = driver->getFontRenderer5x8();

    font58Renderer->draw2DText( x,y, dbStr("cornerId = ",corner.id), txtColor );
    y += ln;
    H3_drawCircle( game, de::Recti(x,y,48,48), 0xFFFFFFFF, H3_getTex( game, H3_Tex::ChipS ) );
    font58Renderer->draw2DText( x+w-40,y+24, dbStr(corner.pos()), txtColor, de::Align::RightMiddle );
    y += 52;

    font58Renderer->draw2DText( x,y, dbStr("NextCount = ",corner.next.size()), txtColor );
    y += ln;
    for ( size_t i = 0; i < corner.next.size(); ++i )
    {
        H3_Corner & n = H3_getCorner( game, corner.next[ i ] );
        font58Renderer->draw2DText( x,y, dbStr("Next[",i,"] = ",corner.next[ i ]), txtColor );
        y += ln;
    }
    y += 4;

    font58Renderer->draw2DText( x,y, dbStr("TileCount = ",corner.tiles.size()), txtColor );
    y += ln;
    for ( size_t i = 0; i < corner.tiles.size(); ++i )
    {
        H3_Tile & n = H3_getTile( game, __func__, corner.tiles[ i ] );
        H3_drawHexagon( game, de::Recti(x+40*(i+1),y,32,32), 0xFFFFFFFF, H3_getTileTex( game, n.tileType ), n.id );
    }
    y += 36;

    font58Renderer->draw2DText( x,y, dbStr("EdgeCount = ",corner.edges.size()), txtColor );
    y += ln;
    H3_drawCircle( game, de::Recti(x,y,32,32), 0xFFFFFFFF, H3_getTex( game, H3_Tex::ChipW ) );

    for ( size_t i = 0; i < corner.edges.size(); ++i )
    {
        H3_Edge & n = H3_getEdge( game, corner.edges[ i ] );
        font58Renderer->draw2DText( x+40,y, dbStr("Edge[",i,"] = ",n.id), txtColor );
        y += ln;
    }

    if ( corner.edges.size() < 3 )
    {
        y += ln * (3 - corner.edges.size());
    }
}

void
UI_drawInfoPanelEdge( H3_Game & game, de::Recti pos, const u32 edgeId )
{
    if ( !edgeId ) { return; }
    //auto driver = &game.m_videoDriver;
    //if ( !driver ) { DE_ERROR("No driver") return; }

    H3_Edge & edge = H3_getEdge(game, edgeId);

    de::TexRef noTex;
    uint32_t txtColor = 0xFFFFFFFF;

    H3_draw2DRoundRect( game, pos, glm::ivec2(12, 12), 0xA0000000, noTex, 13 );

    int w = pos.w;
    int x = pos.x + 20;
    int y = pos.y + 20;
    int ln = 10;

    auto driver = game.getDriver();
    auto screenRenderer = driver->getScreenRenderer();
    auto font58Renderer = driver->getFontRenderer5x8();

    font58Renderer->draw2DText( x,y, dbStr("edgeId = ",edge.id), txtColor );
    y += ln;
    H3_drawCircle( game, de::Recti(x,y,48,48), 0xFFFFFFFF, H3_getTex( game, H3_Tex::ChipW ) );
    font58Renderer->draw2DText( x+w-40,y+24, dbStr(edge.pos()), txtColor, de::Align::RightMiddle );
    y += 52;

    font58Renderer->draw2DText( x,y, dbStr("NextCount = ",edge.next.size()), txtColor );
    y += ln;
    for ( size_t i = 0; i < edge.next.size(); ++i )
    {
        H3_Edge & n = H3_getEdge( game, edge.next[ i ] );
        font58Renderer->draw2DText( x,y, dbStr("Next[",i,"] = ",edge.next[ i ]), txtColor );
        y += ln;
    }
    y += 4;

    font58Renderer->draw2DText( x,y, dbStr("TileCount = ",edge.tiles.size()), txtColor );
    y += ln;
    for ( size_t i = 0; i < edge.tiles.size(); ++i )
    {
        H3_Tile & n = H3_getTile( game, __func__, edge.tiles[ i ] );
        H3_drawHexagon( game, de::Recti(x+40*(i+1),y,32,32), 0xFFFFFFFF, H3_getTileTex( game, n.tileType ), n.id );
    }
    y += 36;

    font58Renderer->draw2DText( x,y, dbStr("CornerCount = ",edge.corners.size()), txtColor );
    y += ln;
    H3_drawCircle( game, de::Recti(x,y,32,32), 0xFFFFFFFF, H3_getTex( game, H3_Tex::ChipS ) );
    for ( size_t i = 0; i < edge.corners.size(); ++i )
    {
        H3_Corner & n = H3_getCorner( game, edge.corners[ i ] );
        font58Renderer->draw2DText( x+40,y, dbStr("Corner[",i,"] = ",n.id), txtColor );
        y += ln;
    }

    if ( edge.corners.size() < 3 )
    {
        y += ln * (3 - edge.corners.size());
    }
}

*/

