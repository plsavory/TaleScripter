//
// Created by Pete on 2019-06-09.
//
// Handles UI elements and windows that are common across multiple screens (Dialog boxes, overlay menus, save/load game menus etc)

#ifndef TALESCRIPTER_COMMON_UI_H
#define TALESCRIPTER_COMMON_UI_H

class CommonUI {
public:
    CommonUI(sf::RenderWindow *renderWindow, ResourceManager *rManager);
    ~CommonUI();
    void update(sf::Clock *gameTime);
    void draw();
    bool isDoingNothing();
private:
    sf::RenderWindow *window;
    ResourceManager *resourceManager;
};

#endif //TALESCRIPTER_COMMON_UI_H
