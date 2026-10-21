#pragma once
#include <H3/H3_Board.h>
#include <de/gui/Panel.h>
#include <de/gui/Label.h>
#include <de/gui/Button.h>

// Dialog to donate cards to the robber when (dice == 7).
// =====================================================================
class UI_Banking : public de::gui::Panel
// =====================================================================
{
public:
    UI_Banking( H3_Game* game, de::gui::Env* env, de::gui::Widget* parent, u32 id, const de::Recti& r );
    ~UI_Banking() override;

    void draw() override;
    void resize(int x, int y, int w, int h) override;

    void startBanking( uint32_t playerId );

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

    H3_Bank m_bank;
    H3_Bank m_user;

    de::Recti m_rcHead;
    de::Recti m_rcBank;
    de::Recti m_rcUser;
    de::Recti m_rcName;
    de::Recti m_rcBtns;

    // de::Font m_fontTitle;
    // de::Font m_fontCancel;
    // de::Font m_fontLabel;

    struct TripleWidget
    {
        de::Recti rect;
        de::gui::Label* lup;
        de::gui::Button* btn;
        de::gui::Label* ldn;

        void resize(int x,int y,int w,int h, int p, int label_h, const de::Font& font)
        {
            int btn_h = h - 2*(label_h + p);
            int lup_y = y;
            int btn_y = y + label_h + p;
            int ldn_y = y + label_h + p + btn_h + p;
            lup->resize(x, lup_y, w, label_h);
            btn->resize(x, btn_y, w, btn_h);
            ldn->resize(x, ldn_y, w, label_h);

            lup->setFont(font);
            btn->setFont(font);
            ldn->setFont(font);
        }
    };

    TripleWidget m_bankA;
    TripleWidget m_bankB;
    TripleWidget m_bankC;
    TripleWidget m_bankD;
    TripleWidget m_bankE;

    TripleWidget m_userA;
    TripleWidget m_userB;
    TripleWidget m_userC;
    TripleWidget m_userD;
    TripleWidget m_userE;

    // std::array<TripleWidget, 5> m_bankWidgets;
    // std::array<TripleWidget, 5> m_userWidgets;

    de::gui::Button* m_btnCancel; // Cancel dialog.
    de::gui::Button* m_btnAccept; // Accept transaction.

    void createTripleWidget(TripleWidget& w, int tileType);

    void drawTitle();
    void drawBankIconAndName();
    void drawUserIcon();
    void drawUserName();

    void updateUserSide(int lastType);
    void updateBankSide();
};
