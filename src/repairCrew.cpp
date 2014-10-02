#include "repairCrew.h"

const static int16_t CMD_SET_TARGET_POSITION = 0x0000;

PVector<RepairCrew> repairCrewList;

REGISTER_MULTIPLAYER_CLASS(RepairCrew, "RepairCrew");
RepairCrew::RepairCrew()
: MultiplayerObject("RepairCrew")
{
    ship_id = -1;
    position.x = -1;
    action = RC_Idle;

    registerMemberReplication(&ship_id);
    registerMemberReplication(&position, 1.0);
    registerMemberReplication(&target_position);

    repairCrewList.push_back(this);
}

struct PathNode
{
    ERepairCrewAction arrive_action;
    bool right, down;
};

ERepairCrewAction pathFind(sf::Vector2i start_pos, sf::Vector2i target_pos, P<ShipTemplate> t)
{
    sf::Vector2i size = t->interiorSize();
    PathNode node[size.x][size.y];
    memset(node, 0, sizeof(PathNode) * size.x * size.y);
    for(unsigned int n=0; n<t->rooms.size(); n++)
    {
        for(int x=0; x<t->rooms[n].size.x-1; x++)
        {
            node[t->rooms[n].position.x + x][t->rooms[n].position.y + t->rooms[n].size.y - 1].right = true;
            for(int y=0; y<t->rooms[n].size.y-1; y++)
            {
                node[t->rooms[n].position.x + x][t->rooms[n].position.y + y].right = true;
                node[t->rooms[n].position.x + x][t->rooms[n].position.y + y].down = true;
            }
        }
        for(int y=0; y<t->rooms[n].size.y-1; y++)
            node[t->rooms[n].position.x + t->rooms[n].size.x - 1][t->rooms[n].position.y + y].down = true;
    }
    for(unsigned int n=0; n<t->doors.size(); n++)
    {
        if (t->doors[n].horizontal)
        {
            node[t->doors[n].position.x][t->doors[n].position.y - 1].down = true;
        }else{
            node[t->doors[n].position.x - 1][t->doors[n].position.y].right = true;
        }
    }

    std::vector<sf::Vector2i> search_points;
    search_points.push_back(start_pos);
    while(search_points.size() > 0)
    {
        sf::Vector2i pos = search_points[0];
        if (pos == target_pos)
            return node[pos.x][pos.y].arrive_action;
        search_points.erase(search_points.begin());

        if (node[pos.x][pos.y].right && node[pos.x + 1][pos.y].arrive_action == RC_Idle)
        {
            node[pos.x + 1][pos.y].arrive_action = node[pos.x][pos.y].arrive_action;
            if (node[pos.x + 1][pos.y].arrive_action == RC_Idle) node[pos.x + 1][pos.y].arrive_action = RC_MoveRight;
            search_points.push_back(sf::Vector2i(pos.x + 1, pos.y));
        }
        if (pos.x > 0 && node[pos.x - 1][pos.y].right && node[pos.x - 1][pos.y].arrive_action == RC_Idle)
        {
            node[pos.x - 1][pos.y].arrive_action = node[pos.x][pos.y].arrive_action;
            if (node[pos.x - 1][pos.y].arrive_action == RC_Idle) node[pos.x - 1][pos.y].arrive_action = RC_MoveLeft;
            search_points.push_back(sf::Vector2i(pos.x - 1, pos.y));
        }
        if (node[pos.x][pos.y].down && node[pos.x][pos.y + 1].arrive_action == RC_Idle)
        {
            node[pos.x][pos.y + 1].arrive_action = node[pos.x][pos.y].arrive_action;
            if (node[pos.x][pos.y + 1].arrive_action == RC_Idle) node[pos.x][pos.y + 1].arrive_action = RC_MoveDown;
            search_points.push_back(sf::Vector2i(pos.x, pos.y + 1));
        }
        if (pos.y > 0 && node[pos.x][pos.y - 1].down && node[pos.x][pos.y - 1].arrive_action == RC_Idle)
        {
            node[pos.x][pos.y - 1].arrive_action = node[pos.x][pos.y].arrive_action;
            if (node[pos.x][pos.y - 1].arrive_action == RC_Idle) node[pos.x][pos.y - 1].arrive_action = RC_MoveUp;
            search_points.push_back(sf::Vector2i(pos.x, pos.y - 1));
        }
    }

    return RC_Idle;
}

void RepairCrew::update(float delta)
{
    P<PlayerSpaceship> ship;
    if (game_server)
        ship = game_server->getObjectById(ship_id);
    else if (game_client)
        ship = game_client->getObjectById(ship_id);
    else
    {
        destroy();
        return;
    }


    if (game_server && !ship)
    {
        destroy();
        return;
    }
    if (!ship || !ship->ship_template)
        return;

    if (position.x < -0.5)
    {
        int n=irandom(0, ship->ship_template->rooms.size() - 1);
        position = sf::Vector2f(ship->ship_template->rooms[n].position + sf::Vector2i(irandom(0, ship->ship_template->rooms[n].size.x - 1), irandom(0, ship->ship_template->rooms[n].size.y - 1)));
        target_position = sf::Vector2i(position);
    }

    action_delay -= delta;
    sf::Vector2i pos = sf::Vector2i(position + sf::Vector2f(0.5, 0.5));
    switch(action)
    {
    case RC_Idle:
        {
            action_delay = 1.0 / move_speed;
            if (pos != target_position)
                action = pathFind(pos, target_position, ship->ship_template);
            position = sf::Vector2f(pos);

            ESystem system = ship->ship_template->getSystemAtRoom(pos);
            if (system != SYS_None)
            {
                ship->systems[system].health += repair_per_second * delta;
                if (ship->systems[system].health > 1.0)
                    ship->systems[system].health = 1.0;
            }
            if (ship->auto_repair_enabled && pos == target_position && (system == SYS_None || !ship->hasSystem(system) || ship->systems[system].health == 1.0))
            {
                int n=irandom(0, SYS_COUNT - 1);

                if (ship->hasSystem(ESystem(n)) && ship->systems[n].health < 1.0)
                {
                    for(unsigned int idx=0; idx<ship->ship_template->rooms.size(); idx++)
                    {
                        if (ship->ship_template->rooms[idx].system == ESystem(n))
                        {
                            target_position = ship->ship_template->rooms[idx].position + sf::Vector2i(irandom(0, ship->ship_template->rooms[idx].size.x - 1), irandom(0, ship->ship_template->rooms[idx].size.y - 1));
                        }
                    }
                }
            }
        }
        break;
    case RC_MoveLeft:
        position.x -= delta * move_speed;
        if (action_delay < 0.0)
            action = RC_Idle;
        break;
    case RC_MoveRight:
        position.x += delta * move_speed;
        if (action_delay < 0.0)
            action = RC_Idle;
        break;
    case RC_MoveUp:
        position.y -= delta * move_speed;
        if (action_delay < 0.0)
            action = RC_Idle;
        break;
    case RC_MoveDown:
        position.y += delta * move_speed;
        if (action_delay < 0.0)
            action = RC_Idle;
        break;
    }
}

void RepairCrew::onReceiveClientCommand(int32_t clientId, sf::Packet& packet)
{
    int16_t command;
    packet >> command;
    switch(command)
    {
    case CMD_SET_TARGET_POSITION:
        packet >> target_position;
        break;
    }
}

void RepairCrew::commandSetTargetPosition(sf::Vector2i position)
{
    sf::Packet packet;
    packet << CMD_SET_TARGET_POSITION << position;
    sendClientCommand(packet);
}

PVector<RepairCrew> getRepairCrewFor(P<PlayerSpaceship> ship)
{
    PVector<RepairCrew> ret;
    if (!ship)
        return ret;

    foreach(RepairCrew, c, repairCrewList)
        if (c->ship_id == ship->getMultiplayerId())
            ret.push_back(c);
    return ret;
}
