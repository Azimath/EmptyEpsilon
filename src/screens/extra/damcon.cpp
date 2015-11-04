#include "damcon.h"

#include "playerInfo.h"
#include "screenComponents/shipInternalView.h"

DamageControlScreen::DamageControlScreen(GuiContainer* owner)
: GuiOverlay(owner, "DAMCON_SCREEN", sf::Color::Black)
{
    (new GuiShipInternalView(this, "SHIP_INTERNAL_VIEW", 48.0f * 1.5f))->setShip(my_spaceship)->setPosition(300, 0, ATopLeft)->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
    
    GuiAutoLayout* system_health_layout = new GuiAutoLayout(this, "DAMCON_LAYOUT", GuiAutoLayout::LayoutVerticalTopToBottom);
    system_health_layout->setPosition(0, 0, ACenterLeft)->setSize(300, 600);
    for(unsigned int n=0; n<SYS_COUNT; n++)
    {
        system_health[n] = new GuiKeyValueDisplay(system_health_layout, "DAMCON_HEALTH_" + string(n), 0.8, getSystemName(ESystem(n)), "0%");
        system_health[n]->setSize(GuiElement::GuiSizeMax, 40);
    }
}

void DamageControlScreen::onDraw(sf::RenderTarget& window)
{
    GuiOverlay::onDraw(window);
    
    if (my_spaceship)
    {
        for(unsigned int n=0; n<SYS_COUNT; n++)
        {
            system_health[n]->setVisible(my_spaceship->hasSystem(ESystem(n)));
            system_health[n]->setValue(string(int(my_spaceship->systems[n].health * 100)) + "%");
        }
    }
}
