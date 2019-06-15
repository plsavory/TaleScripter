//
// Created by Pete on 2019-06-09.
//

#ifndef TALESCRIPTER_TITLESCREEN_H
#define TALESCRIPTER_TITLESCREEN_H

#include "UI/Groups/Menu.h"

class TitleScreen {
public:
    TitleScreen(sf::RenderWindow *renderWindow, DatabaseConnection *novelDatabase, ResourceManager *rManager,
                InputManager *iManager, ScreenState *sState, CommonUI *cui);

    ~TitleScreen();

    void start();

    void update(sf::Clock *gameTime);

    void draw();

private:
    sf::RenderWindow *window;
    DatabaseConnection *novelDb;
    ResourceManager *resourceManager;
    Menu *menu;
    InputManager *inputManager;
    ScreenState *screenState;
    CommonUI *commonUI;
    ChoiceDialog *quitChoice;
};

#endif //TALESCRIPTER_TITLESCREEN_H
