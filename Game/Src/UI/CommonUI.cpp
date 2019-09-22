//
// Created by Pete on 2019-06-09.
//

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "DatabaseConnection.hpp"
#include "BackgroundImageRenderer.hpp"
#include "ResourceManager.hpp"
#include "InputManager.hpp"
#include "Novel.hpp"
#include "ScreenState.h"
#include "UI/CommonUI.h"

CommonUI::CommonUI(sf::RenderWindow *renderWindow, ResourceManager *rManager, InputManager *iManager,
                   UIThemeManager *uiTManager, GameSaveManager *gsManager, ScreenState *screenState) {
    window = renderWindow;
    resourceManager = rManager;
    inputManager = iManager;
    activeDialog = nullptr;
    uiThemeManager = uiTManager;
    dataMenu = nullptr;
    saveScreenshot = nullptr;
    gameSaveManager = gsManager;
    this->screenState = screenState;
}

CommonUI::~CommonUI() {
    if (activeDialog) {
        delete (activeDialog);
    }
}

void CommonUI::update(sf::Clock *gameTime) {

    if (activeDialog) {
        activeDialog->update(gameTime);

        if (!dataMenu) {
            return;
        }

        if (!dataMenu->hasSelectedASave()) {
            return;
        }
    }

    if (dataMenu) {
        dataMenu->update(gameTime);

        // We're going to have to do some extra handling here for dialogs, the data menu should always be called through this class anyway though so it isn't a problem.
        if (dataMenu->hasSelectedASave()) {

            // Handle the input from the dialog
            if (dataMenu->getMode() == DataMenu::MODE_SAVE_ONLY) {

                bool hasDestroyedDialog = false;

                // We have shown the choice dialog and it is now gone - act on it.
                if (activeDialog && !activeDialog->getSelectedItem().empty()) {
                    if (activeDialog->getSelectedItem() == "yes") {
                        gameSaveManager->save(dataMenu->getSelectedSave());
                        gameSaveManager->storeSaves();
                        dataMenu->getSaves();
                        dataMenu->resetSelectedSave();
                        removeChoiceDialog();
                    } else {
                        // Reset the data menu
                        dataMenu->resetSelectedSave();
                        removeChoiceDialog();
                    }

                    hasDestroyedDialog = true;
                }

                if (!activeDialog && !hasDestroyedDialog) {
                    // We need to show a dialog in this instance
                    showChoiceDialog("Save to this data slot?", {"yes", "no"}, {"Yes", "No"});
                }

            } else {

                bool hasDestroyedDialog = false;

                if (activeDialog && !activeDialog->getSelectedItem().empty()) {

                    // Dialog is currently open...
                    if (activeDialog->getSelectedItem() == "yes") {
                        screenState->loadGame();
                        gameSaveManager->load(dataMenu->getSelectedSave());
                        dataMenu->resetSelectedSave();
                        removeChoiceDialog();
                        dataMenu->close();
                    } else {
                        dataMenu->resetSelectedSave();
                        removeChoiceDialog();
                    }

                    hasDestroyedDialog = true;

                }

                if (!activeDialog && !hasDestroyedDialog) {
                    // We need to show a dialog in this instance
                    showChoiceDialog("Load this save?", {"yes", "no"}, {"Yes", "No"});
                }

            }

        }
    }

    if (dataMenu && dataMenu->needsToBeClosed()) {
        delete (dataMenu);
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
void CommonUI::showLoadMenu() {
    dataMenu = new DataMenu(window, resourceManager, inputManager, DataMenu::MODE_LOAD_ONLY, gameSaveManager);
}

void CommonUI::showSaveMenu() {
    takeScreenshotForSave();
    dataMenu = new DataMenu(window, resourceManager, inputManager, DataMenu::MODE_SAVE_ONLY, gameSaveManager);
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
    delete (activeDialog);
    activeDialog = nullptr;
}

UIThemeManager *CommonUI::getUIThemeManager() {
    if (!uiThemeManager) {
        throw MisuseException("UI Theme manager has not yet been set");
    }

    return uiThemeManager;
}

void CommonUI::takeScreenshotForSave() {
    if (saveScreenshot) {
        delete (saveScreenshot);
    }

    saveScreenshot = new sf::Texture();
    saveScreenshot->update(*window);
}