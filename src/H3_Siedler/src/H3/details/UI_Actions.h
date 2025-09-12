#pragma once
#include <H3/H3_Board.h>

// =====================================================================
class UI_Actions : public de::gui::Widget
// =====================================================================
{
public:
    UI_Actions( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& r );
    ~UI_Actions() override;
    //void draw() override;
    void resizeLayout();
    void updateVisibility();
protected:
    void hideActions();
private:
    H3_Game* m_game;
    //de::gui::Widget* m_panel; // Element == Non pickable panel.
    de::gui::Button* m_btnDice;
    de::gui::Button* m_btnEndTurn;
    de::gui::Button* m_btnBank;
    de::gui::Button* m_btnTrade;
    de::gui::Button* m_btnCancel;
    de::gui::Button* m_btnBuyRoad;
    de::gui::Button* m_btnBuyFarm;
    de::gui::Button* m_btnBuyCity;
    de::gui::Button* m_btnBuyCard;
};
