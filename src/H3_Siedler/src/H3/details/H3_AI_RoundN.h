#pragma once
#include <H3/H3_Board.h>

// Compute how many cards can be exchanged through banking
// param[in] player. Queries bank and ports/banking ratio.
// param[in] cost <optional>. Initial cost that will be substracted before the computation.
// return[out] Each resource type will contain the max number of possible bankings.
// The bank.sum() is total number of new resource cards poss. through banking.
H3_Bank H3_computeBankings( const H3_Player& player, const H3_Bank& cost = {} );

// Compute how many resource cards are missing to buy the cost
H3_Bank H3_computeMissingCards( const H3_Player& player, const H3_Bank& cost );

// Compute possible bank after banking resource cards to cover the cost
// A non empty bank indicates it is possible to buy the cost with banking.
//
// Banking|A|B|C|D|E
//--------+-+-+-+-+-------------------------
//    have|2|1|3|0|6 + Port 3:1
//    cost|1|1|1|1|0
// missing|0|0|0|1|0 -> sum() == 1
// testing|1|0|2|0|6
//  ratios|3|3|3|3|3
// options|0|0|0|0|2 -> sum() == 2 -> We can buy after banking E 3:1 for D
//--------+-+-+-+-+-------------------------
// banking|1|1|3|1|3 -> After banking
H3_Bank H3_couldBuy(const H3_Game & game, u32 playerId, const H3_Bank& cost);

// Single wishType card is exchanged for enough giveType cards.
// Amount of giveType cards are automatically computed and substracted.
// A successful transaction returns true, triggers game animations and similar.
bool H3_exchangeCardWithBank( H3_Game & game, u32 playerId, H3_TileType giveType, H3_TileType wishType );

// Automatic banking to reach a possible way to pay the given cost.
bool H3_autoBanking(H3_Game & game, u32 playerId, const H3_Bank& cost);


// Determine if player could buy something
// taking (4:1, 3:1, 2:1) banking into account.

// H3_Bank H3_canBankBuy( H3_Game & game, u32 playerId, const H3_Bank& cost );
// H3_Bank H3_couldBuyRoad( H3_Game & game, u32 playerId );
// H3_Bank H3_couldBuyFarm( H3_Game & game, u32 playerId );
// H3_Bank H3_couldBuyCity( H3_Game & game, u32 playerId );
// H3_Bank H3_couldBuyCard( H3_Game & game, u32 playerId );



void AI_RoundN( H3_Game & game );


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
