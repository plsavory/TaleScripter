//
// Created by Pete on 2019-06-09.
//
// Handles UI elements and windows that are common across multiple screens (Dialog boxes, overlay menus, save/load game menus etc)

#ifndef TALESCRIPTER_COMMON_UI_H
#define TALESCRIPTER_COMMON_UI_H

#include "UI/Themes/UIThemeManager.h"
#include "UI/Groups/ChoiceDialog.h"

class CommonUI {
public:
    CommonUI(sf::RenderWindow *renderWindow, ResourceManager *rManager, InputManager *iManager, UIThemeManager *uiTManager);

    ~CommonUI();

    void update(sf::Clock *gameTime);

    void draw();

    bool isDoingNothing();

    ChoiceDialog *
    showChoiceDialog(const std::string &text, const std::vector<std::string> &optionNames,
                     const std::vector<std::string> &optionText);

    UIThemeManager* getUIThemeManager();

private:
    sf::RenderWindow *window;
    ResourceManager *resourceManager;
    ChoiceDialog *activeDialog;
    InputManager *inputManager;
    UIThemeManager *uiThemeManager;
};

#endif //TALESCRIPTER_COMMON_UI_H
