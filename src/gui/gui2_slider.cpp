#include "gui2_slider.h"

GuiSlider::GuiSlider(GuiContainer* owner, string id, float min_value, float max_value, float start_value, func_t func)
: GuiElement(owner, id), min_value(min_value), max_value(max_value), value(start_value), func(func)
{
}

void GuiSlider::onDraw(sf::RenderTarget& window)
{
    draw9Cut(window, rect, "button_background", sf::Color(64,64,64, 255));

    float x;
/*
    x = rect.left + (rect.width - rect.height) * (normal_value - min_value) / (max_value - min_value);
    sf::RectangleShape backgroundZero(sf::Vector2f(8.0, rect.height));
    backgroundZero.setPosition(x + rect.height / 2.0 - 4.0, rect.top);
    backgroundZero.setFillColor(sf::Color(8,8,8,255));
    renderTarget->draw(backgroundZero);
*/
    x = rect.left + (rect.width - rect.height) * (value - min_value) / (max_value - min_value);
    sf::Color color = sf::Color::White;
    if (has_focus)
        color = sf::Color(255,255,255, 128);
    draw9Cut(window, sf::FloatRect(x, rect.top, rect.height, rect.height), "button_background", color);
}

GuiElement* GuiSlider::onMouseDown(sf::Vector2f position)
{
    return this;
}

void GuiSlider::onMouseDrag(sf::Vector2f position)
{
    float new_value = (position.x - rect.left - (rect.height / 2.0)) / (rect.width - rect.height);
    new_value = min_value + (max_value - min_value) * new_value;
    if (min_value < max_value)
    {
        if (new_value < min_value)
            new_value = min_value;
        if (new_value > max_value)
            new_value = max_value;
    }else{
        if (new_value > min_value)
            new_value = min_value;
        if (new_value < max_value)
            new_value = max_value;
    }
    if (value != new_value)
    {
        value = new_value;
        func(value);
    }
}

void GuiSlider::onMouseUp(sf::Vector2f position)
{
}
