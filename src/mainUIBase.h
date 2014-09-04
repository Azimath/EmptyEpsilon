#ifndef MAIN_UI_H
#define MAIN_UI_H

#include "gui.h"
#include "spaceship.h"

class ScanGhost
{
public:
    sf::Vector2f position;
    P<SpaceObject> object;
};

class MainUIBase : public GUI, public Updatable
{
public:
    float scan_angle;
    std::vector<ScanGhost> scan_ghost;

    MainUIBase();

    virtual void onGui();
    virtual void update(float delta);
    
    void mainScreenSelectGUI();
    void drawStatic(float alpha=1.0);
    void drawRaderBackground(sf::Vector2f view_position, sf::Vector2f position, float size, float range, sf::FloatRect rect = sf::FloatRect(0, 0, getWindowSize().x, 900));
    void drawHeadingCircle(sf::Vector2f position, float size, sf::FloatRect rect = sf::FloatRect(0, 0, getWindowSize().x, 900));
    void drawRadarCuttoff(sf::Vector2f position, float size, sf::FloatRect rect = sf::FloatRect(0, 0, getWindowSize().x, 900));
    void drawWaypoints(sf::Vector2f view_position, sf::Vector2f position, float size, float range);
    void drawRadarSweep(sf::Vector2f position, float size, float angle);
    void drawRadar(sf::Vector2f position, float size, float range, bool long_range, P<SpaceObject> target, sf::FloatRect rect = sf::FloatRect(0, 0, getWindowSize().x, 900));
    void drawShipInternals(sf::Vector2f position, P<SpaceShip> ship, ESystem highlight_system);
    void draw3Dworld(sf::FloatRect rect = sf::FloatRect(0, 0, getWindowSize().x, 900));
};

#endif//MAIN_UI_H
