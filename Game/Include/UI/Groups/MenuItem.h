//
// Created by Pete on 2019-06-09.
//

#ifndef TALESCRIPTER_MENU_ITEM_H
#define TALESCRIPTER_MENU_ITEM_H

#include "Exceptions/MisuseException.hpp"
#include "UI/Elements/Button.h"

class MenuItem {
public:
    MenuItem(sf::RenderWindow *renderWindow, ResourceManager *rManager);

    ~MenuItem();

    void update(sf::Clock *gameTime);

    void drawFront();

    void drawBack(bool selected);

    void setAsButton(std::string name, std::string text, sf::Vector2i position);

    bool isSelected();

    std::string getName();

private:
    sf::RenderWindow *window;
    ResourceManager *resourceManager;
    int type;

    void typeSetValidation();

    static const int TYPE_NONE = 0;
    static const int TYPE_BUTTON = 1;

    // Holds the actual UI elements
    Button *button;
};

#endif //TALESCRIPTER_MENUITEM_H
