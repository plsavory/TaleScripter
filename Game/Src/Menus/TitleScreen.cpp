//
// Created by Pete on 2019-06-09.
//

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "Database/DatabaseConnection.hpp"
#include "BackgroundImageRenderer.hpp"
#include "ResourceManager.hpp"
#include "Menus/TitleScreen.h"

TitleScreen::TitleScreen(sf::RenderWindow *renderWindow, DatabaseConnection *novelDatabase,
                         ResourceManager *rManager) {
    window = renderWindow;
    novelDb = novelDatabase;
    resourceManager = rManager;
}

TitleScreen::~TitleScreen() {

}

void TitleScreen::start() {

}

void TitleScreen::update(sf::Clock *gameTime) {

}

void TitleScreen::draw() {

}