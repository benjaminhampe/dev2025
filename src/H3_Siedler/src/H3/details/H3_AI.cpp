#include <H3/details/H3_AI.h>
#include <H3/H3_Game.h>
#include <H3/details/H3_Topology.h>

void CPU_doRound1( H3_Game & game )
{
    const auto & player = game.getCurrentPlayer();
    if (player.typ != H3_PlayerType::CPU)
    {
        DE_ERROR("No cpu player ", player.id)
    }

    if (game.m_bDebugLogic)
    {
        DE_DEBUG("CPU[",player.id,"] ",player.name)
    }
    game.doDice();
    game.doEndTurn();
}

/*
struct CornerRating
{
    u32 m_cornerId = 0;
    int m_strategy = 0;
    float m_rating = 0.0f;

    // float D = 0.0f;   // Sum of dice chances of all tiles around corner
    // int T = 0;   // Rating neighbor tiles [1,2,3]
    // int U = 0;   // Rating unique resources [1,2,3]
    // int F = 0;   // Rating to build farms [A=2,B=2,C=1,D=1,E=0]

    static CornerRating fromCornerId( H3_Game & game, u32 cornerId, int strat = 0)
    {
        CornerRating cr;
        H3_Bank bank = H3_countResourceTilesOfCorner(game, cornerId);


        {
            int rating = 0;
            rating += bank.A * 2;
            rating += bank.B * 2;
            rating += bank.C;
            rating += bank.D;
            return rating;
        }
        cr.cornerId = cornerId;
        rating.bank
        rating.D = H3_getDiceChanceOfCorner(game,cornerId);
        rating.T = rating.bank.sum();
        rating.U = rating.bank.countTypes();
        rating.F = H3_getBuildFarmRatingFromBank(rating.bank);
        return rating;
    }
};

// 1. Look for corners with 3 tiles
// 2. Prefer unique resources
// 3. Prefer resources to build farm and roads
// 4. Prefer good dice probabilities
u32 findBestInitialPlaces( H3_Game & game, int playerId )
{
    struct Candidate
    {
        u32 farm1 = 0;  // Corner for first farm
        u32 farm2 = 0;  // Corner for second farm
        u32 road1 = 0;  // Edge for first road next to first farm
        u32 road2 = 0;  // Edge for second road next to second farm
        int strat = 0;  // 0 = land, 1 = port3v1, 2 = port2v1

        float rating = 0.0f;    // Total rating


        float ratingT = 0.0f;   // Rating neighbor tiles [1,2,3]
        float ratingU = 0.0f;   // Rating unique resources [1,2,3]
        float ratingF = 0.0f;   // Rating resources to build farms [A=2,B=2,C=1,D=1,E=0]
        float ratingD = 0.0f;   // Rating dice [2, ... 12

    };

    std::vector<Candidate> cands;






    return 0;
}


*/


std::vector< FarmMatch >
CPU_findBestFarm( H3_Game & game, uint32_t playerId )
{
    H3_computeCornersForFarm( game, playerId );

    std::vector< FarmMatch > farmMatches;

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
            ttc[ tile.tt ]++;
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


std::vector< RoadMatch >
CPU_findBestRoad( H3_Game & game, uint32_t playerId )
{
    std::vector< RoadMatch > roadMatches;

    //   PioPlayer & player = H3_getCurrentPlayer( __func__, game );

    //   DEM_DEBUG("game.round = ", game.round )
    //   DEM_DEBUG("player.id = ", player.id )
    //   DEM_DEBUG("player.name = ", player.name )
    //   DEM_DEBUG("player.farms.size() = ", player.farms.size() )

    H3_computeEdgesForRoad( game, playerId );

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
    if (player.typ != H3_PlayerType::CPU)
    {
        DE_ERROR("No CPU player ", player.id)
    }

    if (game.m_bDebugLogic)
    {
        DE_DEBUG("CPU[",player.id,"] ",player.name)
    }

    AI_Data* ai_data = reinterpret_cast<AI_Data*>(player.ai_data);

    ai_data->bestePaare = FindeBesteStartPlaetze(game, player.id);
    DE_DEBUG("Player[",player.name,"] bestPairs = ", ai_data->bestePaare.size())
    for (size_t i = 0; i < ai_data->bestePaare.size(); ++i)
    {
        DE_DEBUG("Player[",player.name,"].bestPair[",i,"] ", ai_data->bestePaare[i].str())
    }

    if (ai_data->bestePaare.empty())
    {
        DE_ERROR("No best pair. ", player.name)
        return;
    }

    BestesStartPaar & bestPair = ai_data->bestePaare.front();

    // DEM_DEBUG("BuyFarm with Match ", match.toString() )
    game.doBuyFarm();
    H3_Corner & corner = H3_getCorner( game, __func__, bestPair.cornerX );
    game.finalizeBuyFarm( corner );



    // // DEM_DEBUG("BuyRoad with Match ", match.toString() )
    // game.doBuyRoad();
    // H3_Edge & edge = H3_getEdge( game, __func__, match.edge );
    // game.finalizeBuyRoad( edge );
/*


    std::vector< FarmMatch >
    farmMatches = CPU_findBestFarm( game, player.id );

    if ( player.state.farms.empty() )
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

    std::vector< RoadMatch >
    roadMatches = CPU_findBestRoad( game, player.id );

    if ( player.state.roads.empty() )
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
*/

    game.doEndTurn();
}

/*
void CPU_doRound2( H3_Game & game )
{
    auto& player = game.getCurrentPlayer();
    if (player.typ != H3_PlayerType::CPU)
    {
        DE_ERROR("No CPU player ", player.id)
    }

    if (game.m_bDebugLogic)
    {
        DE_DEBUG("CPU[",player.id,"] ",player.name)
    }

    std::vector< FarmMatch >
    farmMatches = CPU_findBestFarm( game, player.id );

    if ( player.state.farms.empty() )
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

    std::vector< RoadMatch >
    roadMatches = CPU_findBestRoad( game, player.id );

    if ( player.state.roads.empty() )
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
*/
void CPU_doRound3( H3_Game & game )
{
    auto& player = game.getCurrentPlayer();
    if (player.typ != H3_PlayerType::CPU)
    {
        DE_ERROR("No CPU player ", player.id)
    }

    if (game.m_bDebugLogic)
    {
        DE_DEBUG("CPU[",player.id,"] ",player.name)
    }

    AI_Data* ai_data = reinterpret_cast<AI_Data*>(player.ai_data);

    ai_data->bestePaare = FindeBesteStartPlaetze(game, player.id);

    if (ai_data->bestePaare.empty())
    {
        DE_ERROR("No best pair. ", player.name)
        return;
    }

    BestesStartPaar & bestPair = ai_data->bestePaare.front();

    // DEM_DEBUG("BuyFarm with Match ", match.toString() )
    game.doBuyFarm();
    H3_Corner & corner = H3_getCorner( game, __func__, bestPair.cornerY );
    game.finalizeBuyFarm( corner );

    game.doEndTurn();

}

void CPU_doRoundN( H3_Game & game )
{
    auto& player = game.getCurrentPlayer();
    if (player.typ != H3_PlayerType::CPU)
    {
        DE_ERROR("No CPU player ", player.id)
    }

    if (game.m_bDebugLogic)
    {
        DE_DEBUG("CPU[",player.id,"] ",player.name)
    }

    game.doDice();

    if ( player.state.bank.canBuyFarm() )
    {
        std::vector< FarmMatch >
        farmMatches = CPU_findBestFarm( game, player.id );

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
        std::vector< RoadMatch >
        roadMatches = CPU_findBestRoad( game, player.id );

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
        int a = player.state.knightCards.size();
        int b = player.state.pointCards.size();
        int c = player.state.inventCards.size();
        int d = player.state.monopolCards.size();
        int e = player.state.farms.size();
        int f = player.state.citys.size();
        if (a + b + c + d < e + f)
        {
            game.doBuyCard();
        }
    }

    game.doEndTurn();
}

/*
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
*/



void AI_donateCardsToRobber( H3_Game & game, int playerId )
{
    H3_Player & player = game.getPlayer( playerId );
    int cardLimit = game.m_cfg.cardLimitNoSteal;

    if (player.state.bank.sum() <= cardLimit)
    {
        DE_BENNI("Player[",player.name,"] is below CardLimit")
        return;
    }

    DE_DEBUG("")

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
            numLose--;
        }
        if (bank.D > 0 && numLose > 0)
        {
            bank.D--;
            D++;
            numLose--;
        }
        if (bank.C > 0 && numLose > 0)
        {
            bank.C--;
            C++;
            numLose--;
        }
        if (bank.B > 0 && numLose > 0)
        {
            bank.B--;
            B++;
            numLose--;
        }
        if (bank.A > 0 && numLose > 0)
        {
            bank.A--;
            A++;
            numLose--;
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
