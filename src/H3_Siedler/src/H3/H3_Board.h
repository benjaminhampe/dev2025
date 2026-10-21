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
struct H3_GuiState // Actually just H3_UIGameState
// =====================================================================
{
    // bool bMainMenu = false;
    // bool bHelpWindow = false;
    bool bWorldCreator = false;
    bool bGameOverlay = false;    // Actions + stats + logger + dice
    bool bDonateCards = false;
    bool bBanking = false;
    bool bWinnerScreen = false;
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
    getString( eState state )
    {
        switch( state )
        {
            // case StartMenu: return "StartMenu";
            // case NewGame: return "NewGame";
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

// =====================================================================
struct LineBoxAnim
// =====================================================================
{
    float ang_y = 45.0;
    float ang_x = 14.0;

    void
    draw( de::gpu::VideoDriver* driver, de::smesh::SMaterialRenderer* renderer )
    {
        if ( !driver ) { DE_ERROR("No driver") return; }
        auto camera = driver->getCamera();
        if ( !camera ) { DE_ERROR("No camera") return; }
        auto old_pos = camera->getPos();
        auto old_lat = camera->getTarget();
        auto old_up = camera->getUp();

        camera->lookAt( glm::vec3( 0, 60, -100 ),
                        glm::vec3( 0, 50, 10 ),
                        glm::vec3( 0,1,0 ) );

        de::TRSd trs;
        trs.rotate.y = ang_y;
        trs.rotate.x = ang_x;
        trs.update();
        driver->setModelMatrix( trs.modelMat );

        de::smesh::SMeshBuffer m( de::gpu::PrimitiveType::Lines );

        de::smesh::SMeshBox::addLines(m, glm::vec3(20), glm::vec3(-10), 0xFF1060FF );
        m.setDepth( false );
        //int msaa = driver->getMSAA();
        //if ( msaa > 0 ) m.setLineWidth( msaa );

        renderer->setMaterial( m.material );
        m.draw();
        m.destroy();
        //driver->draw3D( lines );
        //driver->draw3DLineBox( de::gpu::Box3f( -10,-10,-10,10,10,10), 0xFF1060FF );
        ang_y += 5.0f;
        ang_x += 1.0f;

        camera->lookAt( old_pos, old_lat, old_up );
    }
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

    H3_TileType() : value( Unknown ) {}
    H3_TileType( EType type ) : value( type ) {}
    H3_TileType( uint32_t type ) : value( static_cast< EType >( type ) ) {}
    //operator EType () const { return m_Value; }
    operator int () const { return value; }
    // void set( EType type ) { value = type; }
    // EType get() const { return value; }

    std::string str() const { return getString(value); }

    static std::string getString( const H3_TileType tt )
    {
        switch ( tt )
        {
        //case Unknown: return "Unknown";
        case Water: return "Water";
        case Desert: return "Desert";
        case TileA: return "TileA"; // Holz
        case TileB: return "TileB"; // Lehm
        case TileC: return "TileC"; // Wolle
        case TileD: return "TileD"; // Weizen
        case TileE: return "TileE"; // Erz
        case Port3v1: return "Port3v1";
        case PortA: return "PortA";
        case PortB: return "PortB";
        case PortC: return "PortC";
        case PortD: return "PortD";
        case PortE: return "PortE";
        default: return "Unknown";
        }
    }

    static uint32_t getColor( const H3_TileType tt )
    {
        switch ( tt )
        {
        case Water: return 0xFFC03030;
        case Desert: return 0xFF00C0FF;
        case TileA: return 0xFF00CC00; // Holz
        case TileB: return 0xFF0000FF; // Lehm
        case TileC: return 0xFF60FF60; // Wolle
        case TileD: return 0xFF00AFAF; // Weizen
        case TileE: return 0xFF404040; // Erz
        case Port3v1: return 0xFFB01010;
        case PortA: return 0xFF00CC00; // Holz
        case PortB: return 0xFF0000FF; // Lehm
        case PortC: return 0xFF60FF60; // Wolle
        case PortD: return 0xFF00AFAF; // Weizen
        case PortE: return 0xFF404040; // Erz
        default: return 0xFFFFFFFF;
        }
    }

    static bool isResource( const H3_TileType tt )
    {
        return ( tt >= TileA && tt <= TileE );
    }

    bool hasChip() const { return isResource() | isPort(); }

    bool isResource() const { return isResource(value); }

    bool isWater() const { return value == Water; }

    bool isWaterOrPort() const { return isWater() || isPort(); }

    bool isPort() const { return isPort3v1() || isPort2v1(); }

    bool isPort3v1() const { return value == Port3v1; }

    bool isPort2v1() const
    {
        return value == H3_TileType::PortA
               || value == H3_TileType::PortB
               || value == H3_TileType::PortC
               || value == H3_TileType::PortD
               || value == H3_TileType::PortE;
    }


    bool empty() const { return value == Unknown; }
    uint32_t color() const { return getColor(value); }
    std::string name() const { return getString(value); }


    static H3_TileType
    parse( std::string const & txt )
    {
        if      ( txt == "Water" )    { return Water; }
        else if ( txt == "Desert" )   { return Desert; }
        else if ( txt == "TileA" )    { return TileA; }
        else if ( txt == "TileB" )    { return TileB; }
        else if ( txt == "TileC" )    { return TileC; }
        else if ( txt == "TileD" )    { return TileD; }
        else if ( txt == "TileE")     { return TileE; }
        else if ( txt == "PortA")     { return PortA; }
        else if ( txt == "PortB")     { return PortB; }
        else if ( txt == "PortC")     { return PortC; }
        else if ( txt == "PortD")     { return PortD; }
        else if ( txt == "PortE")     { return PortE; }
        else if ( txt == "Port3v1")   { return Port3v1; }
        return Unknown;
    }
};

// ===========================================================================
struct H3_Cost
// ===========================================================================
{
    H3_TileType resource;
    uint8_t count;

    H3_Cost() : resource(H3_TileType::Unknown), count(0) {}
    H3_Cost( H3_TileType rc, uint8_t n) : resource(rc), count(n) {}
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

    std::string str() const
    {
        std::ostringstream o; o <<
        "tileType(" << tileType.str() << "), "
        "chipValue(" << chipValue << "), "
        "i(" << i << "), "
        "j(" << j << "), "
        "angle60("<< angle60 << "), ";
        //"pos("<< trs.pos << "), ";
        return o.str();
    }
};

// ==================================================================
struct H3_PlayerType
// ==================================================================
{
    enum EType { CPU = 0, HUMAN };
};

// ==================================================================
struct H3_PlayerConnect
// ==================================================================
{
    enum EConnect { LOCAL = 0, TCP, UDP };
};

// ==================================================================
struct H3_CfgPlayer
// ==================================================================
{
    int typ = H3_PlayerType::CPU;
    int connect = H3_PlayerConnect::LOCAL;
    uint32_t color = 0xFFFF00FF;
    std::string name;
    u32 avatar = 0;

    std::string str() const
    {
        std::ostringstream o; o <<
        "name(" << name << "), "
        "typ(" << (typ == H3_PlayerType::CPU ? "CPU" : "HUMAN") << "), "
        "color(" << dbHex(color) << "), "
        "avatar(" << avatar << ")";
        return o.str();
    }
};

// ===========================================================================
struct H3_Cfg
// ===========================================================================
{
    std::string presetName;  //

    u32 victoryPoints;      // Num points to victory
    u32 cardLimitNoSteal;   // Num cards player can hold without robber steal. 0 = disable
    u32 buildingSpacing;    // Num of corners between neighboring buildings, default = 1, can be 0 for more dense use of board.
    // u32 numPlayers;         // 1+
    u32 useCardKnight;      // +1 Knight with this card
    u32 useCardVictoryPoint;// +1 Victory point with this card
    u32 useCardMonopol;     // Take all cards of one resource of all players
    u32 useCardInvention;   // +2..+4 Arbitrary resource cards from bank
    u32 skipFirstRound;
    u32 bonusRoadLimit;     // 0 = disabled, +2 points for atleast default:5 connected roads
    u32 bonusArmyLimit;     // 0 = disabled, +2 points for atleast default:3 knights

    std::vector< H3_CfgTile > tileList;
    std::vector< H3_CfgPlayer > playerList;

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
    H3_Bank() : A(0), B(0), C(0), D(0), E(0) {}
    H3_Bank( int a, int b, int c, int d, int e ) : A(a), B(b), C(c), D(d), E(e) {}
    H3_Bank( H3_Bank const & o ) : A(o.A), B(o.B), C(o.C), D(o.D), E(o.E) {}

    H3_Bank& operator= ( H3_Bank const & other )
    {
        A = other.A;
        B = other.B;
        C = other.C;
        D = other.D;
        E = other.E;
        return *this;
    }

    H3_TileType getTileTypeFromCardIndex( int index ) const
    {
        if ( index < 0 || index >= sum())
        {
            throw std::runtime_error( dbStr("Invalid card index ", index, " of sum ", sum()));
        }
        if (index >= A + B + C + D) { return H3_TileType::TileE; }
        if (index >= A + B + C) { return H3_TileType::TileD; }
        if (index >= A + B) { return H3_TileType::TileC; }
        if (index >= A) { return H3_TileType::TileB; }
        return H3_TileType::TileA;
    }

    void clear() { A = B = C = D = E = 0; }
    int sum() const { return A + B + C + D + E; }

    static H3_Bank costOfRoad() { H3_Bank b; b.A = 1; b.B = 1; return b; }
    static H3_Bank costOfFarm() { H3_Bank b; b.A = 1; b.B = 1; b.C = 1; b.D = 1; return b; }
    static H3_Bank costOfCity() { H3_Bank b; b.C = 2; b.E = 3; return b; }
    static H3_Bank costOfCard() { H3_Bank b; b.C = 1; b.D = 1; b.E = 1; return b; }
    bool canBuyRoad() const { return canBuy( costOfRoad() ); }
    bool canBuyFarm() const { return canBuy( costOfFarm() ); }
    bool canBuyCity() const { return canBuy( costOfCity() ); }
    bool canBuyCard() const { return canBuy( costOfCard() ); }
    bool canBuy( H3_Bank const & o ) const
    {
        return (o.A <= A)
            && (o.B <= B)
            && (o.C <= C)
            && (o.D <= D)
            && (o.E <= E);
    }
    bool buy( H3_Bank const & price )
    {
        if ( canBuy( price ) )
        { (*this) -= price; return true; }
        else
        { return false; }
    }
    bool buyRoad() { return buy( costOfRoad() ); }
    bool buyFarm() { return buy( costOfFarm() ); }
    bool buyCity() { return buy( costOfCity() ); }
    bool buyCard() { return buy( costOfCard() ); }

    // [<=]
    int32_t
    getAmount( const H3_TileType tt ) const
    {
        switch ( tt.value )
        {
            case H3_TileType::TileA: return A;
            case H3_TileType::TileB: return B;
            case H3_TileType::TileC: return C;
            case H3_TileType::TileD: return D;
            case H3_TileType::TileE: return E;
            default: return 0;
        }
    }

    // [=>]
    void
    resetAmount( const H3_TileType tt, const int32_t value )
    {
        switch ( tt.value )
        {
            case H3_TileType::TileA: A = std::clamp( value, 0, 16000 ); break;
            case H3_TileType::TileB: B = std::clamp( value, 0, 16000 ); break;
            case H3_TileType::TileC: C = std::clamp( value, 0, 16000 ); break;
            case H3_TileType::TileD: D = std::clamp( value, 0, 16000 ); break;
            case H3_TileType::TileE: E = std::clamp( value, 0, 16000 ); break;
            default: break;
        }
    }

    // [+] Deposit to current amount...
    void
    addAmount( const H3_TileType tt, const int value )
    {
        //DE_DEBUG( "tt(",TileType::getString(tt),"), value(",value,")")
        resetAmount( tt, getAmount( tt ) + value );
    }

    // [-] Withdraw from current amount...
    void
    subAmount( const H3_TileType tt, const int value )
    {
        const int amount = getAmount( tt );
        if (value > amount)
        {
            auto s = dbStr("Cannot subAmount(", tt.str(), ",", value, ") from bank ", str());
            throw std::runtime_error(s);
        }
        resetAmount( tt, amount - value );
    }

    H3_Bank &
    add( const H3_Bank & o )
    {
        A += o.A; B += o.B; C += o.C; D += o.D; E += o.E; return *this;
    }

    H3_Bank
    sub( H3_Bank const & o ) const
    {
        H3_Bank n = *this;
        if (n.A - o.A < 0)
        {
            DE_ERROR("Negative card amount A")
            throw std::runtime_error("Negative card amount A");
        }
        if (n.B - o.B < 0)
        {
            DE_ERROR("Negative card amount B")
            throw std::runtime_error("Negative card amount B");
        }
        if (n.C - o.C < 0)
        {
            DE_ERROR("Negative card amount C")
            throw std::runtime_error("Negative card amount C");
        }
        if (n.D - o.D < 0)
        {
            DE_ERROR("Negative card amount D")
            throw std::runtime_error("Negative card amount D");
        }
        if (n.E - o.E < 0)
        {
            DE_ERROR("Negative card amount E")
            throw std::runtime_error("Negative card amount E");
        }
        n.A -= o.A;
        n.B -= o.B;
        n.C -= o.C;
        n.D -= o.D;
        n.E -= o.E;
        return n;
    }

    std::string
    str() const
    {
        std::stringstream o;
        o << "sum:" << sum()
          << "|A:" << A << "|B:" << B <<
             "|C:" << C << "|D:" << D << "|E:" << E;
        return o.str();
    }

    H3_Bank& operator-= ( H3_Bank const & o )
    {
        if (A - o.A < 0)
        {
            DE_ERROR("Negative card amount A")
            throw std::runtime_error("Negative card amount A");
        }
        if (B - o.B < 0)
        {
            DE_ERROR("Negative card amount B")
            throw std::runtime_error("Negative card amount B");
        }
        if (C - o.C < 0)
        {
            DE_ERROR("Negative card amount C")
            throw std::runtime_error("Negative card amount C");
        }
        if (D - o.D < 0)
        {
            DE_ERROR("Negative card amount D")
            throw std::runtime_error("Negative card amount D");
        }
        if (E - o.E < 0)
        {
            DE_ERROR("Negative card amount E")
            throw std::runtime_error("Negative card amount E");
        }
        A -= o.A;
        B -= o.B;
        C -= o.C;
        D -= o.D;
        E -= o.E;
        return *this;
    }

    H3_Bank operator- ( H3_Bank const & o ) const
    {
        return sub(o);
    }
    H3_Bank& operator+= ( H3_Bank const & o )
    {
        A += o.A;
        B += o.B;
        C += o.C;
        D += o.D;
        E += o.E;
        return *this;
    }
    H3_Bank operator+ ( H3_Bank const & o ) const
    {
        return H3_Bank( *this ) += o;
    }
};

// =====================================================================
struct H3_Dice
// =====================================================================
{
    int a;
    int b;

    H3_Dice() : a(0), b(0) {}
    void clear() { a = 0; b = 0; }
    int sum() const { return a + b; }
    int max() const { return std::max( a, b ); }
    int min() const { return std::min( a, b ); }

    std::string
    toString() const
    {
        std::ostringstream o;
        o << int(a+b) << " = " << int(a) << " + " << int(b);
        return o.str();
    }
};

// =====================================================================
struct H3_Util
// =====================================================================
{
    static bool
    push_back_unique( std::vector<uint32_t> & ids, uint32_t id)
    {
        if ( !id ) return false;   // Dont add invalid id

        // Is unique?
        for ( const u32 cached : ids )
        {
            if ( cached == id ) { return false; } // found
        }

        // Is unique!
        ids.emplace_back( id );
        return true;
    }
};

// =====================================================================
struct H3_BonusArmy
// =====================================================================
{
    u32 owner = 0;   // player id
    u32 knights = 0; // number of knights (Default: min. 3)
};

// =====================================================================
struct H3_BonusRoad
// =====================================================================
{
    u32 owner = 0; // player id
    u32 roads = 0; // number of connected roads. (Default: min. 6)
};

// =====================================================================
struct H3_Corner
// =====================================================================
{
    u32 id = 0;

    u32 owner = 0; // player id

    u32 building_type = 0; // default: none = 0, farm = 1, city = 2, bespin sky city = 3

    bool visible = true;

    bool enabled = true; // not disabled by thief, can generate rescards

    de::TRSf trs;    // Computed 3d model space transform

    de::Box3f collisionBoundingBox;

    std::vector<de::Triangle3f> collisionTriangles;

    std::vector< u32 > tiles; // Max 3 neighbouring tiles.
    std::vector< u32 > edges; // Max 3 neighbouring road edges
    std::vector< u32 > next; // Max 3 next corners

    H3_Corner() {}

    // Connect corner with max 3 corners
    // One corner per call. Corner must be unique.
    // Returns num of added corners. 1 = success, 0 = did nothing/already there.
    bool addCornerId( const u32 cornerId )
    {
        if ( cornerId == id ) return false; // Dont add yourself

        return H3_Util::push_back_unique( next, cornerId );
    }

    // Connect corner with max 3 tiles
    bool addTileId( const u32 tileId )
    {
        return H3_Util::push_back_unique( tiles, tileId );
    }

    // Connect corner with max. 3 edges
    bool addEdgeId( const u32 edgeId )
    {
        return H3_Util::push_back_unique( edges, edgeId );
    }
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

    H3_Edge() {}

    // Connect edge with max 4 next edges
    bool addEdgeId( const u32 edgeId )
    {
        if ( edgeId == id ) return false; // Dont add yourself.

        return H3_Util::push_back_unique( next, edgeId );
    }

    // Connect edge with max. 2 tiles
    bool addTileId( const u32 tileId )
    {
        return H3_Util::push_back_unique( tiles, tileId );
    }

    // Connect edge with max 2 corners
    bool addCornerId( const u32 cornerId )
    {
        return H3_Util::push_back_unique( corners, cornerId );
    }

    std::string str() const
    {
        std::ostringstream o;
        o << "id:" << id << "; "
            "vis:" << visible << "; owner:" << owner << "; "
            "next:" << next.size() << "; "
            "tiles:" << tiles.size() << "; "
            "corners:" << corners.size() << "; ";
        return o.str();
    }
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

    bool hasRobber = false;

    // bool bRobbed = false; // not disabled by thief, can generate rescards

    // int building_type = 0; // default: none = 0, farm = 1, city = 2, bespin sky city = 3

    glm::vec3 pos = glm::vec3(0,0,0);
    //H3_TRS trs;    // Computed 3d model space transform

    de::TRSf trsChip;

    de::Box3f collisionBoundingBox;

    std::vector<de::Triangle3f> collisionTriangles;

    std::vector< u32 > next;    // Topology - max. 6 Neighbours
    std::vector< u32 > corners; // Topology - max. 6 Corners
    std::vector< u32 > edges;   // Topology - max. 6 Edges
    //std::vector< u32 > owners;  // Logic - max. 3-6 owners

    H3_Tile() {}

    //const glm::vec3& pos() const { return trs.pos; }

    bool isWaterOrPort() const { return tileType.isWaterOrPort(); }

    std::string str() const
    {
        std::stringstream o; o <<
        "tileType(" << tileType.str() << "), "
        "chipValue(" << chipValue << "), "
        "i(" << i << "), "
        "j(" << j << "), "
        "angle60("<< angle60 << "), "
        "pos("<< pos << ")";

        // "owners(";
        // for ( size_t i = 0; i < owners.size(); ++i )
        // {
        // if ( i > 0 ) s << ",";
        // s << owners[ i ];
        // }
        // s << ")";
        return o.str();
    }

    int
    getCornerIndex( glm::vec3 const & tileSize, H3_Corner & corner ) const
    {
        for ( int i = 0; i < 6; ++i )
        {
            auto relPos = de::smesh::SMeshHexagon::getCorner( i, tileSize.x, tileSize.z );
            auto cornerPos = pos + glm::vec3( relPos.x, 0.0f, relPos.y );
            if ( dbEquals( cornerPos, corner.trs.pos ) )
            {
                return i;
            }
        }
        return -1;
    }

    // Connect tiles with max. 6 neighbor tiles
    bool addTileId( const u32 tileId )
    {
        if (tileId == id) return false; // Don't add self.

        return H3_Util::push_back_unique( next, tileId );
    }

    // Connect tile with max. 6 unique edges
    bool addEdgeId( const u32 edgeId )
    {
        return H3_Util::push_back_unique( edges, edgeId );
    }

    // Connect tile with max. 6 unique corners
    bool addCornerId( const u32 cornerId )
    {
        return H3_Util::push_back_unique( corners, cornerId );
    }
};

// =====================================================================
struct H3_Thief
// =====================================================================
{
    u32 id = 0;

    u32 owner = 0; // tileId

    de::TRSf trs;    // Computed 3d model space transform

    std::vector<de::Triangle3f> triangles; // Cylinder

    H3_Thief() {}
};


// =====================================================================
struct H3_Card
// =====================================================================
{
    u32 id = 0; // cardId

    u32 owner = 0; // playerId

    u32 typ = 0; // 0 = Ritter, 1 = Invention, 2 = Monopol

    de::TRSf trs;     // Gfx 3d model space transform

    H3_Card() {}
};

// =====================================================================
struct H3_Road
// =====================================================================
{
    u32 id = 0; // roadId

    u32 owner = 0; // playerId

    u32 edgeId = 0; // edgeId

    u32 color = 0xFFFFFFFF; // Gfx player color, reduces some CPU time.

    de::TRSf trs;     // Gfx 3d model space transform

    std::vector<de::Triangle3f> collisionTriangles;

    H3_Road() {}
};

// =====================================================================
struct H3_Farm
// =====================================================================
{
    u32 id = 0; // farmId

    u32 owner = 0; // playerId, who owns this farm.

    u32 cornerId = 0; // cornerId, where this farm stands on.

    u32 color = 0xFFFFFFFF; // Gfx player color, reduces some CPU time.

    de::TRSf trs; // Gfx 3d model space transform

    std::vector<de::Triangle3f> collisionTriangles;

    H3_Farm() {}
};

// =====================================================================
struct H3_City
// =====================================================================
{
    u32 id = 0; // cityId

    u32 owner = 0; // playerId that owns this farm.

    u32 cornerId = 0; // cornerId, where this farm stands on.

    u32 color = 0xFFFFFFFF; // Gfx player color, reduces some CPU time.

    de::TRSf trs; // Gfx 3d model space transform

    std::vector<de::Triangle3f> collisionTriangles; // Clickable to steal cards from sel player.

    H3_City() {}
};

// =====================================================================
struct H3_RankedPlayer
// =====================================================================
{
   u32 playerId = 0;
   u32 diceSum = 0; // diceValue or victoryPoints
   u32 rank = 0;
};

// =====================================================================
struct H3_Player
// =====================================================================
{
    H3_Player() {}

    u32 id = 0;
    u32 typ = H3_PlayerType::CPU;
    u32 connect = H3_PlayerConnect::LOCAL;
    u32 color = 0xFFFFFFFF;
    u32 avatar = 0; // (H3_Tex::eID) = PlayU....PlayF
    std::string name;

    int ai_strategy; // Determines AI player behaviour
    int ai_handicap; // Determines AI player stupidity
    int ai_cheating; // Determines AI player advantages

    int rank_start; // Round 1
    int rank_dice;  // Round 2,3

    enum eFlags : u32
    {
        eFlagPort3v1    = 1,
        eFlagPortA      = 1 << 1,
        eFlagPortB      = 1 << 2,
        eFlagPortC      = 1 << 3,
        eFlagPortD      = 1 << 4,
        eFlagPortE      = 1 << 5,
        //eFlagDidDice  = 1 << 6,
        eFlagBonusArmy  = 1 << 7,
        eFlagBonusRoad  = 1 << 8,
    };

    struct State
    {
        u32 flags;
        H3_Bank bank;
        u32 victoryPoints;
        int rank;

        H3_Dice dice;
        std::vector< u32 > newRoads;
        std::vector< u32 > newFarms;
        std::vector< u32 > newCitys;
        std::vector< u32 > newCards;
        std::vector< u32 > newPointCards;  // Bought in this round, not revealed = not counted in victoryPoints!
        std::vector< u32 > newKnightCards; // Bought, not revealed.
        std::vector< u32 > newInventCards;     // Bought, not revealed.
        std::vector< u32 > newMonopolCards;    // Bought, not revealed.
        std::vector< u32 > newBankings;
        std::vector< u32 > newTrades;

        std::vector< u32 > tiles;       // tile-ids
        std::vector< u32 > corners;     // corner-ids
        std::vector< u32 > edges;       // edge-ids
        std::vector< u32 > roads;       // road-ids
        std::vector< u32 > farms;       // farm-ids
        std::vector< u32 > citys;       // city-ids
        std::vector< u32 > ships;       // ship-ids
        std::vector< u32 > points;      // Revealed = now counts into victoryPoints!
        std::vector< u32 > knights;     // Revealed = now counts into bonus!
        std::vector< u32 > invents;     // Revealed (aka not a card anymore)
        std::vector< u32 > monopols;    // Revealed (aka not a card anymore)

        std::vector< u32 > pointCards;      // Bought in previous rounds, not revealed = not counted in victoryPoints!
        std::vector< u32 > knightCards;     // Bought, not revealed.
        std::vector< u32 > inventCards;     // Bought, not revealed.
        std::vector< u32 > monopolCards;    // Bought, not revealed.

        void endTurn()
        {
            dice.clear();       // Needed for every round.
            newRoads.clear();
            newFarms.clear();
            newCitys.clear();
            newCards.clear();       // New resource cards
            newBankings.clear();
            newTrades.clear();

            // Move new event cards to stash of unrevealed event cards.
            pointCards.insert(pointCards.end(), newPointCards.begin(), newPointCards.end());
            knightCards.insert(knightCards.end(), newKnightCards.begin(), newKnightCards.end());
            inventCards.insert(inventCards.end(), newInventCards.begin(), newInventCards.end());
            monopolCards.insert(monopolCards.end(), newMonopolCards.begin(), newMonopolCards.end());

            // Clear stash of unrevealed event cards.
            newPointCards.clear();
            newKnightCards.clear();
            newInventCards.clear();
            newMonopolCards.clear();
        }

        void clear()
        {
            endTurn();

            flags = 0;
            bank.clear();
            rank = 0;
            victoryPoints = 0; // = 1 per farm + 2 per city + 1 per pointCard

            tiles.clear();
            edges.clear();
            corners.clear();
            roads.clear();
            farms.clear();
            citys.clear();
            ships.clear();
            pointCards.clear();
            knightCards.clear();
            inventCards.clear();
            monopolCards.clear();
        }


        bool hasPort3v1() const { return flags & eFlagPort3v1; }
        bool hasPortA() const { return flags & eFlagPortA; }
        bool hasPortB() const { return flags & eFlagPortB; }
        bool hasPortC() const { return flags & eFlagPortC; }
        bool hasPortD() const { return flags & eFlagPortD; }
        bool hasPortE() const { return flags & eFlagPortE; }

        bool canBanking() const
        {
            if (bank.A >= 4) return true;
            if (bank.B >= 4) return true;
            if (bank.C >= 4) return true;
            if (bank.D >= 4) return true;
            if (bank.E >= 4) return true;

            if (hasPort3v1())
            {
                if (bank.A >= 3) return true;
                if (bank.B >= 3) return true;
                if (bank.C >= 3) return true;
                if (bank.D >= 3) return true;
                if (bank.E >= 3) return true;
            }

            if (hasPortA()) { if (bank.A >= 2) return true; }
            if (hasPortB()) { if (bank.B >= 2) return true; }
            if (hasPortC()) { if (bank.C >= 2) return true; }
            if (hasPortD()) { if (bank.D >= 2) return true; }
            if (hasPortE()) { if (bank.E >= 2) return true; }

            return false;
        }

        int unrevealedPointCards() const
        {
            return newPointCards.size() + pointCards.size();
        }

        int unrevealedKnightCards() const
        {
            return newKnightCards.size() + knightCards.size();
        }

        int unrevealedInventCards() const
        {
            return newInventCards.size() + inventCards.size();
        }

        int unrevealedMonopolCards() const
        {
            return newMonopolCards.size() + monopolCards.size();
        }

        bool canTrading() const
        {
            if (bank.sum() > 0) return true;
            if (unrevealedPointCards() > 0) return true;
            if (unrevealedKnightCards() > 0) return true;
            if (unrevealedInventCards() > 0) return true;
            if (unrevealedMonopolCards() > 0) return true;
            return false;
        }

        bool hasBonusArmy() const { return flags & eFlagBonusArmy; }
        bool hasBonusRoad() const { return flags & eFlagBonusRoad; }

        u32 computeVictoryPoints() const
        {
            size_t n = 0;
            n += farms.size();
            n += citys.size() * 2;
            n += hasBonusArmy() ? 2 : 0;
            n += hasBonusRoad() ? 2 : 0;
            n += points.size();
            return static_cast<u32>(n);
        }
    };

    // Current state
    State state;


    // Statistics
    std::vector< State > history;

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

    void updateVictoryPoints()
    {
        u32 a = state.computeVictoryPoints();
        u32 b = state.victoryPoints;
        if (a == b) return;
        DE_DEBUG("Update player[",id,"] ",name," victoryPoints from ",a, " to ", b)
        state.victoryPoints = a;
    }

    void setBonusArmy(bool value)
    {
        u32 flags = state.flags;
        if (value) { flags |= eFlagBonusArmy; }
        else       { flags &= ~eFlagBonusArmy; }
        if (flags == state.flags) { return; }
        state.flags = flags;
        DE_DEBUG("Update player[",id,"] ",name," BonusArmy = ",value," flags = ",dbHex(state.flags))
    }

    void setBonusRoad(bool value)
    {
        u32 flags = state.flags;
        if (value) { flags |= eFlagBonusRoad; }
        else       { flags &= ~eFlagBonusRoad; }
        if (flags == state.flags) { return; }
        state.flags = flags;
        DE_DEBUG("Update player[",id,"] ",name," BonusRoad = ",value," flags = ",dbHex(state.flags))
    }

    int getBankingRatio(int tt) const
    {
        if ( tt == 0 && state.hasPortA() ) { return 2; }
        if ( tt == 1 && state.hasPortB() ) { return 2; }
        if ( tt == 2 && state.hasPortC() ) { return 2; }
        if ( tt == 3 && state.hasPortD() ) { return 2; }
        if ( tt == 4 && state.hasPortE() ) { return 2; }
        if ( state.hasPort3v1() ) return 3;
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

/*
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
