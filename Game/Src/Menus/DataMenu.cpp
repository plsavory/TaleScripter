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
#include "Novel.hpp"
#include "Menus/DataMenu.hpp"
#include <cmath>
#include <math.h>

DataMenu::DataMenu(sf::RenderWindow *renderWindow, ResourceManager *rManager, InputManager *inputManager, int menuMode,
                   GameSaveManager *gsManager) {
    window = renderWindow;
    resourceManager = rManager;
    mode = menuMode;
    offset = 0;
    navigationButtonCount = 0;
    selectedSave = -1;
    this->inputManager = inputManager;
    gameSaveManager = gsManager;
    navigationMenu = nullptr;
    needsClosing = false;

    // Create the save menu's background (Just a translucent box for now)
    sf::Vector2u windowSize = window->getSize(); // A little conversion needed for the next line...
    bg = new sf::RectangleShape(sf::Vector2f(windowSize.x, windowSize.y));
    bg->setPosition(0, 0);
    bg->setFillColor(sf::Color(0, 0, 0, 220));

    // Create the text to draw to the screen. we're drawing it directly so just bypass TextRenderer.
    heading = new sf::Text();
    heading->setFont(*resourceManager->getFontManager()->getFont("story_font")->getFont());
    heading->setFillColor(sf::Color::White);

    pageText = new sf::Text();
    pageText->setFont(*resourceManager->getFontManager()->getFont("story_font")->getFont());
    pageText->setFillColor(sf::Color::White);
    pageText->setPosition(sf::Vector2f(650, 80));
    pageText->setString("Page 1");

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

    heading->setPosition(50, 50);

    screenshot = new sf::Texture();
    screenshot->create(1280,720);
    screenshot->update(*window);
    gameSaveManager->setScreenshot(screenshot); // To ensure that the save manager has a pointer to the screenshot texture at all times.
    getSaves();
}

DataMenu::~DataMenu() {
    delete (bg);
    delete (heading);
    delete (navigationMenu);
    delete (screenshot);
}

void DataMenu::update(sf::Clock *gameTime) {

    if (hasSelectedASave()) {
        return;
    }

    // Handle menu input
    if (navigationMenu->getSelectedItemName() == "close") {
        needsClosing = true;
    }

    if (navigationMenu->getSelectedItemName() == "prev") {

        if (offset > 0) {
            setOffset(offset - 1);
        }

        navigationMenu->resetSelection();
    }

    if (navigationMenu->getSelectedItemName() == "next") {

        if ((offset * 9) + 9 < saves.size()) {
            setOffset(offset + 1);
        }

        navigationMenu->resetSelection();
    }

    // Handle the page-select buttons
    for (int i = 0; i < navigationButtonCount; i++) {
        std::string name = Utils::implodeString({"page_", std::to_string(i)});
        if (navigationMenu->getSelectedItemName() == name) {
            setOffset(i);
            navigationMenu->resetSelection();
            break;
        }
    }

    navigationMenu->update(gameTime);

    // Update the save items
    for (int i = 0; i < saves.size(); i++) {
        if (saves[i]->update()) {
            // Set the id of the selected slot so that we know which one is selected (1-indexed as it'll be a database ID if loading) if a save item is clicked.
            selectedSave = mode == MODE_LOAD_ONLY ? i+1 : i;
            return;
        }
    }

}

void DataMenu::draw() {
    window->draw(*bg);
    window->draw(*heading);

    int max = 0;

    if ((offset * 9) + 9 < saves.size()) {
        max = offset * 9 + 9;
    } else {
        max = saves.size();
    }

    for (int i = offset * 9; i < max; i++) {
        saves[i]->draw();
    }

    navigationMenu->draw();

    window->draw(*pageText);
}

bool DataMenu::hasSelectedASave() {
    return selectedSave != -1;
}

int DataMenu::getSelectedSave() {
    return selectedSave;
}

bool DataMenu::needsToBeClosed() {
    return needsClosing;
}

void DataMenu::setOffset(int newOffset) {
    offset = newOffset;
    pageText->setString(Utils::implodeString({"Page ", std::to_string(offset + 1)}));
}

int DataMenu::getMode() {
    return mode;
}

void DataMenu::resetSelectedSave() {
    selectedSave = -1;
}

void DataMenu::close() {
    needsClosing = true;
}

void DataMenu::getSaves() {

    // Get a list of game saves that we need to handle
    int numberOfPages = populateSaves();

    // Create the navigation menu and its buttons
    if (navigationMenu) {
        delete(navigationMenu);
    }

    navigationMenu = new Menu(window, resourceManager, inputManager);
    navigationMenu->setKeyboardInputEnabled(false);
    navigationMenu->addButton("close", "Close", sf::Vector2f(150, 320));
    navigationMenu->addButton("prev", "Previous", sf::Vector2f(320, 550));
    navigationMenu->addButton("next", "Next", sf::Vector2f(1118, 550));

    // Create the numbered navigation buttons
    for (int i = numberOfPages; i >= 0; i--) {

        if (numberOfPages <= 1) {
            break;
        }

        ++navigationButtonCount;
        int position = 320 + (399 - (numberOfPages * 32) / 2) + (i * 32);
        navigationMenu->addButton(Utils::implodeString({"page_", std::to_string(i)}), std::to_string(i + 1),
                                  sf::Vector2f(position, 550), sf::Vector2f(32, 32));
    }
}

/**
 * Loads saves from the database and positions them appropriately on the main menu
 * @return
 */
int DataMenu::populateSaves() {

    // TODO: Support both modes of displaying saves, selectable by the game's author in the novel database

    // Delete any already existing save items
    for (auto &save : saves) {
        delete(save);
    }
    saves.clear();

    auto *gameSaves = gameSaveManager->getSaves();

    float x = 0;
    float y = 0;

    int numberOfSaves = 0;

    for (int i = 0; i <= gameSaves->getRowCount(); i++) {

        if (i == 0 && mode == MODE_LOAD_ONLY) {
            continue;
        }

        int index = mode == MODE_LOAD_ONLY ? i-1 : i;

        if (index % 3 == 0) {
            y += 130;
            x = 0;
        }

        if (index % 9 == 0) {
            y = 0;
        }

        auto *newSaveEntry = new DataMenuGameSave(window, resourceManager, inputManager, sf::Vector2f(320 + x, 120 + y),
                                                  i, mode, gameSaveManager);

        saves.push_back(newSaveEntry);

        x += 266;

        numberOfSaves = i;
    }

    getThumbnails();

    return ceil((double)numberOfSaves / (double)9);

}

void DataMenu::getThumbnails() {
    for (int i = 0; i < saves.size(); i++) {

        if (i == 0 && mode == MODE_SAVE_ONLY) {
            continue;
        }

        saves[i]->setThumbnail(gameSaveManager->getThumbnail(mode == MODE_SAVE_ONLY ? i : i+1));
    }
}

DataMenuGameSave::DataMenuGameSave(sf::RenderWindow *renderWindow, ResourceManager *rManager,
                                   InputManager *inputManager, sf::Vector2f pos, int saveId, int saveMode,
                                   GameSaveManager *gsManager) {
    window = renderWindow;
    resourceManager = rManager;
    position = pos;
    id = saveId;
    this->inputManager = inputManager;
    this->saveMode = saveMode;
    gameSaveManager = gsManager;
    sprite = nullptr;

    dateTimeText = nullptr;

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
    text->setPosition(position.x + 5, position.y + 5);
    text->setOutlineColor(sf::Color::White);
    text->setCharacterSize(16);

    if (saveId > 0) {
        auto *save = gameSaveManager->getSave(saveId);
        exists = true;
        dateTimeText = new sf::Text();
        dateTimeText->setFont(*resourceManager->getFontManager()->getFont("story_font")->getFont());
        dateTimeText->setCharacterSize(12);
        dateTimeText->setString(save->getColumn("formatted_saved_at")->getData()->asString());
        dateTimeText->setPosition(position.x + 72, position.y + 105);

        std::string textString = save->getColumn("text_line")->getData()->asString();

        if (textString.size() > 30) {
            textString = Utils::implodeString({textString.substr(1, 30), "..."});
        }

    } else {
        exists = false;
        setText("Empty");
    }
}

DataMenuGameSave::~DataMenuGameSave() {
    delete (bg);
    delete (text);
}

/**
 * Handles user input amongst other things
 */
bool DataMenuGameSave::update() {

    auto *mouseHandler = inputManager->getMouseHandler();

    return mouseHandler->isButtonClicked(sf::Mouse::Left) &&
           bg->getGlobalBounds().contains(mouseHandler->getMousePosition().x, mouseHandler->getMousePosition().y);

}

void DataMenuGameSave::draw() {
    window->draw(*bg);
    window->draw(*text);

    if (dateTimeText) {
        window->draw(*dateTimeText);
    }

    if (sprite) {
        window->draw(*sprite);
    }
}

void DataMenuGameSave::setText(std::string newText) {
    text->setString(newText);
}

void DataMenuGameSave::setThumbnail(sf::Texture *texture) {

    if (!sprite) {
        sprite = new sf::Sprite();
    }

    sprite->setTexture(*texture);
    sprite->setPosition(position.x+10, position.y+5);
    sprite->setScale(235/(float)texture->getSize().x, 100/(float)texture->getSize().y);
}