#include "H3_AI_ThiefPlacement.h"
#include <H3/H3_Game.h>
#include <H3/details/H3_Topology.h>
#include <vector>
#include <algorithm>

void AI_donateCardsToRobber(H3_Game & game, u32 playerId)
{
    // DE_DEBUG(game.playerStr())

    const int cardLimit = game.m_cfg.cardLimitNoSteal;

    H3_Player & player = game.getPlayer( playerId );

    if (player.state.bank.sum() <= cardLimit)
    {
        DE_BENNI(game.playerStr(),"is below CardLimit")
        return;
    }

    H3_Bank bank = player.state.bank;

    bool bWantBuyRoad = true;
    bool bWantBuyFarm = true;
    bool bWantBuyCard = true;
    bool bWantBuyCity = true;

    const int maxLose = player.state.bank.sum() / 2;
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

    // DE_OK(game.playerStr(),"before Bank = ", player.state.bank.str())
    // DE_OK(game.playerStr(),"donate Bank = ", bank.str())

    if (!player.state.bank.buy( bank ))
    {
        DE_ERROR(game.playerStr(),"Cannot donate ", bank.str())
    }
    else
    {
        DE_DEBUG(game.playerStr(),"donated ", bank.str(), " to thief.")
    }
    // DE_OK(game.playerStr(),"robbed Bank = ", player.state.bank.str())
}

void AI_ThiefPlacement(H3_Game& game, u32 playerId)
{
    DE_DEBUG(game.playerStr())

    // ===================================================
    // PlacementThief: Score Data
    // ===================================================

    struct Candidate
    {
        float score = 0.0f;
        u32 tileId = 0;
        u32 diceValue = 0;
        bool bHasPlayer = false;
        std::vector<u32> owners;

        bool hasPlayer(u32 playerId) const
        {
            auto found = std::find_if(owners.begin(), owners.end(),
                [=] (u32 owner) { return owner == playerId; });
            return found != owners.end();
        }

        void computeScore(u32 playerId)
        {
            int ownerScore = int(owners.size());
            bHasPlayer = hasPlayer(playerId);
            if (bHasPlayer)
            {
                ownerScore -= 1;
            }
            score = ownerScore * H3_getDiceChance(diceValue);
        }

        bool operator< (const Candidate & other) const
        {
            return (score < other.score);
        }

        std::string str() const
        {
            std::ostringstream o; o <<
            "score(" << score << "), "
            "id(" << tileId << "), "
            "dice(" << diceValue << "), "
            "chance(" << H3_getDiceChance(diceValue) << "), "
            "hasPlayer(" << bHasPlayer << "), "
            "owners(" << H3_Util::str(owners) << ")";
            return o.str();
        }
    };

    // ===================================================
    // PlacementThief: Gather candidates
    // ===================================================

    std::vector<Candidate> candidates;
    candidates.reserve( game.m_tiles.size() );

    u32 thiefTileId = game.m_thief.occupiedTileId;

    for (const H3_Tile& tile : game.m_tiles)
    {
        if (tile.id == thiefTileId) continue;
        if (tile.tt.isWaterOrPort()) continue;

        candidates.emplace_back();
        Candidate & c = candidates.back();
        c.tileId = tile.id;
        c.diceValue = tile.chipValue;
        for (u32 cornerId : tile.corners)
        {
            const H3_Corner& corner = H3_getCorner(game,__func__,cornerId);
            if (corner.owner > 0)
            {
                H3_Util::push_back_unique(c.owners, corner.owner);
            }
        }
        c.computeScore(playerId);
    }

    std::sort( candidates.begin(), candidates.end() );

    // ===================================================
    // PlacementThief: Decide
    // ===================================================

// <debug>
    DE_DEBUG(game.playerStr(),"Candidates = ", candidates.size())

    size_t firstTen = std::min<size_t>(10,candidates.size());
    for (size_t i = 0; i < firstTen; ++i)
    {
        DE_DEBUG(game.playerStr(),"Candidate[",i,"] ", candidates[i].str())
    }
// </debug>

    if (firstTen == 0)
    {
        auto s = dbStr(game.playerStr(),"firstTen = 0");
        throw std::runtime_error(s);
    }

    int index = size_t( rand() ) % firstTen;
    const Candidate& decided = candidates[ index ];

// <debug>
    DE_DEBUG(game.playerStr(),"decided for candidate ", index)
// </debug>

    // ===================================================
    // Place Thief: Finally
    // ===================================================
    H3_Tile & tile = H3_getTile(game,__func__,decided.tileId);
    game.m_thief.occupiedTileId = tile.id;
    game.m_thief.trs.pos = tile.pos + glm::vec3(20,0,-30);
    game.m_thief.trs.update();

    // ===================================================
    // Steal Random Card:
    // ===================================================

    // Get list of owners that are not the current player.
    std::vector<u32> owners;
    owners.reserve(game.m_players.size());

    for (u32 cornerId : tile.corners )
    {
        H3_Corner & corner = H3_getCorner(game, __func__, cornerId);
        // Owner must exist
        if (corner.owner < 1) continue;
        // Owner must not be current player
        if (corner.owner == playerId) continue;
        // Owner must not be in list of unique owners already..., could use a set.
        H3_Util::push_back_unique(owners, corner.owner);
    }

// <debug>
    DE_DEBUG(game.playerStr(),"can steal from ", H3_Util::str(owners))
// </debug>

    if (owners.empty())
    {
        DE_DEBUG(game.playerStr(),"nothing to steal")
    }
    else if (owners.size() == 1)
    {
        DE_DEBUG(game.playerStr(),"auto steals from ",game.getPlayer(owners[0]).name)
        game.stealRandomCard( owners[0], playerId );
    }
    else
    {
        int n = owners.size();
        int i = rand() % n;
        DE_DEBUG(game.playerStr(),"random-steals from ",game.getPlayer(owners[i]).name)
        game.stealRandomCard( owners[i], playerId );
    }
}

