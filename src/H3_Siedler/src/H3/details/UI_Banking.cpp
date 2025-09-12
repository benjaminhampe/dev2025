#include "UI_Banking.h"
#include <H3/details/H3_Topology.h>
#include <H3/details/H3_UI.h>
#include <H3/H3_Game.h>

void
UIBanking_create( H3_Game & game )
{
/*
    de::Font5x8 menuFont;
    menuFont.quadWidth = 4;
    menuFont.quadHeight = 4;
    menuFont.quadSpacingX = 1;
    menuFont.quadSpacingY = 1;
    menuFont.glyphSpacingX = 1;
    menuFont.glyphSpacingY = 1;

    int x= 10;
    int y = 12;
    int d = 128+20;
    auto align = de::Align::BottomCenter;

    // Banking DLG items:
    {
        align = de::Align::BottomCenter;

        UI_addElem( game, H3_UI::BankLblTitle, H3_UIElem::Text, de::Recti( 20,340,0,0 ),
                   0, align, 0, "Banking...");

        int y1 = 230;
        int y2 = 210;
        int lw = 60;
        int lh = 12;
        int x = -140;
        int d = 80;

        UI_addElem( game, H3_UI::BankBtnA, H3_UIElem::Button, de::Recti( x,y1,60,100 ),
                   6, align, int(H3_Tex::CardA), "BankBtnA");
        UI_addElem( game, H3_UI::BankLblA, H3_UIElem::Text, de::Recti( x,y2,lw,lh ),
                   0, align, 0, "BankLblA");
        x += d;
        UI_addElem( game, H3_UI::BankBtnB, H3_UIElem::Button, de::Recti( x,y1,60,100 ),
                   6, align, int(H3_Tex::CardB), "BankBtnB");
        UI_addElem( game, H3_UI::BankLblB, H3_UIElem::Text, de::Recti( x,y2,lw,lh ),
                   0, align, 0, "BankLblB");
        x += d;
        UI_addElem( game, H3_UI::BankBtnC, H3_UIElem::Button, de::Recti( x,y1,60,100 ),
                   6, align, int(H3_Tex::CardC), "BankBtnC");
        UI_addElem( game, H3_UI::BankLblC, H3_UIElem::Text, de::Recti( x,y2,lw,lh ),
                   0, align, 0, "BankLblC");
        x += d;
        UI_addElem( game, H3_UI::BankBtnD, H3_UIElem::Button, de::Recti( x,y1,60,100 ),
                   6, align, int(H3_Tex::CardD), "BankBtnD");
        UI_addElem( game, H3_UI::BankLblD, H3_UIElem::Text, de::Recti( x,y2,lw,lh ),
                   0, align, 0, "BankLblD");
        x += d;
        UI_addElem( game, H3_UI::BankBtnE, H3_UIElem::Button, de::Recti( x,y1,60,100 ),
                   6, align, int(H3_Tex::CardE), "BankBtnE");
        UI_addElem( game, H3_UI::BankLblE, H3_UIElem::Text, de::Recti( x,y2,lw,lh ),
                   0, align, 0, "BankLblE");
        x += d;

        UI_addElem( game, H3_UI::BankBtnOK, H3_UIElem::Button, de::Recti( -70,160,160,40 ),
                   8, align, int(0), "OK, pls bank now");

        UI_addElem( game, H3_UI::BankBtnCancel, H3_UIElem::Button, de::Recti( 110,160,160,40 ),
                   8, align, int(0), "Cancel transaction.");
    }

    //   UI_addElem( game, H3_UI::BtnBonusArmy, H3_UIElem::ImageLabel, de::Recti( x,y,0,0 ),
    //               0, align, int(H3_Tex::BonusArmy), "ImgBonusArmy");
*/
}

void
UIBanking_update( H3_Game & game )
{
/*
    UIBanking_hide( game );

    if ( game.m_state == H3_State::Banking )
    {
        UIBanking_show( game );

        if ( game.m_bankingMode == 0 )
        {
            UI_get( game, H3_UI::BankLblA ).msg = "1";
        }
        else
        {
            UI_get( game, H3_UI::BankLblA ).msg = "1";
        }
        UI_get( game, H3_UI::BankLblA ).msg = "0";
        UI_get( game, H3_UI::BankLblB ).msg = "0";
        UI_get( game, H3_UI::BankLblC ).msg = "0";
        UI_get( game, H3_UI::BankLblD ).msg = "0";
        UI_get( game, H3_UI::BankLblE ).msg = "0";
        if ( game.m_bankingWish == 0 )
        {
            UI_get( game, H3_UI::BankLblA ).msg = "1";
        }
        else if ( game.m_bankingWish == 1 )
        {
            UI_get( game, H3_UI::BankLblB ).msg = "1";
        }
        else if ( game.m_bankingWish == 2 )
        {
            UI_get( game, H3_UI::BankLblC ).msg = "1";
        }
        else if ( game.m_bankingWish == 3 )
        {
            UI_get( game, H3_UI::BankLblD ).msg = "1";
        }
        else if ( game.m_bankingWish == 4 )
        {
            UI_get( game, H3_UI::BankLblE ).msg = "1";
        }
    }
*/
}

int UIBanking_computeRatio( H3_Game & game, int bankingMode, H3_Player & player )
{
    int ratio = 4;
    if ( player.hasPort3v1() ) ratio = 3;

    if ( bankingMode == 0 && player.hasPortA() )
    {
        ratio = 2;
    }
    if ( bankingMode == 1 && player.hasPortB() )
    {
        ratio = 2;
    }
    if ( bankingMode == 2 && player.hasPortC() )
    {
        ratio = 2;
    }
    if ( bankingMode == 3 && player.hasPortD() )
    {
        ratio = 2;
    }
    if ( bankingMode == 4 && player.hasPortE() )
    {
        ratio = 2;
    }
    return ratio;
}

void UIBanking_start( H3_Game & game, int bankingMode )
{
/*
    if ( game.m_state != H3_State::Idle ) { DE_ERROR("No correct idle state") return; }

    H3_Player & player = H3_getCurrentPlayer( game );

    int ratio = UIBanking_computeRatio( game, bankingMode, player );
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
*/
}

void UI_cancelBanking( H3_Game & game )
{
/*
    if ( game.m_state == H3_State::Banking )
    {
        DE_DEBUG(__func__)
        game.m_state = H3_State::Idle;
        game.m_bankingMode = -1;
        game.m_bankingWish = -1;
        UI_update( game );
    }
*/
}

void UI_finishBanking( H3_Game & game )
{
    /*
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
*/
}


void UI_setBankingVisible( H3_Game & game, bool bVisible )
{
    /*
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
*/
}

void
UI_hideBanking( H3_Game & game ) { UI_setBankingVisible( game, false ); }

void
UI_showBanking( H3_Game & game ) { UI_setBankingVisible( game, true ); }


/*
void UI_drawRoundStats( H3_Game & game)
{
    auto driver = &game.m_videoDriver;
    if ( !driver ) { DE_ERROR("No driver") return; }

    const int w = driver->getScreenWidth();
    const int h = driver->getScreenHeight();
    const int mx = game.m_mouseX;
    const int my = game.m_mouseY;
    const int p = 10;

    int x = w - p;
    int y = p;
    de::Align::EAlign align = de::Align::TopRight;

    de::Font5x8 font1(2,2,1,1,1,1);
    int ln1 = font1.getTextSize("W").height + p;
    auto s1 = dbStrJoin("Screen(",w,",",h,")");
    auto s2 = dbStrJoin("Mouse(",mx,",",my,")");
    H3_drawText( game, x,y, s1, dbRGBA(255,100,100), align, font1 );
    y += ln1;
    H3_drawText( game, x,y, s2, dbRGBA(255,200,100), align, font1 );
    y += ln1;

    de::Font5x8 font(4,4,1,1,1,1);
    //de::Font font( "garton", 36 );

    int ln = font.getTextSize("W").height + 10;
    H3_drawText( game, x,y, dbStrJoin("State = ", H3_State::getString( game.m_state )), 0xFFFFFFFF, align, font );
    y += ln;
    H3_drawText( game, x,y, dbStrJoin("Round = ", game.m_round), 0xFFFFFFFF, align, font );
    y += ln;
    H3_drawText( game, x,y, dbStrJoin("Turn = ", game.m_turn), 0xFFFFFFFF, align, font );
    y += ln;
    H3_drawText( game, x,y, dbStrJoin("Player = ", H3_getCurrentPlayer( game ).name), 0xFFFFFFFF, align, font );
    y += ln;
    H3_drawText( game, x,y, dbStrJoin("Dice = ", H3_getCurrentPlayer( game ).dice.sum()), 0xFFFFFFFF, align, font );
    y += ln;
    H3_drawDice( game, de::Recti(x - 128-11, y, 64,64), 12, game.m_dice.a );
    H3_drawDice( game, de::Recti(x - 64-1, y, 64,64), 12, game.m_dice.b );
    y += 64;
}
*/
