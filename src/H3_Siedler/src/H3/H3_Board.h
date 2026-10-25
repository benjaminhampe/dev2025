#pragma once
#include <cstdint>
#include <string>
//#include <unordered_map>
//#include <deque>
//#include <array>
//#include <optional>
//#include <de/IrrlichtDevice.h>
#include <de/gpu/VideoDriver.h>
#include <de/gui/Env.h>
#include <de/gui/Panel.h>
#include <de/gui/Label.h>
#include <de/gui/Button.h>
#include <de/gui/IntField.h>

typedef uint32_t u32;
typedef int32_t s32;

struct H3_TileType;
struct H3_Cost;
struct H3_CfgTile;
struct H3_CfgPlayer;
struct H3_Cfg;
struct H3_Bank;
struct H3_Dice;
struct H3_Tile;
struct H3_Corner;
struct H3_Edge;
struct H3_Thief;
struct H3_Card;
struct H3_Road;
struct H3_Farm;
struct H3_City;
struct H3_RankedPlayer;
struct H3_Player;
struct LineBoxAnim;
// struct H3_UIElem;
// struct H3_UI;
struct H3_Action;
struct H3_ActionLog;
struct H3_Game;

// =====================================================================
struct LineBoxAnim
// =====================================================================
{
    float ang_y = 45.0;
    float ang_x = 14.0;
    void draw( de::gpu::VideoDriver* driver );
};

// =====================================================================
struct H3_Util
// =====================================================================
{
    static bool
    push_back_unique( std::vector<uint32_t> & ids, uint32_t id);

    static std::string
    str( const std::vector<uint32_t> & v);
};

// =====================================================================
struct H3_GuiState // Actually just H3_UIGameState
// =====================================================================
{
    bool bWorldCreator = false;
    bool bGameOverlay = false;    // Actions + stats + logger + dice
    bool bDonateCards = false;
    bool bBanking = false;
    bool bWinnerScreen = false;

    std::string str() const;
};

// ===========================================================================
struct H3_State
// ===========================================================================
{
    enum eState
    {
        // StartMenu = 0,  // Show Wallpaper and MainMenu
        // NewGame,        // Show WorldCreator
        Idle = 0,           // InGame Idle round1/2/3/4+
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
    getString( eState state );

    eState m_state;

    H3_State();
    H3_State(eState state);

    operator eState () const { return m_state; }

    std::string str() const;

    H3_State& operator= (eState state);
};

// =====================================================================
struct H3_TileType
// =====================================================================
{
    enum EType : uint8_t
    {
        Unknown = 0, Water, Desert, TileA, TileB, TileC, TileD, TileE,
        Port3v1, PortA, PortB, PortC, PortD, PortE, Count
    };
    EType value;

    H3_TileType();
    H3_TileType( EType type );
    H3_TileType( uint32_t type );
    //operator EType () const { return m_Value; }
    operator int () const { return value; }
    // void set( EType type ) { value = type; }
    // EType get() const { return value; }

    std::string str() const;

    static std::string getString( const H3_TileType tt );
    static uint32_t getColor( const H3_TileType tt );
    static bool isResource( const H3_TileType tt );

    bool hasChip() const;
    bool isResource() const;
    bool isWater() const;
    bool isWaterOrPort() const;
    bool isPort() const;
    bool isPort3v1() const;
    bool isPort2v1() const;
    bool empty() const;
    uint32_t color() const;
    std::string name() const;

    static H3_TileType
    parse( std::string const & txt );
};

// ===========================================================================
struct H3_Cost
// ===========================================================================
{
    H3_TileType resource;
    uint8_t count;

    H3_Cost();
    H3_Cost( H3_TileType rc, uint8_t n);
};

// =====================================================================
struct H3_CfgTile
// =====================================================================
{
    H3_TileType tileType = H3_TileType::Unknown;
    int chipValue = 0;
    int i = 0;      // 2d board index x
    int j = 0;      // 2d board index y
    int angle60 = 0;// angle [0..5] to rotate tile in 60 degrees steps.
    //H3_TRS trs;   // computed 3d transform = func(i,j,angle)

    std::string str() const;
};

// ==================================================================
struct H3_PlayerType
// ==================================================================
{
    enum eType { CPU = 0, HUMAN };
    eType m_type;

    H3_PlayerType();
    H3_PlayerType(eType state);

    operator eType () const { return m_type; }
    H3_PlayerType& operator= (eType state);

    bool isHuman() const { return m_type == HUMAN; }
    bool isCPU() const { return m_type == CPU; }

    std::string str() const;
    static std::string getString( eType state );
};

// ==================================================================
struct H3_PlayerConnect
// ==================================================================
{
    enum eType { LOCAL = 0, TCP, UDP };
    eType m_type;

    H3_PlayerConnect();
    H3_PlayerConnect(eType state);

    operator eType () const { return m_type; }
    H3_PlayerConnect& operator= (eType state);

    std::string str() const;
    static std::string getString( eType state );
};

// ==================================================================
struct H3_CfgPlayer
// ==================================================================
{
    H3_PlayerType typ;
    H3_PlayerConnect connect;
    u32 color;
    u32 avatar;
    std::string name;

    H3_CfgPlayer();

    std::string str() const;

    static std::string getPlayerName(int i); // H3_Img::PlayU, Play0 ... PlayF

    static uint32_t getPlayerColor(int i); // 6 colors 0...5
};

// ===========================================================================
struct H3_Cfg
// ===========================================================================
{
    std::string presetName;  //

    u32 victoryPoints;      // Num points to victory
    u32 cardLimitNoSteal;   // Num cards player can hold without robber steal. 0 = disable
    u32 buildingSpacing;    // Num of corners between neighboring buildings, default = 1, can be 0 for more dense use of board.
    u32 numPlayers;         // 1+
    u32 useCardKnight;      // +1 Knight with this card
    u32 useCardVictoryPoint;// +1 Victory point with this card
    u32 useCardMonopol;     // Take all cards of one resource of all players
    u32 useCardInvention;   // +2..+4 Arbitrary resource cards from bank
    u32 skipFirstRound;
    u32 bonusRoadLimit;     // 0 = disabled, +2 points for atleast default:5 connected roads
    u32 bonusArmyLimit;     // 0 = disabled, +2 points for atleast default:3 knights

    std::vector< H3_CfgTile > tileList;

    H3_CfgPlayer player; // human

    // std::vector< H3_CfgPlayer > playerList;

    // std::vector< H3_Cost > cost4card;   // Standard
    // std::vector< H3_Cost > cost4road;   // Standard
    // std::vector< H3_Cost > cost4farm;   // Standard
    // std::vector< H3_Cost > cost4city;   // Standard

    static H3_Cfg
    createStandard();

    void dump() const;

    std::string
    str() const;

protected:
    void
    addTile( int i, int j, H3_TileType tileType, int angle60 = 0, int diceValue = -1 );

    // void
    // createStandardTiles();
};

// =====================================================================
struct H3_Bank
// =====================================================================
{
    int A, B, C, D, E;

    H3_Bank();
    H3_Bank( int a, int b, int c, int d, int e );
    H3_Bank( const H3_Bank & o );
    H3_Bank& operator= ( const H3_Bank & other );
    // Used in stealRandomCard()
    // The bank has sum() cards.
    // Get TileType at index[ i ] in range { 0, sum()-1 }.
    H3_TileType getTileTypeFromCardIndex( int index ) const;
    void clear();
    int sum() const;
    static H3_Bank costOfRoad();
    static H3_Bank costOfFarm();
    static H3_Bank costOfCity();
    static H3_Bank costOfCard();
    bool canBuyRoad() const;
    bool canBuyFarm() const;
    bool canBuyCity() const;
    bool canBuyCard() const;
    bool canBuy( H3_Bank const & o ) const;
    bool buy( H3_Bank const & price );
    bool buyRoad();
    bool buyFarm();
    bool buyCity();
    bool buyCard();
    // [<=]
    int32_t getAmount( const H3_TileType tt ) const;
    // [=>]
    void resetAmount( const H3_TileType tt, const int32_t value );
    // [+] Deposit to current amount...
    void addAmount( const H3_TileType tt, const int value );
    // [-] Withdraw from current amount...
    void subAmount( const H3_TileType tt, const int value );
    std::string str() const;
    H3_Bank & add( const H3_Bank & o );
    H3_Bank & sub( const H3_Bank & o );
    H3_Bank operator+ ( H3_Bank const & o ) const;
    H3_Bank operator- ( H3_Bank const & o ) const;
    H3_Bank& operator+= ( H3_Bank const & o );
    H3_Bank& operator-= ( H3_Bank const & o );

    // TODO: Remove if not used anymore. (Was initially used in banking)
    int countTypes() const;
};

// =====================================================================
struct H3_Dice
// =====================================================================
{
    int a;
    int b;

    H3_Dice();
    void clear();
    int sum() const;
    int max() const;
    int min() const;

    std::string str() const;
};

// =====================================================================
struct H3_BonusArmy
// =====================================================================
{
    u32 owner = 0;   // player id
    u32 knights = 0; // number of knights (Default: min. 3)
    std::string str() const;
};

// =====================================================================
struct H3_BonusRoad
// =====================================================================
{
    u32 owner = 0; // player id
    u32 roads = 0; // number of connected roads. (Default: min. 6)
    std::string str() const;
};

// =====================================================================
struct H3_Corner
// =====================================================================
{
    u32 id = 0;
    u32 owner = 0; // player id
    u32 building_type = 0; // default: none = 0, farm = 1, city = 2, bespin sky city = 3
    bool visible = true;
    // bool enabled = true; // not disabled by thief, can generate rescards
    de::TRSf trs;    // Computed 3d model space transform
    de::Box3f collisionBoundingBox;
    std::vector<de::Triangle3f> collisionTriangles;
    std::vector< u32 > tiles; // Max 3 neighbouring tiles.
    std::vector< u32 > edges; // Max 3 neighbouring road edges
    std::vector< u32 > next; // Max 3 next corners

    H3_Corner();

    // Connect corner with max 3 corners
    // One corner per call. Corner must be unique.
    // Returns num of added corners. 1 = success, 0 = did nothing/already there.
    bool addCornerId( const u32 cornerId );

    // Connect corner with max 3 tiles
    bool addTileId( const u32 tileId );

    // Connect corner with max. 3 edges
    bool addEdgeId( const u32 edgeId );

    std::string str() const;

    void dump() const;
};

// =====================================================================
struct H3_Edge
// =====================================================================
{
    u32 id = 0;
    u32 owner = 0; // playerId, has road if != 0
    bool visible = true;
    de::TRSf trs;    // Computed 3d model space transform
    de::Box3f collisionBoundingBox;
    std::vector<de::Triangle3f> collisionTriangles;
    std::vector< u32 > tiles; // Max 3 neighbor tiles
    std::vector< u32 > corners; // Max 3 neighbouring village corners
    std::vector< u32 > next; // Max 4 next edges

    H3_Edge();

    // Connect edge with max 4 next edges
    bool addEdgeId( const u32 edgeId );

    // Connect edge with max. 2 tiles
    bool addTileId( const u32 tileId );

    // Connect edge with max 2 corners
    bool addCornerId( const u32 cornerId );

    std::string str() const;
};

// =====================================================================
struct H3_Tile
// =====================================================================
{
    u32 id = 0;
    int chipValue = 0;
    int i = 0;      // 2d board index x
    int j = 0;      // 2d board index y
    int angle60 = 0;// angle [0..5] to rotate tile in 60 degrees steps.
    H3_TileType tileType = H3_TileType::Unknown;
    bool visible = true;
    glm::vec3 pos = glm::vec3(0,0,0);
    de::TRSf trsChip;
    de::Box3f collisionBoundingBox;
    std::vector<de::Triangle3f> collisionTriangles;
    std::vector< u32 > next;    // Topology - max. 6 Neighbours
    std::vector< u32 > corners; // Topology - max. 6 Corners
    std::vector< u32 > edges;   // Topology - max. 6 Edges
    //std::vector< u32 > owners;  // Logic - max. 3-6 owners

    H3_Tile();
    bool isWaterOrPort() const;
    int getCornerIndex( glm::vec3 const & tileSize, H3_Corner & corner ) const;
    // Connect tiles with max. 6 neighbor tiles
    bool addTileId( const u32 tileId );
    // Connect tile with max. 6 unique edges
    bool addEdgeId( const u32 edgeId );
    // Connect tile with max. 6 unique corners
    bool addCornerId( const u32 cornerId );

    std::string str() const;
};

// =====================================================================
struct H3_Thief
// =====================================================================
{
    u32 id;
    u32 occupiedTileId; // tileId
    de::TRSd trs;    // Computed 3d model space transform
    std::vector<de::Triangle3f> triangles; // Cylinder
    H3_Thief();
    std::string str() const;
};

// =====================================================================
struct H3_Card
// =====================================================================
{
    u32 id; // cardId
    u32 owner; // playerId
    u32 typ; // 0 = Ritter, 1 = Invention, 2 = Monopol
    de::TRSf trs;     // Gfx 3d model space transform
    H3_Card();
    std::string str() const;
};

// =====================================================================
struct H3_Road
// =====================================================================
{
    u32 id; // roadId
    u32 owner; // playerId
    u32 edgeId; // edgeId
    u32 color; // Gfx player color, reduces some CPU time.
    de::TRSf trs;     // Gfx 3d model space transform
    std::vector<de::Triangle3f> collisionTriangles;
    H3_Road();
    std::string str() const;
};

// =====================================================================
struct H3_Farm
// =====================================================================
{
    u32 id; // farmId
    u32 owner; // playerId, who owns this farm.
    u32 cornerId; // cornerId, where this farm stands on.
    u32 color; // Gfx player color, reduces some CPU time.
    de::TRSf trs; // Gfx 3d model space transform
    std::vector<de::Triangle3f> collisionTriangles;

    H3_Farm();
    std::string str() const;
};

// =====================================================================
struct H3_City
// =====================================================================
{
    u32 id; // cityId
    u32 owner; // playerId that owns this farm.
    u32 cornerId; // cornerId, where this farm stands on.
    u32 color; // Gfx player color, reduces some CPU time.
    de::TRSf trs; // Gfx 3d model space transform
    std::vector<de::Triangle3f> collisionTriangles; // Clickable to steal cards from sel player.

    H3_City();
    std::string str() const;
};

// =====================================================================
struct H3_RankedPlayer
// =====================================================================
{
    u32 playerId;
    u32 diceSum; // diceValue or victoryPoints
    u32 rank;

    H3_RankedPlayer();
    std::string str() const;
};

// =====================================================================
struct H3_PlayerState
// =====================================================================
{
    int rankDice;
    int rankVicPoints;

    u32 victoryPoints;
    bool hasPort3v1;
    bool hasPortA;
    bool hasPortB;
    bool hasPortC;
    bool hasPortD;
    bool hasPortE;
    bool hasBonusArmy;
    bool hasBonusRoad;

    H3_Bank bank;
    H3_Dice dice;

    std::vector< u32 > tiles;       // tile-ids
    std::vector< u32 > corners;     // corner-ids
    std::vector< u32 > edges;       // edge-ids

    std::vector< u32 > roads;       // road-ids
    std::vector< u32 > farms;       // farm-ids
    std::vector< u32 > citys;       // city-ids
    std::vector< u32 > ships;       // ship-ids

    std::vector< u32 > points;      // Revealed = count into victoryPoints!
    std::vector< u32 > knights;     // Revealed = count into bonusArmy!
    std::vector< u32 > invents;     // Revealed (used up)
    std::vector< u32 > monopols;    // Revealed (used up)

    std::vector< u32 > pointCards;  // Bought in previous rounds, not revealed = not counted in victoryPoints!
    std::vector< u32 > knightCards; // Bought and unrevealed.
    std::vector< u32 > inventCards; // Bought and unrevealed.
    std::vector< u32 > monopolCards;// Bought and unrevealed.

    H3_PlayerState();

    void endTurn();
    void clear();

    bool canTrading() const;
    bool canBanking() const;

    int unrevealedPointCards() const;
    int unrevealedKnightCards() const;
    int unrevealedInventCards() const;
    int unrevealedMonopolCards() const;

    u32 computeVictoryPoints() const;
    void updateVictoryPoints();

    u32 numEventCards(bool bUnrevealed) const
    {
        u32 n = 0;
        n += points.size();
        n += knights.size();
        n += invents.size();
        n += monopols.size();

        if (bUnrevealed)
        {
            n += pointCards.size();
            n += knightCards.size();
            n += inventCards.size();
            n += monopolCards.size();
        }
        return n;
    }

    std::string str() const;
};

// =====================================================================
struct H3_Player
// =====================================================================
{
    u32 id = 0;
    H3_PlayerType typ = H3_PlayerType::CPU;
    H3_PlayerConnect connect = H3_PlayerConnect::LOCAL;
    u32 color = 0xFFFFFFFF;
    u32 avatar = 0; // (H3_Tex::eID) = PlayU....PlayF
    std::string name;

    int ai_strategy; // Determines AI player behaviour
    int ai_handicap; // Determines AI player stupidity
    int ai_cheating; // Determines AI player advantages

    int rank_start; // Round 1
    int rank_dice;  // Round 2,3

    H3_Player() {}

    std::string str() const
    {
        std::ostringstream o;
        o <<
        "id(" << id << "), "
        "name(" << name << "), "
        "typ(" << (H3_PlayerType::HUMAN ? "HUMAN" : "CPU") << "), "
        "color(" << dbHex(color) << "), "
        "connect(" << (H3_PlayerConnect::LOCAL ? "LOCAL" : "REMOTE") << "), "
        "state{" << state.str() << "}";
        return o.str();
    }

    // Current state
    H3_PlayerState state;

    // Statistics
    std::vector< H3_PlayerState > history;

    void endTurn()
    {
        history.emplace_back(state);
        state.endTurn();
    }

    void startGame()
    {
        history.clear();
        state.clear();
        rank_start = 0;
        rank_dice = 0;
    }

    void setBonusArmy(bool value)
    {
        state.hasBonusArmy = value;
        DE_DEBUG("Update player[",id,"] ",name," BonusArmy = ",value)
    }

    void setBonusRoad(bool value)
    {
        state.hasBonusRoad = value;
        DE_DEBUG("Update player[",id,"] ",name," BonusRoad = ",value)
    }

    int getBankingRatio(int tt) const
    {
        if ( tt == 0 && state.hasPortA ) { return 2; }
        if ( tt == 1 && state.hasPortB ) { return 2; }
        if ( tt == 2 && state.hasPortC ) { return 2; }
        if ( tt == 3 && state.hasPortD ) { return 2; }
        if ( tt == 4 && state.hasPortE ) { return 2; }
        if ( state.hasPort3v1 ) return 3;
        return 4;
    }

    H3_Bank getBankingRatios() const
    {
        H3_Bank ratios;
        ratios.A = getBankingRatio(0);
        ratios.B = getBankingRatio(1);
        ratios.C = getBankingRatio(2);
        ratios.D = getBankingRatio(3);
        ratios.E = getBankingRatio(4);
        return ratios;
    }
};

/*
// =====================================================================
struct H3_UIElem
// =====================================================================
{
    enum eType
    {
        Panel = 0,
        Image,
        Button,
        Text,
        Count
    };

    std::string name;

    u32 id = 0;
    u32 typ = 0; // Panel = 0, Image, Button, Text, Count
    u32 radius = 0; // RoundedRect Radius
    u32 border = 3;
    u32 fillColor = 0xFFFFFFFF;
    u32 textColor = 0xFF000000;
    u32 borderColor = 0xFF848484;

    bool visible = true;
    bool keepAspectWidthOverHeight = false;
    bool keepAspectHeightOverWidth = false;

    de::Align align;
    de::Font5x8 font;

    de::Rectf posf;

    de::Recti start_pos;



    de::Recti pos;
    de::gpu::TexRef ref;

    std::string msg;

    std::string str() const
    {
        std::ostringstream s;
        s << name << ", "
            "id(" << id << "), typ(" << typ << "), "
            "msg(" << msg << "), pos:" << pos.str() << ", "
            "ref:" << ref.str();
        return s.str();
    }

    void updateSize( float fScale)
    {
        pos.w = fScale * start_pos.w;
        pos.h = fScale * start_pos.h;
    }
};

// =====================================================================
struct H3_Action
// =====================================================================
{
    u32 playerId;
    u32 action; // 0 - EndTurn, 1 - Dice, 2 - Buy Road, 3 - Buy Card,
    u32 object; // dice1 or card-type, edgeId, cornerId or thief
    u32 amount; // dice2
    H3_Bank bankIn;   // Used for banking and trade
    H3_Bank bankOut;  // Used for banking and trade
    double pts; // time stamp

    std::string msg;
};

// H3_Action( p,0,0,0 ); // = EndTurn
// H3_Action( p,1,0,7 ); // = Dice7
// H3_Action( p,2,edgeId,0 ); // buy road on edgeId
// H3_Action( p,3,cornerId,0 ); // buy farm on cornerId
// H3_Action( p,4,cornerId,0 ); // buy city on cornerId
// H3_Action( p,5,0,0 ); // buy card
// H3_Action( p,5,0,0 ); // buy card

// =====================================================================
struct H3_ActionLog
// =====================================================================
{
    de::Font5x8 font;
    std::vector< H3_Action > actions;
};

// ==================================================================
struct DeviceConfig
// ==================================================================
{
   // Screen0 - Is the given frameBuffer by OS, after we asked for it.
   //           Is always the window's client rect area GetClientRect().
   // 		  	 Controlled by dragging and resizing the window on desktop.
   //	Screen1 is the actual render target where we draw our scene onto
   //			  with resolution controlled by user
   irr::core::dimension2du ScreenSize = irr::core::dimension2du( 1024, 768 );
   uint32_t AntiAlias = irr::video::EAAM_OFF;
   uint32_t BitsPerPixel = 32;
   bool DoubleBuffered = true;
   bool VSync = false;
   uint32_t FrameTimeMs = 1000 / 60; // in [ms], waittime for next render for 60Hz fps.
   uint32_t TextureQuality = 3;
};

*/
