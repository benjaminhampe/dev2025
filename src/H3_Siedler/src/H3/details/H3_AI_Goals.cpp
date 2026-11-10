#include <H3/details/H3_AI_Goals.h>
#include <H3/H3_Game.h>
#include <H3/details/H3_Topology.h>

GetCardGoal::GetCardGoal(H3_Game* game, u32 playerId, int prio)
    : Goal{ game, playerId, prio }
{}

GetCardGoal::~GetCardGoal()
{}

bool
GetCardGoal::canReachGoal() const
{
    const H3_Player& player = m_game->getPlayer(m_playerId);
    if (player.state.bank.A > 0) return true;



    return false;
}

bool
GetCardGoal::reachGoal()
{
    return false;
}
