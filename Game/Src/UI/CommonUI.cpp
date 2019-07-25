//
// Created by Pete on 2019-06-09.
//

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "DatabaseConnection.hpp"
#include "BackgroundImageRenderer.hpp"
#include "ResourceManager.hpp"
#include "InputManager.hpp"
#include "UI/CommonUI.h"

CommonUI::CommonUI(sf::RenderWindow *renderWindow, ResourceManager *rManager, InputManager *iManager, UIThemeManager *uiTManager) {
    window = renderWindow;
    resourceManager = rManager;
    inputManager = iManager;
    activeDialog = nullptr;
    uiThemeManager = uiTManager;
    dataMenu = nullptr;
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

    if (dataMenu) {
        dataMenu->update(gameTime);
    }

    if (dataMenu && dataMenu->needsToBeClosed()) {
        delete(dataMenu);
        dataMenu = nullptr;
    }
}

void CommonUI::draw() {

    if (dataMenu) {
        dataMenu->draw();
    }

    if (activeDialog) {
        activeDialog->draw();
    }

}

/**
 * Returns true if no overlay menus or dialogs are currently being drawn
 * @return
 */
bool CommonUI::isDoingNothing() {

    if (activeDialog) {
        if (activeDialog->getState() == ChoiceDialog::STATE_ACTIVE) {
            return false;
        }
    }

    return dataMenu == nullptr;

}

/**
 * Displays the data menu for loading and saving progress
 */
void CommonUI::showDataMenu() {
    dataMenu = new DataMenu(window, resourceManager, inputManager);
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

void CommonUI::removeChoiceDialog() {
    delete(activeDialog);
    activeDialog = nullptr;
}

UIThemeManager* CommonUI::getUIThemeManager() {
    if (!uiThemeManager) {
        throw MisuseException("UI Theme manager has not yet been set");
    }

    return uiThemeManager;
}