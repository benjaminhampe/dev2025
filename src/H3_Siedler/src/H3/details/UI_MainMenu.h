#pragma once
#include <H3/H3_Board.h>
#include <de/gui/Panel.h>

// =====================================================================
class UI_MainMenu : public de::gui::Panel
// =====================================================================
{
public:
    UI_MainMenu( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& r );
    ~UI_MainMenu() override;
    //void draw() override;
    void resize(int x, int y, int w, int h) override;

    void forceResize() { m_forceResize = true; }

private:
    H3_Game* m_game;

    bool m_forceResize;

    de::gui::Button* m_btnNewGame;
    de::gui::Button* m_btnRestart;
    de::gui::Button* m_btnOptions;
    de::gui::Button* m_btnHelp;
    de::gui::Button* m_btnExit;
    LineBoxAnim m_lineBoxAnim;

    de::gui::Widget* m_uiHelpWindow;

protected:
    de::gui::Button* createButton(std::string txt);
};
