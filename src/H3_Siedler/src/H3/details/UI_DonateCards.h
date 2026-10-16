#pragma once
#include <H3/H3_Board.h>
#include <de/gui/Panel.h>

// Dialog to donate cards to the robber when (dice == 7).
// =====================================================================
class UI_DonateCards : public de::gui::Panel
// =====================================================================
{
public:
    UI_DonateCards( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& r );
    ~UI_DonateCards() override;
    void resize(int x, int y, int w, int h) override;
    //void draw() override;
    //void onEvent( const de::Event& event ) override;
    void updateTexts();

    void resizeLayoutThief( de::Recti pos );
    void resizeLayoutPlayer( de::Recti pos );

    void startDonation( uint32_t playerId, H3_GuiState oldState );

protected:
    de::gui::Button* createButton(std::string txt, de::gpu::TexRef ref, de::Recti pos);
private:
    // [Necessary Game Logic]
    H3_Game* m_game;
    uint32_t m_playerId;
    H3_GuiState m_oldState;

    int m_cardsToRemove;
    H3_Bank m_bankThief;
    H3_Bank m_bankPlayer;

    // [Row0] Title
    de::gui::Label* m_lblTitle;

    // [Row1] Thief
    de::gui::Label* m_lblThiefName;
    de::gui::Label* m_lblThiefIcon;
    de::gui::Button* m_btnA1;
    de::gui::Button* m_btnB1;
    de::gui::Button* m_btnC1;
    de::gui::Button* m_btnD1;
    de::gui::Button* m_btnE1;

    // [Row2] Player
    de::gui::Label* m_lblPlayerName;
    de::gui::Label* m_lblPlayerIcon;
    de::gui::Button* m_btnA2;
    de::gui::Button* m_btnB2;
    de::gui::Button* m_btnC2;
    de::gui::Button* m_btnD2;
    de::gui::Button* m_btnE2;

    // [Row3] Accept Button
    de::gui::Button* m_btnAccept;
};
