#include "H3_Board.h"
#include <H3/details/H3_Tex.h>

void H3_Cfg::dump() const
{
    DE_DEBUG("cfg.presetName = ",presetName)
    DE_DEBUG("cfg.victoryPoints = ",victoryPoints)
    DE_DEBUG("cfg.cardLimitNoSteal = ",cardLimitNoSteal)

    DE_DEBUG("cfg.buildingSpacing = ",buildingSpacing)
    DE_DEBUG("cfg.useBonusRoad = ",useBonusRoad)
    DE_DEBUG("cfg.useBonusArmy = ",useBonusArmy)

    DE_DEBUG("cfg.useCardKnight = ",useCardKnight)
    DE_DEBUG("cfg.useCardVictoryPoint = ",useCardVictoryPoint)
    DE_DEBUG("cfg.useCardMonopol = ",useCardMonopol)
    DE_DEBUG("cfg.useCardInvention = ",useCardInvention)
    DE_DEBUG("cfg.skipFirstRound = ",skipFirstRound)

    DE_DEBUG("cfg.playerList = ",playerList.size())

    for (size_t i= 0; i < playerList.size(); ++i)
    {
        DE_DEBUG("cfg.player[",i,"] ",playerList[i].str())
    }

    DE_DEBUG("cfg.tileList = ",tileList.size())

    for (size_t i= 0; i < tileList.size(); ++i)
    {
        DE_DEBUG("cfg.tile[",i,"] ",tileList[i].str())
    }
}

std::string
H3_Cfg::str() const
{
    std::ostringstream o;
    o << "PresetName = " << presetName << "";
    return o.str();
}

void
H3_Cfg::addTile( int i, int j, H3_TileType tileType, int angle60, int diceValue )
{
    H3_CfgTile cfg;
    cfg.i = i;
    cfg.j = j;
    cfg.tileType = tileType;
    cfg.chipValue = diceValue;
    cfg.angle60 = angle60;
    tileList.push_back( std::move( cfg ) );
}

// static
H3_Cfg
H3_Cfg::createStandard()
{
    H3_Cfg cfg;

    cfg.presetName = "Standard";

    cfg.victoryPoints = 3;      // Num points to victory (Default = 10)
    cfg.cardLimitNoSteal = 7;   // Num cards player can hold without robber steal. 0 = disable
    cfg.buildingSpacing = 1;    // Num of corners between neighboring buildings, default = 1, can be 0 for more dense use of board.
    cfg.useBonusRoad = 1;       // +2 points for 5+ connected roads
    cfg.useBonusArmy = 1;       // +2 points for 3+ knights
    cfg.useCardKnight = 1;      // +1 Knight with this card
    cfg.useCardVictoryPoint = 1;// +1 Victory point with this card
    cfg.useCardMonopol = 1;     // Take all cards of one resource of all players
    cfg.useCardInvention = 1;   // +2..+4 Arbitrary resource cards from bank
    cfg.skipFirstRound = 1;     //

    cfg.addTile( -2, 3, H3_TileType::PortE, 5 );
    cfg.addTile( -1, 3, H3_TileType::Water );
    cfg.addTile( -0, 3, H3_TileType::PortD );
    cfg.addTile(  1, 3, H3_TileType::Water );

    cfg.addTile( -2, 2, H3_TileType::Water, 5 );
    cfg.addTile( -1, 2, H3_TileType::TileC, 0, 4 );
    cfg.addTile( -0, 2, H3_TileType::TileA, 0, 6 );
    cfg.addTile(  1, 2, H3_TileType::TileC, 0, 9 );
    cfg.addTile(  2, 2, H3_TileType::Port3v1, 1 );

    cfg.addTile( -3, 1, H3_TileType::PortA, 4 );
    cfg.addTile( -2, 1, H3_TileType::TileB, 0, 2 );
    cfg.addTile( -1, 1, H3_TileType::TileA, 0, 5 );
    cfg.addTile( -0, 1, H3_TileType::TileD, 0, 12 );
    cfg.addTile(  1, 1, H3_TileType::TileD, 0, 4 );
    cfg.addTile(  2, 1, H3_TileType::Water, 1 );

    cfg.addTile( -3,  0, H3_TileType::Water, 4 );
    cfg.addTile( -2,  0, H3_TileType::TileD, 0, 9 );
    cfg.addTile( -1,  0, H3_TileType::TileB, 0, 8 );
    cfg.addTile( -0,  0, H3_TileType::Desert );
    cfg.addTile(  1,  0, H3_TileType::TileE, 0, 8 );
    cfg.addTile(  2,  0, H3_TileType::TileD, 0, 10 );
    cfg.addTile(  3,  0, H3_TileType::Port3v1, 1 );

    cfg.addTile( -3, -1, H3_TileType::Port3v1, 3 );
    cfg.addTile( -2, -1, H3_TileType::TileA, 0, 3 );
    cfg.addTile( -1, -1, H3_TileType::TileE, 0, 5 );
    cfg.addTile( -0, -1, H3_TileType::TileB, 0, 10 );
    cfg.addTile(  1, -1, H3_TileType::TileA, 0, 11 );
    cfg.addTile(  2, -1, H3_TileType::Water, 2 );

    cfg.addTile( -2,-2, H3_TileType::Water, 4 );
    cfg.addTile( -1,-2, H3_TileType::TileC, 0, 3 );
    cfg.addTile( -0,-2, H3_TileType::TileC, 0, 6 );
    cfg.addTile(  1,-2, H3_TileType::TileE, 0, 11 );
    cfg.addTile(  2,-2, H3_TileType::PortC, 1 );

    cfg.addTile( -2,-3, H3_TileType::PortB, 3 );
    cfg.addTile( -1,-3, H3_TileType::Water, 3 );
    cfg.addTile( -0,-3, H3_TileType::Port3v1, 3 );
    cfg.addTile(  1,-3, H3_TileType::Water, 2 );

    {
        H3_CfgPlayer player;
        player.name = "Benni";
        player.color = dbRGB(255,0,0);
        player.typ = H3_PlayerType::HUMAN;
        player.connect = H3_PlayerConnect::LOCAL;
        player.avatar = H3_Tex::Play3;
        cfg.playerList.push_back( player );
    }

    {
        H3_CfgPlayer player;
        player.name = "Kleopatra";
        player.color = dbRGB(255,255,0);
        player.typ = H3_PlayerType::CPU;
        player.connect = H3_PlayerConnect::LOCAL;
        player.avatar = H3_Tex::Play2;
        cfg.playerList.push_back( player );
    }

    // {
    //     H3_CfgPlayer player;
    //     player.name = "Alexander";
    //     player.color = dbRGB(0,255,0);
    //     player.typ = H3_PlayerType::CPU;
    //     player.connect = H3_PlayerConnect::LOCAL;
    //     player.avatar = H3_Tex::Play3;
    //     cfg.playerList.push_back( player );
    // }

    // {
    //     H3_CfgPlayer player;
    //     player.name = "Napoleon";
    //     player.color = dbRGB(0,0,255);
    //     player.typ = H3_PlayerType::CPU;
    //     player.connect = H3_PlayerConnect::LOCAL;
    //     player.avatar = H3_Tex::Play4;
    //     cfg.playerList.push_back( player );
    // }

    /*
    cfg.cost4road.push_back({ H3_TileType::TileA, 1});
    cfg.cost4road.push_back({ H3_TileType::TileB, 1});

    cfg.cost4farm.emplace_back({{ H3_TileType::TileA, 1},
                                { H3_TileType::TileB, 1},
                                { H3_TileType::TileC, 1},
                                { H3_TileType::TileD, 1} });

    cfg.cost4city.emplace_back({{ H3_TileType::TileD, 2},
                                { H3_TileType::TileE, 3} });

    cfg.cost4card.emplace_back({{ H3_TileType::TileC, 1},
                                { H3_TileType::TileD, 1},
                                { H3_TileType::TileE, 1} });
*/
    return cfg;
}
