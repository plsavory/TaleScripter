//
// Created by Pete on 2019-06-11.
//

#ifndef TALESCRIPTER_CHOICE_DIALOG_H
#define TALESCRIPTER_CHOICE_DIALOG_H

#include "InputManager.hpp"
#include "UI/Groups/Menu.h"

class ChoiceDialog {
public:
    ChoiceDialog(sf::RenderWindow *renderWindow, ResourceManager *rManager, const std::string &text,
                 const std::vector<std::string> &optionNames, const std::vector<std::string> &optionText, InputManager *inputManager);

    ~ChoiceDialog();

    std::string getSelectedItem();

    void update(sf::Clock *gameTime);

    void draw();

    void markForDeletion();

    bool needsDeleting();

    int getState();

    // States
    static const int STATE_ACTIVE = 0;
    static const int STATE_INACTIVE = 1;
    static const int STATE_DELETED = 2;

private:
    sf::RenderWindow *window;
    ResourceManager *resourceManager;
    Menu *menu;
    sf::Text *textObject;
    sf::FloatRect bounds;
    int state;
};

#endif //TALESCRIPTER_CHOICEDIALOG_H
