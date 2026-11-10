#pragma once
#include <H3/H3_Board.h>

bool H3_bankingTransaction( H3_Game & game, u32 playerId, H3_TileType giveType, H3_TileType wishType );

// Compute how many cards can be exchanged through banking
/// param[in] player. Queries bank and ports/banking ratio.
/// param[in] cost <optional>. Initial cost that will be substracted before the computation.
/// return[out] Each resource type will contain the max number of possible bankings.
/// The bank.sum() is total number of new resource cards poss. through banking.
H3_Bank H3_computeBankings( const H3_Player& player, const H3_Bank& cost = {} );

// Determine if player could buy something
// taking (4:1, 3:1, 2:1) banking into account.

// H3_Bank H3_canBankBuy( H3_Game & game, u32 playerId, const H3_Bank& cost );
// H3_Bank H3_couldBuyRoad( H3_Game & game, u32 playerId );
// H3_Bank H3_couldBuyFarm( H3_Game & game, u32 playerId );
// H3_Bank H3_couldBuyCity( H3_Game & game, u32 playerId );
// H3_Bank H3_couldBuyCard( H3_Game & game, u32 playerId );



void CPU_doRoundN( H3_Game & game );


/*
H3_Bank H3_couldBuyRoad( H3_Game & game, u32 playerId )
{
    return H3_couldBuy( game, playerId, H3_Bank::costOfRoad() );
}

H3_Bank H3_couldBuyFarm( H3_Game & game, u32 playerId )
{
    return H3_couldBuy( game, playerId, H3_Bank::costOfFarm() );
}

H3_Bank H3_couldBuyCity( H3_Game & game, u32 playerId )
{
    return H3_couldBuy( game, playerId, H3_Bank::costOfCity() );
}

H3_Bank H3_couldBuyCard( H3_Game & game, u32 playerId )
{
    return H3_couldBuy( game, playerId, H3_Bank::costOfCard() );
}

struct FarmMatch
{
    int corner; // cornerId
    float prob;

    std::string
    toString() const
    {
        std::stringstream s; s <<
            "cornerId:" << corner << ", "
                                     "probability:" << prob << ""
            ;
        return s.str();
    }
};

std::vector< FarmMatch >
CPU_findBestFarm( H3_Game & game, uint32_t playerId );

struct RoadMatch
{
    int edge = 0;
    float prob = 0.0f;

    std::string
    toString() const
    {
        std::stringstream s;
        s << "edgeId:" << edge << ", "
                                  "probability:" << prob;
        return s.str();
    }
};

//void H3_updatePossibleEdges( H3_Game & game );

std::vector< RoadMatch >
CPU_findBestRoad( H3_Game & game, uint32_t playerId );



*/
