#ifndef CREW_HELMS_UI
#define CREW_HELMS_UI

#include "crewUI.h"

class HelmsGhostDot
{
public:
    constexpr static float total_lifetime = 60.0f;
    
    sf::Vector2f position;
    float lifetime;
    
    HelmsGhostDot(sf::Vector2f pos) : position(pos), lifetime(total_lifetime) {}
};

class CrewHelmsUI : public CrewUI
{
public:
    float jump_distance;
    float ghost_delay;
    std::vector<HelmsGhostDot> ghost_dot;
    
    CrewHelmsUI();
    
    virtual void update(float delta);
    virtual void onCrewUI();
    
    virtual void onPauseHelpGui();
};

#endif//CREW_HELMS_UI
