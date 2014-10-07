#include "crewUI.h"
#include "playerInfo.h"
#include "factionInfo.h"
#include "repairCrew.h"
#include "spaceStation.h"

CrewUI::CrewUI()
{
}

void CrewUI::onGui()
{
    if (my_spaceship)
    {
        onCrewUI();
        if (my_spaceship->activate_self_destruct)
        {
            selfDestructGUI();
        }else{
            if (my_player_info->main_screen_control)
                mainScreenSelectGUI();
        }
    }else{
        drawStatic();
    }
        
    int cnt = 0;
    for(int n=0; n<max_crew_positions; n++)
        if (my_player_info->crew_position[n])
            cnt++;
    
    if (cnt > 1)
    {
        int offset = 0;
        for(int n=0; n<max_crew_positions; n++)
        {
            if (my_player_info->crew_position[n])
            {
                if (toggleButton(sf::FloatRect(200 * offset, 0, 200, 25), my_player_info->crew_active_position == ECrewPosition(n), getCrewPositionName(ECrewPosition(n)), 20))
                {
                    if (my_player_info->crew_active_position != ECrewPosition(n))
                    {
                        my_player_info->crew_active_position = ECrewPosition(n);
                        destroy();
                        my_player_info->spawnUI();
                    }
                }
                offset++;
            }
        }
    }

    MainUIBase::onGui();
}

void CrewUI::onCrewUI()
{
    drawStatic();
    text(sf::FloatRect(0, 500, 1600, 100), "???", AlignCenter, 100);
}

void CrewUI::impulseSlider(sf::FloatRect rect, float text_size)
{
    float res = vslider(rect, my_spaceship->impulseRequest, 1.0, -1.0);
    if (res > -0.15 && res < 0.15)
        res = 0.0;
    if (res != my_spaceship->impulseRequest)
    {
        my_spaceship->commandImpulse(res);
        my_spaceship->impulseRequest = res; //Set the impulseRequest directly, so it looks smooth on the client.
    }
    text(sf::FloatRect(rect.left, rect.top + rect.height, rect.width, text_size), string(int(my_spaceship->impulseRequest * 100)) + "%", AlignLeft, text_size);
    text(sf::FloatRect(rect.left, rect.top + rect.height + text_size, rect.width, text_size), string(int(my_spaceship->currentImpulse * 100)) + "%", AlignLeft, text_size);
}

void CrewUI::warpSlider(sf::FloatRect rect, float text_size)
{
    float res = vslider(rect, my_spaceship->warpRequest, 4.0, 0.0);
    if (res != my_spaceship->warpRequest)
        my_spaceship->commandWarp(res);
    text(sf::FloatRect(rect.left, rect.top + rect.height, rect.width, text_size), string(int(my_spaceship->warpRequest)), AlignLeft, text_size);
    text(sf::FloatRect(rect.left, rect.top + rect.height + text_size, rect.width, text_size), string(int(my_spaceship->currentWarp * 100)) + "%", AlignLeft, text_size);
}

void CrewUI::jumpSlider(float& jump_distance, sf::FloatRect rect, float text_size)
{
    jump_distance = vslider(rect, jump_distance, 40.0, 5.0, 10.0);
    jump_distance = roundf(jump_distance * 10.0f) / 10.0f;
    text(sf::FloatRect(rect.left, rect.top + rect.height, rect.width, text_size), string(jump_distance, 1) + "km", AlignLeft, text_size);
}

void CrewUI::jumpButton(float jump_distance, sf::FloatRect rect, float text_size)
{
    if (my_spaceship->jumpDelay > 0.0)
    {
        if (rect.width < text_size * 5)
            textbox(rect, string(int(ceilf(my_spaceship->jumpDelay))), AlignCenter, text_size);
        else
            keyValueDisplay(rect, 0.5, "Jump in", string(int(ceilf(my_spaceship->jumpDelay))), text_size);
    }else{
        if (button(rect, "Jump", text_size))
            my_spaceship->commandJump(jump_distance);
    }
}

void CrewUI::dockingButton(sf::FloatRect rect, float text_size)
{
    switch(my_spaceship->docking_state)
    {
    case DS_NotDocking:
        {
            PVector<Collisionable> obj_list = CollisionManager::queryArea(my_spaceship->getPosition() - sf::Vector2f(1000, 1000), my_spaceship->getPosition() + sf::Vector2f(1000, 1000));
            P<SpaceObject> dock_object;
            foreach(Collisionable, obj, obj_list)
            {
                dock_object = obj;
                if (dock_object && dock_object->canBeDockedBy(my_spaceship) && sf::length(dock_object->getPosition() - my_spaceship->getPosition()) < 1000.0)
                    break;
                dock_object = NULL;
            }
            
            if (dock_object)
            {
                if (button(rect, "Request Dock", text_size))
                    my_spaceship->commandDock(dock_object);
            }else{
                disabledButton(rect, "Request Dock", text_size);
            }
        }
        break;
    case DS_Docking:
        disabledButton(rect, "Docking...", text_size);
        break;
    case DS_Docked:
        if (button(rect, "Undock", text_size))
            my_spaceship->commandUndock();
        break;
    }
}

void CrewUI::weaponTube(EMissileWeapons load_type, int n, sf::FloatRect load_rect, sf::FloatRect fire_rect, float text_size)
{
    switch(my_spaceship->weaponTube[n].state)
    {
    case WTS_Empty:
        if (toggleButton(load_rect, load_type != MW_None && my_spaceship->weapon_storage[load_type] > 0, "Load", text_size) && load_type != MW_None)
            my_spaceship->commandLoadTube(n, load_type);
        disabledButton(fire_rect, "Empty", text_size);
        break;
    case WTS_Loaded:
        if (button(load_rect, "Unload", text_size))
            my_spaceship->commandUnloadTube(n);
        if (button(fire_rect, getMissileWeaponName(my_spaceship->weaponTube[n].type_loaded), text_size))
            my_spaceship->commandFireTube(n);
        break;
    case WTS_Loading:
        progressBar(fire_rect, my_spaceship->weaponTube[n].delay, my_spaceship->tubeLoadTime, 0.0);
        text(fire_rect, getMissileWeaponName(my_spaceship->weaponTube[n].type_loaded), AlignCenter, text_size, sf::Color(128, 128, 128));
        disabledButton(load_rect, "Loading", text_size);
        break;
    case WTS_Unloading:
        progressBar(fire_rect, my_spaceship->weaponTube[n].delay, 0.0, my_spaceship->tubeLoadTime);
        text(fire_rect, getMissileWeaponName(my_spaceship->weaponTube[n].type_loaded), AlignCenter, text_size, sf::Color(128, 128, 128));
        disabledButton(load_rect, "Unloading", text_size * 0.8);
        break;
    }
}

int CrewUI::frequencyCurve(sf::FloatRect rect, bool frequency_is_beam, bool more_damage_is_positive, int frequency)
{
    sf::RenderTarget& window = *getRenderTarget();
    
    float w = rect.width / (SpaceShip::max_frequency + 1);
    for(int n=0; n<=SpaceShip::max_frequency; n++)
    {
        float x = rect.left + w * n;
        float f;
        if (frequency_is_beam)
            f = frequencyVsFrequencyDamageFactor(frequency, n);
        else
            f = frequencyVsFrequencyDamageFactor(n, frequency);
        f = Tween<float>::linear(f, 0.5, 1.5, 0.1, 1.0);
        float h = rect.height * f;
        sf::RectangleShape bar(sf::Vector2f(w * 0.8, h));
        bar.setPosition(x, rect.top + rect.height - h);
        if (more_damage_is_positive)
            bar.setFillColor(sf::Color(255 * (1.0 - f), 255 * f, 0));
        else
            bar.setFillColor(sf::Color(255 * f, 255 * (1.0 - f), 0));
        window.draw(bar);
    }
    
    if (rect.contains(InputHandler::getMousePos()))
    {
        return int((InputHandler::getMousePos().x - rect.left) / w);
    }
    return -1;
}
