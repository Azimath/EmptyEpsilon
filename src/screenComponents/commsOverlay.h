#ifndef GUI_COMMS_OVERLAY_H
#define GUI_COMMS_OVERLAY_H

#include "gui/gui2.h"

class GuiCommsOverlay : public GuiElement
{
private:
    GuiBox* opening_box;
    GuiProgressbar* opening_progress;
    
    GuiBox* hailed_box;
    GuiLabel* hailed_label;

    GuiBox* no_response_box;
public:
    GuiCommsOverlay(GuiContainer* owner);
    
    virtual void onDraw(sf::RenderTarget& window);
};

#endif//GUI_INDICATOR_OVERLAYS_H

