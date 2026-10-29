#pragma once
#include <H3/H3_Board.h>


// Speichert das beste gefundene Knoten-Paar für die zwei Start-Farmen
struct BestesStartPaar
{
    u32 cornerX = 0;
    u32 cornerY = 0;
    double score = 0.0;

    std::string str() const
    {
        std::ostringstream o; o <<
        "x(" << cornerX << "), "
        "y(" << cornerY << "), "
        "score(" << score << ")";
        return o.str();
    }
};

std::vector<BestesStartPaar>
FindeBesteStartPlaetze(const H3_Game& game, const uint32_t playerId);
