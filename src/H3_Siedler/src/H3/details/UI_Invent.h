#pragma once
#include <H3/H3_Board.h>
#include <de/gui/Panel.h>
#include <de/gui/Label.h>
#include <de/gui/Button.h>

// Dialog to donate cards to the robber when (dice == 7).
// =====================================================================
class UI_Invent : public de::gui::Panel
// =====================================================================
{
public:
    UI_Invent( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& r );
    ~UI_Invent() override;

    void draw() override;
    void resize(int x, int y, int w, int h) override;

    void startInvent( uint32_t playerId );

private:

    // [Necessary Game Logic]
    H3_Game* m_game;
    uint32_t m_playerId;
    float m_aspect = 0.6f; // Card Width / Card Height

    /*
    struct Transaction
    {
        int giveCard; // H3_TileType resource Card type the player gives
        int wishCard; // H3_TileType the player wants from bank
    };

    std::vector<Transaction> m_transaction;
    */

    de::Recti m_rcHead;
    de::Recti m_rcBank;
    de::Recti m_rcName;
    de::Recti m_rcBtns;

    // de::Font m_fontTitle;
    // de::Font m_fontCancel;
    // de::Font m_fontLabel;

    struct DoubleWidget
    {
        de::Recti rect;
        de::gui::Button* btn;
        de::gui::Label* lbl;

        void resize(int x,int y,int w,int h, int p, int label_h, const de::Font& font)
        {
            int btn_h = h - 2*(label_h + p);
            int btn_y = y;
            int ldn_y = y + btn_h + p;
            btn->resize(x, btn_y, w, btn_h);
            lbl->resize(x, ldn_y, w, label_h);

            btn->setFont(font);
            lbl->setFont(font);
        }
    };

    H3_Bank m_bank;
    DoubleWidget m_cardA;
    DoubleWidget m_cardB;
    DoubleWidget m_cardC;
    DoubleWidget m_cardD;
    DoubleWidget m_cardE;

    de::gui::Button* m_btnCancel; // Cancel dialog.
    de::gui::Button* m_btnAccept; // Accept transaction.

    void createDoubleWidget(DoubleWidget& w, int tileType);

    void drawTitle();

    /*
    void drawBankIconAndName();
    void drawUserIcon();
    void drawUserName();
    */

    void updateWidgetLogic();


    void onCancel();
    void onAccept();
};
