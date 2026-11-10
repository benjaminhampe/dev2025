#pragma once
#include <H3/H3_Board.h>

struct GoalType
{
    enum eType
    {
        None = 0, PlaceThief,
        BankA, BankB, BankC, BankD, BankE,      // Get another cardA
        BuyRoad, BuyFarm, BuyCity, BuyCard,
        BonusArmy, BonusRoad,
    };
};

class IGoal
{
public:
    virtual ~IGoal() {}
    virtual void destroy() = 0;

    virtual u32 getPlayerId() const = 0;
    virtual int getGoalType() const = 0;
    virtual int getGoalPrio() const = 0;

    virtual bool canReachGoal() const = 0;
    virtual bool reachGoal() = 0;
};

class Goal : public IGoal
{
public:
    H3_Game* m_game = nullptr;
    u32 m_playerId = 0;
    int m_goalPrio = 0; // 0 = highest
    //std::vector<Goal> subgoals;

    Goal(H3_Game* game, u32 playerId, int prio)
        : m_game{ game }
        , m_playerId{ playerId }
        , m_goalPrio{ prio }
    {}

    ~Goal() override {}
    void destroy() override {}

    int getGoalType() const override { return GoalType::None; }
    int getGoalPrio() const override { return m_goalPrio; }
    u32 getPlayerId() const override { return m_playerId; }

    bool canReachGoal() const override { return true; }
    bool reachGoal() override { return false; }
};

class GetCardGoal : public Goal
{
public:
    GetCardGoal(H3_Game* game, u32 playerId, int prio);
    ~GetCardGoal() override;
    // void destroy() override;

    int getGoalType() const override { return GoalType::BankA; }
    bool canReachGoal() const override;
    bool reachGoal() override;
};

