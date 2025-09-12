#pragma once
#include <cstdint>
#include <string>
//#include <unordered_map>
//#include <deque>
//#include <array>
//#include <optional>
#include <de/IrrlichtDevice.h>
#include <de/gui/Env.h>

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
struct H3_UIElem;
struct H3_UI;
struct H3_Action;
struct H3_ActionLog;
struct H3_Game;

namespace de::gui
{
    class Widget;
    class Button;
    class Label;
    class IntField;
}

// =====================================================================
struct LineBoxAnim
// =====================================================================
{
    float ang_y = 45.0;
    float ang_x = 14.0;

    void
    draw( de::gpu::VideoDriver* driver, de::gpu::SMaterialRenderer* renderer )
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

        de::gpu::SMeshBuffer m( de::gpu::PrimitiveType::Lines );

        de::gpu::SMeshBox::addLines(m, glm::vec3(20), glm::vec3(-10), 0xFF1060FF );
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

    std::string toString() const
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
};

// ===========================================================================
struct H3_Cfg
// ===========================================================================
{
    std::string presetName;  //

    u32 victoryPoints;      // Num points to victory
    u32 cardLimitNoSteal;   // Num cards player can hold without robber steal. 0 = disable
    u32 buildingSpacing;    // Num of corners between neighboring buildings, default = 1, can be 0 for more dense use of board.
    u32 useBonusRoad;       // +2 points for 5+ connected roads
    u32 useBonusArmy;       // +2 points for 3+ knights
    u32 useCardKnight;      // +1 Knight with this card
    u32 useCardVictoryPoint;// +1 Victory point with this card
    u32 useCardMonopol;     // Take all cards of one resource of all players
    u32 useCardInvention;   // +2..+4 Arbitrary resource cards from bank

    std::vector< H3_CfgTile > tileList;
    std::vector< H3_CfgPlayer > playerList;

    // std::vector< H3_Cost > cost4card;   // Standard
    // std::vector< H3_Cost > cost4road;   // Standard
    // std::vector< H3_Cost > cost4farm;   // Standard
    // std::vector< H3_Cost > cost4city;   // Standard

    static H3_Cfg
    createStandard();

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
    void reset() { A = B = C = D = E = 0; }
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
        A -= o.A;
        B -= o.B;
        C -= o.C;
        D -= o.D;
        E -= o.E;
        return *this;
    }
    H3_Bank operator- ( H3_Bank const & o ) const
    {
        return H3_Bank( *this ) -= o;
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
    void reset() { a = 0; b = 0; }
    void dice() { a = rand()%6 + 1; b = rand()%6 + 1; }
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
            auto relPos = de::gpu::SMeshHexagon::getCorner( i, tileSize.x, tileSize.z );
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
    u32 avatar = 0; // H3_Tex::eID = PlayU....PlayF
    std::string name;

    int ai_strategy; // Determines AI player behaviour
    int ai_handicap; // Determines AI player stupidity
    int ai_cheating; // Determines AI player advantages

    int didDice = 0; // Reset every round, indicates if player did dice every round, determines if other actions are doable now
    int didBuyRoad = 0;
    int didBuyFarm = 0;
    int didBuyCity = 0;
    int didBuyCard = 0;

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

    // State
    struct State
    {
        u32 flags;
        H3_Dice dice;
        H3_Bank bank;
        u32 victoryPoints;
        int rank;

        std::vector< u32 > tiles;       // tile-ids
        std::vector< u32 > corners;     // corner-ids
        std::vector< u32 > edges;       // edge-ids
        std::vector< u32 > roads;       // road-ids
        std::vector< u32 > farms;       // farm-ids
        std::vector< u32 > citys;       // city-ids
        std::vector< u32 > ships;       // ship-ids

        std::vector< u32 > newPointCards;  // Bought, not revealed.
        std::vector< u32 > newKnightCards; // Bought, not revealed.
        std::vector< u32 > newInventCards; // Bought, not revealed.
        std::vector< u32 > newMonoplCards; // Bought, not revealed.

        std::vector< u32 > oldPointCards;  // Used/revealed.
        std::vector< u32 > oldKnightCards; // Used/revealed.
        std::vector< u32 > oldInventCards; // Used/revealed.
        std::vector< u32 > oldMonoplCards; // Used/revealed.

        void reset()
        {
            flags = 0;
            rank = 0;
            victoryPoints = 0;
            bank.clear();
            dice.reset();
            tiles.clear();
            edges.clear();
            corners.clear();
            roads.clear();
            farms.clear();
            citys.clear();
            ships.clear();
            newPointCards.clear();
            newKnightCards.clear();
            newInventCards.clear();
            newMonoplCards.clear();
            oldPointCards.clear();
            oldKnightCards.clear();
            oldInventCards.clear();
            oldMonoplCards.clear();
        }
    };

    // State
    State state;    

    // Statistics
    std::vector< State > history;

    void endTurn()
    {
        didDice = 0;    // Needed for every round.
        didBuyRoad = 0; // Needed for AI + round 2+3.
        didBuyFarm = 0; // Needed for AI + round 2+3.
        didBuyCity = 0; // Needed for AI
        didBuyCard = 0; // Needed for AI
    }

    void reset()
    {
        endTurn();
        rank_start = 0;
        rank_dice = 0;
        state.reset();
        history.clear();
    }

    bool hasPort3v1() const { return state.flags & eFlagPort3v1; }
    bool hasPortA() const { return state.flags & eFlagPortA; }
    bool hasPortB() const { return state.flags & eFlagPortB; }
    bool hasPortC() const { return state.flags & eFlagPortC; }
    bool hasPortD() const { return state.flags & eFlagPortD; }
    bool hasPortE() const { return state.flags & eFlagPortE; }

    bool hasBonusArmy() const { return state.flags & eFlagBonusArmy; }
    bool hasBonusRoad() const { return state.flags & eFlagBonusRoad; }

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
