#ifndef CREW_STATION_SCREEN_H
#define CREW_STATION_SCREEN_H

#include "engine.h"
#include "gui/gui2_canvas.h"
#include "screenComponents/viewport3d.h"
#include "threatLevelEstimate.h"

class GuiButton;
class GuiToggleButton;
class GuiPanel;

class CrewStationScreen : public GuiCanvas, public Updatable
{
    P<ThreatLevelEstimate> threat_estimate;
private:
    GuiButton* select_station_button;
    GuiPanel* button_strip;
    struct CrewTabInfo {
        string name;
        GuiToggleButton* button;
        GuiElement* element;
    };
    std::vector<CrewTabInfo> tabs;
public:
    CrewStationScreen();
    void addStationTab(GuiElement* element, string name, string icon);
    void finishCreation();
    
    virtual void update(float delta);
    virtual void onKey(sf::Keyboard::Key key, int unicode);
};

#endif//CREW_STATION_SCREEN_H

