#include <SFML/OpenGL.hpp>
#include "spaceship.h"
#include "mesh.h"
#include "main.h"
#include "shipTemplate.h"
#include "beamEffect.h"

static const int16_t CMD_TARGET_ROTATION = 0x0001;
static const int16_t CMD_IMPULSE = 0x0002;
static const int16_t CMD_WARP = 0x0003;
static const int16_t CMD_JUMP = 0x0004;
static const int16_t CMD_SET_TARGET = 0x0005;
static const int16_t CMD_LOAD_TUBE = 0x0006;
static const int16_t CMD_UNLOAD_TUBE = 0x0007;
static const int16_t CMD_FIRE_TUBE = 0x0008;

#include "scriptInterface.h"
REGISTER_SCRIPT_CLASS(SpaceShip)
{
    REGISTER_SCRIPT_CLASS_FUNCTION(SpaceShip, setShipTemplate);
}

REGISTER_MULTIPLAYER_CLASS(SpaceShip, "SpaceShip");
SpaceShip::SpaceShip()
: SpaceObject(50, "SpaceShip")
{
    setCollisionPhysics(true, false);

    targetRotation = getRotation();
    impulseRequest = 0;
    currentImpulse = 0;
    hasWarpdrive = true;
    warpRequest = 0.0;
    currentWarp = 0.0;
    hasJumpdrive = true;
    jumpDistance = 0.0;
    jumpDelay = 0.0;
    tubeLoadTime = 8.0;
    weaponTubes = 6;
    
    registerMemberReplication(&targetRotation);
    registerMemberReplication(&impulseRequest);
    registerMemberReplication(&currentImpulse);
    registerMemberReplication(&hasWarpdrive);
    registerMemberReplication(&warpRequest);
    registerMemberReplication(&currentWarp);
    registerMemberReplication(&hasJumpdrive);
    registerMemberReplication(&jumpDelay, 0.2);
    registerMemberReplication(&tubeLoadTime);
    registerMemberReplication(&weaponTubes);
    registerMemberReplication(&targetId);
    
    for(int n=0; n<maxBeamWeapons; n++)
    {
        beamWeapons[n].arc = 0;
        beamWeapons[n].direction = 0;
        beamWeapons[n].range = 0;
        beamWeapons[n].cycleTime = 6.0;
        beamWeapons[n].cooldown = 0.0;
        
        registerMemberReplication(&beamWeapons[n].arc);
        registerMemberReplication(&beamWeapons[n].direction);
        registerMemberReplication(&beamWeapons[n].range);
        registerMemberReplication(&beamWeapons[n].cycleTime);
        registerMemberReplication(&beamWeapons[n].cooldown, 0.2);
    }
    for(int n=0; n<maxWeaponTubes; n++)
    {
        weaponTube[n].typeLoaded = MW_None;
        weaponTube[n].state = WTS_Empty;
        weaponTube[n].delay = 0.0;

        registerMemberReplication(&weaponTube[n].typeLoaded);
        registerMemberReplication(&weaponTube[n].state);
        registerMemberReplication(&weaponTube[n].delay, 0.5);
    }
    for(int n=0; n<MW_Count; n++)
    {
        weaponStorage[n] = 0;
        registerMemberReplication(&weaponStorage[n]);
    }
    registerMemberReplication(&templateName);


    templateName = "scout";
    beamWeapons[0].arc = 90.0;
    beamWeapons[0].direction = -20;
    beamWeapons[0].range = 1000.0;
    
    beamWeapons[1].arc = 90.0;
    beamWeapons[1].direction = 20;
    beamWeapons[1].range = 1000.0;
    
    beamWeapons[2].arc = 30.0;
    beamWeapons[2].direction = 180;
    beamWeapons[2].range = 2000.0;
    
    weaponStorage[MW_Homing] = 15;
    weaponStorage[MW_Nuke] = 2;
    weaponStorage[MW_Mine] = 20;
    weaponStorage[MW_EMP] = 5;
}

void SpaceShip::setShipTemplate(string templateName)
{
    this->templateName = templateName;
    shipTemplate = ShipTemplate::getTemplate(templateName);
}

void SpaceShip::draw3D()
{
    if (!shipTemplate) return;
    
    glScalef(shipTemplate->scale, shipTemplate->scale, shipTemplate->scale);
    objectShader.setParameter("baseMap", *textureManager.getTexture(shipTemplate->colorTexture));
    objectShader.setParameter("illuminationMap", *textureManager.getTexture(shipTemplate->illuminationTexture));
    objectShader.setParameter("specularMap", *textureManager.getTexture(shipTemplate->specularTexture));
    sf::Shader::bind(&objectShader);
    Mesh* m = Mesh::getMesh(shipTemplate->model);
    m->render();
}

void SpaceShip::drawRadar(sf::RenderTarget& window, sf::Vector2f position, float scale)
{
    for(int n=0; n<maxBeamWeapons; n++)
    {
        if (beamWeapons[n].range == 0.0) continue;
        sf::Color color = sf::Color::Red;
        if (beamWeapons[n].cooldown > 0)
            color = sf::Color(255, 255 * (beamWeapons[n].cooldown / beamWeapons[n].cycleTime), 0);
        
        sf::VertexArray a(sf::LinesStrip, 3);
        a[0].color = color;
        a[1].color = color;
        a[2].color = sf::Color(color.r, color.g, color.b, 0);
        a[0].position = position;
        a[1].position = position + sf::vector2FromAngle(getRotation() + (beamWeapons[n].direction + beamWeapons[n].arc / 2.0f)) * beamWeapons[n].range * scale;
        a[2].position = position + sf::vector2FromAngle(getRotation() + (beamWeapons[n].direction + beamWeapons[n].arc / 2.0f)) * beamWeapons[n].range * scale * 1.3f;
        window.draw(a);
        a[1].position = position + sf::vector2FromAngle(getRotation() + (beamWeapons[n].direction - beamWeapons[n].arc / 2.0f)) * beamWeapons[n].range * scale;
        a[2].position = position + sf::vector2FromAngle(getRotation() + (beamWeapons[n].direction - beamWeapons[n].arc / 2.0f)) * beamWeapons[n].range * scale * 1.3f;
        window.draw(a);
        
        int arcPoints = int(beamWeapons[n].arc / 10) + 1;
        sf::VertexArray arc(sf::LinesStrip, arcPoints);
        for(int i=0; i<arcPoints; i++)
        {
            arc[i].color = color;
            arc[i].position = position + sf::vector2FromAngle(getRotation() + (beamWeapons[n].direction - beamWeapons[n].arc / 2.0f + 10 * i)) * beamWeapons[n].range * scale;
        }
        arc[arcPoints-1].position = position + sf::vector2FromAngle(getRotation() + (beamWeapons[n].direction + beamWeapons[n].arc / 2.0f)) * beamWeapons[n].range * scale;
        window.draw(arc);
    }

    sf::Sprite objectSprite;
    textureManager.setTexture(objectSprite, "RadarArrow.png");
    objectSprite.setRotation(getRotation());
    objectSprite.setPosition(position);
    window.draw(objectSprite);
}

void SpaceShip::update(float delta)
{
    if (!shipTemplate)
        shipTemplate = ShipTemplate::getTemplate(templateName);

    float rotationDiff = targetRotation - getRotation();
    if (rotationDiff < -180)
    {
        targetRotation += 360;
        rotationDiff = targetRotation - getRotation();
    }
    if (rotationDiff > 180)
    {
        targetRotation -= 360;
        rotationDiff = targetRotation - getRotation();
    }

    if (rotationDiff > 1.0)
        setAngularVelocity(10);
    else if (rotationDiff < -1.0)
        setAngularVelocity(-10);
    else
        setAngularVelocity(rotationDiff * 10.0);
    
    if (hasJumpdrive && jumpDelay > 0)
    {
        if (currentImpulse > 1.0)
        {
            currentImpulse -= delta;
            if (currentImpulse < 0.0)
                currentImpulse = 0.0;
        }
        jumpDelay -= delta;
        if (jumpDelay <= 0.0)
        {
            setPosition(getPosition() + sf::vector2FromAngle(getRotation()) * jumpDistance * 1000.0f);
            jumpDelay = 0.0;
        }
    }else if (hasWarpdrive && (warpRequest > 0 || currentWarp > 0))
    {
        if (currentImpulse < 1.0)
        {
            currentImpulse += delta;
            if (currentImpulse > 1.0)
                currentImpulse = 1.0;
        }else{
            if (currentWarp < warpRequest)
            {
                currentWarp += delta;
                if (currentWarp > warpRequest)
                    currentWarp = warpRequest;
            }else if (currentWarp > warpRequest)
            {
                currentWarp -= delta;
                if (currentWarp < warpRequest)
                    currentWarp = warpRequest;
            }
        }
    }else{
        if (currentImpulse < impulseRequest)
        {
            currentImpulse += delta;
            if (currentImpulse > impulseRequest)
                currentImpulse = impulseRequest;
        }else if (currentImpulse > impulseRequest)
        {
            currentImpulse -= delta;
            if (currentImpulse < impulseRequest)
                currentImpulse = impulseRequest;
        }
    }
    setVelocity(sf::vector2FromAngle(getRotation()) * (currentImpulse * 500.0f + currentWarp * 1000.0f));

    for(int n=0; n<maxBeamWeapons; n++)
    {
        if (beamWeapons[n].cooldown > 0.0)
            beamWeapons[n].cooldown -= delta;
    }
    
    P<SpaceObject> target = getTarget();
    if (gameServer && target && delta > 0) // Only fire beam weapons if we are on the server, have a target, and are not paused.
    {
        sf::Vector2f diff = target->getPosition() - getPosition();
        float distance = sf::length(diff);
        float angle = sf::vector2ToAngle(diff);
        for(int n=0; n<maxBeamWeapons; n++)
        {
            if (target && beamWeapons[n].cooldown <= 0.0 && distance < beamWeapons[n].range)
            {
                float angleDiff = angle - (beamWeapons[n].direction + getRotation());
                while(angleDiff > 180) angleDiff -= 360;
                while(angleDiff < -180) angleDiff += 360;
                if (abs(angleDiff) < beamWeapons[n].arc / 2.0)
                {
                    sf::Vector2f hitLocation = target->getPosition() - (diff / distance) * target->getRadius();
                    
                    beamWeapons[n].cooldown = beamWeapons[n].cycleTime;
                    P<BeamEffect> effect = new BeamEffect();
                    effect->setSource(this, shipTemplate->beamPosition[n] * shipTemplate->scale);
                    effect->setTarget(target, hitLocation);
                    
                    target->takeDamage(30, hitLocation, DT_Energy);
                }
            }
        }
    }
    
    for(int n=0; n<maxWeaponTubes; n++)
    {
        if (weaponTube[n].delay > 0.0)
        {
            weaponTube[n].delay -= delta;
        }else{
            switch(weaponTube[n].state)
            {
            case WTS_Loading:
                weaponTube[n].state = WTS_Loaded;
                break;
            case WTS_Unloading:
                weaponTube[n].state = WTS_Empty;
                weaponStorage[weaponTube[n].typeLoaded] ++;
                weaponTube[n].typeLoaded = MW_None;
                break;
            default:
                break;
            }
        }
    }
}

P<SpaceObject> SpaceShip::getTarget()
{
    if (gameServer)
        return gameServer->getObjectById(targetId);
    return gameClient->getObjectById(targetId);
}

void SpaceShip::onReceiveCommand(int32_t clientId, sf::Packet& packet)
{
    int16_t command;
    packet >> command;
    switch(command)
    {
    case CMD_TARGET_ROTATION:
        packet >> targetRotation;
        break;
    case CMD_IMPULSE:
        packet >> impulseRequest;
        break;
    case CMD_WARP:
        packet >> warpRequest;
        break;
    case CMD_JUMP:
        if (jumpDelay <= 0.0)
        {
            packet >> jumpDistance;
            jumpDelay = 10.0;
        }
        break;
    case CMD_SET_TARGET:
        {
            packet >> targetId;
        }
        break;
    case CMD_LOAD_TUBE:
        {
            int8_t tubeNr;
            EMissileWeapons type;
            packet >> tubeNr >> type;
            
            if (tubeNr >= 0 && tubeNr < maxWeaponTubes && type > MW_None && type < MW_Count)
            {
                if (weaponTube[tubeNr].state == WTS_Empty && weaponStorage[type] > 0)
                {
                    weaponTube[tubeNr].state = WTS_Loading;
                    weaponTube[tubeNr].delay = tubeLoadTime;
                    weaponTube[tubeNr].typeLoaded = type;
                    weaponStorage[type]--;
                }
            }
        }
        break;
    case CMD_UNLOAD_TUBE:
        {
            int8_t tubeNr;
            packet >> tubeNr;
            
            if (tubeNr >= 0 && tubeNr < maxWeaponTubes && weaponTube[tubeNr].state == WTS_Loaded)
            {
                weaponTube[tubeNr].state = WTS_Unloading;
                weaponTube[tubeNr].delay = tubeLoadTime;
            }
        }
        break;
    case CMD_FIRE_TUBE:
        {
            int8_t tubeNr;
            packet >> tubeNr;
            
            if (tubeNr >= 0 && tubeNr < maxWeaponTubes && weaponTube[tubeNr].state == WTS_Loaded)
            {
                weaponTube[tubeNr].state = WTS_Empty;
                weaponTube[tubeNr].typeLoaded = MW_None;
            }
        }
        break;
    }
}

void SpaceShip::commandTargetRotation(float target)
{
    sf::Packet packet;
    packet << CMD_TARGET_ROTATION << target;
    sendCommand(packet);
}

void SpaceShip::commandImpulse(float target)
{
    sf::Packet packet;
    packet << CMD_IMPULSE << target;
    sendCommand(packet);
}

void SpaceShip::commandWarp(int8_t target)
{
    sf::Packet packet;
    packet << CMD_WARP << target;
    sendCommand(packet);
}

void SpaceShip::commandJump(float distance)
{
    sf::Packet packet;
    packet << CMD_JUMP << distance;
    sendCommand(packet);
}

void SpaceShip::commandSetTarget(P<SpaceObject> target)
{
    sf::Packet packet;
    if (target)
        packet << CMD_SET_TARGET << target->getMultiplayerId();
    else
        packet << CMD_SET_TARGET << int32_t(-1);
    sendCommand(packet);
}

void SpaceShip::commandLoadTube(int8_t tubeNumber, EMissileWeapons missileType)
{
    sf::Packet packet;
    packet << CMD_LOAD_TUBE << tubeNumber << missileType;
    sendCommand(packet);
}

void SpaceShip::commandUnloadTube(int8_t tubeNumber)
{
    sf::Packet packet;
    packet << CMD_UNLOAD_TUBE << tubeNumber;
    sendCommand(packet);
}

void SpaceShip::commandFireTube(int8_t tubeNumber)
{
    sf::Packet packet;
    packet << CMD_FIRE_TUBE << tubeNumber;
    sendCommand(packet);
}

string getMissileWeaponName(EMissileWeapons missile)
{
    switch(missile)
    {
    case MW_None:
        return "-";
    case MW_Homing:
        return "Homing";
    case MW_Nuke:
        return "Nuke";
    case MW_Mine:
        return "Mine";
    case MW_EMP:
        return "EMP";
    default:
        return "UNK: " + string(int(missile));
    }
}
