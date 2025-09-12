#include <H3/details/H3_AI.h>
#include <H3/H3_Game.h>
#include <H3/details/H3_Topology.h>

void AI_donateCardsToRobber( H3_Game & game, int playerId )
{
    H3_Player & player = game.getPlayer( playerId );
    int cardLimit = game.m_cfg.cardLimitNoSteal;

    if (player.state.bank.sum() <= cardLimit)
    {
        DE_BENNI("Player[",player.name,"] is below CardLimit")
        return;
    }

    H3_Bank bank = player.state.bank;

    bool bWantBuyRoad = true;
    bool bWantBuyFarm = true;
    bool bWantBuyCard = true;
    bool bWantBuyCity = true;

    const int maxLose = player.state.bank.sum() - cardLimit;
    int numLose = maxLose;
    int A = 0;
    int B = 0;
    int C = 0;
    int D = 0;
    int E = 0;

    while (numLose > 0)
    {
        if (bank.E > 0 && numLose > 0)
        {
            bank.E--;
            E++;
        }
        if (bank.D > 0 && numLose > 0)
        {
            bank.D--;
            D++;
        }
        if (bank.C > 0 && numLose > 0)
        {
            bank.C--;
            C++;
        }
        if (bank.B > 0 && numLose > 0)
        {
            bank.B--;
            B++;
        }
        if (bank.A > 0 && numLose > 0)
        {
            bank.A--;
            A++;
        }
    }

    bank.A = A;
    bank.B = B;
    bank.C = C;
    bank.D = D;
    bank.E = E;

    DE_OK("Player[",player.name,"] has to donate ", maxLose, " to robber.")
    DE_OK("Before Bank = ", player.state.bank.str())
    DE_OK("Donate Bank = ", bank.str())

    if (!player.state.bank.buy( bank ))
    {
        DE_ERROR("Cannot buy")
    }

    DE_OK("Robbed Bank = ", player.state.bank.str())
}

float H3_getDiceChance( int diceSum )
{
    switch ( diceSum )
    {
    case 2: return 1.0f / 36.0f;
    case 3: return 2.0f / 36.0f;
    case 4: return 3.0f / 36.0f;
    case 5: return 4.0f / 36.0f;
    case 6: return 5.0f / 36.0f;
    case 7: return 6.0f / 36.0f;
    case 8: return 5.0f / 36.0f;
    case 9: return 4.0f / 36.0f;
    case 10: return 3.0f / 36.0f;
    case 11: return 2.0f / 36.0f;
    case 12: return 1.0f / 36.0f;
    default: return 0.0f;
    }
}

void CPU_doRound1( H3_Game & game )
{
    game.doDice();
    game.doEndTurn();
}

std::vector< FarmMatch >
CPU_findBestFarm( H3_Game & game )
{
    std::vector< FarmMatch > farmMatches;

    //PioPlayer & player = H3_getCurrentPlayer( __func__, game );

    for ( size_t i = 0; i < game.m_corners.size(); ++i )
    {
        auto& corner = game.m_corners[ i ];
        if ( corner.owner > 0 )
        {
            continue;
        }

        // We want to know how many unique tile types this corner has...
        std::map< H3_TileType, int > ttc;

        for ( size_t k = 0; k < corner.tiles.size(); ++k )
        {
            int tileId = corner.tiles[ k ];
            if ( !tileId ) continue;
            auto& tile = H3_getTile( game, __func__, tileId );
            ttc[ tile.tileType ]++;
        }

        FarmMatch match;
        match.corner = corner.id;
        match.prob = float( corner.tiles.size() );
        match.prob *= float( ttc.size() );  // How many different ress this tile corner

        for ( size_t k = 0; k < corner.tiles.size(); ++k )
        {
            int tileId = corner.tiles[ k ];
            if ( !tileId ) continue;
            auto& tile = H3_getTile( game, __func__, tileId );
            match.prob += H3_getDiceChance( tile.chipValue );  // How many different ress this tile corner
        }

        farmMatches.emplace_back( std::move( match ) );
    }

    std::sort( farmMatches.begin(), farmMatches.end(),
    [] ( FarmMatch const & a, FarmMatch const & b ) { return a.prob > b.prob; });

    //   DEM_DEBUG("FarmMatches.size() = ", farmMatches.size() )
    //   for ( size_t i = 0; i < std::min( farmMatches.size(), size_t(10) ); ++i )
    //   {
    //      DEM_DEBUG("FarmMatches[",i,"] = ", farmMatches[ i ].toString() )
    //   }

    return farmMatches;
}

#if 0
void H3_updatePossibleEdges( H3_Game & game )
{
    H3_hideEdges( game );

    //   PioPlayer & player = H3_getCurrentPlayer( __func__, game );

    //   DEM_DEBUG("game.round = ", game.round )
    //   DEM_DEBUG("player.id = ", player.id )
    //   DEM_DEBUG("player.name = ", player.name )
    //   DEM_DEBUG("player.tiles.size() = ", player.tiles.size() )
    //   DEM_DEBUG("player.edges.size() = ", player.edges.size() )
    //   DEM_DEBUG("player.corners.size() = ", player.corners.size() )
    //   DEM_DEBUG("player.roads.size() = ", player.roads.size() )
    //   DEM_DEBUG("player.farms.size() = ", player.farms.size() )
    //   DEM_DEBUG("player.cities.size() = ", player.cities.size() )
    //   DEM_DEBUG("player.ships.size() = ", player.ships.size() )
    //   DEM_DEBUG("player.cards.size() = ", player.cards.size() )

    //   if ( game.round < 4 )
    //   {
    for ( H3_Edge & edge : game.m_edges)
    {
        if ( edge.owner < 1 )
        {
            edge.visible = true;
        }
    }
    //   }
    //   else
    //   {
    //      for ( size_t i = 0; i < player.corners.size(); ++i )
    //      {
    //         H3_Corner & corner = H3_getCorner( game, player.corners[i] );
    //         DEM_DEBUG("corner.id = ", corner.id )
    //         DEM_DEBUG("corner.state = ", corner.state )
    //         DEM_DEBUG("corner.edges = ", corner.getEdgeCount() )

    //         if ( corner.edges[ 0 ] )
    //         {
    //            H3_Edge & edge = H3_getEdge( game, corner.edges[ 0 ] );
    //            if ( edge.owner > 0 )
    //            {
    //               continue;
    //            }
    //            edges.emplace_back( edge.id );
    //         }
    //      }

    //   }

    //   DEM_DEBUG("PossibleRoads.size() = ", edges.size() )
    //   for ( size_t i = 0; i < edges.size(); ++i )
    //   {
    //      DEM_DEBUG("PossibleRoads[",i,"] = ", edges[ i ] )
    //   }

    //   return edges;
}
#endif

std::vector< RoadMatch >
CPU_findBestRoad( H3_Game & game )
{
    std::vector< RoadMatch > roadMatches;

    //   PioPlayer & player = H3_getCurrentPlayer( __func__, game );

    //   DEM_DEBUG("game.round = ", game.round )
    //   DEM_DEBUG("player.id = ", player.id )
    //   DEM_DEBUG("player.name = ", player.name )
    //   DEM_DEBUG("player.farms.size() = ", player.farms.size() )

    game.computeVisibleEdges( game.m_player );

    if ( game.m_round < 4 )
    {

    }

    for ( size_t i = 0; i < game.m_edges.size(); ++i )
    {
        H3_Edge & edge = game.m_edges[ i ];
        //DEM_DEBUG("edge[",i,"].id = ", edge.id )

        RoadMatch match;
        match.edge = edge.id;
        match.prob = 1.0; // make distance dependent smaller
        roadMatches.emplace_back( std::move( match ) );
    }

    std::sort( roadMatches.begin(), roadMatches.end(),
              [] ( RoadMatch const & a, RoadMatch const & b )
              {
                  return a.prob > b.prob;
              } );


    //   DEM_DEBUG("RoadMatches.size() = ", roadMatches.size() )
    //   for ( size_t i = 0; i < std::min( roadMatches.size(), size_t(10) ); ++i )
    //   {
    //      DEM_DEBUG("RoadMatches[",i,"] = ", roadMatches[ i ].toString() )
    //   }


    return roadMatches;
}

void CPU_doRound2( H3_Game & game )
{
    auto& player = game.getCurrentPlayer();

    std::vector< FarmMatch > farmMatches = CPU_findBestFarm( game );

    if ( !player.didBuyFarm )
    {
        for ( size_t i = 0; i < farmMatches.size(); ++i )
        {
            FarmMatch & match = farmMatches[ i ];
            if ( match.corner )
            {
                // DEM_DEBUG("BuyFarm with Match ", match.toString() )
                game.doBuyFarm();
                H3_Corner & corner = H3_getCorner( game, __func__, match.corner );
                game.finalizeBuyFarm( corner );

                if ( game.m_state == H3_State::Idle )
                {
                    break;
                }
            }
        }

    }

    std::vector< RoadMatch > roadMatches = CPU_findBestRoad( game );

    if ( !player.didBuyRoad )
    {
        for ( size_t i = 0; i < roadMatches.size(); ++i )
        {
            RoadMatch & match = roadMatches[ i ];
            if ( match.edge )
            {
                // DEM_DEBUG("BuyRoad with Match ", match.toString() )
                game.doBuyRoad();
                H3_Edge & edge = H3_getEdge( game, __func__, match.edge );
                game.finalizeBuyRoad( edge );
                if ( game.m_state == H3_State::Idle )
                {
                    break;
                }
            }
        }

    }

    game.doEndTurn();
}

void CPU_doRound3( H3_Game & game )
{
    CPU_doRound2( game );
}

void CPU_doRoundN( H3_Game & game )
{
    game.doDice();

    auto& player = game.getCurrentPlayer();

    if ( player.state.bank.canBuyFarm() )
    {
        std::vector< FarmMatch > farmMatches = CPU_findBestFarm( game );

        for ( size_t i = 0; i < farmMatches.size(); ++i )
        {
            FarmMatch & match = farmMatches[ i ];
            if ( match.corner )
            {
                // DEM_DEBUG("BuyFarm with Match ", match.toString() )
                game.doBuyFarm();
                H3_Corner & corner = H3_getCorner( game, __func__, match.corner );
                game.finalizeBuyFarm( corner );

                if ( game.m_state == H3_State::Idle )
                {
                    break;
                }
            }
        }
    }

    if ( player.state.bank.canBuyRoad() &&
        player.state.roads.size() < 2*player.state.farms.size() )
    {
        std::vector< RoadMatch > roadMatches = CPU_findBestRoad( game );

        for ( size_t i = 0; i < roadMatches.size(); ++i )
        {
            RoadMatch & match = roadMatches[ i ];
            if ( match.edge )
            {
                // DEM_DEBUG("BuyRoad with Match ", match.toString() )
                game.doBuyRoad();
                H3_Edge & edge = H3_getEdge( game, __func__, match.edge );
                game.finalizeBuyRoad( edge );

                if ( game.m_state == H3_State::Idle )
                {
                    break;
                }
            }
        }

    }

    if ( player.state.bank.canBuyCard())
    {
        int a = player.state.oldKnightCards.size();
        int b = player.state.oldPointCards.size();
        int c = player.state.oldInventCards.size();
        int d = player.state.oldMonoplCards.size();
        int e = player.state.farms.size();
        int f = player.state.citys.size();
        if (a + b + c + d < e + f)
        {
            game.doBuyCard();
        }
    }

    game.doEndTurn();
}

void CPU_work( H3_Game & game )
{
    auto& player = game.getCurrentPlayer();
    if ( player.typ == H3_PlayerType::HUMAN )
    {
        return;
    }

    if ( game.m_round < 1 )
    {
        DE_ERROR("No valid round")
        return;
    }
    else if ( game.m_round == 1 )
    {
        CPU_doRound1( game);
    }
    else if ( game.m_round == 2 )
    {
        CPU_doRound2( game );
    }
    else if ( game.m_round == 3 )
    {
        CPU_doRound3( game );
    }
    else
    {
        CPU_doRoundN( game );
    }
}
