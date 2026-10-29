#pragma once
#include <H3/H3_Board.h>

class H3_Game;

// ======================================================================
// ID
// ======================================================================
uint32_t H3_createId();

// =======================================================================
// Const
// =======================================================================

const H3_Tile&
H3_getTile( const H3_Game& game, const std::string& caller, const u32 tileId );

const H3_Corner&
H3_getCorner( const H3_Game& game, const std::string& caller, const u32 cornerId );

const H3_Edge&
H3_getEdge( const H3_Game& game, const std::string& caller, const u32 edgeId );

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


void
H3_computeEdgesForRoad( H3_Game & game, uint32_t playerId );

void
H3_computeCornersForFarm( H3_Game& game, uint32_t playerId );


float H3_getDiceChance( int diceSum );

float H3_getDiceChanceOfCorner( const H3_Game& game, uint32_t cornerId );

int H3_numPort3v1AtCorner( const H3_Game& game, uint32_t cornerId );

H3_Bank H3_numPort2v1AtCorner( const H3_Game& game, uint32_t cornerId );

H3_Bank H3_countResourceTilesOfCorner( const H3_Game& game, uint32_t cornerId );



std::vector<u32>
H3_computeVisibleCorners( const H3_Game& game, const uint32_t playerId );

std::vector<u32>
H3_computeVisibleEdges( const H3_Game & game, const uint32_t playerId );
