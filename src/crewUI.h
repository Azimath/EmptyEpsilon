#ifndef CREW_UI_H
#define CREW_UI_H

#include "mainUI.h"
#include "playerInfo.h"
#include "spaceship.h"
#include "repairCrew.h"

class CrewUI : public MainUI
{
    ECrewPosition showPosition;
    EMissileWeapons tubeLoadType;
    float jumpDistance;
    P<RepairCrew> selected_crew;
    
    P<SpaceObject> scienceTarget;
    float scienceRadarDistance;
public:
    CrewUI();
    
    virtual void onGui();
    
    void helmsUI();
    void weaponsUI();
    void engineeringUI();
    void scienceUI();
};
#endif//CREW_UI_H
