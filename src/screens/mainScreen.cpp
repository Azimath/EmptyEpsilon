#include "playerInfo.h"
#include "gameGlobalInfo.h"
#include "mainScreen.h"
#include "main.h"
#include "menus/shipSelectionScreen.h"

#include "screenComponents/indicatorOverlays.h"
#include "screenComponents/selfDestructIndicator.h"
#include "screenComponents/globalMessage.h"
#include "screenComponents/jumpIndicator.h"

ScreenMainScreen::ScreenMainScreen()
{
    viewport = new GuiViewport3D(this, "VIEWPORT");
    viewport->showCallsigns()->showHeadings()->showSpacedust();
    viewport->setPosition(0, 0, ATopLeft)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
    // TODO: Small corner radar
    
    new GuiJumpIndicator(this);
    new GuiSelfDestructIndicator(this);
    new GuiGlobalMessage(this);
    new GuiIndicatorOverlays(this);
}

void ScreenMainScreen::update(float delta)
{
    /*
    if (game_client && !game_client->isConnected())
    {
        destroy();
        disconnectFromServer();
        returnToMainMenu();
        return;
    }
    */

    if (my_spaceship)
    {
        float target_camera_yaw = my_spaceship->getRotation();
        switch(my_spaceship->main_screen_setting)
        {
        case MSS_Back: target_camera_yaw += 180; break;
        case MSS_Left: target_camera_yaw -= 90; break;
        case MSS_Right: target_camera_yaw += 90; break;
        default: break;
        }
        camera_pitch = 30.0f;

        const float camera_ship_distance = 420.0f;
        const float camera_ship_height = 420.0f;
        sf::Vector2f cameraPosition2D = my_spaceship->getPosition() + sf::vector2FromAngle(target_camera_yaw) * -camera_ship_distance;
        sf::Vector3f targetCameraPosition(cameraPosition2D.x, cameraPosition2D.y, camera_ship_height);
#ifdef DEBUG
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
        {
            targetCameraPosition.x = my_spaceship->getPosition().x;
            targetCameraPosition.y = my_spaceship->getPosition().y;
            targetCameraPosition.z = 3000.0;
            camera_pitch = 90.0f;
        }
#endif
        camera_position = camera_position * 0.9f + targetCameraPosition * 0.1f;
        camera_yaw += sf::angleDifference(camera_yaw, target_camera_yaw) * 0.1f;

        switch(my_spaceship->main_screen_setting)
        {
        case MSS_Front:
        case MSS_Back:
        case MSS_Left:
        case MSS_Right:
            viewport->show();
            break;
        case MSS_Tactical:
            //renderTactical(*getRenderTarget());
            viewport->hide();
            break;
        case MSS_LongRange:
            //renderLongRange(*getRenderTarget());
            viewport->hide();
            break;
        }
    }
}

void ScreenMainScreen::onClick(sf::Vector2f mouse_position)
{
    if (!my_spaceship)
        return;
    
    if (InputHandler::mouseIsPressed(sf::Mouse::Left))
    {
        switch(my_spaceship->main_screen_setting)
        {
        case MSS_Front: my_spaceship->commandMainScreenSetting(MSS_Left); break;
        case MSS_Left: my_spaceship->commandMainScreenSetting(MSS_Back); break;
        case MSS_Back: my_spaceship->commandMainScreenSetting(MSS_Right); break;
        case MSS_Right: my_spaceship->commandMainScreenSetting(MSS_Front); break;
        default: my_spaceship->commandMainScreenSetting(MSS_Front); break;
        }
    }
    if (InputHandler::mouseIsPressed(sf::Mouse::Right))
    {
        switch(my_spaceship->main_screen_setting)
        {
        case MSS_Front: my_spaceship->commandMainScreenSetting(MSS_Right); break;
        case MSS_Right: my_spaceship->commandMainScreenSetting(MSS_Back); break;
        case MSS_Back: my_spaceship->commandMainScreenSetting(MSS_Left); break;
        case MSS_Left: my_spaceship->commandMainScreenSetting(MSS_Front); break;
        default: my_spaceship->commandMainScreenSetting(MSS_Front); break;
        }
    }
    if (InputHandler::mouseIsPressed(sf::Mouse::Middle))
    {
        switch(my_spaceship->main_screen_setting)
        {
        default:
            if (gameGlobalInfo->allow_main_screen_tactical_radar)
                my_spaceship->commandMainScreenSetting(MSS_Tactical);
            else if (gameGlobalInfo->allow_main_screen_long_range_radar)
                my_spaceship->commandMainScreenSetting(MSS_LongRange);
            break;
        case MSS_Tactical:
            if (gameGlobalInfo->allow_main_screen_long_range_radar)
                my_spaceship->commandMainScreenSetting(MSS_LongRange);
            break;
        case MSS_LongRange:
            if (gameGlobalInfo->allow_main_screen_tactical_radar)
                my_spaceship->commandMainScreenSetting(MSS_Tactical);
            break;
        }
    }
}

void ScreenMainScreen::onKey(sf::Keyboard::Key key, int unicode)
{
    if (!my_spaceship)
        return;

    switch(key)
    {
    case sf::Keyboard::Up:
        if (my_spaceship)
            my_spaceship->commandMainScreenSetting(MSS_Front);
        break;
    case sf::Keyboard::Left:
        if (my_spaceship)
            my_spaceship->commandMainScreenSetting(MSS_Left);
        break;
    case sf::Keyboard::Right:
        if (my_spaceship)
            my_spaceship->commandMainScreenSetting(MSS_Right);
        break;
    case sf::Keyboard::Down:
        if (my_spaceship)
            my_spaceship->commandMainScreenSetting(MSS_Back);
        break;
    case sf::Keyboard::Tab:
        if (my_spaceship && gameGlobalInfo->allow_main_screen_tactical_radar)
            my_spaceship->commandMainScreenSetting(MSS_Tactical);
        break;
    case sf::Keyboard::Q:
        if (my_spaceship && gameGlobalInfo->allow_main_screen_long_range_radar)
            my_spaceship->commandMainScreenSetting(MSS_LongRange);
        break;
    
    case sf::Keyboard::Escape:
    case sf::Keyboard::Home:
        destroy();
        new ShipSelectionScreen();
        break;
    case sf::Keyboard::P:
        if (game_server)
            engine->setGameSpeed(0.0);
        break;
    default:
        break;
    }
}
