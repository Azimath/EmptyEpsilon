#include "gui.h"
#include "mainMenus.h"
#include "factionInfo.h"
#include "main.h"

sf::Vector3f cameraPosition;
float cameraRotation;
sf::Shader objectShader;
sf::Shader basicShader;
sf::Font mainFont;
RenderLayer* backgroundLayer;
RenderLayer* objectLayer;
RenderLayer* effectLayer;
RenderLayer* hudLayer;
RenderLayer* mouseLayer;

int main(int argc, char** argv)
{
    new Engine();
    new DirectoryResourceProvider("resources/");
    textureManager.setDefaultSmooth(true);
    textureManager.setDefaultRepeated(true);
    textureManager.setAutoSprite(false);

    //Setup the rendering layers.
    backgroundLayer = new RenderLayer();
    objectLayer = new RenderLayer(backgroundLayer);
    effectLayer = new RenderLayer(objectLayer);
    hudLayer = new RenderLayer(effectLayer);
    mouseLayer = new RenderLayer(hudLayer);
    defaultRenderLayer = objectLayer;

    int width = 1600;
    int height = 900;
    int fsaa = 0;
    engine->registerObject("windowManager", new WindowManager(width, height, false, mouseLayer, fsaa));
    engine->registerObject("mouseRenderer", new MouseRenderer());
    
    P<ResourceStream> stream = getResourceStream("sansation.ttf");
    mainFont.loadFromStream(**stream);

    P<ResourceStream> vertexStream = getResourceStream("objectShader.vert");
    P<ResourceStream> fragmentStream = getResourceStream("objectShader.frag");
    objectShader.loadFromStream(**vertexStream, **fragmentStream);
    vertexStream = getResourceStream("basicShader.vert");
    fragmentStream = getResourceStream("basicShader.frag");
    basicShader.loadFromStream(**vertexStream, **fragmentStream);
    
    P<ScriptObject> shipTemplatesScript = new ScriptObject("shipTemplates.lua");
    shipTemplatesScript->destroy();
    
    factionInfo[0].name = "Neutral";
    factionInfo[1].name = "Human";
    factionInfo[2].name = "SpaceCow";
    factionInfo[3].name = "Sheeple";
    factionInfo[4].name = "PirateScorpions";
    factionInfo[0].gm_color = sf::Color(128, 128, 128);
    factionInfo[1].gm_color = sf::Color(255, 255, 255);
    factionInfo[2].gm_color = sf::Color(255, 0, 0);
    factionInfo[3].gm_color = sf::Color(255, 128, 0);
    factionInfo[4].gm_color = sf::Color(255, 0, 128);
    FactionInfo::setState(0, 4, FVF_Enemy);

    FactionInfo::setState(1, 2, FVF_Enemy);
    FactionInfo::setState(1, 3, FVF_Enemy);
    FactionInfo::setState(1, 4, FVF_Enemy);
    
    FactionInfo::setState(2, 3, FVF_Enemy);
    FactionInfo::setState(2, 4, FVF_Enemy);
    FactionInfo::setState(3, 4, FVF_Enemy);
    
    new MainMenu();
    
    engine->runMainLoop();
    
    delete engine;
    return 0;
}
