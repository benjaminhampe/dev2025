#pragma once
#include <H3/H3_Board.h>

void AI_Round1( H3_Game & game );

/*

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
