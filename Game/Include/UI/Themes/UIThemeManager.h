//
// Created by Pete on 2019-06-15.
// Handles storage of colour values and links to UI images
//

#ifndef TALESCRIPTER_UI_THEME_MANAGER_H
#define TALESCRIPTER_UI_THEME_MANAGER_H

#include "UITheme.h"

class UIThemeManager {
public:
    UIThemeManager(sf::RenderWindow *renderWindow, ResourceManager *rManager, DatabaseConnection *novelDB);
    ~UIThemeManager();
    UITheme* getActiveTheme();
private:
    sf::RenderWindow *window;
    ResourceManager *resourceManager;
    DatabaseConnection *novel;
    std::vector<UITheme*> themes;
    int activeTheme; // Stores the array index of the currently-active theme
};

#endif //TALESCRIPTER_UI_THEME_H
