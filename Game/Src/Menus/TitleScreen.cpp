//
// Created by Pete on 2019-06-09.
//

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "InputManager.hpp"
#include "Database/DatabaseConnection.hpp"
#include "BackgroundImageRenderer.hpp"
#include "ResourceManager.hpp"
#include "ScreenState.h"
#include "UI/CommonUI.h"
#include "Menus/TitleScreen.h"

TitleScreen::TitleScreen(sf::RenderWindow *renderWindow, DatabaseConnection *novelDatabase,
                         ResourceManager *rManager, InputManager *iManager, ScreenState *sState, CommonUI *cui) {
    window = renderWindow;
    novelDb = novelDatabase;
    resourceManager = rManager;
    inputManager = iManager;
    menu = new Menu(window, resourceManager, inputManager);
    screenState = sState;
    commonUI = cui;
    quitChoice = nullptr;

    // Add the menu items that we need
    // TODO: Load these from the database
    menu->addButton("start", "Start", sf::Vector2f(570, 360));
    menu->addButton("quit", "Quit", sf::Vector2f(720, 360));
}

TitleScreen::~TitleScreen() {
    delete (menu);
}

void TitleScreen::start() {
    // TODO: Play title music
}

void TitleScreen::update(sf::Clock *gameTime) {

    if (quitChoice) {
        // Don't continue unless we have processed dialog input
        if (quitChoice->getSelectedItem().empty()) {
            return;
        }

        if (quitChoice->getSelectedItem() == "yes") {

        }
        return;
    }

    std::string selectedMenuItem = menu->getSelectedItemName();

    if (selectedMenuItem == "start") {
        // Move to the novel screen when user selected 'start'
        screenState->changeState(ScreenState::STATE_NOVEL);
    }

    if (selectedMenuItem == "quit") {
        quitChoice = commonUI->showChoiceDialog("Are you sure that you want to quit?", {"yes", "no"}, {"Yes", "No"});
    }

    menu->update(gameTime);

}

void TitleScreen::draw() {
    menu->draw();
}