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

    void updateMaxima();
    void updatePlots( const de::Recti& rcPlot );
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

    struct Max
    {
        u32 vicPoints;
        // bool hasPort3v1;
        // bool hasPortA;
        // bool hasPortB;
        // bool hasPortC;
        // bool hasPortD;
        // bool hasPortE;
        // bool hasBonusArmy;
        // bool hasBonusRoad;

        H3_Bank bank;
        H3_Bank bankAccum;
        // H3_Dice dice;

        // u32 tiles;
        // u32 corners;
        // u32 edges;

        u32 roads;
        u32 farms;
        u32 citys;
        u32 cards;
        u32 ships;

        u32 points;
        u32 knights;
        u32 invents;
        u32 monopols;

        u32 pointCards;
        u32 knightCards;
        u32 inventCards;
        u32 monopolCards;

        void clear()
        {
            vicPoints = 0;
            // bool hasPort3v1;
            // bool hasPortA;
            // bool hasPortB;
            // bool hasPortC;
            // bool hasPortD;
            // bool hasPortE;
            // bool hasBonusArmy;
            // bool hasBonusRoad;

            bank.clear();
            bankAccum.clear();
            // H3_Dice dice;

            // tiles = 0;
            // corners = 0;
            // edges = 0;

            roads = 0;
            farms = 0;
            citys = 0;
            cards = 0;
            ships = 0;

            points = 0;
            knights = 0;
            invents = 0;
            monopols = 0;

            pointCards = 0;
            knightCards = 0;
            inventCards = 0;
            monopolCards = 0;
        }
    };

    Max m_max; // Needed to fitting scale plots in y direction.

    struct Plot
    {
        enum eCategory
        {
            VicPoints = 0,
            BankSum,
            Road,
            Farm,
            City,
            EventCard,
        };

        std::string name;
        u32 playerId = 0;
        u32 category = 0;
        u32 icon = 0;
        int maxValue = 0;
        std::vector< int > values;

        de::smesh::SMeshBuffer lineStrip;
    };

    typedef std::vector<Plot> Plots; // Plots per Player

    std::vector<Plots> m_categories; // Plots per Player x plot categories

    de::smesh::SMeshBuffer m_xAxis;
};



