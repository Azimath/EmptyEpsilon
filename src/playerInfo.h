#ifndef PLAYER_INFO_H
#define PLAYER_INFO_H

#include "engine.h"
#include "spaceship.h"

class PlayerInfo;
class GameGlobalInfo;
extern P<GameGlobalInfo> game_global_info;
extern P<PlayerInfo> my_player_info;
extern P<SpaceShip> my_spaceship;
extern PVector<PlayerInfo> player_info_list;

enum ECrewPosition
{
    helmsOfficer,
    tacticalOfficer,
    engineering,
    scienceOfficer,
    commsOfficer,
    max_crew_positions
};

class GameGlobalInfo : public MultiplayerObject
{
public:
    static const int max_player_ships = 32; //Cause we had to set it at something.
private:
    int32_t player_ship_id[max_player_ships];
public:
    GameGlobalInfo();

    P<SpaceShip> getPlayerShip(int index);
    void setPlayerShip(int index, P<SpaceShip> ship);

    int findPlayerShip(P<SpaceShip> ship);
    int insertPlayerShip(P<SpaceShip> ship);
};

class PlayerInfo : public MultiplayerObject
{
public:
    int32_t client_id;

    bool crewPosition[max_crew_positions];

    PlayerInfo();

    bool isMainScreen();
    void setCrewPosition(ECrewPosition position, bool active);
    virtual void onReceiveCommand(int32_t client_id, sf::Packet& packet);
};

string getCrewPositionName(ECrewPosition position);

#endif//PLAYER_INFO_H
