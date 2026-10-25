#pragma once
#include <stack>
#include <H3/H3_Board.h>
#include <de/gui/Panel.h>
#include <H3/details/UI_WorldCreator.h>

#include <H3/details/UI_ActionLogger.h>
#include <H3/details/UI_Banking.h>
#include <H3/details/UI_Actions.h>
#include <H3/details/UI_DonateCards.h>
#include <H3/details/UI_WinnerScreen.h>
#include <H3/details/UI_Marquee.h>
#include <H3/details/UI_CpuPlayer.h>

// =====================================================================
class UI_Game : public de::gui::Widget
// =====================================================================
{
public:
    UI_Game( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& r );
    ~UI_Game() override;

    H3_GuiState getState() const;
    void setState(const H3_GuiState& guiState);

    void pushState(const H3_GuiState& guiState);
    void popState();

    void resize(int x, int y, int w, int h) override;
    void draw() override;

    void updateVisibleActions();

    void forceResize();

    void setDebug(bool bDebug);
    bool isDebug() const;

protected:
    void updateLayouts();

    void drawDice();

private:
    H3_Game* m_game;

    bool m_forceResize;

public:

    std::stack<H3_GuiState> m_stack;


    UI_WorldCreator* m_worldCreator;

    UI_Marquee* m_marquee;

    UI_WinnerScreen* m_winnerScreen;
    UI_Actions* m_actions;
    UI_DonateCards* m_donateCards;
    UI_Banking* m_banking;


    UI_ActionLogger m_actionLogger;

    bool m_bDebug;
};
