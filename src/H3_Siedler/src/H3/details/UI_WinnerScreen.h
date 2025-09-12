#pragma once
#include <H3/H3_Board.h>

// =====================================================================
struct UI_WinnerScreen : public de::gui::Widget
// =====================================================================
{
    UI_WinnerScreen( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& r );
    ~UI_WinnerScreen() override;
    //void draw() override;
    void resizeLayout();
    void updateData();
protected:
    de::gui::Button* createButton(std::string txt, de::gpu::TexRef ref, de::Recti pos);
private:
    // [Necessary Game Logic]
    H3_Game* m_game;

    de::gui::Button* m_btnClose;

    de::gui::Label* m_lblWinner;
    de::gui::Label* m_lblWinnerIcon;
    de::gui::Label* m_lblWinnerName;

};



