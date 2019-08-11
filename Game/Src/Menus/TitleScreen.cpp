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
#include "Novel.hpp"
#include "UI/CommonUI.h"
#include "MusicManager.hpp"
#include "Engine.hpp"
#include "Menus/TitleScreen.h"

TitleScreen::TitleScreen(Engine *engine, DatabaseConnection *novelDatabase,
                         ResourceManager *rManager, InputManager *iManager, ScreenState *sState, CommonUI *cui) {
    window = engine->getWindow();
    backgroundImageRenderer = engine->getBackgroundImageRenderer();
    novelDb = novelDatabase;
    resourceManager = rManager;
    inputManager = iManager;
    menu = nullptr;
    screenState = sState;
    commonUI = cui;
    quitChoice = nullptr;
    musicManager = resourceManager->getMusicManager();
}

TitleScreen::~TitleScreen() {
    delete (menu);
}

void TitleScreen::start() {
    getData();
}

void TitleScreen::update(sf::Clock *gameTime) {

    if (quitChoice) {
        // Don't continue unless we have processed dialog input
        if (quitChoice->getSelectedItem().empty()) {
            return;
        }

        if (quitChoice->getSelectedItem() == "yes") {
            window->close();
        }

        if (quitChoice->getSelectedItem() == "no") {
            commonUI->removeChoiceDialog();
            quitChoice = nullptr; // We don't want any bad memory access exceptions now, do we?
            menu->resetSelection(); // Prevent the quit menu from immediately opening again
        }
        return;
    }

    std::string selectedMenuItem = menu->getSelectedItemName();

    if (selectedMenuItem == "start") {
        // Move to the novel screen when user selected 'start'
        screenState->changeState(ScreenState::STATE_NOVEL);
    }

    if (selectedMenuItem == "data_loadonly") {
        commonUI->showLoadMenu();
    }

    if (selectedMenuItem == "quit") {
        quitChoice = commonUI->showChoiceDialog("Are you sure that you want to quit?", {"yes", "no"}, {"Yes", "No"});
    }

    menu->resetSelection();

    if (menu) {
        menu->update(gameTime);
    }

}

void TitleScreen::draw() {
    if (menu) {
        menu->draw();
    }
}

void TitleScreen::getData() {
    // TODO: Fetch a different title screen depending on the game's state, currently we only support one.
    auto *dataSet = new DataSet();
    novelDb->executeQuery("SELECT * FROM title_screens WHERE id = 1;", dataSet);

    if (dataSet->getRowCount() == 0) {
        throw ResourceException("No title screen data was found.");
    }

    // Construct the menu
    menu = new Menu(window, resourceManager, inputManager, novelDb, dataSet->getRow(0)->getColumn("id")->getData()->asInteger());

    backgroundImageRenderer->setBackground(dataSet->getRow(0)->getColumn("background_image_id")->getData()->asInteger());
    musicManager->playAudioStream(dataSet->getRow(0)->getColumn("background_music_id")->getData()->asInteger(), nullptr);
}