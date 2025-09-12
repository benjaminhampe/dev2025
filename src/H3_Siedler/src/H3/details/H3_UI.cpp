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
        const auto fScale = 0.01f * game.getScalePc();
        auto font = de::Font5x8(fScale,fScale);
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
        const auto fScale = 0.01f * game.getScalePc();
        auto font = de::Font5x8(fScale,fScale);
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

void UI_drawRoundStats( H3_Game & game)
{
    auto device = game.m_device;
    if ( !device ) { DE_ERROR("No device") return; }

    auto driver = game.getDriver();
    if ( !driver ) { DE_ERROR("No driver") return; }

    const int mx = device->getMouseX();
    const int my = device->getMouseY();
    const int fps = driver->getFPS();
    const int w = driver->getScreenWidth();
    const int h = driver->getScreenHeight();
    const int p = 10;

    int x = w - p;
    int y = p;
    de::Align::EAlign align = de::Align::TopRight;

    //de::Font5x8 font(4,4,1,1,1,1);
    //de::Font font( "garton", 36 );
    de::Font5x8 font1(3,3,0,0,1,1);

    int ln1 = font1.getTextSize("W").height + p;
    auto s0 = dbStr("FPS(",fps,")");
    auto s1 = dbStr("Screen(",w,",",h,")");
    auto s2 = dbStr("Mouse(",mx,",",my,") L(", game.m_isMouseLeftPressed, "), R(", game.m_isMouseRightPressed,"), M(", game.m_isMouseMiddlePressed,")");
    H3_drawText( game, x,y, s0, dbRGBA(255,225,100), align, font1 );
    y += ln1;
    H3_drawText( game, x,y, s1, dbRGBA(255,100,100), align, font1 );
    y += ln1;
    H3_drawText( game, x,y, s2, dbRGBA(255,200,100), align, font1 );
    y += ln1;

    auto camera = driver->getCamera();
    if ( camera )
    {
        auto s1 = dbStr("Camera-Pos(",glm::ivec3(camera->getPos()),")");
        auto s2 = dbStr("Camera-Target(",glm::ivec3(camera->getTarget()),")");
        auto s3 = dbStr("Camera-Angle(",camera->getAng(),")");
        H3_drawText( game, x,y, s1, dbRGBA(100,200,100), align, font1 );
        y += ln1;
        H3_drawText( game, x,y, s2, dbRGBA(125,215,125), align, font1 );
        y += ln1;
        H3_drawText( game, x,y, s3, dbRGBA(145,225,145), align, font1 );
        y += ln1;
    }

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

    //int ln = font1.getTextSize("W").height + 10;
    H3_drawText( game, x,y, s6, 0xFFFFFFFF, align, font1 ); y += ln1;
    H3_drawText( game, x,y, s7, 0xFFFFFFFF, align, font1 ); y += ln1;
    H3_drawText( game, x,y, s8, 0xFFFFFFFF, align, font1 ); y += ln1;
    H3_drawText( game, x,y, s9, 0xFFFFFFFF, align, font1 ); y += ln1;
    H3_drawText( game, x,y, sA, 0xFFFFFFFF, align, font1 ); y += ln1;
    H3_drawText( game, x,y, sB, 0xFFFFFFFF, align, font1 ); y += ln1;
    H3_drawText( game, x,y, sC, 0xFFFFFFFF, align, font1 ); y += ln1;
    H3_drawText( game, x,y, sD, 0xFFFFFFFF, align, font1 ); y += ln1;
    H3_drawText( game, x,y, sE, 0xFFFFFFFF, align, font1 ); y += ln1;
    H3_drawText( game, x,y, sF, 0xFFFFFFFF, align, font1 ); y += ln1;

    H3_drawDice( game, de::Recti(x - 128-11, y, 64,64), 12, game.m_dice.a );
    H3_drawDice( game, de::Recti(x - 64-1, y, 64,64), 12, game.m_dice.b );
    //y += 64;
}

void UI_Stats_update( H3_Game & game )
{
    // Nothing todo (yet), render method does gathering.
    /*
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
    */
}

/*
void UI_Stats_setVisible( H3_Game & game, bool bVisible )
{
}

void UI_Stats_hide( H3_Game & game ) { UI_Stats_setVisible( game, false ); }

void UI_Stats_show( H3_Game & game ) { UI_Stats_setVisible( game, true ); }
*/

void UI_Stats_drawItem( H3_Game & game, int x, int y, int w, int h,
                       int texId, int value, de::Font5x8 font )
{
    auto driver = game.getDriver();
    if ( !driver ) { DE_ERROR("No driver") return; }

    const auto tex = game.getTex(H3_Tex::eID(texId), __func__);
    const auto str = std::to_string( value );

    auto r = glm::ivec2(8,8);
    H3_draw2DRoundRect( game, de::Recti(x,y,w,h), r, tex );

    const auto fScale = 0.01f * game.getScalePc();

    auto ts = font.getTextSize( str );

    int s = fScale;
    int tw = ts.width;
    int th = (ts.height / 8) * 7;

    auto r_num = de::Recti(x + w - 3 - s - tw,
                           y + h - 3 - s - th,
                           tw + 2*s,
                           th + 2*s);

    H3_draw2DRect( game, r_num, dbRGBA(0,0,0,128) );

    H3_drawText( game, r_num.x + 1, r_num.y + 1, str, dbRGBA(255,255,255), de::Align::Default, font);
}

void UI_Stats_render( H3_Game & game )
{
    const auto fScale = 0.01f * game.getScalePc();

    H3_Player & player = game.getCurrentPlayer();

    auto driver = game.getDriver();
    auto screenRenderer = driver->getScreenRenderer();
    auto font58Renderer = driver->getFontRenderer5x8();
    const auto scr_w = driver->getScreenWidth();
    const auto scr_h = driver->getScreenHeight();

    de::Font5x8 font;
    font.quadWidth = 2 * fScale;
    font.quadHeight = 2 * fScale;
    font.quadSpacingX = 0;
    font.quadSpacingY = 0;
    font.glyphSpacingX = 1;
    font.glyphSpacingY = 1;

    const auto playerTex = game.getTex((H3_Tex::eID)player.avatar, __func__ );
    const auto playerName = player.name;
    const auto playerColor = player.color;

    int s = 1; // spacing
    int p = fScale * 5; // padding
    int w = fScale * 39;

    int h0 = fScale * 64;
    int h1 = fScale * 64;
    int h2 = fScale * 39;

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
    UI_Stats_drawItem( game, x,y,w,h1, H3_Tex::CardE, player.state.bank.E, font); x += w + s;

    x = p;
    y = scr_h - 1 - p - h2;
    UI_Stats_drawItem( game, x,y,w,h2, H3_Tex::StatVic, player.state.victoryPoints, font); x += w + s;
    UI_Stats_drawItem( game, x,y,w,h2, H3_Tex::StatRoad, player.state.roads.size(), font); x += w + s;
    UI_Stats_drawItem( game, x,y,w,h2, H3_Tex::StatFarm, player.state.farms.size(), font); x += w + s;
    UI_Stats_drawItem( game, x,y,w,h2, H3_Tex::StatCity, player.state.citys.size(), font); x += w + s;
    UI_Stats_drawItem( game, x,y,w,h2, H3_Tex::StatCard,
                       player.state.oldKnightCards.size(), font); x += w + s;

    // Cards
    int wC = fScale * 39;
    int hC = fScale * 64;
    x += p;
    y = scr_h - 1 - p - hC;
    UI_Stats_drawItem( game, x,y,wC,hC, H3_Tex::EventCardKnight, player.state.oldKnightCards.size(), font); x += wC + s;
    UI_Stats_drawItem( game, x,y,wC,hC, H3_Tex::EventCardPoint, player.state.oldPointCards.size(), font); x += wC + s;

    // Bonus
    int wB = fScale * 64;
    int hB = fScale * 64;
    x = scr_w - 1 - p - wB - p - wB;
    y = scr_h - 1 - p - hB;
    UI_Stats_drawItem( game, x,y,wB,hB, H3_Tex::BonusArmy, player.hasBonusArmy(), font); x += wB + p;
    UI_Stats_drawItem( game, x,y,wB,hB, H3_Tex::BonusRoad, player.hasBonusRoad(), font); x += wB + p;
}


void UI_drawDlgLooseCards( H3_Game & game )
{
    auto driver = game.getDriver();
    if ( !driver ) { DE_ERROR("No driver") return; }
}

/*
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

*/



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

    const auto fScale = 0.01f * game.getScalePc();
    //int p = 20;
    //de::Font nameFont( "garton", 26, false, false, true, true, false );
    de::Font5x8 nameFont(fScale * 2, fScale * 2,0,0,1,1);

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
    UI_drawStatNum( game, H3_Tex::StatCard, de::Recti(x,y,w,h), player.state.oldKnightCards.size(), 0); x += d;
    UI_drawStatNum( game, H3_Tex::EventCardPoint, de::Recti(x,y,w,h), player.state.oldPointCards.size(), 16); // x += d;

    // <--> ROW 2 <-->
    x = r_panel.x+103;
    y = r_panel.y+62;
    UI_drawStatNum( game, H3_Tex::CardA, de::Recti(x,y,w,h), player.state.bank.A, 16); x += d;
    UI_drawStatNum( game, H3_Tex::CardB, de::Recti(x,y,w,h), player.state.bank.B, 16); x += d;
    UI_drawStatNum( game, H3_Tex::CardC, de::Recti(x,y,w,h), player.state.bank.C, 16); x += d;
    UI_drawStatNum( game, H3_Tex::CardD, de::Recti(x,y,w,h), player.state.bank.D, 16); x += d;
    UI_drawStatNum( game, H3_Tex::CardE, de::Recti(x,y,w,h), player.state.bank.E, 16); x += d;
    UI_drawStatNum( game, H3_Tex::EventCardKnight, de::Recti(x,y,w,h), player.state.oldKnightCards.size(), 16); // x += d;
}

void
UI_drawInfoPanelTile( H3_Game & game, de::Recti pos, const u32 tileId )
{
    if ( !tileId ) { return; }
    //auto driver = &game.m_videoDriver;
    //if ( !driver ) { DE_ERROR("No driver") return; }

    H3_Tile& tile = H3_getTile(game, __func__, tileId);

    uint32_t white = 0xFFFFFFFF;

    H3_draw2DRoundRect( game, pos, glm::ivec2(12, 12), 0xA0000000, 13 );

    const auto fScale = 0.01f * game.getScalePc();

    int w = pos.w;
    int x = pos.x + 20;
    int y = pos.y + 20;
    int ln = fScale * 10;
    de::Align align = de::Align::Default;
    de::Font5x8 font(fScale, fScale, 0,0,0,0);

    auto s1 = dbStr(H3_TileType::getString( tile.tileType )," (",tile.i, ",", tile.j,")");
    auto s3 = dbStr(tile.pos);
    auto s4 = dbStr("angle60 = ",tile.angle60);
    auto s5 = dbStr("hasRobber = ",tile.hasRobber);
    auto s6 = dbStr("NextCount = ",tile.next.size());
    auto s7 = dbStr("CornerCount = ",tile.corners.size());
    auto s9 = dbStr("EdgeCount = ",tile.edges.size());

    H3_drawText( game, x,y, s1, white, align, font );
    if ( tile.chipValue > 0 )
    {
        auto s2 = dbStr("Dice = ",tile.chipValue);
        H3_drawText( game, x+w-40,y, s2, white, de::Align::TopRight, font );
    }
    y += ln;
    H3_drawHexagon( game, de::Recti(x+50,y,50,50), 0xFFFFFFFF, H3_getTileTex( game, tile.tileType ), tile.id );
    H3_drawText( game, x+w-40,y+25, s3, white, de::Align::RightMiddle, font ); y += 50;
    H3_drawText( game, x,y, s4, white, align, font ); y += ln;
    H3_drawText( game, x,y, s5, white, align, font ); y += ln;

    // [Info] Next Tiles
    y += 4;
    H3_drawText( game, x,y, s6, white, align, font ); y += ln;
    for ( size_t i = 0; i < tile.next.size(); ++i )
    {
        H3_Tile & n = H3_getTile( game, __func__, tile.next[ i ] );
        H3_drawHexagon( game, de::Recti(x+28*i,y,24,24), 0xFFFFFFFF, H3_getTileTex( game, n.tileType ), n.id );
    }
    y += 28;

    // [Info] Corners

    H3_drawText( game, x,y, s7, white, align, font ); y += ln;
    H3_drawCircle( game, de::Recti(x,y,24,24), 0xFFFFFFFF, game.getTex(H3_Tex::ChipS, __func__) );
    for ( size_t i = 0; i < tile.corners.size(); ++i )
    {
        H3_Corner & n = H3_getCorner( game, __func__, tile.corners[ i ] );
        auto s8 = dbStr("Corner[",i,"] = ",n.id);
        H3_drawText( game, 30 + x,y, s8, white, align, font );
        y += ln;
    }
    if ( tile.corners.size() < 3 )
    {
        y += ln * (3 - tile.corners.size());
    }
    y += 4;

    // [Info] Edges
    H3_drawText( game, x,y, s9, white, align, font );
    y += ln;
    H3_drawCircle( game, de::Recti(x,y,24,24), 0xFFFFFFFF, game.getTex(H3_Tex::ChipW, __func__) );
    for ( size_t i = 0; i < tile.edges.size(); ++i )
    {
        H3_Edge & n = H3_getEdge( game, __func__, tile.edges[ i ] );
        H3_drawText( game, 30 + x,y, dbStr("Edge[",i,"] = ",n.id), white, align, font );
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

    H3_Corner& corner = H3_getCorner(game, __func__, cornerId);

    uint32_t white = 0xFFFFFFFF;
    H3_draw2DRoundRect( game, pos, glm::ivec2(12, 12), 0xA0000000, 13 );

    const auto fScale = 0.01f * game.getScalePc();
    auto font = de::Font5x8(fScale, fScale);
    int w = pos.w;
    int x = pos.x + 20;
    int y = pos.y + 20;
    int ln = fScale * 10;


    auto driver = game.getDriver();
    auto screenRenderer = driver->getScreenRenderer();
    auto font58Renderer = driver->getFontRenderer5x8();

    auto s0 = dbStr("cornerId = ",corner.id);
    auto s1 = dbStr(corner.trs.pos);
    auto s2 = dbStr("NextCount = ",corner.next.size());
    H3_drawText( game, x,y, s0, white, de::Align::Default, font );
    y += ln;
    H3_drawCircle( game, de::Recti(x,y,48,48), 0xFFFFFFFF, game.getTex(H3_Tex::ChipS, __func__) );
    H3_drawText( game, x+w-40,y+24, s1, white, de::Align::RightMiddle, font );
    y += 52;

    H3_drawText( game, x,y, s2, white, de::Align::Default, font );
    y += ln;
    for ( size_t i = 0; i < corner.next.size(); ++i )
    {
        H3_Corner & n = H3_getCorner( game, __func__, corner.next[ i ] );

        auto s3 = dbStr("Next[",i,"] = ",corner.next[ i ]);

        H3_drawText( game, x,y, s3, white, de::Align::Default, font );
        y += ln;
    }
    y += 4;

    auto s4 = dbStr("TileCount = ",corner.tiles.size());
    H3_drawText( game, x,y, s4, white, de::Align::Default, font );
    y += ln;
    for ( size_t i = 0; i < corner.tiles.size(); ++i )
    {
        H3_Tile & n = H3_getTile( game, __func__, corner.tiles[ i ] );
        H3_drawHexagon( game, de::Recti(x+40*(i+1),y,32,32), 0xFFFFFFFF, H3_getTileTex( game, n.tileType ), n.id );
    }
    y += 36;

    auto s5 = dbStr("EdgeCount = ",corner.edges.size());
    H3_drawText( game, x,y, s5, de::Align(), white, font );
    y += ln;
    H3_drawCircle( game, de::Recti(x,y,32,32), 0xFFFFFFFF, game.getTex(H3_Tex::ChipW, __func__) );

    for ( size_t i = 0; i < corner.edges.size(); ++i )
    {
        H3_Edge & n = H3_getEdge( game, __func__, corner.edges[ i ] );
        auto s6 = dbStr("Edge[",i,"] = ",n.id);
        H3_drawText( game, x+40,y, s6, white, de::Align(), font );
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

    H3_Edge & edge = H3_getEdge(game, __func__, edgeId);

    uint32_t white = 0xFFFFFFFF;

    H3_draw2DRoundRect( game, pos, glm::ivec2(12, 12), 0xA0000000, 13 );

    const auto fScale = 0.01f * game.getScalePc();
    auto font = de::Font5x8(fScale, fScale);
    int w = pos.w;
    int x = pos.x + 20;
    int y = pos.y + 20;
    int ln = fScale * 10;

    auto driver = game.getDriver();
    auto screenRenderer = driver->getScreenRenderer();
    auto font58Renderer = driver->getFontRenderer5x8();

    auto s1 = dbStr("edgeId = ",edge.id);
    H3_drawText( game, x,y,s1, white, de::Align(), font );
    y += ln;
    H3_drawCircle( game, de::Recti(x,y,48,48), 0xFFFFFFFF, game.getTex(H3_Tex::ChipW, __func__) );
    auto s2 = dbStr(edge.trs.pos);
    H3_drawText( game, x+w-40,y+24, s2, white, de::Align::RightMiddle, font );
    y += 52;

    auto s3 = dbStr("NextCount = ",edge.next.size());
    H3_drawText( game, x,y, s3, white, de::Align(), font );
    y += ln;
    for ( size_t i = 0; i < edge.next.size(); ++i )
    {
        H3_Edge & n = H3_getEdge( game, __func__, edge.next[ i ] );
        auto s4 = dbStr("Next[",i,"] = ",edge.next[ i ]);
        H3_drawText( game, x,y, s4, white, de::Align(), font );
        y += ln;
    }
    y += 4;

    auto s4 = dbStr("TileCount = ",edge.tiles.size());
    H3_drawText( game, x,y, s4, white, de::Align(), font );
    y += ln;
    for ( size_t i = 0; i < edge.tiles.size(); ++i )
    {
        H3_Tile & n = H3_getTile( game, __func__, edge.tiles[ i ] );
        int d = fScale*32;
        int k = x + (d + 8)*(i+1);
        H3_drawHexagon( game, de::Recti(k,y,d,d), 0xFFFFFFFF, H3_getTileTex( game, n.tileType ), n.id );
    }
    y += (fScale * 32) + 4;

    auto s5 = dbStr("CornerCount = ",edge.corners.size());
    H3_drawText( game, x,y, s5, white, de::Align(), font );
    y += ln;
    H3_drawCircle( game, de::Recti(x,y,fScale*32,fScale*32), 0xFFFFFFFF, game.getTex(H3_Tex::ChipS, __func__) );
    for ( size_t i = 0; i < edge.corners.size(); ++i )
    {
        H3_Corner & n = H3_getCorner( game, __func__, edge.corners[ i ] );
        auto s6 = dbStr("Corner[",i,"] = ",n.id);
        H3_drawText( game, x+(fScale*32)+8,y, s6, white, de::Align(), font );
        y += ln;
    }

    if ( edge.corners.size() < 3 )
    {
        y += ln * (3 - edge.corners.size());
    }
}
