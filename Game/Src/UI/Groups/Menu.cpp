//
// Created by Pete on 2019-06-09.
//

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "InputManager.hpp"
#include "DatabaseConnection.hpp"
#include "BackgroundImageRenderer.hpp"
#include "ResourceManager.hpp"
#include "UI/Groups/Menu.h"

Menu::Menu(sf::RenderWindow *renderWindow, ResourceManager *rManager, InputManager *iManager) {
    window = renderWindow;
    resourceManager = rManager;
    selectedItem = "";
    orientation = ORIENTATION_HORIZONTAL;
    inputManager = iManager;
    currentSelection = 0;

    decrementEventName = orientation == ORIENTATION_HORIZONTAL ? "menu_left" : "menu_up";
    incrementEventName = orientation == ORIENTATION_HORIZONTAL ? "menu_right" : "menu_down";
}

Menu::~Menu() {

    // Delete all menu items from memory
    for (auto &item : items) {
        delete(item);
    }

}

void Menu::update(sf::Clock *gameTime) {

    if (inputManager->isEventPressed(decrementEventName)) {
        if (currentSelection > 0) {
            --currentSelection;
        } else {
            currentSelection = items.size()-1;
        }
    }

    if (inputManager->isEventPressed(incrementEventName)) {
        if (currentSelection < items.size()-1) {
            ++currentSelection;
        } else {
            currentSelection = 0;
        }
    }

    // TODO Handle mouse input

    for (auto &item : items) {
        if (item->isSelected()) {
            selectedItem = item->getName();
        }
    }

}

void Menu::draw() {

    for (auto &item : items) {
        item->drawFront();
    }

    for (int i = 0; i < items.size(); i++) {
        items[i]->drawBack(currentSelection == i);
    }

}

void Menu::addButton(std::string name, std::string text) {
    // TODO: Add a button with a pre-defined location depending on the menu type
}

void Menu::addButton(const std::string &name, const std::string &text, const sf::Vector2i &position) {
    auto *newItem = new MenuItem(window, resourceManager);
    newItem->setAsButton(name, text, position);
    items.push_back(newItem);
}

MenuItem* Menu::getItem(std::string name) {

    for (auto &item : items) {
        if (item->getName() == name) {
            return item;
        }
    }
    throw GeneralException(Utils::implodeString({"No button with name '", name, "' was found."}));
}