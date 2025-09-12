#pragma once
#include <H3/H3_Board.h>
#include <H3/details/H3_Font.h>
#include <H3/details/H3_Img.h>
#include <H3/details/H3_Tex.h>
#include <H3/details/UI_ActionLogger.h>
#include <H3/details/UI_Banking.h>
#include <H3/renderers/H3_ChipRenderer.h>
#include <H3/renderers/H3_ObjRenderer.h>
#include <H3/renderers/H3_DiceRenderer.h>
#include <H3/renderers/H3_WaterTileRenderer.h>

#include <de/terrain/Hillplane.h>
#include <de/terrain/HTerrain.h>

#ifdef HAVE_DE_AUDIO
    #include <de/audio/AudioEngine.h>
#endif

class UI_MainMenu;
class UI_WorldCreator;
class UI_Actions;
class UI_ThiefIntro;
class UI_WinnerScreen;
//class H3_UI_Banking m_uiBanking;
//class H3_UI_Logger m_uiActionLogger;
//class H3_Marquee m_uiMarquee;

//#include <de/gui/IntField.h>
//#include <de/os/CommonDialogs.h>
//#include <H3/ImageSkyboxRepair.h>
//#include <de/gpu/mtl/MTL.h>

// ===========================================================================
struct H3_State
// ===========================================================================
{
    enum eState
    {
        StartMenu = 0,  // Show Wallpaper and MainMenu
        NewGame,        // Show WorldCreator
        Idle,           // InGame Idle round1/2/3/4+
        Dice,
        DonateCards,
        PlaceThief,
        StealRandomCard,
        UseKnightCard,
        PlaceRoad,
        PlaceFarm,
        PlaceCity,
        Banking,
        Winner,
        EStateCount
    };

    static std::string
    getString( eState state )
    {
        switch( state )
        {
            case StartMenu: return "StartMenu";
            case NewGame: return "NewGame";
            case Idle: return "Idle";
            case Dice: return "Dice";
            case DonateCards: return "DonateCards";
            case PlaceThief: return "PlaceThief";
            case StealRandomCard: return "StealRandomCard";
            case UseKnightCard: return "UseKnightCard";
            //case UseMonopolyCard: return "UseMonopolyCard";
            //case UseInventionCard: return "UseInventionCard";
            //case UseVictoryCard: return "UseVictoryCard";
            case PlaceRoad: return "PlaceRoad";
            case PlaceFarm: return "PlaceFarm";
            case PlaceCity: return "PlaceCity";
            case Banking: return "Banking";
            case Winner: return "Winner";
            default: return "Unknown";
        }
    }

    eState m_state;

    H3_State() : m_state(Idle) {}
    H3_State(eState state) : m_state(state) {}

    operator eState () const { return m_state; }

    std::string str() const { return getString( m_state ); }

    H3_State& operator= (eState state)
    {
        m_state = state;
        return *this;
    }
};

// ===========================================================================
struct UI_MarqueeItem
// ===========================================================================
{
    std::wstring msg;
    uint32_t color = 0xFFFFFFFF;
    int timeDurationInMs = 0; // 0 = display forever until clear()
    std::optional<double> timeStartInSec = 0;
    //bool hasStarted = false;
    de::Font font = de::Font("Garton", 128);
};

// ===========================================================================
struct UI_Marquee
// ===========================================================================
{
    de::Font fontH1 = de::Font("Postamt", 48);
    de::Font fontH3 = de::Font("Postamt", 24);
    std::deque< UI_MarqueeItem > h1;
    std::deque< UI_MarqueeItem > h3;
};

// ===========================================================================
struct H3_Game : public de::IEventReceiver
// ===========================================================================
{
    H3_Game();
    ~H3_Game();
    const de::Image& getImg( H3_Img::eID id ) const { return *H3_getImg(*this,id); }
    de::gpu::TexRef getTex( H3_Tex::eID id, const std::string & caller ) const;

    int getScalePc() const { return m_guienv.getScalePc(); }
    void setScalePc( int iScalePc ) { m_guienv.setScalePc( iScalePc ); }

    void init(de::IrrlichtDevice* device);
    void setPreset(const H3_Cfg& preset);
    void exitProgram();
    void exitGame();
    void newGame();
    void restartGame();

    void enterStartMenu();
    void enterRound1();
    void enterRound2();
    void enterRound3();
    void enterRoundN();
    void enterThiefIntro();     // Donate cards to robber
    void enterThiefPlacement();     // Place robber on new tile
    void leaveThiefPlacement( H3_Tile& tile ); // Place robber on new tile
    void leaveThiefOutro( H3_Corner& corner ); // Select player to steal card from
    void leaveThiefOutro( H3_Edge& edge );     // Select player to steal card from
    void enterUseKnightCard();
    void enterWinnerScreen();

    // After 1st round we sort players
    // ordered by highest to lowest dice value
    void computeRanking(); // Used in: enterRound2()
    void doEndTurn();
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

    void computeVisibleCorners( uint32_t playerId );
    void computeVisibleEdges( uint32_t playerId );

	void pick();
    void stealRandomCard( int playerFrom, int playerTo );

    H3_Player& getPlayer( const u32 playerId );
    H3_Player& getCurrentPlayer();

    bool isRunning() const { return m_round > 0; }

    de::Window* getWindow() { return m_device->getWindow(); }

    de::gpu::VideoDriver* getDriver() { return m_device->getVideoDriver(); }

    de::gpu::Camera* getCamera() { return m_device->getVideoDriver()->getCamera(); }

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

    void drawMarquee();
    void clearMarquee();
    void marqueeH1( std::wstring msg, int durationMs, uint32_t color = 0xFFFFFFFF );
    void marqueeH3( std::wstring msg, int durationMs, uint32_t color = 0xFFFFFFFF );

    bool run();

    void updateWindowTitle();

    void toggleHelpWindow();

    void UI_resizeLayouts();
    void UI_updateActions();

    void onEvent( const de::Event& event ) override;

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
        case 1: return dbRGBA(255,0,0);
        case 2: return dbRGBA(255,255,0);
        case 3: return dbRGBA(0,255,0);
        case 4: return dbRGBA(0,0,255);
        case 5: return dbRGBA(255,0,255);
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

    //de::audio::AudioEngine m_audioEngine;
    H3_Cfg m_cfg;

    de::IrrlichtDevice* m_device;
    de::gpu::Camera m_camera;
    de::gui::Env m_guienv;

    bool m_acceptPaintEvents = false;
    uint32_t m_fpsTimerId;  // Screen FPS...
    uint32_t m_capTimerId;  // Window title update...

    de::gpu::SMeshBuffer m_smeshSun;
    de::gpu::SMeshBuffer m_smeshTile;
    de::gpu::SMeshBuffer m_smeshChip;
    de::gpu::SMeshBuffer m_smeshCorner;
    de::gpu::SMeshBuffer m_smeshEdge;
    de::gpu::SMeshBuffer m_smeshRoad;
    de::gpu::SMeshBuffer m_smeshFarm;
    de::gpu::SMeshBuffer m_smeshCity;
    H3_ObjMeshBuffer m_tmeshRoad;
    H3_ObjMeshBuffer m_tmeshFarm;
    H3_ObjMeshBuffer m_tmeshCity;
    de::gpu::SMeshBuffer m_smeshThief;

    // dc == (instanced) drawcall of one SMeshBuffer.
    H3_ChipMeshBuffer m_dcChips;
    H3_ChipMeshBuffer m_dcCorners;
    H3_ChipMeshBuffer m_dcEdges;

    de::gpu::HMeshRenderer m_hmeshRenderer;
    de::gpu::HMesh m_hmeshTileDesert; // Nevada
    de::gpu::HMesh m_hmeshTileA; // Mt Wilder
    de::gpu::HMesh m_hmeshTileB; // ?
    de::gpu::HMesh m_hmeshTileC; // Thuringen
    de::gpu::HMesh m_hmeshTileD; // Italia
    de::gpu::HMesh m_hmeshTileE; // Alpen

    H3_Font m_font;
    H3_Img m_img;
    H3_Tex m_tex;

    de::gui::Widget* m_uiHelpWindow;
    UI_MainMenu* m_uiMainmenu;
    UI_WorldCreator* m_uiWorldCreator;
    UI_Actions* m_uiActions;
    UI_Banking m_uiBanking;
    UI_ActionLogger m_uiActionLogger;
    UI_Marquee m_uiMarquee;
    UI_ThiefIntro* m_uiThiefIntroPanel;
    UI_WinnerScreen* m_uiWinnerScreen;

    // Topology
    std::vector<H3_Tile>    m_tiles;
    //std::vector<H3_Chip>  m_chips;
    std::vector<H3_Corner>  m_corners;
    std::vector<H3_Edge>    m_edges;
    std::vector<H3_Player>  m_players;

    // Board objects
    H3_Thief                m_thief;
    std::vector< H3_Card >  m_cards;
    std::vector< H3_Road >  m_roads;
    std::vector< H3_Farm >  m_farms;
    std::vector< H3_City >  m_citys;

    H3_State m_state = H3_State::StartMenu;
    int m_round = 0;    // current round index
    int m_turn = 0;     // current round index
    int m_player = 0;   // current Player index
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
    int m_leftDragStartX = 0;
    int m_leftDragStartY = 0;
    int m_leftDragLastX = 0;
    int m_leftDragLastY = 0;
    int m_leftDragPosX = 0;
    int m_leftDragPosY = 0;
    double cylinderCamRadius = 500;
    double cylinderCamHeight = 300;
    double cylinderCamAngleY = 0;

    std::array<bool, 1024> m_keyStates;

    //int placeMode = 0; // 0 = thief, 1 = road, 2 = home, 3 = city, 4 = thief.
    // Scene-Collision: Pick
    // int m_mouseX = 0;
    // int m_mouseY = 0;
    int m_pickState = 0; // 0 = none, 1 = road, 2 = villa, 3 = city, 4 = thief.

    // Scene-Collision: Hover
    glm::vec3 m_hitPos;
    float m_hitDist;
    // Scene-Collision: Hover
    int m_hoverState = 0; // 0 = none, 1 = road, 2 = villa, 3 = city, 4 = thief.
    int m_hoverTileId = 0;
    int m_hoverCornerId = 0;
    int m_hoverEdgeId = 0;
    int m_hoverRoadId = 0;
    int m_hoverFarmId = 0;
    int m_hoverCityId = 0;
    //H3_Tile* m_hoverTile = nullptr;
    //H3_Corner* m_hoverCorner = nullptr;
    //H3_Edge* m_hoverEdge = nullptr;
    //};

    //de::BillboardRenderer m_billboardRenderer;
    //de::RainbowRenderer m_rainbowRenderer;

    //H3_TileRenderer m_tileRenderer;
    H3_ChipRenderer m_chipRenderer;
    H3_ObjRenderer m_objRenderer;
    H3_DiceRenderer m_diceRenderer;
    //H3_UI_Renderer m_uiRenderer;
    H3_WaterTileRenderer m_waterTileRenderer;
    //};

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




