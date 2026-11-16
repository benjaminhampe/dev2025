#pragma once
#include <H3/H3_Board.h>
#include <de/gui/Panel.h>
#include <de/gui/Label.h>
#include <de/gui/Button.h>

// Dialog to choose a resource card type and
// All other players who have cards of this type lose those cards to player.
// =====================================================================
class UI_Monopol : public de::gui::Panel
// =====================================================================
{
public:
    UI_Monopol( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent,
                u32 id, const de::Recti& r );
    ~UI_Monopol() override;

    void draw() override;
    void resize(int x, int y, int w, int h) override;

    void startMonopol( uint32_t playerId );

private:

    // [Necessary Game Logic]
    H3_Game* m_game;
    uint32_t m_playerId;
    float m_aspect = 0.6f; // Card Width / Card Height

    de::Recti m_rcHead;
    de::Recti m_rcCard;
    de::Recti m_rcName;
    de::Recti m_rcBtns;

    H3_Bank m_bank;
    de::gui::Button* m_btnCardA;
    de::gui::Button* m_btnCardB;
    de::gui::Button* m_btnCardC;
    de::gui::Button* m_btnCardD;
    de::gui::Button* m_btnCardE;

    de::gui::Button* m_btnCancel; // Cancel dialog.
    de::gui::Button* m_btnAccept; // Accept transaction.

    void drawTitle();
    //void drawUserName();

    void updateWidgetLogic();

    void onCancel();
    void onAccept();
};
