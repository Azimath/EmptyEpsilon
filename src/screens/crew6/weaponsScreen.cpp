#include "playerInfo.h"
#include "weaponsScreen.h"

#include "screenComponents/missileTubeControls.h"
#include "screenComponents/shieldsEnableButton.h"

WeaponsScreen::WeaponsScreen(GuiContainer* owner)
: GuiOverlay(owner, "WEAPONS_SCREEN", sf::Color::Black)
{
    radar = new GuiRadarView(this, "HELMS_RADAR", 5000.0);
    radar->setPosition(0, 0, ACenter)->setSize(GuiElement::GuiSizeMatchHeight, 800);
    radar->setRangeIndicatorStepSize(1000.0)->shortRange()->enableCallsigns()->enableHeadingIndicators()->setStyle(GuiRadarView::Circular);
    radar->setCallbacks(
        [this](sf::Vector2f position) {
            P<SpaceObject> target;
            PVector<Collisionable> list = CollisionManager::queryArea(position - sf::Vector2f(50, 50), position + sf::Vector2f(50, 50));
            foreach(Collisionable, obj, list)
            {
                P<SpaceObject> spaceObject = obj;
                if (spaceObject && spaceObject->canBeTargeted() && spaceObject != my_spaceship)
                {
                    if (!target || sf::length(position - spaceObject->getPosition()) < sf::length(position - target->getPosition()))
                        target = spaceObject;
                }
            }
            if (target && my_spaceship)
            {
                my_spaceship->commandSetTarget(target);
            }else{
                //missile_targeting = true;
            }
        },
        [this](sf::Vector2f position) {
            
        },
        [this](sf::Vector2f position) {
            
        }
    );
    //Missile targeting, missile target projection, beam frequency selection, beam subtarget target selection

    energy_display = new GuiKeyValueDisplay(this, "ENERGY_DISPLAY", 0.45, "Energy", "");
    energy_display->setTextSize(20)->setPosition(20, 100, ATopLeft)->setSize(240, 40);
    shields_display = new GuiKeyValueDisplay(this, "SHIELDS_DISPLAY", 0.45, "Shields", "");
    shields_display->setTextSize(20)->setPosition(20, 140, ATopLeft)->setSize(240, 40);
    
    new GuiMissileTubeControls(this, "MISSILE_TUBES");
    
    (new GuiShieldsEnableButton(this, "SHIELDS_ENABLE"))->setPosition(-20, -20, ABottomRight)->setSize(280, 50);
}

void WeaponsScreen::onDraw(sf::RenderTarget& window)
{
    if (my_spaceship)
    {
        energy_display->setValue(string(int(my_spaceship->energy_level)));
        shields_display->setValue(string(int(100 * my_spaceship->front_shield / my_spaceship->front_shield_max)) + ":" + string(int(100 * my_spaceship->rear_shield / my_spaceship->rear_shield_max)));
        radar->setTarget(my_spaceship->getTarget());
    }
    GuiOverlay::onDraw(window);
}
