#include "H3_Board.h"
#include <H3/details/H3_Tex.h>

// =====================================================================
void LineBoxAnim::draw( de::gpu::VideoDriver* driver )
// =====================================================================
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

    de::smesh::SMaterialRenderer* renderer = driver->getSMaterialRenderer();
    renderer->setMaterial( m.material );
    m.draw();
    m.destroy();
    //driver->draw3D( lines );
    //driver->draw3DLineBox( de::gpu::Box3f( -10,-10,-10,10,10,10), 0xFF1060FF );
    ang_y += 5.0f;
    ang_x += 1.0f;

    camera->lookAt( old_pos, old_lat, old_up );
}

// =====================================================================
// struct H3_Util
// =====================================================================
// static
bool
H3_Util::push_back_unique( std::vector<uint32_t> & ids, uint32_t id)
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

// static
std::string
H3_Util::str( const std::vector<uint32_t> & v)
{
    std::ostringstream o;
    o << "[" << v.size() << "]{";

    for (size_t i = 0; i < v.size(); ++i)
    {
        if (i>0) o << ",";
        o << v[i];
    }
    o << "}";
    return o.str();
}

// =====================================================================
//struct H3_GuiState // Actually just H3_UIGameState
// =====================================================================
// bool bWorldCreator = false;
// bool bGameOverlay = false;    // Actions + stats + logger + dice
// bool bDonateCards = false;
// bool bBanking = false;
// bool bWinnerScreen = false;

std::string H3_GuiState::str() const
{
    std::ostringstream o; o <<
    "bWorldCreator(" << bWorldCreator << "), "
    "bGameOverlay(" << bGameOverlay << "), "
    "bDonateCards(" << bDonateCards << "), "
    "bBanking(" << bBanking << "), "
    "bWinnerScreen(" << bWinnerScreen << "), ";
    return o.str();
}

// ===========================================================================
//struct H3_State
// ===========================================================================

// static
std::string
H3_State::getString( eState state )
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

H3_State::H3_State()
    : m_state(Idle)
{}

H3_State::H3_State(eState state)
    : m_state(state)
{}

// operator eState () const { return m_state; }

std::string
H3_State::str() const
{
    return getString( m_state );
}

H3_State&
H3_State::operator= (eState state)
{
    m_state = state;
    return *this;
}

// =====================================================================
//struct H3_TileType
// =====================================================================

H3_TileType::H3_TileType()
    : value( Unknown )
{}
H3_TileType::H3_TileType( EType type )
    : value( type )
{}
H3_TileType::H3_TileType( uint32_t type )
    : value( static_cast< EType >( type ) )
{}
//operator EType () const { return m_Value; }
//operator int () const { return value; }
// void set( EType type ) { value = type; }
// EType get() const { return value; }

std::string
H3_TileType::str() const { return getString(value); }

// static
std::string
H3_TileType::getString( const H3_TileType tt )
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

// static
uint32_t
H3_TileType::getColor( const H3_TileType tt )
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

// static
bool
H3_TileType::isResource( const H3_TileType tt )
{
    return ( tt >= TileA && tt <= TileE );
}

bool
H3_TileType::hasChip() const { return isResource() | isPort(); }

bool
H3_TileType::isResource() const { return isResource(value); }

bool
H3_TileType::isWater() const { return value == Water; }

bool
H3_TileType::isWaterOrPort() const { return isWater() || isPort(); }

bool
H3_TileType::isPort() const { return isPort3v1() || isPort2v1(); }

bool
H3_TileType::isPort3v1() const { return value == Port3v1; }

bool
H3_TileType::isPort2v1() const
{
    return value == H3_TileType::PortA
        || value == H3_TileType::PortB
        || value == H3_TileType::PortC
        || value == H3_TileType::PortD
        || value == H3_TileType::PortE;
}

bool
H3_TileType::empty() const { return value == Unknown; }

uint32_t
H3_TileType::color() const { return getColor(value); }

std::string
H3_TileType::name() const { return getString(value); }

//static
H3_TileType
H3_TileType::parse( std::string const & txt )
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

// ===========================================================================
// struct H3_Cost
// ===========================================================================
// H3_TileType resource;
// uint8_t count;

H3_Cost::H3_Cost()
    : resource(H3_TileType::Unknown)
    , count(0)
{
}

H3_Cost::H3_Cost( H3_TileType rc, uint8_t n)
    : resource(rc)
    , count(n)
{
}

// =====================================================================
// struct H3_CfgTile
// =====================================================================
// H3_TileType tileType = H3_TileType::Unknown;
// int chipValue = 0;
// int i = 0;      // 2d board index x
// int j = 0;      // 2d board index y
// int angle60 = 0;// angle [0..5] to rotate tile in 60 degrees steps.
// //H3_TRS trs;   // computed 3d transform = func(i,j,angle)

std::string
H3_CfgTile::str() const
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


// =====================================================================
// struct H3_PlayerType
// =====================================================================

// static
std::string
H3_PlayerType::getString( eType typ )
{
    switch( typ )
    {
        case CPU: return "CPU";
        case HUMAN: return "HUMAN";
        default: return "Unknown";
    }
}

H3_PlayerType::H3_PlayerType()
    : m_type(CPU)
{}

H3_PlayerType::H3_PlayerType(eType typ)
    : m_type(typ)
{}

// operator eType () const { return m_type; }

std::string
H3_PlayerType::str() const
{
    return getString( m_type );
}

H3_PlayerType&
H3_PlayerType::operator= (eType typ)
{
    m_type = typ;
    return *this;
}

// =====================================================================
// struct H3_PlayerConnect
// =====================================================================

// static
std::string
H3_PlayerConnect::getString( eType typ )
{
    switch( typ )
    {
        case LOCAL: return "LOCAL";
        case TCP: return "TCP";
        case UDP: return "UDP";
        default: return "Unknown";
    }
}

H3_PlayerConnect::H3_PlayerConnect()
    : m_type(LOCAL)
{}

H3_PlayerConnect::H3_PlayerConnect(eType typ)
    : m_type(typ)
{}

// operator eType () const { return m_type; }

std::string
H3_PlayerConnect::str() const
{
    return getString( m_type );
}

H3_PlayerConnect&
H3_PlayerConnect::operator= (eType typ)
{
    m_type = typ;
    return *this;
}

// ==================================================================
//struct H3_CfgPlayer
// ==================================================================

H3_CfgPlayer::H3_CfgPlayer()
    : typ{ H3_PlayerType::CPU }
    , connect{ H3_PlayerConnect::LOCAL }
    , color{ 0xFFFF00FF }
    , avatar{ 0 }
{
}

std::string H3_CfgPlayer::str() const
{
    std::ostringstream o; o <<
    "name(" << name << "), "
    "typ(" << typ.str() << "), "
    "color(" << dbHex(color) << "), "
    "avatar(" << avatar << ")";
    return o.str();
}

// ==================================================================

void
H3_Cfg::dump() const
{
    DE_DEBUG("cfg.presetName = ",presetName)
    DE_DEBUG("cfg.victoryPoints = ",victoryPoints)
    DE_DEBUG("cfg.cardLimitNoSteal = ",cardLimitNoSteal)

    DE_DEBUG("cfg.buildingSpacing = ",buildingSpacing)
    DE_DEBUG("cfg.bonusRoadLimit = ",bonusRoadLimit)
    DE_DEBUG("cfg.bonusArmyLimit = ",bonusArmyLimit)

    DE_DEBUG("cfg.useCardKnight = ",useCardKnight)
    DE_DEBUG("cfg.useCardVictoryPoint = ",useCardVictoryPoint)
    DE_DEBUG("cfg.useCardMonopol = ",useCardMonopol)
    DE_DEBUG("cfg.useCardInvention = ",useCardInvention)
    DE_DEBUG("cfg.skipFirstRound = ",skipFirstRound)

    DE_DEBUG("cfg.playerList = ",playerList.size())

    for (size_t i= 0; i < playerList.size(); ++i)
    {
        DE_DEBUG("cfg.player[",i,"] ",playerList[i].str())
    }

    DE_DEBUG("cfg.tileList = ",tileList.size())

    for (size_t i= 0; i < tileList.size(); ++i)
    {
        DE_DEBUG("cfg.tile[",i,"] ",tileList[i].str())
    }
}

std::string
H3_Cfg::str() const
{
    std::ostringstream o;
    o << "PresetName = " << presetName << "";
    return o.str();
}

void
H3_Cfg::addTile( int i, int j, H3_TileType tileType, int angle60, int diceValue )
{
    H3_CfgTile cfg;
    cfg.i = i;
    cfg.j = j;
    cfg.tileType = tileType;
    cfg.chipValue = diceValue;
    cfg.angle60 = angle60;
    tileList.push_back( std::move( cfg ) );
}

// static
H3_Cfg
H3_Cfg::createStandard()
{
    H3_Cfg cfg;

    cfg.presetName = "Standard";

    cfg.victoryPoints = 3;      // Num points to victory (Default = 10)
    cfg.cardLimitNoSteal = 7;   // Num cards player can hold without robber steal. 0 = disable
    cfg.buildingSpacing = 1;    // Num of corners between neighboring buildings, default = 1, can be 0 for more dense use of board.
    cfg.skipFirstRound = 1;     //
    cfg.bonusRoadLimit = 5;     // +2 points for 5+ connected roads
    cfg.bonusArmyLimit = 3;     // +2 points for 3+ knights

    cfg.useCardKnight = 1;      // +1 Knight with this card
    cfg.useCardVictoryPoint = 1;// +1 Victory point with this card
    cfg.useCardMonopol = 1;     // Take all cards of one resource of all players
    cfg.useCardInvention = 1;   // +2..+4 Arbitrary resource cards from bank

    cfg.addTile( -2, 3, H3_TileType::PortE, 5 );
    cfg.addTile( -1, 3, H3_TileType::Water );
    cfg.addTile( -0, 3, H3_TileType::PortD );
    cfg.addTile(  1, 3, H3_TileType::Water );

    cfg.addTile( -2, 2, H3_TileType::Water, 5 );
    cfg.addTile( -1, 2, H3_TileType::TileC, 0, 4 );
    cfg.addTile( -0, 2, H3_TileType::TileA, 0, 6 );
    cfg.addTile(  1, 2, H3_TileType::TileC, 0, 9 );
    cfg.addTile(  2, 2, H3_TileType::Port3v1, 1 );

    cfg.addTile( -3, 1, H3_TileType::PortA, 4 );
    cfg.addTile( -2, 1, H3_TileType::TileB, 0, 2 );
    cfg.addTile( -1, 1, H3_TileType::TileA, 0, 5 );
    cfg.addTile( -0, 1, H3_TileType::TileD, 0, 12 );
    cfg.addTile(  1, 1, H3_TileType::TileD, 0, 4 );
    cfg.addTile(  2, 1, H3_TileType::Water, 1 );

    cfg.addTile( -3,  0, H3_TileType::Water, 4 );
    cfg.addTile( -2,  0, H3_TileType::TileD, 0, 9 );
    cfg.addTile( -1,  0, H3_TileType::TileB, 0, 8 );
    cfg.addTile( -0,  0, H3_TileType::Desert );
    cfg.addTile(  1,  0, H3_TileType::TileE, 0, 8 );
    cfg.addTile(  2,  0, H3_TileType::TileD, 0, 10 );
    cfg.addTile(  3,  0, H3_TileType::Port3v1, 1 );

    cfg.addTile( -3, -1, H3_TileType::Port3v1, 3 );
    cfg.addTile( -2, -1, H3_TileType::TileA, 0, 3 );
    cfg.addTile( -1, -1, H3_TileType::TileE, 0, 5 );
    cfg.addTile( -0, -1, H3_TileType::TileB, 0, 10 );
    cfg.addTile(  1, -1, H3_TileType::TileA, 0, 11 );
    cfg.addTile(  2, -1, H3_TileType::Water, 2 );

    cfg.addTile( -2,-2, H3_TileType::Water, 4 );
    cfg.addTile( -1,-2, H3_TileType::TileC, 0, 3 );
    cfg.addTile( -0,-2, H3_TileType::TileC, 0, 6 );
    cfg.addTile(  1,-2, H3_TileType::TileE, 0, 11 );
    cfg.addTile(  2,-2, H3_TileType::PortC, 1 );

    cfg.addTile( -2,-3, H3_TileType::PortB, 3 );
    cfg.addTile( -1,-3, H3_TileType::Water, 3 );
    cfg.addTile( -0,-3, H3_TileType::Port3v1, 3 );
    cfg.addTile(  1,-3, H3_TileType::Water, 2 );

    {
        H3_CfgPlayer player;
        player.name = "Benni";
        player.color = dbRGB(255,0,0);
        player.typ = H3_PlayerType::HUMAN;
        player.connect = H3_PlayerConnect::LOCAL;
        player.avatar = H3_Tex::Play3;
        cfg.playerList.push_back( player );
    }

    {
        H3_CfgPlayer player;
        player.name = "Kleopatra";
        player.color = dbRGB(255,255,0);
        player.typ = H3_PlayerType::CPU;
        player.connect = H3_PlayerConnect::LOCAL;
        player.avatar = H3_Tex::Play2;
        cfg.playerList.push_back( player );
    }

    // {
    //     H3_CfgPlayer player;
    //     player.name = "Alexander";
    //     player.color = dbRGB(0,255,0);
    //     player.typ = H3_PlayerType::CPU;
    //     player.connect = H3_PlayerConnect::LOCAL;
    //     player.avatar = H3_Tex::Play3;
    //     cfg.playerList.push_back( player );
    // }

    // {
    //     H3_CfgPlayer player;
    //     player.name = "Napoleon";
    //     player.color = dbRGB(0,0,255);
    //     player.typ = H3_PlayerType::CPU;
    //     player.connect = H3_PlayerConnect::LOCAL;
    //     player.avatar = H3_Tex::Play4;
    //     cfg.playerList.push_back( player );
    // }

    /*
    cfg.cost4road.push_back({ H3_TileType::TileA, 1});
    cfg.cost4road.push_back({ H3_TileType::TileB, 1});

    cfg.cost4farm.emplace_back({{ H3_TileType::TileA, 1},
                                { H3_TileType::TileB, 1},
                                { H3_TileType::TileC, 1},
                                { H3_TileType::TileD, 1} });

    cfg.cost4city.emplace_back({{ H3_TileType::TileD, 2},
                                { H3_TileType::TileE, 3} });

    cfg.cost4card.emplace_back({{ H3_TileType::TileC, 1},
                                { H3_TileType::TileD, 1},
                                { H3_TileType::TileE, 1} });
*/
    return cfg;
}









// static
H3_Bank
H3_Bank::costOfRoad()
{
    H3_Bank b;
    b.A = 1;
    b.B = 1;
    return b;
}
// static
H3_Bank
H3_Bank::costOfFarm()
{
    H3_Bank b;
    b.A = 1;
    b.B = 1;
    b.C = 1;
    b.D = 1;
    return b;
}

// static
H3_Bank
H3_Bank::costOfCity()
{
    H3_Bank b;
    b.C = 2;
    b.E = 3;
    return b;
}

// static
H3_Bank
H3_Bank::costOfCard()
{
    H3_Bank b;
    b.C = 1;
    b.D = 1;
    b.E = 1;
    return b;
}

// =====================================================================
H3_Bank::H3_Bank()
// =====================================================================
    : A(0), B(0), C(0), D(0), E(0)
{}
H3_Bank::H3_Bank( int a, int b, int c, int d, int e )
    : A(a), B(b), C(c), D(d), E(e)
{}
H3_Bank::H3_Bank( const H3_Bank & o )
    : A(o.A), B(o.B), C(o.C), D(o.D), E(o.E)
{}

H3_Bank& H3_Bank::operator= ( const H3_Bank & other )
{
    A = other.A;
    B = other.B;
    C = other.C;
    D = other.D;
    E = other.E;
    return *this;
}

// Used in stealRandomCard()
// The bank has sum() cards.
// Get TileType at index[ i ] in range { 0, sum()-1 }.
H3_TileType
H3_Bank::getTileTypeFromCardIndex( int index ) const
{
    // index out of bounds.
    if ( index < 0 || index >= sum())
    {
        throw std::runtime_error( dbStr("Invalid card index ", index, " of sum ", sum()));
    }
    // test beginning from end
    if (index >= A + B + C + D) { return H3_TileType::TileE; }
    if (index >= A + B + C) { return H3_TileType::TileD; }
    if (index >= A + B) { return H3_TileType::TileC; }
    if (index >= A) { return H3_TileType::TileB; }
    return H3_TileType::TileA;
}

void H3_Bank::clear()
{
    A = B = C = D = E = 0;
}

int H3_Bank::sum() const
{
    return A + B + C + D + E;
}

bool H3_Bank::canBuyRoad() const { return canBuy( costOfRoad() ); }
bool H3_Bank::canBuyFarm() const { return canBuy( costOfFarm() ); }
bool H3_Bank::canBuyCity() const { return canBuy( costOfCity() ); }
bool H3_Bank::canBuyCard() const { return canBuy( costOfCard() ); }

bool H3_Bank::canBuy( const H3_Bank & o ) const
{
    if ((A - o.A) < 0) return false;
    if ((B - o.B) < 0) return false;
    if ((C - o.C) < 0) return false;
    if ((D - o.D) < 0) return false;
    if ((E - o.E) < 0) return false;
    return true;
}

bool H3_Bank::buy( const H3_Bank & price )
{
    if ( !canBuy( price ) )
    {
        return false;
    }
    sub(price);
    return true;
}
bool H3_Bank::buyRoad() { return buy( costOfRoad() ); }
bool H3_Bank::buyFarm() { return buy( costOfFarm() ); }
bool H3_Bank::buyCity() { return buy( costOfCity() ); }
bool H3_Bank::buyCard() { return buy( costOfCard() ); }

// [<=]
int32_t
H3_Bank::getAmount( const H3_TileType tt ) const
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
H3_Bank::resetAmount( const H3_TileType tt, const int32_t value )
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
H3_Bank::addAmount( const H3_TileType tt, const int value )
{
    //DE_DEBUG( "tt(",TileType::getString(tt),"), value(",value,")")
    resetAmount( tt, getAmount( tt ) + value );
}

// [-] Withdraw from current amount...
void
H3_Bank::subAmount( const H3_TileType tt, const int value )
{
    const int amount = getAmount( tt );
    if (value > amount)
    {
        auto s = dbStr("Cannot subAmount(", tt.str(), ",", value, ") from bank ", str());
        throw std::runtime_error(s);
    }
    resetAmount( tt, amount - value );
}

std::string
H3_Bank::str() const
{
    std::stringstream o;
    o << "sum:" << sum()
      << "|A:" << A << "|B:" << B <<
         "|C:" << C << "|D:" << D << "|E:" << E;
    return o.str();
}

H3_Bank &
H3_Bank::add( const H3_Bank & o )
{
    A += o.A;
    B += o.B;
    C += o.C;
    D += o.D;
    E += o.E;
    return *this;
}

H3_Bank &
H3_Bank::sub( const H3_Bank & o )
{
    DE_DEBUG("Before(",str(), "), other(",o.str(),")")
    if (A - o.A < 0)
    {
        DE_ERROR("Negative card amount A, ")
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
    DE_DEBUG("After(",str(), ")")
    return *this;
}

H3_Bank H3_Bank::operator+ ( H3_Bank const & o ) const
{
    H3_Bank n = *this;
    n.add(o);
    return n;
}

H3_Bank H3_Bank::operator- ( H3_Bank const & o ) const
{
    H3_Bank n = *this;
    n.sub(o);
    return n;
}

H3_Bank& H3_Bank::operator+= ( H3_Bank const & o ) { return add(o); }

H3_Bank& H3_Bank::operator-= ( H3_Bank const & o ) { return sub(o); }

// TODO: Remove if not used anymore. (Was initially used in banking)
int H3_Bank::countTypes() const
{
    int n = 0;
    if (A > 0) n++;
    if (B > 0) n++;
    if (C > 0) n++;
    if (D > 0) n++;
    if (E > 0) n++;
    return n;
}


// =====================================================================
H3_Dice::H3_Dice()
// =====================================================================
    : a(0), b(0)
{}

void H3_Dice::clear() { a = 0; b = 0; }
int H3_Dice::sum() const { return a + b; }
int H3_Dice::max() const { return std::max( a, b ); }
int H3_Dice::min() const { return std::min( a, b ); }

std::string
H3_Dice::str() const
{
    std::ostringstream o;
    o << int(a+b) << " = " << int(a) << " + " << int(b);
    return o.str();
}

// =====================================================================
std::string H3_BonusArmy::str() const
// =====================================================================
{
    std::ostringstream o;
    o << "owner(" << owner << "), knights(" << knights << ")";
    return o.str();
}

// =====================================================================
std::string H3_BonusRoad::str() const
// =====================================================================
{
    std::ostringstream o;
    o << "owner(" << owner << "), roads(" << roads << ")";
    return o.str();
}


// =====================================================================
// struct H3_Corner
// =====================================================================

// u32 id = 0;
// u32 owner = 0; // player id
// u32 building_type = 0; // default: none = 0, farm = 1, city = 2, bespin sky city = 3
// bool visible = true;
// de::TRSf trs;    // Computed 3d model space transform
// de::Box3f collisionBoundingBox;
// std::vector<de::Triangle3f> collisionTriangles;
// std::vector< u32 > tiles; // Max 3 neighbouring tiles.
// std::vector< u32 > edges; // Max 3 neighbouring road edges
// std::vector< u32 > next; // Max 3 next corners

void H3_Corner::dump() const
{
    // DE_DEBUG("id(" << id << "; "
    //     "vis:" << visible << "; owner:" << owner << "; "
    //     "next:" << next.size() << "; "
    //     "tiles:" << tiles.size() << "; "
    //     "corners:" << corners.size() << "; ";
    // return o.str();
}

std::string H3_Corner::str() const
{
    std::ostringstream o; o <<
    "id(" << id << "), "
    "owner(" << owner << "), "
    "building_type(" << building_type << "), "
    "visible(" << visible << "), "

    "tiles" << H3_Util::str(tiles) << ", "
    "edges" << H3_Util::str(edges) << ", "
    "next" << H3_Util::str(next) << ", "

    "trs(" << trs.str() << ")";
    return o.str();
}

H3_Corner::H3_Corner() {}

// Connect corner with max 3 corners
// One corner per call. Corner must be unique.
// Returns num of added corners. 1 = success, 0 = did nothing/already there.
bool H3_Corner::addCornerId( const u32 cornerId )
{
    if ( cornerId == id ) return false; // Dont add yourself

    return H3_Util::push_back_unique( next, cornerId );
}

// Connect corner with max 3 tiles
bool H3_Corner::addTileId( const u32 tileId )
{
    return H3_Util::push_back_unique( tiles, tileId );
}

// Connect corner with max. 3 edges
bool H3_Corner::addEdgeId( const u32 edgeId )
{
    return H3_Util::push_back_unique( edges, edgeId );
}



// =====================================================================
//struct H3_Edge
// =====================================================================

// u32 id = 0;
// u32 owner = 0; // playerId, has road if != 0
// bool visible = true;

// de::TRSf trs;    // Computed 3d model space transform
// de::Box3f collisionBoundingBox;
// std::vector<de::Triangle3f> collisionTriangles;

// std::vector< u32 > tiles; // Max 3 neighbor tiles
// std::vector< u32 > corners; // Max 3 neighbouring village corners
// std::vector< u32 > next; // Max 4 next edges

std::string H3_Edge::str() const
{
    std::ostringstream o; o <<
    "id(" << id << "), "
    "owner(" << owner << "), "
    "visible(" << visible << "), "

    "tiles" << H3_Util::str(tiles) << ", "
    "corners" << H3_Util::str(corners) << ", "
    "edges" << H3_Util::str(next) << ", "

    "trs(" << trs.str() << ")";
    return o.str();
}

H3_Edge::H3_Edge() {}

// Connect edge with max 4 next edges
bool H3_Edge::addEdgeId( const u32 edgeId )
{
    if ( edgeId == id ) return false; // Dont add yourself.

    return H3_Util::push_back_unique( next, edgeId );
}

// Connect edge with max. 2 tiles
bool H3_Edge::addTileId( const u32 tileId )
{
    return H3_Util::push_back_unique( tiles, tileId );
}

// Connect edge with max 2 corners
bool H3_Edge::addCornerId( const u32 cornerId )
{
    return H3_Util::push_back_unique( corners, cornerId );
}


// =====================================================================
//struct H3_Tile
// =====================================================================

std::string
H3_Tile::str() const
{
    std::stringstream o; o <<
    "id(" << id << "), "
    "chipValue(" << chipValue << "), "
    "i(" << i << "), "
    "j(" << j << "), "
    "angle60("<< angle60 << "), "
    "tileType(" << tileType.str() << "), "
    "visible("<< visible << "), "
    "pos("<< pos << "), "

    "tiles" << H3_Util::str(next) << ", "
    "corners" << H3_Util::str(corners) << ", "
    "edges" << H3_Util::str(edges) << ", "

    "trsChip(" << trsChip.str() << ")";

    return o.str();
}

H3_Tile::H3_Tile() {}

bool
H3_Tile::isWaterOrPort() const { return tileType.isWaterOrPort(); }

int
H3_Tile::getCornerIndex( glm::vec3 const & tileSize, H3_Corner & corner ) const
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
bool
H3_Tile::addTileId( const u32 tileId )
{
    if (tileId == id) return false; // Don't add self.

    return H3_Util::push_back_unique( next, tileId );
}

// Connect tile with max. 6 unique edges
bool
H3_Tile::addEdgeId( const u32 edgeId )
{
    return H3_Util::push_back_unique( edges, edgeId );
}

// Connect tile with max. 6 unique corners
bool
H3_Tile::addCornerId( const u32 cornerId )
{
    return H3_Util::push_back_unique( corners, cornerId );
}



// =====================================================================
// struct H3_Thief
// =====================================================================
// u32 id = 0;
// u32 occupiedTileId = 0; // tileId
// de::TRSf trs;    // Computed 3d model space transform
// std::vector<de::Triangle3f> triangles; // Cylinder

H3_Thief::H3_Thief()
    : id{0}
    , occupiedTileId{0}
{}

std::string H3_Thief::str() const
{
    std::stringstream o;
    o << "id(" << id << "), occupiedTileId(" << occupiedTileId << ")";
    return o.str();
}

// =====================================================================
// struct H3_Card
// =====================================================================
// u32 id = 0; // cardId
// u32 owner = 0; // playerId
// u32 typ = 0; // 0 = Ritter, 1 = Invention, 2 = Monopol
// de::TRSf trs;     // Gfx 3d model space transform
H3_Card::H3_Card()
    : id{0}
    , owner{0}
    , typ{0} // 0 = Ritter, 1 = Invention, 2 = Monopol
{
}

std::string H3_Card::str() const
{
    std::stringstream o; o <<
    "id(" << id << "), "
    "owner(" << owner << "), "
    "typ(" << typ << "), "
    "trs(" << trs.str() << ")";
    return o.str();
}

// =====================================================================
//struct H3_Road
// =====================================================================
// u32 id = 0; // roadId
// u32 owner = 0; // playerId
// u32 edgeId = 0; // edgeId
// u32 color = 0xFFFFFFFF; // Gfx player color, reduces some CPU time.
// de::TRSf trs;     // Gfx 3d model space transform
// std::vector<de::Triangle3f> collisionTriangles;
H3_Road::H3_Road()
    : id{0}
    , owner{0}
    , edgeId{0}
    , color{0xFFFFFFFF}
{
}

std::string H3_Road::str() const
{
    std::stringstream o; o <<
    "id(" << id << "), "
    "owner(" << owner << "), "
    "edgeId(" << edgeId << "), "
    "color(" << color << "), "
    "trs(" << trs.str() << ")";
    return o.str();
}

// =====================================================================
//struct H3_Farm
// =====================================================================
// u32 id = 0; // farmId
// u32 owner = 0; // playerId, who owns this farm.
// u32 cornerId = 0; // cornerId, where this farm stands on.
// u32 color = 0xFFFFFFFF; // Gfx player color, reduces some CPU time.
// de::TRSf trs; // Gfx 3d model space transform
// std::vector<de::Triangle3f> collisionTriangles;
H3_Farm::H3_Farm()
    : id{0}
    , owner{0}
    , cornerId{0}
    , color{0xFFFFFFFF}
{
}
std::string H3_Farm::str() const
{
    std::stringstream o; o <<
    "id(" << id << "), "
    "owner(" << owner << "), "
    "cornerId(" << cornerId << "), "
    "color(" << color << "), "
    "trs(" << trs.str() << ")";
    return o.str();
}

// =====================================================================
//struct H3_City
// =====================================================================
// u32 id = 0; // cityId
// u32 owner = 0; // playerId that owns this farm.
// u32 cornerId = 0; // cornerId, where this farm stands on.
// u32 color = 0xFFFFFFFF; // Gfx player color, reduces some CPU time.
// de::TRSf trs; // Gfx 3d model space transform
// std::vector<de::Triangle3f> collisionTriangles; // Clickable to steal cards from sel player.
H3_City::H3_City()
    : id{0}
    , owner{0}
    , cornerId{0}
    , color{0xFFFFFFFF}
{
}
std::string H3_City::str() const
{
    std::stringstream o; o <<
    "id(" << id << "), "
    "owner(" << owner << "), "
    "cornerId(" << cornerId << "), "
    "color(" << color << "), "
    "trs(" << trs.str() << ")";
    return o.str();
}

// =====================================================================
//struct H3_RankedPlayer
// =====================================================================
// u32 playerId = 0;
// u32 diceSum = 0; // diceValue or victoryPoints
// u32 rank = 0;
H3_RankedPlayer::H3_RankedPlayer()
    : playerId{0}
    , diceSum{0}
    , rank{0}
{
}
std::string H3_RankedPlayer::str() const
{
    std::stringstream o; o <<
    "playerId(" << playerId << "), "
    "diceSum(" << diceSum << "), "
    "rank(" << rank << ")";
    return o.str();
}

// =====================================================================
//struct H3_PlayerState
// =====================================================================


std::string H3_PlayerState::str() const
{
    std::ostringstream o; o <<
    "rank(" << rankDice << "), "
    "rankWin(" << rankVicPoints << "), "
    "vic(" << victoryPoints << "), "
    "dice(" << dice.str() << "), "
    "bank(" << bank.str() << "), "
    "hasPort3v1(" << hasPort3v1 << "), "
    "hasPortA(" << hasPortA << "), "
    "hasPortB(" << hasPortB << "), "
    "hasPortC(" << hasPortC << "), "
    "hasPortD(" << hasPortD << "), "
    "hasPortE(" << hasPortE << "), "
    "hasBonusArmy(" << hasBonusArmy << "), "
    "hasBonusRoad(" << hasBonusRoad << "), "

    "roads" << H3_Util::str(roads) << ", "
    "farms" << H3_Util::str(farms) << ", "
    "citys" << H3_Util::str(citys) << ", "
    "ships" << H3_Util::str(ships) << ", "

    "points" << H3_Util::str(points) << ", "
    "knights" << H3_Util::str(knights) << ", "
    "invents" << H3_Util::str(invents) << ", "
    "monopols" << H3_Util::str(monopols) << ", "

    "pointCards" << H3_Util::str(pointCards) << ", "
    "knightCards" << H3_Util::str(knightCards) << ", "
    "inventCards" << H3_Util::str(inventCards) << ", "
    "monopolCards" << H3_Util::str(monopolCards) << ", "

    "tiles" << H3_Util::str(tiles) << ", "
    "edges" << H3_Util::str(edges) << ", "
    "corners" << H3_Util::str(corners) << ", "

    ;
    return o.str();
}

H3_PlayerState::H3_PlayerState()
{
    clear();
}


void H3_PlayerState::endTurn()
{
    dice.clear();       // Needed for every round.
}

void H3_PlayerState::clear()
{
    endTurn();

    hasPort3v1 = false;
    hasPortA = false;
    hasPortB = false;
    hasPortC = false;
    hasPortD = false;
    hasPortE = false;
    hasBonusArmy = false;
    hasBonusRoad = false;

    bank.clear();
    rankDice = 0;
    rankVicPoints = 0;
    victoryPoints = 0; // = 1 per farm + 2 per city + 1 per pointCard

    tiles.clear();
    edges.clear();
    corners.clear();

    roads.clear();
    farms.clear();
    citys.clear();
    ships.clear();

    points.clear();
    knights.clear();
    invents.clear();
    monopols.clear();

    pointCards.clear();
    knightCards.clear();
    inventCards.clear();
    monopolCards.clear();
}

bool H3_PlayerState::canBanking() const
{
    if (bank.A >= 4) return true;
    if (bank.B >= 4) return true;
    if (bank.C >= 4) return true;
    if (bank.D >= 4) return true;
    if (bank.E >= 4) return true;

    if (hasPort3v1)
    {
        if (bank.A >= 3) return true;
        if (bank.B >= 3) return true;
        if (bank.C >= 3) return true;
        if (bank.D >= 3) return true;
        if (bank.E >= 3) return true;
    }

    if (hasPortA) { if (bank.A >= 2) return true; }
    if (hasPortB) { if (bank.B >= 2) return true; }
    if (hasPortC) { if (bank.C >= 2) return true; }
    if (hasPortD) { if (bank.D >= 2) return true; }
    if (hasPortE) { if (bank.E >= 2) return true; }

    return false;
}

int H3_PlayerState::unrevealedPointCards() const
{
    return pointCards.size(); // newPointCards.size() +
}

int H3_PlayerState::unrevealedKnightCards() const
{
    return knightCards.size(); // newKnightCards.size() +
}

int H3_PlayerState::unrevealedInventCards() const
{
    return inventCards.size(); // newInventCards.size() +
}

int H3_PlayerState::unrevealedMonopolCards() const
{
    return monopolCards.size(); // newMonopolCards.size() +
}

bool H3_PlayerState::canTrading() const
{
    if (bank.sum() > 0) return true;
    if (unrevealedPointCards() > 0) return true;
    if (unrevealedKnightCards() > 0) return true;
    if (unrevealedInventCards() > 0) return true;
    if (unrevealedMonopolCards() > 0) return true;
    return false;
}

u32 H3_PlayerState::computeVictoryPoints() const
{
    size_t n = 0;
    n += farms.size();
    n += citys.size() * 2;
    n += hasBonusArmy ? 2 : 0;
    n += hasBonusRoad ? 2 : 0;
    n += points.size();
    return static_cast<u32>(n);
}

void H3_PlayerState::updateVictoryPoints()
{
    victoryPoints = computeVictoryPoints();
}
