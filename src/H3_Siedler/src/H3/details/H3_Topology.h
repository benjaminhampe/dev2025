#pragma once
#include <H3/H3_Board.h>

class H3_Game;

// ======================================================================
// ID
// ======================================================================
uint32_t H3_createId();

// =======================================================================
// Tile
// =======================================================================
H3_Tile& H3_findFirstDesertTile( H3_Game& game);
uint32_t H3_countTiles( H3_Game& game, H3_TileType tt );
std::vector<uint32_t> H3_getTileIds( H3_Game& game, H3_TileType tt );

H3_Tile& H3_getTile( H3_Game& game, const std::string& caller, const u32 tileId );
H3_Tile* H3_getTile( H3_Game& game, const glm::vec3& searchPos );

std::vector< u32 >
H3_findTiles( const H3_Game& game, glm::vec3 const & cornerPos );

// =======================================================================
// Corner
// =======================================================================
H3_Corner& H3_getCorner( H3_Game& game, const std::string& caller, const u32 cornerId );
H3_Corner* H3_getCorner( H3_Game& game, const glm::vec3& searchPos );

// =======================================================================
// Edge
// =======================================================================
H3_Edge& H3_getEdge( H3_Game& game, const std::string& caller, const u32 edgeId );
H3_Edge* H3_getEdge( H3_Game& game, const glm::vec3& searchPos );
// =======================================================================
// Road
// =======================================================================
H3_Road& H3_getRoad( H3_Game& game, const std::string& caller, const u32 roadId );

// =======================================================================
// Farm
// =======================================================================
H3_Farm& H3_getFarm( H3_Game& game, const std::string& caller, const u32 farmId );

// =======================================================================
// City
// =======================================================================
H3_City& H3_getCity( H3_Game& game, const std::string& caller, const u32 cityId );

// =======================================================================
// Card
// =======================================================================
H3_Card& H3_getEventCard( H3_Game& game, const std::string& caller, const u32 cardId );

// ========================================================================
// Topology
// ========================================================================
void H3_createTopology( H3_Game& game );

void H3_Topology_createCorners( H3_Game& game );
void H3_Topology_createEdges( H3_Game& game );
void H3_Topology_connectTiles( H3_Game& game );
void H3_Topology_connectCorners( H3_Game& game );
void H3_Topology_connectEdges( H3_Game& game );
void H3_Topology_connectCornersWithEdges( H3_Game& game );
void H3_Topology_connectNextCorners( H3_Game& game );
void H3_Topology_connectNextEdges( H3_Game& game );


/*
struct H3_ChipDrawCall
{
    glm::mat4 modelMat;
    uint32_t id;        // Ref to chip to get collision triangles.
    uint8_t texLayer;
    uint8_t angle60;
};

// Collect RoadWaypoints:
std::vector< H3_ChipDrawCall >
H3_collectVisibleEdges( H3_Game & game );

// Collect FarmWaypoints:
std::vector< H3_ChipDrawCall >
H3_collectVisibleCorners( H3_Game & game );

// Collect TileChips: Pretty constant for now
std::vector< H3_ChipDrawCall >
H3_collectVisibleTileChips( H3_Game & game );

void H3_ChipRenderer::collectInstances(const H3_Game& game)
{
    if (!m_driver) { DE_ERROR("No driver") return; }

    const auto & g_tiles = game.m_tiles;
    const auto & g_corners = game.m_corners;
    const auto & g_edges = game.m_edges;

    const size_t nTiles = g_tiles.size();
    const size_t nCorners = g_corners.size();
    const size_t nEdges = g_edges.size();
    const size_t n = nTiles + nCorners + nEdges;

    //========================================================================
    // CPU Memory:
    //========================================================================

    // Instance Data (Transform Matrices)
    m_instanceMat.clear();
    m_instanceMat.reserve(n);
    m_instanceLayer.clear();
    m_instanceLayer.reserve(n);
    m_instanceAngle60.clear();
    m_instanceAngle60.reserve(n);

    enum eType
    {
        Chip2 = 0, Chip3, Chip4, Chip5, Chip6, Chip8, Chip9, Chip10, Chip11, Chip12,
        ChipS, ChipW, Chip3v1, ChipA, ChipB, ChipC, ChipD, ChipE, eTypeCount
    };

    // Collect Corners
    if (game.m_bCornersVisible)
    {
        for (const auto& corner : g_corners)
        {
            //if (corner.enabled)
            if (corner.building_type == 0)
            {
                m_instanceMat.push_back( corner.trs.trs );
                m_instanceLayer.push_back( ChipS );
                m_instanceAngle60.push_back( 0 );
            }
        }
    }

    // Collect Edges
    if (game.m_bEdgesVisible)
    {
        for (const auto& edge : g_edges)
        {
            //if (edge.enabled)
            if (edge.owner == 0)
            {
                m_instanceMat.push_back( edge.trs.trs );
                m_instanceLayer.push_back( ChipW );
                m_instanceAngle60.push_back( 0 );
            }
        }
    }

    // Collect TileChips
    for (const auto& tile : g_tiles)
    {
        int layer = -1; // Unknown tex (question mark on black background)

        if (tile.tileType.isResource())
        {
            switch( tile.chipValue )
            {
            case 2: layer = Chip2; break;
            case 3: layer = Chip3; break;
            case 4: layer = Chip4; break;
            case 5: layer = Chip5; break;
            case 6: layer = Chip6; break;
            case 8: layer = Chip8; break;
            case 9: layer = Chip9; break;
            case 10: layer = Chip10; break;
            case 11: layer = Chip11; break;
            case 12: layer = Chip12; break;
            default: break;
            }
        }
        else if (tile.tileType.isPort())
        {
            switch( tile.tileType.value )
            {
            case H3_TileType::Port3v1: layer = Chip3v1; break;
            case H3_TileType::PortA: layer = ChipA; break;
            case H3_TileType::PortB: layer = ChipB; break;
            case H3_TileType::PortC: layer = ChipC; break;
            case H3_TileType::PortD: layer = ChipD; break;
            case H3_TileType::PortE: layer = ChipE; break;
            default: break;
            }
        }

        if ( layer < 0 ) continue;

        m_instanceMat.push_back( tile.trsChip.trs );

        m_instanceLayer.push_back( layer );

        m_instanceAngle60.push_back( 0 );
    }

}


// ========================================================================

int
Pio_getCardCount( PioBoard & board, int playerId, PioImageType::EType typ )
{
   int found = Pio_findPlayer( board, playerId );
   if (found < 0)
   {
      std::ostringstream s;
      s << __func__ << " :: Invalid playerId " << playerId;
      throw std::runtime_error( s.str() );
   }

   auto & player = board.players[ found ];

   int n = 0;
   for ( int i = 0; i < player.cards.size(); ++i )
   {
      if ( Pio_getCard( board, player.cards[ i ] ).typ == typ ) n++;
   }

   return n;
}


*/
