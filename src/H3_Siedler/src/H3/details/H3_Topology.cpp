#include "H3_Topology.h"
#include <H3/H3_Game.h>

// ===========================================================================
uint32_t H3_createId()
{
    static uint32_t idCounter = 0;
    idCounter++;
    return idCounter;
}

// ===========================================================================
std::vector< u32 >
H3_findTilesFromCornerPos( const H3_Game& game, const glm::vec3& cornerPos )
{
    std::vector< u32 > container;

    const float dx = game.m_tileSize.x;
    const float dz = game.m_tileSize.z;

    for ( const auto & tile : game.m_tiles )
    {
        for ( int k = 0; k < 6; ++k )
        {
            const auto e2 = de::gpu::SMeshHexagon::getCorner( k, dx, dz );
            const auto e3 = tile.pos + glm::vec3( e2.x, 0.0f, e2.y );
            if ( dbEquals( e3, cornerPos ) )
            {
                // Add unique
                const auto it = std::find_if( container.begin(), container.end(),
                    [&] ( const u32 tileId ) { return tileId == tile.id; } );
                if (it == container.end())
                {
                    container.emplace_back( tile.id );
                }
            }
        }
    }

    return container;
}

// =======================================================================
// Tile
// =======================================================================
H3_Tile&
H3_findFirstDesertTile( H3_Game& game )
{
    const auto & v = game.m_tiles;

    auto it = std::find_if(v.begin(),
                 v.end(),
                 [] ( const H3_Tile & tile )
                 { return tile.tileType == H3_TileType::Desert; } );

    if (it == v.end())
    {
        std::ostringstream o;
        o << __func__ << " :: No DesertTile ";
        //o << ", caller(" << caller <<")";
        throw std::runtime_error( o.str() );
    }

    return game.m_tiles[ std::distance(v.begin(),it) ];
}

uint32_t
H3_countTiles( H3_Game& game, H3_TileType tt )
{
    uint32_t n = 0;
    for ( const auto & tile : game.m_tiles)
    {
        if (tile.tileType == tt)
        {
            n++;
        }
    }
    return n;
}

std::vector<uint32_t>
H3_getTileIds( H3_Game& game, H3_TileType tt )
{
    std::vector<uint32_t> tileIds;
    tileIds.reserve( H3_countTiles( game, tt ) );

    for ( const auto & tile : game.m_tiles)
    {
        if (tile.tileType == tt)
        {
            tileIds.push_back( tile.id );
        }
    }

    return tileIds;
}

// ===========================================================================

H3_Tile& H3_getTile( H3_Game& game, const std::string& caller, const u32 tileId )
{
    auto & g_tiles = game.m_tiles;

    auto it = std::find_if( g_tiles.begin(), g_tiles.end(),
        [=] (const H3_Tile& tile) { return tile.id == tileId; });
    if (it == g_tiles.end())
    {
        std::ostringstream o;
        o << __func__ << " :: Invalid tileId " << tileId;
        o << ", caller(" << caller <<")";
        throw std::runtime_error( o.str() );
    }
    return g_tiles[ std::distance(g_tiles.begin(),it) ];
}

H3_Tile* H3_getTile( H3_Game& game, const glm::vec3& searchPos )
{
    auto & g_tiles = game.m_tiles;

    for ( auto & g_tile : g_tiles )
    {
        if ( dbEquals( searchPos, g_tile.pos ) ) { return &g_tile; }
    }
    return nullptr;
}

// =======================================================================
// Corner
// =======================================================================
H3_Corner& H3_getCorner( H3_Game& game, const std::string& caller, const u32 cornerId )
{
    auto & g = game.m_corners;

    auto it = std::find_if( g.begin(), g.end(),
        [=] (const H3_Corner& corner) { return corner.id == cornerId; });

    if (it == g.end())
    {
        std::ostringstream o;
        o << __func__ << "["<<caller<<"()] :: No corner id " << cornerId;
        throw std::runtime_error( o.str() );
    }
    return g[ std::distance(g.begin(),it) ];
}

H3_Corner* H3_getCorner( H3_Game& game, const glm::vec3& searchPos )
{
    auto & g_corners = game.m_corners;

    for ( size_t i = 0; i < g_corners.size(); ++i )
    {
        auto & g_corner = g_corners[ i ];
        if ( dbEquals( searchPos, g_corner.trs.pos ) )
        {
            return &g_corner;
        }
    }
    return nullptr;
}

// =======================================================================
// Edge
// =======================================================================
H3_Edge& H3_getEdge( H3_Game& game, const std::string& caller, const u32 edgeId )
{
    auto & g = game.m_edges;

    auto it = std::find_if( g.begin(), g.end(),
        [=] (const auto& cached) { return cached.id == edgeId; });

    if (it == g.end())
    {
        std::ostringstream o;
        o << __func__ << "["<<caller<<"()] :: No edge id " << edgeId;
        throw std::runtime_error( o.str() );
    }
    return g[ std::distance(g.begin(),it) ];
}

H3_Edge* H3_getEdge( H3_Game& game, const glm::vec3& searchPos )
{
    for (auto & edge : game.m_edges)
    {
        const bool x = std::abs( searchPos.x - edge.trs.pos.x ) < 1.0f;
        const bool y = std::abs( searchPos.y - edge.trs.pos.y ) < 1.0f;
        const bool z = std::abs( searchPos.z - edge.trs.pos.z ) < 1.0f;
        if ( x && y && z )
        {
            return &edge;
        }
    }
    return nullptr;
}

// =======================================================================
// Road
// =======================================================================
H3_Road& H3_getRoad( H3_Game& game, const std::string& caller, const u32 id )
{
    auto & g = game.m_roads;

    auto it = std::find_if( g.begin(), g.end(),
       [=] (const auto& cached) { return cached.id == id; });

    if (it == g.end())
    {
        std::ostringstream o;
        o << __func__ << "["<<caller<<"()] :: No road id " << id << " of " << g.size();
        throw std::runtime_error( o.str() );
    }
    return g[ std::distance(g.begin(),it) ];
}

// =======================================================================
// Farm
// =======================================================================
H3_Farm& H3_getFarm( H3_Game& game, const std::string& caller, const u32 id )
{
    auto & g = game.m_farms;

    auto it = std::find_if( g.begin(), g.end(),
        [=] (const auto& cached) { return cached.id == id; });

    if (it == g.end())
    {
        std::ostringstream o;
        o << __func__ << "["<<caller<<"()] :: No farm id " << id << " of " << g.size();
        throw std::runtime_error( o.str() );
    }
    return g[ std::distance(g.begin(),it) ];
}

// =======================================================================
// City
// =======================================================================
H3_City& H3_getCity( H3_Game& game, const std::string& caller, const u32 id )
{
    auto & g = game.m_citys;

    auto it = std::find_if( g.begin(), g.end(),
        [=] (const auto& cached) { return cached.id == id; });

    if (it == g.end())
    {
        std::ostringstream o;
        o << __func__ << "["<<caller<<"()] :: No city id " << id << " of " << g.size();
        throw std::runtime_error( o.str() );
    }
    return g[ std::distance(g.begin(),it) ];
}

// =======================================================================
// Card
// =======================================================================
H3_Card& H3_getEventCard( H3_Game& game, const std::string& caller, const u32 id )
{
    auto & g = game.m_cards;

    auto it = std::find_if( g.begin(), g.end(),
        [=] (const auto& cached) { return cached.id == id; });

    if (it == g.end())
    {
        std::ostringstream o;
        o << __func__ << "["<<caller<<"()] :: No card id " << id;
        throw std::runtime_error( o.str() );
    }
    return g[ std::distance(g.begin(),it) ];
}


// ========================================================================
// Topology
// ========================================================================
void H3_Topology_createCorners( H3_Game& game )
{
    auto & g_tiles = game.m_tiles;
    auto & g_tileSize = game.m_tileSize;
    auto & g_corners = game.m_corners;

    const auto cornerTriangles = game.m_smeshCorner.createCollisionTriangles();

    // ===============================
    // H3_Topology_createCorners():
    // ===============================
    for (H3_Tile & tile : g_tiles)
    {
        if ( tile.isWaterOrPort() ) { continue; }

        for (u32 c = 0; c < 6; ++c)
        {
            auto cornerPos2 = de::gpu::SMeshHexagon::getCorner( c, g_tileSize.x, g_tileSize.z );
            auto cornerPos3 = tile.pos + glm::vec3( cornerPos2.x, 0.0f, cornerPos2.y ); //game.m_cornerSize.y * 0.5f

            // Be unique in regard to float position XYZ.
            auto cornerPtr = H3_getCorner( game, cornerPos3 );
            if (cornerPtr) { continue; }

            H3_Corner corner;
            corner.id = H3_createId();
            corner.owner = 0;
            corner.trs.pos = cornerPos3;
            //corner.trs.rotate = glm::vec3(0,0,0);
            //corner.trs.scale = glm::vec3(1,1,1);
            corner.trs.update();
            corner.building_type = 0;

            // Create Collision triangles:
            corner.collisionTriangles.clear();
            corner.collisionTriangles.reserve( cornerTriangles.size() );
            for (const auto& triangle : cornerTriangles)
            {
                auto A = corner.trs.pos + triangle.A;
                auto B = corner.trs.pos + triangle.B;
                auto C = corner.trs.pos + triangle.C;
                corner.collisionTriangles.emplace_back(A,B,C);
            }
            //de::SMeshBuffer tmp = createCornerMeshBuffer();
            //de::SMeshBufferTool::translateVertices( tmp, corner.pos );
            //tmp.recalculateBoundingBox();
            //corner.mesh.addMeshBuffer( tmp );
            g_corners.emplace_back( std::move(corner) );
        }
    }

    DE_TRACE("Created corners = ",g_corners.size())

}
// ========================================================================
void H3_Topology_createEdges( H3_Game& game )
{
    auto & g_tiles = game.m_tiles;
    auto & g_tileSize = game.m_tileSize;
    auto & g_edges = game.m_edges;

    // ===============================
    // H3_Topology_createEdges():
    // ===============================

    const auto edgeTriangles = game.m_smeshEdge.createCollisionTriangles();

    for (const H3_Tile& tile : g_tiles)
    {
        if ( tile.isWaterOrPort() ) { continue; }

        for (u32 e = 0; e < 6; ++e)
        {
            auto edgePos2 = de::gpu::SMeshHexagon::getEdge( e, g_tileSize.x, g_tileSize.z );
            auto edgePos3 = tile.pos + glm::vec3( edgePos2.x, 0.f, edgePos2.y ); // game.m_edgeSize.y * 0.5f

            // Be unique in regard to float position XYZ.
            auto edgePtr = H3_getEdge( game, edgePos3 );
            if (edgePtr) { continue; }

            float angle = de::gpu::SMeshHexagon::computeEdgeAngle( e, g_tileSize.x, g_tileSize.z );

            //DE_WARN("tile[",tile.id,"].angle = ",angle)

            H3_Edge edge;
            edge.id = H3_createId();
            edge.owner = 0;
            edge.trs.pos = edgePos3;
            edge.trs.rotate = glm::vec3(0,angle,0);
            //edge.trs.scale = game.m_cfg.edgeSize;
            edge.trs.update();

            // Create Collision triangles:
            edge.collisionTriangles.clear();
            edge.collisionTriangles.reserve( edgeTriangles.size() );
            for (const auto& triangle : edgeTriangles)
            {
                auto A = edge.trs.pos + triangle.A;
                auto B = edge.trs.pos + triangle.B;
                auto C = edge.trs.pos + triangle.C;
                edge.collisionTriangles.emplace_back(A,B,C);
            }

            //de::SMeshBuffer tmp = createEdgeMeshBuffer();
            //de::SMeshBufferTool::translateVertices( tmp, edge.pos );
            //tmp.recalculateBoundingBox();
            //edge.mesh.addMeshBuffer( tmp );

            g_edges.emplace_back( std::move(edge) );
        }
    }

    DE_TRACE("Created edges = ",g_edges.size())
}

// ========================================================================
// Connect tiles by searching all corners for tiles with same corner pos
void H3_Topology_connectTiles( H3_Game& game )
{
    auto & g_tiles = game.m_tiles;
    auto & g_tileSize = game.m_tileSize;

    // ===============================
    // H3_Topology_connectTiles():
    // ===============================
    u32 numTileConnections = 0;

    for (H3_Tile & tile : g_tiles)
    {
        for (u32 c = 0; c < 6; ++c)
        {
            auto c2 = de::gpu::SMeshHexagon::getCorner( c, g_tileSize.x, g_tileSize.z );
            auto c3 = tile.pos + glm::vec3( c2.x, 0.0f, c2.y );
            auto next = H3_findTilesFromCornerPos( game, c3 );
            for (const u32 n : next)
            {
                numTileConnections += tile.addTileId( n );
            }
        }
    }
    DE_TRACE("Made connections tile.addTile() = ",numTileConnections)
}
// ========================================================================
void H3_Topology_connectCorners( H3_Game& game )
{
    auto & g_tiles = game.m_tiles;
    auto & g_tileSize = game.m_tileSize;
    auto & g_corners = game.m_corners;

    // ===============================
    // H3_Topology_connectCorners():
    // ===============================
    u32 numTileConnections = 0;
    u32 numCornerConnections = 0;

    for (auto & tile : g_tiles)
    {
        for (u32 c = 0; c < 6; ++c)
        {
            auto c2 = de::gpu::SMeshHexagon::getCorner( c, g_tileSize.x, g_tileSize.z );
            auto c3 = tile.pos + glm::vec3( c2.x, 0.0f, c2.y );

            // Be unique in regard to float position XYZ.
            auto cornerPtr = H3_getCorner( game, c3 );
            if ( cornerPtr )
            {
                if ( cornerPtr->addTileId( tile.id ) > 0 )
                {
                    numTileConnections++;
                }
                if ( tile.addCornerId( cornerPtr->id ) > 0 )
                {
                    numCornerConnections++;
                }
            }
        }
    }

    DE_TRACE("Made connections corner.addTile() = ",numTileConnections)
    DE_TRACE("Made connections tile.addCorner() = ",numCornerConnections)
}
// ========================================================================
void H3_Topology_connectEdges( H3_Game& game )
{
    auto & g_tiles = game.m_tiles;
    auto & g_tileSize = game.m_tileSize;
    auto & g_edges = game.m_edges;

    // ===============================
    // H3_Topology_connectEdges():
    // ===============================

    u32 numTileConnections = 0;
    u32 numEdgeConnections = 0;

    for (auto & tile : g_tiles)
    {
        for (u32 c = 0; c < 6; ++c)
        {
            auto e2 = de::gpu::SMeshHexagon::getEdge( c, g_tileSize.x, g_tileSize.z );
            auto e3 = tile.pos + glm::vec3( e2.x, 0.0f, e2.y );

            // Be unique in regard to float position XYZ.
            auto edgePtr = H3_getEdge( game, e3 );
            if ( edgePtr )
            {
                if ( edgePtr->addTileId( tile.id ) > 0 )
                {
                    numTileConnections++;
                }
                if ( tile.addEdgeId( edgePtr->id ) > 0 )
                {
                    numEdgeConnections++;
                }
            }
        }
    }

    DE_TRACE("Made connections edge.addTile() = ",numTileConnections)
    DE_TRACE("Made connections tile.addEdge() = ",numEdgeConnections)
}
// ========================================================================
void H3_Topology_connectCornersWithEdges( H3_Game& game )
{
    auto & g_tileSize = game.m_tileSize;
    auto & g_corners = game.m_corners;

    // =======================================
    // H3_Topology_connectCornersWithEdges():
    // =======================================

    u32 numConnections = 0;

    for (auto & corner : g_corners)
    {
        for (u32 tileId : corner.tiles)
        {
            //if (tileId < 1) continue;
            H3_Tile & tile = H3_getTile( game, __func__, tileId );

            // Loop all edges and find
            int i60 = tile.getCornerIndex( g_tileSize, corner );
            int e1 = (i60) % 6;
            int e2 = (i60 + 5) % 6;

            auto e12 = de::gpu::SMeshHexagon::getEdge( e1, g_tileSize.x, g_tileSize.z );
            auto e13 = tile.pos + glm::vec3( e12.x, 0.0f, e12.y );
            auto e1Ptr = H3_getEdge( game, e13 );
            if ( e1Ptr )
            {
                e1Ptr->addCornerId( corner.id );
                corner.addEdgeId( e1Ptr->id );
                numConnections++;
                numConnections++;
            }

            auto e22 = de::gpu::SMeshHexagon::getEdge( e2, g_tileSize.x, g_tileSize.z );
            auto e23 = tile.pos + glm::vec3( e22.x, 0.0f, e22.y );
            auto e2Ptr = H3_getEdge( game, e23 );
            if ( e2Ptr )
            {
                e2Ptr->addCornerId( corner.id );
                corner.addEdgeId( e2Ptr->id );
                numConnections++;
                numConnections++;
            }
        }
    }

    DE_TRACE("Made connections edge.addCorner() = ", numConnections )
    DE_TRACE("Made connections corner.addEdge() = ", numConnections )
}
// ========================================================================
// Finalize Topology
// ========================================================================
void H3_Topology_connectNextCorners( H3_Game& game )
{
    auto & g_corners = game.m_corners;

    // =======================================
    // H3_Topology_connectNextCorners():
    // =======================================

    u32 numNextCorners = 0;

    // For all Corners...
    for (auto & corner : g_corners)
    {
        // For all Edges
        for (int edgeId : corner.edges)
        {
            //if (edgeId < 1) continue;
            auto & edge = H3_getEdge( game, __func__, edgeId );
            // For all Next Corners
            for (int cornerId : edge.corners)
            {
                numNextCorners += corner.addCornerId( cornerId );
            }
        }
    }

    // DE_TRACE("Next Corner <-> Corner connections = ", counter )
}
// ========================================================================
// Finalize Topology
// ========================================================================
void H3_Topology_connectNextEdges( H3_Game& game )
{
    auto & g_edges = game.m_edges;

    // =======================================
    // H3_Topology_connectNextEdges():
    // =======================================

    u32 numNextEdges = 0;

    // For all Edges
    for (auto & edge : g_edges)
    {
        // For all Corners...
        for (int cornerId : edge.corners)
        {
            //if (cornerId < 1) continue;
            auto & corner = H3_getCorner( game, __func__, cornerId );

            // For all Next Edges
            for (int edgeId : corner.edges)
            {
                numNextEdges += edge.addEdgeId( edgeId );
            }
        }
    }

    // DE_TRACE("Next Edge <-> Edge connections = ", counter )
}

// ========================================================================
// Finalize Topology with one call
// ========================================================================
void H3_createTopology( H3_Game& game ) // Game road network topology
{
    DE_TRACE("Topology :: Start")
    DE_TRACE("TileCount = ", game.m_tiles.size())

    // Real board objects
    game.m_corners.clear();
    game.m_edges.clear();
    game.m_cards.clear();
    game.m_roads.clear();
    game.m_farms.clear();
    game.m_citys.clear();

    H3_Topology_createCorners( game );
    H3_Topology_createEdges( game );
    H3_Topology_connectTiles( game );
    H3_Topology_connectCorners( game );
    H3_Topology_connectEdges( game );
    H3_Topology_connectCornersWithEdges( game );
    H3_Topology_connectNextCorners( game );
    H3_Topology_connectNextEdges( game );
    DE_TRACE("Topology :: End")

    // for (size_t i = 0; i < game.m_edges.size(); ++i)
    // {
    //     DE_WARN("Edge[",i,"] ", game.m_edges[i].str())
    // }
}
