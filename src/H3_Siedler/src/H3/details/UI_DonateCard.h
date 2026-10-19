#pragma once
#include <H3/H3_Board.h>
#include <de/gui/IntField.h>
#include <de/gui/Label.h>
#include <de/gui/Button.h>

// Dialog to donate cards to the robber when (dice == 7).
// =====================================================================
class UI_DonateCard : public de::gui::Widget
// =====================================================================
{
public:
    UI_DonateCard( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id = 0, const de::Recti& pos = de::Recti() );
    ~UI_DonateCard() override;

    void resize(int x, int y, int w, int h) override;
    void draw() override;
    //void onEvent( const de::Event& event ) override;

    void setCardData( int cardType, int amount );

    int getCardData() const { return m_edtDst->getValue(); }
public:
    std::function<void(int)> onValueChanged;

private:
    // [Necessary Game Logic]
    H3_Game* m_game;
    int32_t m_cardType;
    int m_cardAmount;

    de::Recti m_r_head;
    de::Recti m_r_body;
    de::Recti m_r_foot;

    de::gui::IntField* m_edtDst;
    de::gui::Button* m_btnSrc;
    de::gui::Label* m_lblSrc;


};

