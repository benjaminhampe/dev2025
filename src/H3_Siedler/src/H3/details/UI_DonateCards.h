#pragma once
#include <H3/H3_Board.h>
#include <de/gui/Panel.h>
#include <de/gui/IntField.h>
#include <de/gui/Button.h>
#include "UI_DonateCard.h"

// Dialog to donate cards to the robber when (dice == 7).
// =====================================================================
class UI_DonateCards : public de::gui::Panel
// =====================================================================
{
public:
    UI_DonateCards( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& r );
    ~UI_DonateCards() override;

    void draw() override;
    void resize(int x, int y, int w, int h) override;

    void startDonation( uint32_t playerId );

private:
    // [Necessary Game Logic]
    H3_Game* m_game;
    uint32_t m_playerId;
    int m_cardsToRemove;

    H3_Bank m_bankThief;
    H3_Bank m_bankPlayer;

    de::Recti m_title;
    de::Recti m_head;
    de::Recti m_cards;
    de::Recti m_foot;
    de::Recti m_accept;

    UI_DonateCard* m_btnA;
    UI_DonateCard* m_btnB;
    UI_DonateCard* m_btnC;
    UI_DonateCard* m_btnD;
    UI_DonateCard* m_btnE;

    // [Row3] Accept Button
    de::gui::Button* m_btnAccept;
};
