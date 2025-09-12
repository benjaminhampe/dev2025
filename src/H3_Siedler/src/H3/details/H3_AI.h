#pragma once
#include <H3/H3_Board.h>

struct H3_Game;

void AI_donateCardsToRobber( H3_Game & game, int playerId );

float H3_getDiceChance( int diceSum );

void CPU_doRound1( H3_Game & game );

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
CPU_findBestFarm( H3_Game & game );

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
CPU_findBestRoad( H3_Game & game );

void CPU_doRound2( H3_Game & game );

void CPU_doRound3( H3_Game & game );

void CPU_doRoundN( H3_Game & game );

void CPU_work( H3_Game & game );
