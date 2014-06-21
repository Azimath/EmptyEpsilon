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
    engine->registerObject("inputHandler", new InputHandler());
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
    FactionInfo::setState(1, 2, FVF_Enemy);
    
    new MainMenu();
    
    engine->runMainLoop();
    
    delete engine;
    return 0;
}
