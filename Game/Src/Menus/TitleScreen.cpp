//
// Created by Pete on 2019-06-09.
//

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "InputManager.hpp"
#include "Database/DatabaseConnection.hpp"
#include "BackgroundImageRenderer.hpp"
#include "ResourceManager.hpp"
#include "Menus/TitleScreen.h"

TitleScreen::TitleScreen(sf::RenderWindow *renderWindow, DatabaseConnection *novelDatabase,
                         ResourceManager *rManager, InputManager *iManager) {
    window = renderWindow;
    novelDb = novelDatabase;
    resourceManager = rManager;
    inputManager = iManager;
    menu = new Menu(window, resourceManager, inputManager);

    // Add the menu items that we need
    // TODO: Load these from the database
    menu->addButton("start", "Start", sf::Vector2i(570,360));
    menu->addButton("quit", "Quit", sf::Vector2i(720,360));
}

TitleScreen::~TitleScreen() {
    delete(menu);
}

void TitleScreen::start() {
    // TODO: Play title music
}

void TitleScreen::update(sf::Clock *gameTime) {
    menu->update(gameTime);
}

void TitleScreen::draw() {
    menu->draw();
}