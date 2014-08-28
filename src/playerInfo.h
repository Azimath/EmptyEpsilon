#ifndef PLAYER_INFO_H
#define PLAYER_INFO_H

#include "engine.h"
#include "playerSpaceship.h"

class PlayerInfo;
class GameGlobalInfo;
extern P<GameGlobalInfo> gameGlobalInfo;
extern P<PlayerInfo> my_player_info;
extern P<PlayerSpaceship> my_spaceship;
extern PVector<PlayerInfo> playerInfoList;

enum ECrewPosition
{
    helmsOfficer,
    weaponsOfficer,
    engineering,
    scienceOfficer,
    commsOfficer,
    singlePilot,
    max_crew_positions
};

class GameGlobalInfo : public MultiplayerObject
{
public:
    static const int maxPlayerShips = 32;
private:
    int victory_faction;
    int32_t playerShipId[maxPlayerShips];
public:
    GameGlobalInfo();

    P<PlayerSpaceship> getPlayerShip(int index);
    void setPlayerShip(int index, P<PlayerSpaceship> ship);

    int findPlayerShip(P<PlayerSpaceship> ship);
    int insertPlayerShip(P<PlayerSpaceship> ship);
    
    void setVictory(string faction_name) { victory_faction = FactionInfo::findFactionId(faction_name); }
    int getVictoryFactionId() { return victory_faction; }
};

class PlayerInfo : public MultiplayerObject
{
public:
    int32_t clientId;

    bool crew_position[max_crew_positions];
    bool main_screen_control;
    int32_t ship_id;

    PlayerInfo();

    bool isMainScreen();
    void setCrewPosition(ECrewPosition position, bool active);
    void setShipId(int32_t id);
    void setMainScreenControl(bool control);
    virtual void onReceiveCommand(int32_t clientId, sf::Packet& packet);
};

string getCrewPositionName(ECrewPosition position);

#endif//PLAYER_INFO_H
