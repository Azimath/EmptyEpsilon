#include "crewUI.h"
#include "playerInfo.h"
#include "factionInfo.h"
#include "repairCrew.h"

CrewUI::CrewUI()
{
    jumpDistance = 1.0;
    tubeLoadType = MW_None;
    scienceRadarDistance = 50000;
    
    for(int n=0; n<maxCrewPositions; n++)
    {
        if (myPlayerInfo->crewPosition[n])
        {
            showPosition = ECrewPosition(n);
            break;
        }
    }
}

void CrewUI::onGui()
{
    if (mySpaceship)
    {
        switch(showPosition)
        {
        case helmsOfficer:
            helmsUI();
            mainScreenSelectGUI();
            break;
        case weaponsOfficer:
            weaponsUI();
            mainScreenSelectGUI();
            break;
        case engineering:
            engineeringUI();
            mainScreenSelectGUI();
            break;
        case scienceOfficer:
            scienceUI();
            mainScreenSelectGUI();
            break;
        default:
            drawStatic();
            text(sf::FloatRect(0, 500, 1600, 100), "???", AlignCenter, 100);
            mainScreenSelectGUI();
            break;
        }
    }else{
        drawStatic();
    }

    int offset = 0;
    for(int n=0; n<maxCrewPositions; n++)
    {
        if (myPlayerInfo->crewPosition[n])
        {
            if (toggleButton(sf::FloatRect(200 * offset, 0, 200, 20), showPosition == ECrewPosition(n), getCrewPositionName(ECrewPosition(n)), 20))
            {
                showPosition = ECrewPosition(n);
            }
            offset++;
        }
    }
    
    MainUI::onGui();
}

void CrewUI::helmsUI()
{
    sf::RenderTarget* window = getRenderTarget();
    sf::Vector2f mouse = InputHandler::getMousePos();
    if (InputHandler::mouseIsPressed(sf::Mouse::Left))
    {
        sf::Vector2f diff = mouse - sf::Vector2f(800, 450);
        if (sf::length(diff) < 400)
            mySpaceship->commandTargetRotation(sf::vector2ToAngle(diff));
    }
    
    //Radar
    float radarDistance = 5000;
    drawRaderBackground(mySpaceship->getPosition(), sf::Vector2f(800, 450), 400, 400.0f / radarDistance);
    foreach(SpaceObject, obj, spaceObjectList)
    {
        if (obj != mySpaceship && sf::length(obj->getPosition() - mySpaceship->getPosition()) < radarDistance)
            obj->drawRadar(*window, sf::Vector2f(800, 450) + (obj->getPosition() - mySpaceship->getPosition()) / radarDistance * 400.0f, 400.0f / radarDistance, false);
    }

    P<SpaceObject> target = mySpaceship->getTarget();
    if (target)
    {
        sf::Sprite objectSprite;
        textureManager.setTexture(objectSprite, "redicule.png");
        objectSprite.setPosition(sf::Vector2f(800, 450) + (target->getPosition() - mySpaceship->getPosition()) / radarDistance * 400.0f);
        window->draw(objectSprite);
    }
    mySpaceship->drawRadar(*window, sf::Vector2f(800, 450), 400.0f / radarDistance, false);
    drawHeadingCircle(sf::Vector2f(800, 450), 400);
    //!Radar

    text(sf::FloatRect(10, 100, 200, 20), "Energy: " + string(int(mySpaceship->energy_level)), AlignLeft, 20);
    
    float res = vslider(sf::FloatRect(20, 500, 50, 300), mySpaceship->impulseRequest, 1.0, -1.0);
    if (res > -0.15 && res < 0.15)
        res = 0.0;
    if (res != mySpaceship->impulseRequest)
        mySpaceship->commandImpulse(res);
    text(sf::FloatRect(20, 800, 50, 20), string(int(mySpaceship->impulseRequest * 100)) + "%", AlignLeft, 20);
    text(sf::FloatRect(20, 820, 50, 20), string(int(mySpaceship->currentImpulse * 100)) + "%", AlignLeft, 20);

    if (mySpaceship->hasWarpdrive)
    {
        res = vslider(sf::FloatRect(100, 500, 50, 300), mySpaceship->warpRequest, 4.0, 0.0);
        if (res != mySpaceship->warpRequest)
            mySpaceship->commandWarp(res);
        text(sf::FloatRect(100, 800, 50, 20), string(int(mySpaceship->warpRequest)), AlignLeft, 20);
        text(sf::FloatRect(100, 820, 50, 20), string(int(mySpaceship->currentWarp * 100)) + "%", AlignLeft, 20);
    }
    if (mySpaceship->hasJumpdrive)
    {
        float x = mySpaceship->hasWarpdrive ? 180 : 100;
        jumpDistance = vslider(sf::FloatRect(x, 500, 50, 300), jumpDistance, 40.0, 1.0);
        jumpDistance = roundf(jumpDistance * 10.0f) / 10.0f;
        text(sf::FloatRect(x, 800, 50, 20), string(jumpDistance, 1) + "km", AlignLeft, 20);
        if (mySpaceship->jumpDelay > 0.0)
        {
            text(sf::FloatRect(x, 820, 50, 20), string(int(ceilf(mySpaceship->jumpDelay))), AlignLeft, 20);
        }else{
            if (button(sf::FloatRect(x, 820, 70, 30), "Jump", 20))
            {
                mySpaceship->commandJump(jumpDistance);
            }
        }
    }
}

void CrewUI::weaponsUI()
{
    sf::RenderTarget* window = getRenderTarget();
    sf::Vector2f mouse = InputHandler::getMousePos();
    float radarDistance = 5000;

    //Radar
    drawRaderBackground(mySpaceship->getPosition(), sf::Vector2f(800, 450), 400, 400.0f / radarDistance);
    if (InputHandler::mouseIsPressed(sf::Mouse::Left))
    {
        sf::Vector2f diff = mouse - sf::Vector2f(800, 450);
        if (sf::length(diff) < 400)
        {
            P<SpaceObject> target;
            sf::Vector2f mousePosition = mySpaceship->getPosition() + diff / 400.0f * radarDistance;
            PVector<Collisionable> list = CollisionManager::queryArea(mousePosition - sf::Vector2f(50, 50), mousePosition + sf::Vector2f(50, 50));
            foreach(Collisionable, obj, list)
            {
                P<SpaceObject> spaceObject = obj;
                if (spaceObject && spaceObject->canBeTargeted() && spaceObject != mySpaceship)
                {
                    if (!target || sf::length(mousePosition - spaceObject->getPosition()) < sf::length(mousePosition - target->getPosition()))
                        target = spaceObject;
                }
            }
            mySpaceship->commandSetTarget(target);
        }
    }

    foreach(SpaceObject, obj, spaceObjectList)
    {
        if (obj != mySpaceship && sf::length(obj->getPosition() - mySpaceship->getPosition()) < radarDistance)
            obj->drawRadar(*window, sf::Vector2f(800, 450) + (obj->getPosition() - mySpaceship->getPosition()) / radarDistance * 400.0f, 400.0f / radarDistance, false);
    }
    
    P<SpaceObject> target = mySpaceship->getTarget();
    if (target)
    {
        sf::Sprite objectSprite;
        textureManager.setTexture(objectSprite, "redicule.png");
        objectSprite.setPosition(sf::Vector2f(800, 450) + (target->getPosition() - mySpaceship->getPosition()) / radarDistance * 400.0f);
        window->draw(objectSprite);
    }
    mySpaceship->drawRadar(*window, sf::Vector2f(800, 450), 400.0f / radarDistance, false);
    drawHeadingCircle(sf::Vector2f(800, 450), 400);
    //!Radar
    text(sf::FloatRect(10, 100, 200, 20), "Energy: " + string(int(mySpaceship->energy_level)), AlignLeft, 20);
    text(sf::FloatRect(10, 120, 200, 20), "Shields: " + string(int(100 * mySpaceship->front_shield / mySpaceship->front_shield_max)) + "/" + string(int(100 * mySpaceship->rear_shield / mySpaceship->rear_shield_max)), AlignLeft, 20);
    if (mySpaceship->front_shield_max > 0 || mySpaceship->rear_shield_max > 0)
    {
        if (toggleButton(sf::FloatRect(10, 140, 200, 30), mySpaceship->shields_active, mySpaceship->shields_active ? "Shields:ON" : "Shields:OFF", 25))
            mySpaceship->commandSetShields(!mySpaceship->shields_active);
    }

    float y = 900 - 10;
    for(int n=0; n<mySpaceship->weaponTubes; n++)
    {
        y -= 50;
        switch(mySpaceship->weaponTube[n].state)
        {
        case WTS_Empty:
            if (toggleButton(sf::FloatRect(20, y, 150, 50), tubeLoadType != MW_None && mySpaceship->weaponStorage[tubeLoadType] > 0, "Load", 35) && tubeLoadType != MW_None)
                mySpaceship->commandLoadTube(n, tubeLoadType);
            toggleButton(sf::FloatRect(170, y, 350, 50), false, "Empty", 35);
            break;
        case WTS_Loaded:
            if (button(sf::FloatRect(20, y, 150, 50), "Unload", 35))
                mySpaceship->commandUnloadTube(n);
            if (button(sf::FloatRect(170, y, 350, 50), getMissileWeaponName(mySpaceship->weaponTube[n].typeLoaded), 35))
                mySpaceship->commandFireTube(n);
            break;
        case WTS_Loading:
            progressBar(sf::FloatRect(170, 840 - 50 * n, 350, 50), mySpaceship->weaponTube[n].delay, mySpaceship->tubeLoadTime, 0.0);
            toggleButton(sf::FloatRect(20, y, 150, 50), false, "Loading", 35);
            text(sf::FloatRect(170, y, 350, 50), getMissileWeaponName(mySpaceship->weaponTube[n].typeLoaded), AlignCenter, 35);
            break;
        case WTS_Unloading:
            progressBar(sf::FloatRect(170, 840 - 50 * n, 350, 50), mySpaceship->weaponTube[n].delay, 0.0, mySpaceship->tubeLoadTime);
            toggleButton(sf::FloatRect(20, y, 150, 50), false, "Unloading", 25);
            text(sf::FloatRect(170, y, 350, 50), getMissileWeaponName(mySpaceship->weaponTube[n].typeLoaded), AlignCenter, 35);
            break;
        }
    }

    for(int n=0; n<MW_Count; n++)
    {
        y -= 30;
        if (toggleButton(sf::FloatRect(10, y, 200, 30), tubeLoadType == n, getMissileWeaponName(EMissileWeapons(n)) + " x" + string(mySpaceship->weaponStorage[n]), 25))
        {
            if (tubeLoadType == n)
                tubeLoadType = MW_None;
            else
                tubeLoadType = EMissileWeapons(n);
        }
    }
}

void CrewUI::engineeringUI()
{
    if (!mySpaceship->shipTemplate) return;
    sf::RenderTarget& window = *getRenderTarget();
    sf::Vector2f mouse = InputHandler::getMousePos();

    float net_power = 0.0;
    for(int n=0; n<SYS_COUNT; n++)
    {
        if (!mySpaceship->hasSystem(ESystem(n))) continue;
        if (mySpaceship->systems[n].powerUserFactor < 0)
            net_power -= mySpaceship->systems[n].powerUserFactor * mySpaceship->systems[n].health * mySpaceship->systems[n].powerLevel;
        else
            net_power -= mySpaceship->systems[n].powerUserFactor * mySpaceship->systems[n].powerLevel;
    }
    text(sf::FloatRect(10, 100, 200, 20), "Energy: " + string(int(mySpaceship->energy_level)) + " (" + string(net_power) + ")", AlignLeft, 20);
    text(sf::FloatRect(10, 120, 200, 20), "Hull: " + string(int(mySpaceship->hull_strength * 100 / mySpaceship->hull_max)), AlignLeft, 20);
    
    ESystem highlight_system = SYS_None;
    int x = 20;
    for(int n=0; n<SYS_COUNT; n++)
    {
        if (!mySpaceship->hasSystem(ESystem(n))) continue;
        if (sf::FloatRect(x + 20, 530, 140, 320).contains(mouse))
            highlight_system = ESystem(n);
        
        vtext(sf::FloatRect(x + 20, 550, 30, 300), "Dmg:" + string(int(100 - mySpaceship->systems[n].health * 100)) + "%", AlignRight, 15);
        vtext(sf::FloatRect(x, 550, 50, 300), getSystemName(ESystem(n)), AlignLeft);
        text(sf::FloatRect(x + 50, 530, 50, 20), string(int(mySpaceship->systems[n].powerLevel * 100)) + "%", AlignCenter, 20);
        float ret = vslider(sf::FloatRect(x + 50, 550, 50, 300), mySpaceship->systems[n].powerLevel, 3.0, 0.0, 1.0);
        if (ret < 1.25 && ret > 0.75)
            ret = 1.0;
        if (mySpaceship->systems[n].powerLevel != ret)
            mySpaceship->commandSetSystemPower(ESystem(n), ret);
        vprogressBar(sf::FloatRect(x + 110, 500, 50, 50), mySpaceship->systems[n].heatLevel, 0.0, 1.0, sf::Color(255, 255 * (1.0 - mySpaceship->systems[n].heatLevel), 0));
        ret = vslider(sf::FloatRect(x + 110, 550, 50, 300), mySpaceship->systems[n].coolantLevel, 10.0, 0.0);
        if (mySpaceship->systems[n].coolantLevel != ret)
            mySpaceship->commandSetSystemCoolant(ESystem(n), ret);
        x += 160;
    }

    sf::Vector2i interior_size = mySpaceship->shipTemplate->interiorSize();
    sf::Vector2f interial_position = sf::Vector2f(800, 250) - sf::Vector2f(interior_size) * 48.0f / 2.0f;
    drawShipInternals(interial_position, mySpaceship, highlight_system);

    PVector<RepairCrew> rc_list = getRepairCrewFor(mySpaceship);
    foreach(RepairCrew, rc, rc_list)
    {
        sf::Vector2f position = interial_position + sf::Vector2f(rc->position) * 48.0f + sf::Vector2f(1.0, 1.0) * 48.0f / 2.0f + sf::Vector2f(2.0, 2.0);
        sf::Sprite sprite;
        textureManager.setTexture(sprite, "RadarBlip.png");
        sprite.setPosition(position);
        window.draw(sprite);
        
        if (InputHandler::mouseIsPressed(sf::Mouse::Left) && sf::length(mouse - position) < 48.0f/2.0)
        {
            selected_crew = rc;
        }
        
        if (selected_crew == rc)
        {
            sf::Sprite select_sprite;
            textureManager.setTexture(select_sprite, "redicule.png");
            select_sprite.setPosition(position);
            window.draw(select_sprite);
        }
    }
    
    if (InputHandler::mouseIsPressed(sf::Mouse::Right) && selected_crew)
    {
        sf::Vector2i target_pos = sf::Vector2i((mouse - interial_position) / 48.0f);
        if (target_pos.x >= 0 && target_pos.x < interior_size.x && target_pos.y >= 0 && target_pos.y < interior_size.y)
        {
            selected_crew->commandSetTargetPosition(target_pos);
        }
    }
}

void CrewUI::scienceUI()
{
    sf::RenderTarget* window = getRenderTarget();
    sf::Vector2f mouse = InputHandler::getMousePos();


    //Radar
    float radarDistance = scienceRadarDistance;
    drawRaderBackground(mySpaceship->getPosition(), sf::Vector2f(800, 450), 400, 400.0f / radarDistance);
    if (InputHandler::mouseIsPressed(sf::Mouse::Left))
    {
        sf::Vector2f diff = mouse - sf::Vector2f(800, 450);
        if (sf::length(diff) < 400)
        {
            P<SpaceObject> target;
            sf::Vector2f mousePosition = mySpaceship->getPosition() + diff / 400.0f * radarDistance;
            PVector<Collisionable> list = CollisionManager::queryArea(mousePosition - sf::Vector2f(radarDistance / 100, radarDistance / 100), mousePosition + sf::Vector2f(radarDistance / 100, radarDistance / 100));
            foreach(Collisionable, obj, list)
            {
                P<SpaceObject> spaceObject = obj;
                if (spaceObject && spaceObject->canBeTargeted() && spaceObject != mySpaceship)
                {
                    if (!target || sf::length(mousePosition - spaceObject->getPosition()) < sf::length(mousePosition - target->getPosition()))
                        target = spaceObject;
                }
            }
            scienceTarget = target;
        }
    }

    foreach(SpaceObject, obj, spaceObjectList)
    {
        if (obj != mySpaceship && sf::length(obj->getPosition() - mySpaceship->getPosition()) < radarDistance)
            obj->drawRadar(*window, sf::Vector2f(800, 450) + (obj->getPosition() - mySpaceship->getPosition()) / radarDistance * 400.0f, 400.0f / radarDistance, true);
    }
    
    if (scienceTarget)
    {
        sf::Sprite objectSprite;
        textureManager.setTexture(objectSprite, "redicule.png");
        objectSprite.setPosition(sf::Vector2f(800, 450) + (scienceTarget->getPosition() - mySpaceship->getPosition()) / radarDistance * 400.0f);
        window->draw(objectSprite);
    }
    mySpaceship->drawRadar(*window, sf::Vector2f(800, 450), 400.0f / radarDistance, true);
    drawHeadingCircle(sf::Vector2f(800, 450), 400);
    //!Radar
    
    if (scienceTarget)
    {
        float distance = sf::length(scienceTarget->getPosition() - mySpaceship->getPosition());
        float heading = sf::vector2ToAngle(scienceTarget->getPosition() - mySpaceship->getPosition());
        if (heading < 0) heading += 360;
        text(sf::FloatRect(20, 100, 100, 20), "Distance: " + string(distance / 1000.0, 1) + "km", AlignLeft, 20);
        text(sf::FloatRect(20, 120, 100, 20), "Heading: " + string(int(heading)), AlignLeft, 20);

        P<SpaceShip> ship = scienceTarget;
        if (ship && !ship->scanned_by_player)
        {
            if (mySpaceship->scanning_delay > 0.0)
            {
                progressBar(sf::FloatRect(20, 140, 150, 30), mySpaceship->scanning_delay, 8.0, 0.0);
            }else{
                if (button(sf::FloatRect(20, 140, 150, 30), "Scan", 25))
                    mySpaceship->commandScan(scienceTarget);
            }
        }else{
            text(sf::FloatRect(20, 140, 100, 20), factionInfo[scienceTarget->factionId].name, AlignLeft, 20);
            if (ship && ship->shipTemplate)
            {
                text(sf::FloatRect(20, 160, 100, 20), ship->shipTemplate->name, AlignLeft, 20);
                text(sf::FloatRect(20, 180, 200, 20), "Shields: " + string(int(ship->front_shield)) + "/" + string(int(ship->rear_shield)), AlignLeft, 20);
            }
        }
    }
    
    if (scienceRadarDistance == 50000 && button(sf::FloatRect(20, 850, 150, 30), "Zoom: 1x", 25))
        scienceRadarDistance = 25000;
    else if (scienceRadarDistance == 25000 && button(sf::FloatRect(20, 850, 150, 30), "Zoom: 2x", 25))
        scienceRadarDistance = 12500;
    else if (scienceRadarDistance == 12500 && button(sf::FloatRect(20, 850, 150, 30), "Zoom: 4x", 25))
        scienceRadarDistance = 5000;
    else if (scienceRadarDistance == 5000 && button(sf::FloatRect(20, 850, 150, 30), "Zoom: 10x", 25))
        scienceRadarDistance = 50000;
}
