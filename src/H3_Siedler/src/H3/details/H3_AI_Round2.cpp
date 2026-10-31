#include "H3_AI_Round2.h"
#include <H3/H3_Game.h>
#include <H3/details/H3_Topology.h>
#include <vector>
#include <algorithm>

namespace {

struct RatedCorner
{
    u32 cornerId = 0;
    f32 rating = 0.0f;
    // float ratingT = 0.0f;   // Rating neighbor tiles [1,2,3]
    // float ratingU = 0.0f;   // Rating unique resources [1,2,3]
    // float ratingF = 0.0f;   // Rating resources to build farms [A=2,B=2,C=1,D=1,E=0]
    // float ratingD = 0.0f;   // Rating dice [2, ... 12

    std::string str() const
    {
        std::ostringstream o; o <<
        "cornerId(" << cornerId << "), "
        "rating(" << rating << ")";
        return o.str();
    }
};

} // end namespace.

// Hauptfunktion: Gibt die IDs der zwei besten Knoten für die ersten 2 Farmen zurück
void AI_Round2(H3_Game& game, const uint32_t playerId)
{
    H3_Player & player = game.getPlayer(playerId);

    std::vector<u32> visibleCorners = H3_computeVisibleCorners(game, playerId);

    DE_DEBUG("Player[",playerId,"] visibleCorners = ",visibleCorners.size())
    for (size_t i = 0; i < std::min<size_t>(10,visibleCorners.size()); ++i)
    {
        DE_DEBUG("Player[",playerId,"] visibleCorners[",i,"] ",visibleCorners[i])
    }

    std::vector<RatedCorner> candidates;
    candidates.reserve( visibleCorners.size() );

    for (u32 cornerId : visibleCorners)
    {
        H3_Corner & corner = H3_getCorner(game, __func__, cornerId);

        candidates.emplace_back();
        RatedCorner & candidate = candidates.back();
        candidate.cornerId = cornerId;
        candidate.rating = 0.0f;

        for (u32 tileId : corner.tiles)
        {
            H3_Tile & tile = H3_getTile(game, __func__, tileId);

            float weight = 0.0f;
            switch (tile.tt)
            {
                case H3_TileType::TileA: weight = 1.2f; break;
                case H3_TileType::TileB: weight = 1.2f; break;
                case H3_TileType::TileC: weight = 0.8f; break;
                case H3_TileType::TileD: weight = 0.8f; break;
                case H3_TileType::TileE: weight = 0.5f; break;
                default: break;
            }
            float perTileRating = weight * tile.getDiceChance();

            candidate.rating += perTileRating;
        }
    }

    std::sort( candidates.begin(), candidates.end(),
              [] ( const RatedCorner & a, const RatedCorner & b )
              {
                  return a.rating > b.rating;
              } );

    DE_DEBUG("Player[",playerId,"] candidates = ",candidates.size())
    for (size_t i = 0; i < std::min<size_t>(10,candidates.size()); ++i)
    {
        DE_DEBUG("Player[",playerId,"] candidate[",i,"] ",candidates[i].str())
    }

    // DEM_DEBUG("BuyFarm with Match ", match.toString() )
    game.doBuyFarm();
    RatedCorner& candidate = candidates.front();
    H3_Corner & corner = H3_getCorner( game, __func__, candidate.cornerId );
    game.finalizeBuyFarm( corner );


    // Buy Road

    game.enterIdle();


    std::vector<u32> roadCandidates = H3_computeVisibleEdges(game, playerId);

    DE_DEBUG("Player[",playerId,"] roadCandidates = ",roadCandidates.size())
    for (size_t i = 0; i < std::min<size_t>(10,roadCandidates.size()); ++i)
    {
        u32 edgeId = roadCandidates[i];
        H3_Edge & edge = H3_getEdge(game, __func__, edgeId);
        DE_DEBUG("Player[",playerId,"] roadCandidates[",i,"] ",edgeId)
    }

    game.doBuyRoad();
    H3_Edge & edge = H3_getEdge( game, __func__, roadCandidates.front() );
    game.finalizeBuyRoad( edge );

    game.doEndTurn();

}

// Hauptfunktion: Gibt die IDs der zwei besten Knoten für die ersten 2 Farmen zurück
void AI_Round3(H3_Game& game, const uint32_t playerId)
{
    AI_Round2(game, playerId);
}
/*
// Hauptfunktion zur Ermittlung der besten Kante für die Start-Straße
Kante FindeBesteStartStrasse(const H3_Corner& startFarm,
                             const std::vector<Knoten>& weltKnoten,
                             const StrategieGewichte& kiGewichte,
                             bool suchtHafen,
                             int zielHafenTyp)
{
    Kante besteKante;
    besteKante.vonKnotenID = startFarm.id;
    besteKante.zuKnotenID = -1;
    double besterKantenScore = -1.0;

    // Die KI prüft alle direkt angrenzenden Wege (Nachbarknoten)
    for (int nachbarID : startFarm.nachbarKnotenIDs)
    {
        const Knoten& zielKnoten = weltKnoten[nachbarID];

        // 1. Basis-Ressourcenwert des Zielknotens berechnen
        double aktuellerScore = BerechneEinzelKnotenBasisScore(zielKnoten, kiGewichte);

        // 2. Strategischen Richtungs-Bonus addieren
        double richtungsBonus = 0.0;

        if (zielKnoten.hatHafen)
        {
            if (suchtHafen && zielKnoten.hafenTyp == zielHafenTyp)
            {
                // Passender Spezialhafen (z.B. Port2v1 für das Monopol)
                richtungsBonus = 10.0;
            }
            else if (zielKnoten.hafenTyp == 0)
            {
                // Allgemeiner 3:1 Hafen
                richtungsBonus = 6.0;
            }
        }

        double gesamtKantenScore = aktuellerScore + richtungsBonus;

        // Besten Weg loggen
        if (gesamtKantenScore > besterKantenScore)
        {
            besterKantenScore = gesamtKantenScore;
            besteKante.zuKnotenID = nachbarID;
        }
    }

    return besteKante;
}
*/

#if 0
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

#endif
