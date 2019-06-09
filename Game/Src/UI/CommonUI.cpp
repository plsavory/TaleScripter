//
// Created by Pete on 2019-06-09.
//

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "DatabaseConnection.hpp"
#include "BackgroundImageRenderer.hpp"
#include "ResourceManager.hpp"
#include "UI/CommonUI.h"

CommonUI::CommonUI(sf::RenderWindow *renderWindow, ResourceManager *rManager) {
    window = renderWindow;
    resourceManager = rManager;
}

CommonUI::~CommonUI() {

}

void CommonUI::update(sf::Clock *gameTime) {

}

void CommonUI::draw() {

}

bool CommonUI::isDoingNothing() {
    return true; // Not very useful right now I will admit...
}