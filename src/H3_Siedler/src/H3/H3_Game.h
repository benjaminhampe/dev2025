#pragma once
#include <H3/H3_Board.h>
#include <H3/details/H3_Font.h>
#include <H3/details/H3_Img.h>
#include <H3/details/H3_Tex.h>
#include <H3/details/H3_Camera.h>

#include <H3/details/UI_MainMenu.h>
#include <H3/details/UI_Game.h>

#include <H3/renderers/H3_ChipRenderer.h>
#include <H3/renderers/H3_ObjRenderer.h>
#include <H3/renderers/H3_DiceRenderer.h>
#include <H3/renderers/H3_TileRenderer.h>
#include <H3/renderers/H3_WaterTileRenderer.h>
#include <de/os/Window_WGL.h>
#include <de/gpu/AnimatedTexRenderer.h>

#include <H3/animations/H3_Animations.h>

#ifdef HAVE_DE_AUDIO
    #include <de/audio/AudioEngine.h>
#endif

// class UI_MainMenu;
// class UI_WorldCreator;
// class UI_Actions;
// class UI_DonateCards;
// class UI_WinnerScreen;
//class H3_UI_Banking m_uiBanking;
//class H3_UI_Logger m_uiActionLogger;
//class H3_Marquee m_uiMarquee;

// ===========================================================================
struct H3_Game
// ===========================================================================
{
    H3_Game();
    ~H3_Game();
    const de::Image& getImg( H3_Img::eID id ) const;
    de::gpu::TexRef getTex( H3_Tex::eID id, const std::string & caller ) const;

    float getZoom() const;
    void setZoom( float zoom );

    bool init( de::Window* window );
    void resize( int w, int h );
    void destroy();
    void requestClose();
    bool run();

    // void setPreset(const H3_Cfg& preset);
    void exitProgram();
    void newGame();
    void restartGame();
    void toggleMainMenu();

    void exitGame();
    void startGame(); // Actually starts a new Game

    void enterIdle();
    void doEndTurn();
    void enterDonateCards(uint32_t playerId);  // Donate cards to robber
    void enterThiefPlacement();     // Place robber on new tile
    void leaveThiefPlacement( H3_Tile& tile ); // Place robber on new tile
    void leaveThiefOutro( H3_Corner& corner ); // Select player to steal card from
    void leaveThiefOutro( H3_Edge& edge );     // Select player to steal card from
    void enterUseKnightCard();
    void enterWinnerScreen();

    void enterBanking();
    void enterTrading();

    // After 1st round we sort players
    // ordered by highest to lowest dice value
    void computeRanking(); // Used in: enterRound2()

    void computeBonusRoad();
    void computeBonusArmy();
    void computeWinner();

    void doDice();
    void doPlaceCancel();
    void doBuyRoad();
    void finalizeBuyRoad( H3_Edge & edge );
    void doBuyFarm();
    void finalizeBuyFarm( H3_Corner & corner );
    void doBuyCity();
    void finalizeBuyCity( H3_Farm & farm );
    void doBuyCard();
    void doBank();
    void doTrade();
    void revealPointCard();
    void revealKnightCard();
    void revealInventCard();
    void revealMonopolCard();

    void computeVisibleCorners( uint32_t playerId );
    void computeVisibleEdges( uint32_t playerId );

    void pick();
    void stealRandomCard( int playerFrom, int playerTo );

    int findPlayer( const u32 playerId ) const;
    H3_Player& getPlayer( const u32 playerId );
    H3_Player& getCurrentPlayer();

    bool isGameRunning() const { return m_round > 0; }

    de::Window* getWindow() const { return m_window; }

    de::gpu::VideoDriver* getDriver() const { return m_driver; }

    de::gpu::Camera* getCamera() const { return m_driver->getCamera(); }

    void draw();
    void drawSkybox();
    void drawSun();
    void drawTiles();
    void drawPortBridges();
    void drawChips();
    void drawCorners();
    void drawEdges();
    void drawRoads();
    void drawFarms();
    void drawCitys();
    void drawThief();
    void drawDice();
    void drawHoveredEdge();
    void drawHoveredCorner();
    void drawHoveredTile();

    void clearMarquee();
    void marqueeH1( std::wstring msg, int durationMs, uint32_t color = 0xFFFFFFFF );
    void marqueeH3( std::wstring msg, int durationMs, uint32_t color = 0xFFFFFFFF );

    // bool run();

    void updateWindowTitle();

    void toggleHelpWindow();

    H3_GuiState UI_getState() const;
    void UI_setState(const H3_GuiState& params);
    void UI_resizeLayouts();
    void UI_updateActions();

    uint32_t
    getPlayerColor( uint32_t i ) const
    {
        return getDefaultPlayerColor( i );
    }

    static uint32_t
    getDefaultPlayerColor( uint32_t i )
    {
        switch( i % 6 )
        {
        case 1: return dbRGB(255,0,0);
        case 2: return dbRGB(255,255,0);
        case 3: return dbRGB(0,255,0);
        case 4: return dbRGB(0,0,255);
        case 5: return dbRGB(255,0,255);
        default: return 0xFFFFFFFF;
        }
    }

    glm::vec3 m_tileSize = glm::vec3( 100, 10, 100 );
    glm::vec3 m_chipSize = glm::vec3( 25, 4, 25 );
    glm::vec3 m_cornerSize = glm::vec3( 20, 3, 20 );
    glm::vec3 m_edgeSize = glm::vec3( 18, 2, 18 );
    glm::vec3 m_cardSize = glm::vec3( 60, 5, 100 );
    glm::vec3 m_roadSize = glm::vec3( 30, 6,  6 );
    glm::vec3 m_farmSize = glm::vec3( 10, 10, 10 );
    glm::vec3 m_citySize = glm::vec3( 20, 10, 10 );
    glm::vec3 m_playerSize = glm::vec3( 20, 60, 20 );
    glm::vec3 m_thiefSize = glm::vec3( 20, 60, 20 );

    // de::IrrlichtDevice* m_device;
    de::Window* m_window;
    de::gpu::VideoDriver* m_driver;
    //de::audio::AudioEngine m_audioEngine;
    de::gui::Env m_guienv;

    H3_Cfg m_cfg;
    H3_State m_state;
    H3_BonusArmy m_bonusArmy; // current state
    H3_BonusRoad m_bonusRoad; // current state
    int m_round;    // current round index
    int m_turn;     // current round index
    int m_playerId;   // current Player index
    uint32_t m_winnerId;


    bool m_bPaintEvents = false;
    bool m_bShouldRun = true;
    uint32_t m_fpsTimerId;  // Screen FPS...
    uint32_t m_capTimerId;  // Window title update...

    de::smesh::SMeshBuffer m_smeshSun;
    de::smesh::SMeshBuffer m_smeshTile;
    de::smesh::SMeshBuffer m_smeshChip;
    de::smesh::SMeshBuffer m_smeshCorner;
    de::smesh::SMeshBuffer m_smeshEdge;
    de::smesh::SMeshBuffer m_smeshRoad;
    de::smesh::SMeshBuffer m_smeshFarm;
    de::smesh::SMeshBuffer m_smeshCity;
    H3_ObjMeshBuffer m_tmeshRoad;
    H3_ObjMeshBuffer m_tmeshFarm;
    H3_ObjMeshBuffer m_tmeshCity;
    de::smesh::SMeshBuffer m_smeshThief;

    // dc == (instanced) drawcall of one SMeshBuffer.
    H3_ChipMeshBuffer m_dcChips;
    H3_ChipMeshBuffer m_dcCorners;
    H3_ChipMeshBuffer m_dcEdges;

    H3_Font m_font;
    H3_Img m_img;
    H3_Tex m_tex;

    UI_MainMenu* m_uiMenu;
    UI_Game* m_uiGame;

    // Topology
    std::vector<H3_Tile>    m_tiles;
    //std::vector<H3_Chip>  m_chips;
    std::vector<H3_Corner>  m_corners;
    std::vector<H3_Edge>    m_edges;
    std::vector<H3_Player>  m_players;
    std::vector<H3_Player*> m_humans;
    std::vector<H3_Player*> m_enemies;  // Used for faster drawing UI_CpuPlayer

    // Board objects
    H3_Thief                m_thief;
    std::vector< H3_Card >  m_cards;
    std::vector< H3_Road >  m_roads;
    std::vector< H3_Farm >  m_farms;
    std::vector< H3_City >  m_citys;

    H3_Dice m_dice;     // current dice

    // Round 1. - Decide order by dice ranking
    // Round 2. - Place 1st farm + road
    // Round 3. - Place 2nd farm + road
    // Round 4. - Normal gameplay
    std::vector< H3_RankedPlayer > m_ranking;

    // =======================================================================
    // struct H3_Pick m_pick;
    // =======================================================================
    bool m_isCameraMouseInputEnabled = false;
    bool m_isCameraFreeMoveEnabled = false;
    bool m_firstMouse = (true);
    bool m_isMouseLeftPressed = false;
    bool m_isMouseRightPressed = false;
    bool m_isMouseMiddlePressed = false;
    int m_mouseX = 0;
    int m_mouseY = 0;
    int m_lastMouseX = 0;
    int m_lastMouseY = 0;
    int m_mouseMoveX = 0;
    int m_mouseMoveY = 0;
    // int m_leftDragStartX = 0;
    // int m_leftDragStartY = 0;
    // int m_leftDragLastX = 0;
    // int m_leftDragLastY = 0;
    // int m_leftDragPosX = 0;
    // int m_leftDragPosY = 0;
    // double cylinderCamRadius = 500;
    // double cylinderCamHeight = 300;
    // double cylinderCamAngleY = 0;

    // Scene-Collision: Pick
    // int m_mouseX = 0;
    // int m_mouseY = 0;
    int m_pickState = 0; // 0 = none, 1 = road, 2 = farm, 3 = city, 4 = thief.

    // Scene-Collision: Hover
    glm::vec3 m_hitPos;
    float m_hitDist;
    // Scene-Collision: Hover
    int m_hoverState = 0; // 0 = none, 1 = road, 2 = farm, 3 = city, 4 = thief.
    int m_hoverTileId = 0;
    int m_hoverCornerId = 0;
    int m_hoverEdgeId = 0;
    int m_hoverRoadId = 0;
    int m_hoverFarmId = 0;
    int m_hoverCityId = 0;

    H3_ChipRenderer m_chipRenderer;
    H3_ObjRenderer m_objRenderer;
    H3_DiceRenderer m_diceRenderer;
    H3_TileRenderer m_tileRenderer;
    H3_WaterTileRenderer m_waterTileRenderer;
    de::gpu::AnimatedTexRenderer m_animatedTexRenderer;
    //de::BillboardRenderer m_billboardRenderer;
    //de::RainbowRenderer m_rainbowRenderer;
    H3_Animations m_animations;

    bool m_bUpdateDrawCall_Tiles = true;
    bool m_bUpdateDrawCall_Chips = true;
    bool m_bUpdateDrawCall_Corners = true;
    bool m_bUpdateDrawCall_Edges = true;
    bool m_bUpdateDrawCall_Roads = true;
    bool m_bUpdateDrawCall_Farms = true;
    bool m_bUpdateDrawCall_Citys = true;

};

// void H3_Game_setWindow( H3_Game & game, de::Window* window );

// void H3_Game_setHelpWindow( H3_Game & game, de::gpu::gui::Panel* window );

bool H3_MessageBox(std::string title, std::string msg);




