#pragma once
#include <H3/H3_Board.h>
#include <H3/details/H3_AI_StartPlacement.h>
#include <H3/details/H3_AI_Round2.h>

struct AI_Data
{
    u32 playerId;
    // Determines AI player behaviour
    // 0 = Looks for 5 resource types and 6 tiles, prios to build farms
    // 1 = Looks for all resource types - 1, and Port3v1
    // 2 = Looks for all resource types - 2, and Port2v1 of
    int strategy;

    int handicap; // Determines AI player stupidity
    int cheating; // Determines AI player advantages

    std::vector<BestesStartPaar> bestePaare;

    AI_Data()
        : playerId{ 0 }
        , strategy{ 0 }  // Determines AI player behaviour
        , handicap{ 0 }  // Determines AI player stupidity
        , cheating{ 0 }  // Determines AI player advantages
    {
    }
};


struct H3_Game;

void CPU_doRound1( H3_Game & game );
void CPU_doRound2( H3_Game & game );
void CPU_doRound3( H3_Game & game );
void CPU_doRoundN( H3_Game & game );

void AI_donateCardsToRobber( H3_Game & game, int playerId );








float H3_getDiceChance( int diceSum );

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


