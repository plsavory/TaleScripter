//
// Created by Pete on 2019-07-25.
//

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "DatabaseConnection.hpp"
#include "BackgroundImageRenderer.hpp"
#include "ResourceManager.hpp"
#include "InputManager.hpp"
#include "UI/Groups/Menu.h"
#include "Menus/DataMenu.hpp"

DataMenu::DataMenu(sf::RenderWindow *renderWindow, ResourceManager *rManager, InputManager *inputManager) {
    window = renderWindow;
    resourceManager = rManager;
    mode = MODE_DEFAULT;
    offset = 0;
    navigationButtonCount = 0;

    needsClosing = false;

    // Create the save menu's background (Just a translucent box for now)
    sf::Vector2u windowSize = window->getSize(); // A little conversion needed for the next line...
    bg = new sf::RectangleShape(sf::Vector2f(windowSize.x, windowSize.y));
    bg->setPosition(0,0);
    bg->setFillColor(sf::Color(0,0,0,220));

    // Create the text to draw to the screen. we're drawing it directly so just bypass TextRenderer.
    heading = new sf::Text();
    heading->setFont(*resourceManager->getFontManager()->getFont("story_font")->getFont());
    heading->setFillColor(sf::Color::White);

    switch (mode) {
        case MODE_DEFAULT:
            heading->setString("Load/Save Data");
            break;
        case MODE_LOAD_ONLY:
            heading->setString("Load Data");
            break;
        case MODE_SAVE_ONLY:
            heading->setString("Save Data");
            break;
        default:
            throw MisuseException("Unknown data menu mode");
    }

    heading->setPosition(50,50);

    // Get a list of game saves that we need to handle
    // TODO: Un-hardcode this & move into a function
    float x = 0;
    float y = 0;

    int numberOfSaves = 0;

    for (int i = 0; i < 49; i++) {

        if (i % 3 == 0) {
            y+=130;
            x = 0;
        }

        if (i % 9 == 0) {
            y = 0;
        }

        auto *newSaveEntry = new DataMenuGameSave(window, resourceManager, sf::Vector2f(320+x, 120+y));
        newSaveEntry->setText(Utils::implodeString({"Save ", std::to_string(i)}));
        saves.push_back(newSaveEntry);

        x += 266;

        numberOfSaves = i;
    }

    // Create the navigation menu and its buttons
    navigationMenu = new Menu(window, resourceManager, inputManager);
    navigationMenu->setKeyboardInputEnabled(false);
    navigationMenu->addButton("back", "Back", sf::Vector2f(150, 320));
    navigationMenu->addButton("prev", "Previous", sf::Vector2f(320, 550));
    navigationMenu->addButton("next", "Next", sf::Vector2f(1118, 550));

    // Create the numbered navigation buttons
    int numberOfPages =(numberOfSaves/9)+1;

    for (int i = numberOfPages; i >= 0; i--) {

        if (numberOfPages <= 1) {
            break;
        }

        ++navigationButtonCount;
        int position = 320+(399-(numberOfPages*32)/2) + (i*32);
        navigationMenu->addButton(Utils::implodeString({"page_", std::to_string(i)}), std::to_string(i), sf::Vector2f(position, 550), sf::Vector2f(32, 32));
    }

}

DataMenu::~DataMenu() {
    delete(bg);
    delete(heading);
    delete(navigationMenu);
}

void DataMenu::update(sf::Clock *gameTime) {

    // Handle menu input
    if (navigationMenu->getSelectedItemName() == "back") {
        needsClosing = true;
    }

    if (navigationMenu->getSelectedItemName() == "prev") {

        if (offset > 0) {
            offset--;
        }

        navigationMenu->resetSelection();
    }

    if (navigationMenu->getSelectedItemName() == "next") {

        if ((offset*9)+9 < saves.size()) {
            offset++;
        }

        navigationMenu->resetSelection();
    }

    // Handle the page-select buttons
    for (int i = 0; i < navigationButtonCount; i++) {
        std::string name = Utils::implodeString({"page_", std::to_string(i)});
        if (navigationMenu->getSelectedItemName() == name) {
            offset = i;
            navigationMenu->resetSelection();
            break;
        }
    }

    navigationMenu->update(gameTime);
}

void DataMenu::draw() {
    window->draw(*bg);
    window->draw(*heading);

    int max = 0;

    if ((offset*9)+9 < saves.size()) {
        max = offset*9+9;
    } else {
        max = saves.size();
    }

    for (int i = offset*9; i < max; i++) {
        saves[i]->draw();
    }

    navigationMenu->draw();
}

bool DataMenu::needsToBeClosed() {
    return needsClosing;
}

DataMenuGameSave::DataMenuGameSave(sf::RenderWindow *renderWindow, ResourceManager *rManager, sf::Vector2f pos) {
    window = renderWindow;
    resourceManager = rManager;
    position = pos;

    // Create the background rectangle
    bg = new sf::RectangleShape();
    bg->setPosition(pos);
    bg->setSize(sf::Vector2f(256, 120));
    bg->setFillColor(sf::Color::Black);
    bg->setOutlineColor(sf::Color::Blue);
    bg->setOutlineThickness(2);

    // Text
    text = new sf::Text();
    text->setFont(*resourceManager->getFontManager()->getFont("story_font")->getFont());
    text->setPosition(position.x+20, position.y + 20);
    text->setOutlineColor(sf::Color::White);
    text->setCharacterSize(16);
}

DataMenuGameSave::~DataMenuGameSave() {
    delete(bg);
    delete(text);
}

void DataMenuGameSave::update() {

}

void DataMenuGameSave::draw() {
    window->draw(*bg);
    window->draw(*text);
}

void DataMenuGameSave::setText(std::string newText) {
    text->setString(newText);
}