//
// Created by Pete on 2019-06-09.
//

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "DatabaseConnection.hpp"
#include "BackgroundImageRenderer.hpp"
#include "ResourceManager.hpp"
#include "UI/CommonUI.h"

CommonUI::CommonUI(sf::RenderWindow *renderWindow, ResourceManager *rManager, InputManager *iManager) {
    window = renderWindow;
    resourceManager = rManager;
    inputManager = iManager;
    activeDialog = nullptr;
}

CommonUI::~CommonUI() {
    if (activeDialog) {
        delete (activeDialog);
    }
}

void CommonUI::update(sf::Clock *gameTime) {

    if (activeDialog) {
        activeDialog->update(gameTime);
        return;
    }

}

void CommonUI::draw() {

    if (activeDialog) {
        activeDialog->draw();
    }

}

bool CommonUI::isDoingNothing() {

    if (activeDialog) {
        if (activeDialog->getState() == ChoiceDialog::STATE_ACTIVE) {
            return false;
        }
    }

    return true;
}

ChoiceDialog *CommonUI::showChoiceDialog(const std::string &text, const std::vector<std::string> &optionNames,
                                         const std::vector<std::string> &optionText) {
    if (activeDialog) {
        throw MisuseException(
                Utils::implodeString({"Attempting to show a dialog (", text, ") while one is already being drawn."}));
    }
    activeDialog = new ChoiceDialog(window, resourceManager, text, optionNames, optionText, inputManager);
    return activeDialog;
}