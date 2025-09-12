#pragma once
#include <H3/H3_Board.h>

struct H3_Game;

/*
//bool m_isMenuVisible = true;
//bool m_isCameraInputEnabled = false;
int m_bankingMode = -1;
int m_bankingWish = -1;
H3_Bank m_bankingIn;
H3_Bank m_bankingOut;
*/

// =====================================================================
struct UI_Banking
// =====================================================================
{
    enum eID
    {
        Panel = 0,
        TitleLbl,
        //      BankBtnUser,
        //      BankLblUser,
        BtnBankA,
        BtnBankB,
        BtnBankC,
        BtnBankD,
        BtnBankE,
        // LblBankA,
        // LblBankB,
        // LblBankC,
        // LblBankD,
        // LblBankE,

        BtnUserA,
        BtnUserB,
        BtnUserC,
        BtnUserD,
        BtnUserE,
        // LblUserA,
        // LblUserB,
        // LblUserC,
        // LblUserD,
        // LblUserE,
        
        BtnAsk,
        BtnOK,
        BtnCancel,

        Unknown
    };

    //static std::string
    //getString( eID t );

    bool m_isVisible = false;
    u32 m_hoverElement = 0; // eID probably. 
    std::vector< H3_UIElem > m_elems; // UI elements of this panel
};

// int UI_find( const H3_Game & game, const H3_UI::eID id );

// H3_UIElem& UI_get( H3_Game & game, const H3_UI::eID id );

// void UI_addElem( H3_Game & game, H3_UI::eID id, H3_UIElem::eType typ, de::Recti pos, int radius = 0,
    // de::Align align = de::Align::TopLeft, int texId = 0, std::string msg = "", de::Font5x8 const & font = de::Font5x8(),
    // uint32_t textColor = 0xFF000000, uint32_t fillColor = 0xFFFFFFFF, uint32_t borderColor = 0xFFEEEEEE );

void UIBanking_create( H3_Game & game );

void UIBanking_update( H3_Game & game );

void UIBanking_draw( H3_Game & game, bool debug = true );

// void UI_drawElem( H3_Game & game, H3_UIElem & elem );

// void UI_drawElem( H3_Game & game, H3_UI::eID id );

// void UI_drawDlgLooseCards( H3_Game & game );

int UIBanking_computeRatio( H3_Game & game, int bankingMode, H3_Player & player );
void UIBanking_start( H3_Game & game, int bankingMode );
void UIBanking_cancel( H3_Game & game );
void UIBanking_finish( H3_Game & game );
void UIBanking_setVisible( H3_Game & game, bool bVisible );
void UIBanking_hide( H3_Game & game );
void UIBanking_show( H3_Game & game );

// void UI_setActionsVisible( H3_Game & game, bool bVisible );
// void UI_hideActions( H3_Game & game );
// void UI_showActions( H3_Game & game );

// void UI_setStatsVisible( H3_Game & game, bool bVisible );
// void UI_hideStats( H3_Game & game );
// void UI_showStats( H3_Game & game );

// void UI_drawRoundStats( H3_Game & game);
// void UI_drawStatNum( H3_Game & game, int img, de::Recti pos, int value, int icon_width = 0 );
// void UI_drawPlayerStats( H3_Game & game, int px, int py, int playerId );

// void UI_drawInfoPanelTile( H3_Game & game, de::Recti pos, const u32 tileId );

// void UI_drawInfoPanelCorner( H3_Game & game, de::Recti pos, const u32 cornerId );

// void UI_drawInfoPanelEdge( H3_Game & game, de::Recti pos, const u32 edgeId );

// void UI_resetLog( H3_Game & game );
// void UI_logCancel( H3_Game & game );
// void UI_logEndTurn( H3_Game & game );
// void UI_logDice( H3_Game & game );
// void UI_logBank( H3_Game & game, H3_Bank in, H3_Bank out );
// void UI_logBuyRoad( H3_Game & game, int edgeId );
// void UI_logBuyFarm( H3_Game & game, int cornerId );
// void UI_logBuyCity( H3_Game & game, int cornerId );
// void UI_logBuyCard( H3_Game & game, int cardType );

// void UI_drawLogEndTurn( H3_Game & game, const H3_Action& act, int x, int y );
// void UI_drawLogCancel( H3_Game & game, const H3_Action& act, int x, int y );
// void UI_drawLogDice( H3_Game & game, const H3_Action& act, int x, int y );
// void UI_drawLogBank( H3_Game & game, const H3_Action& act, int x, int y );
// void UI_drawLogBuyCard( H3_Game & game, const H3_Action& act, int x, int y );
// void UI_drawLogBuyRoad( H3_Game & game, const H3_Action& act, int x, int y );
// void UI_drawLogBuyFarm( H3_Game & game, const H3_Action& act, int x, int y );
// void UI_drawLogBuyCity( H3_Game & game, const H3_Action& act, int x, int y );
// void UI_drawLog( H3_Game & game );
