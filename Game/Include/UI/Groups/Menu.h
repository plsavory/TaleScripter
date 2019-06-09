//
// Created by Pete on 2019-06-09.
//

#ifndef TALESCRIPTER_MENU_H
#define TALESCRIPTER_MENU_H

#include <vector>

#include "UI/Groups/MenuItem.h"

class Menu {
public:
    Menu(sf::RenderWindow *renderWindow, ResourceManager *rManager, InputManager *iManager);
    ~Menu();
    void update(sf::Clock *gameTime);
    void draw();
    void addButton(std::string name, std::string text);
    void addButton(const std::string &name, const std::string &text, const sf::Vector2i &position);
    MenuItem* getItem(std::string name);
    static const int ORIENTATION_HORIZONTAL = 0;
    static const int ORIENTATION_VERTICAL = 1;
private:
    sf::RenderWindow *window;
    ResourceManager *resourceManager;
    std::vector<MenuItem*> items;
    std::string selectedItem;
    int orientation; // Defines whether the menu will use the up/down or left/right keys/buttons to navigate
    int currentSelection;
    InputManager *inputManager;
    std::string incrementEventName;
    std::string decrementEventName;
};

#endif //TALESCRIPTER_MENU_H
