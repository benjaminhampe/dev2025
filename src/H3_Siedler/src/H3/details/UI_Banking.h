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

    int m_giveCard; // H3_TileType resource Card type the player gives
    int m_wishCard; // H3_TileType the player wants from bank

    de::Recti m_rcHead;
    de::Recti m_rcBank;
    de::Recti m_rcUser;
    de::Recti m_rcName;
    de::Recti m_rcBtns;

public:
    struct LabelButton
    {
        de::Recti rect;
        de::gui::Button* btn;
        de::gui::Label* lbl;

        void setText( const std::string& text )
        {
            lbl->setText( text );
            btn->setText( text );
        }

        void resize(int x,int y,int w,int h, int p, int label_h, bool labelOnTop = false)
        {
            if (labelOnTop)
            {
                int label_y = y;
                int butto_y = y + label_h + p;
                int butto_h = h - label_h - p;
                lbl->resize(x, label_y, w, label_h);
                btn->resize(x, butto_y, w, butto_h);
            }
            else
            {
                int butto_y = y;
                int butto_h = h - label_h - p;
                int label_y = y + butto_h + p;
                lbl->resize(x, label_y, w, label_h);
                btn->resize(x, butto_y, w, butto_h);
            }
        }
    };

private:
    LabelButton m_bankA;
    LabelButton m_bankB;
    LabelButton m_bankC;
    LabelButton m_bankD;
    LabelButton m_bankE;

    LabelButton m_userA;
    LabelButton m_userB;
    LabelButton m_userC;
    LabelButton m_userD;
    LabelButton m_userE;

    de::gui::Button* m_btnCancel; // Cancel dialog.
    de::gui::Button* m_btnAccept; // Accept transaction.

    void createLabelButton(UI_Banking::LabelButton& w, int tileType);

    void drawTitle();
    void drawBankIconAndName();
    void drawUserIcon();
    void drawUserName();

};
