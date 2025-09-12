#pragma once
#include <H3/H3_Board.h>

struct H3_Game;

// =====================================================================
struct UI_Action
// =====================================================================
{
    u32 playerId;
    u32 action; // 0 - EndTurn, 1 - Dice, 2 - Buy Road, 3 - Buy Card,
    u32 object; // dice1 or card-type, edgeId, cornerId or thief
    u32 amount; // dice2
    H3_Bank bankIn;   // Used for banking and trade
    H3_Bank bankOut;  // Used for banking and trade
    double pts; // time stamp

    std::string msg;
};

// UI_Action( p,0,0,0 ); // = EndTurn
// UI_Action( p,1,0,7 ); // = Dice7
// UI_Action( p,2,edgeId,0 ); // buy road on edgeId
// UI_Action( p,3,cornerId,0 ); // buy farm on cornerId
// UI_Action( p,4,cornerId,0 ); // buy city on cornerId
// UI_Action( p,5,0,0 ); // buy card
// UI_Action( p,5,0,0 ); // buy card

// =====================================================================
struct UI_ActionLogger
// =====================================================================
{
    de::Font5x8 font;
    std::vector< UI_Action > history;
};

void UI_resetLog( H3_Game & game );
void UI_logCancel( H3_Game & game );
void UI_logEndTurn( H3_Game & game );
void UI_logDice( H3_Game & game );
void UI_logBank( H3_Game & game, H3_Bank in, H3_Bank out );
void UI_logBuyRoad( H3_Game & game, int edgeId );
void UI_logBuyFarm( H3_Game & game, int cornerId );
void UI_logBuyCity( H3_Game & game, int cornerId );
void UI_logBuyCard( H3_Game & game, int cardType );

void UI_drawLogEndTurn( H3_Game & game, const UI_Action& act, int x, int y );
void UI_drawLogCancel( H3_Game & game, const UI_Action& act, int x, int y );
void UI_drawLogDice( H3_Game & game, const UI_Action& act, int x, int y );
void UI_drawLogBank( H3_Game & game, const UI_Action& act, int x, int y );
void UI_drawLogBuyCard( H3_Game & game, const UI_Action& act, int x, int y );
void UI_drawLogBuyRoad( H3_Game & game, const UI_Action& act, int x, int y );
void UI_drawLogBuyFarm( H3_Game & game, const UI_Action& act, int x, int y );
void UI_drawLogBuyCity( H3_Game & game, const UI_Action& act, int x, int y );
void UI_drawLog( H3_Game & game );
