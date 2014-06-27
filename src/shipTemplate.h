#ifndef SHIP_TEMPLATE_H
#define SHIP_TEMPLATE_H

#include <map>
#include "engine.h"

const static int maxBeamWeapons = 16;
const static int maxWeaponTubes = 16;

enum EMissileWeapons
{
    MW_None = -1,
    MW_Homing = 0,
    MW_Nuke,
    MW_Mine,
    MW_EMP,
    MW_Count
};
/* Define script conversion function for the EMissileWeapons enum. */
template<> void convert<EMissileWeapons>::param(lua_State* L, int& idx, EMissileWeapons& es);

enum ESystem
{
    SYS_None = -1,
    SYS_Reactor = 0,
    SYS_BeamWeapons,
    SYS_MissileSystem,
    SYS_Maneuver,
    SYS_Impulse,
    SYS_Warp,
    SYS_JumpDrive,
    SYS_FrontShield,
    SYS_RearShield,
    SYS_COUNT
};
/* Define script conversion function for the ESystem enum. */
template<> void convert<ESystem>::param(lua_State* L, int& idx, ESystem& es);

class BeamTemplate : public sf::NonCopyable
{
public:
    float arc, direction, range, cycle_time, damage;
};
class ShipRoomTemplate
{
public:
    sf::Vector2i position;
    sf::Vector2i size;
    ESystem system;
    
    ShipRoomTemplate(sf::Vector2i position, sf::Vector2i size, ESystem system) : position(position), size(size), system(system) {}
};
class ShipDoorTemplate
{
public:
    sf::Vector2i position;
    bool horizontal;

    ShipDoorTemplate(sf::Vector2i position, bool horizontal) : position(position), horizontal(horizontal) {}
};

class ShipTemplate : public PObject
{
    static std::map<string, P<ShipTemplate> > templateMap;
public:
    string name;

    float scale;
    float radius;
    string model, colorTexture, specularTexture, illuminationTexture;
    sf::Vector3f beamPosition[maxBeamWeapons];
    BeamTemplate beams[maxBeamWeapons];
    int weaponTubes;
    sf::Vector2f tubePosition[maxWeaponTubes];
    float hull;
    float frontShields, rearShields;
    float impulseSpeed, turnSpeed, warpSpeed;
    bool jumpDrive, cloaking;
    int weaponStorage[MW_Count];
    
    std::vector<ShipRoomTemplate> rooms;
    std::vector<ShipDoorTemplate> doors;

    ShipTemplate();
    
    void setName(string name);
    void setScale(float scale) { this->scale = scale; }
    void setRadius(float radius) { this->radius = radius; }
    void setMesh(string model, string colorTexture, string specularTexture, string illuminationTexture);
    void setBeamPosition(int index, sf::Vector3f position);
    void setBeam(int index, float arc, float direction, float range, float cycle_time, float damage);
    void setTubePosition(int index, sf::Vector2f position);
    void setTubes(int amount) { weaponTubes = std::min(maxWeaponTubes, amount); }
    void setHull(float amount) { hull = amount; }
    void setShields(float front, float rear) { frontShields = front; rearShields = rear; }
    void setSpeed(float impulse, float turn) { impulseSpeed = impulse; turnSpeed = turn; }
    void setWarpSpeed(float warp) { warpSpeed = warp; }
    void setJumpDrive(bool enabled) { jumpDrive = enabled; }
    void setCloaking(bool enabled) { cloaking = enabled; }
    void setWeaponStorage(EMissileWeapons weapon, int amount) { if (weapon != MW_None) weaponStorage[weapon] = amount; }
    void addRoom(sf::Vector2i position, sf::Vector2i size) { rooms.push_back(ShipRoomTemplate(position, size, SYS_None)); }
    void addRoomSystem(sf::Vector2i position, sf::Vector2i size, ESystem system) { rooms.push_back(ShipRoomTemplate(position, size, system)); }
    void addDoor(sf::Vector2i position, bool horizontal) { doors.push_back(ShipDoorTemplate(position, horizontal)); }
    
    sf::Vector2i interiorSize();
    ESystem getSystemAtRoom(sf::Vector2i position);
public:
    static P<ShipTemplate> getTemplate(string name);
    static std::vector<string> getTemplateNameList();
};
string getSystemName(ESystem system);
REGISTER_MULTIPLAYER_ENUM(ESystem);

#endif//SHIP_TEMPLATE_H
