#pragma once
#include <H3/H3_Board.h>

// =====================================================================
class UI_MainMenu : public de::gui::Widget
// =====================================================================
{
public:
    UI_MainMenu( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& r );
    ~UI_MainMenu() override;
    //void draw() override;
    void resizeLayout();

    void setVisible( bool visible ) override
    {
        DE_WARN(visible)
        setFlags( eWF_Visible, visible );
        DE_WARN(isVisible())
    }
protected:
    de::gui::Button* createButton(std::string txt, int x, int y );
private:
    H3_Game* m_game;
    de::gui::Button* m_btnNewGame;
    de::gui::Button* m_btnRestart;
    de::gui::Button* m_btnOptions;
    de::gui::Button* m_btnHelp;
    de::gui::Button* m_btnExit;
    LineBoxAnim m_lineBoxAnim;
};
