#pragma once
#include <H3/H3_Board.h>

// =====================================================================
struct UI_WinnerScreen : public de::gui::Panel
// =====================================================================
{
    UI_WinnerScreen( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& r );
    ~UI_WinnerScreen() override;

    void draw() override;
    void resize(int x, int y, int w, int h) override;

    void updateData();
protected:
    de::gui::Button* createButton(std::string txt, de::gpu::TexRef ref);
private:
    // [Necessary Game Logic]
    H3_Game* m_game;

    de::gui::Button* m_btnContinueGame;
    de::gui::Button* m_btnReturnToMenu;

    // de::gui::Label* m_lblWinner;
    // de::gui::Label* m_lblWinnerIcon;
    // de::gui::Label* m_lblWinnerName;

};



