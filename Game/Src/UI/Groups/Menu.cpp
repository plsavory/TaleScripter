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
    setOrientation(ORIENTATION_HORIZONTAL);
    inputManager = iManager;
    currentSelection = 0;


}

Menu::~Menu() {

    // Delete all menu items from memory
    for (auto &item : items) {
        delete (item);
    }

}

void Menu::update(sf::Clock *gameTime) {

    if (!selectedItem.empty()) {
        return;
    }

    if (inputManager->isEventPressed(decrementEventName)) {
        if (currentSelection > 0) {
            --currentSelection;
        } else {
            currentSelection = items.size() - 1;
        }
    }

    if (inputManager->isEventPressed(incrementEventName)) {
        if (currentSelection < items.size() - 1) {
            ++currentSelection;
        } else {
            currentSelection = 0;
        }
    }

    if (inputManager->isEventPressed("menu_next")) {
        selectedItem = items[currentSelection]->getName();
        return;
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

MenuItem *Menu::addButton(const std::string &name, const std::string &text) {
    auto *newItem = new MenuItem(window, resourceManager);
    newItem->setAsButton(name, text, sf::Vector2f(0, 0));
    items.push_back(newItem);
    return newItem;
}

void Menu::addButton(const std::string &name, const std::string &text, const sf::Vector2f &position) {
    auto *newItem = new MenuItem(window, resourceManager);
    newItem->setAsButton(name, text, position);
    items.push_back(newItem);
}

MenuItem *Menu::getItem(std::string name) {

    for (auto &item : items) {
        if (item->getName() == name) {
            return item;
        }
    }
    throw GeneralException(Utils::implodeString({"No button with name '", name, "' was found."}));
}

std::string Menu::getSelectedItemName() {
    return selectedItem;
}

void Menu::clearSelectedItem() {
    selectedItem = "";
}

/**
 * Calculates and returns the size of the menu
 * @return
 */
sf::FloatRect Menu::getGlobalBounds() {

    // We have no size if we have no menu items...
    if (items.empty()) {
        return sf::FloatRect(0, 0, 0, 0);
    }

    // Work out the height and width of the menu
    sf::FloatRect returnData = sf::FloatRect(9999, 9999, 0, 0);

    for (auto &item : items) {
        sf::FloatRect itemBounds = item->getGlobalBounds();

        if (itemBounds.left < returnData.left) {
            returnData.left = itemBounds.left;
        }

        if (itemBounds.top < returnData.top) {
            returnData.top = itemBounds.top;
        }

        if ((itemBounds.left - position.x) + itemBounds.width > returnData.width) {
            returnData.width = (itemBounds.left - position.x) + itemBounds.width;
        }

        if ((itemBounds.top - position.y) + itemBounds.height > returnData.height) {
            returnData.height = (itemBounds.top - position.y) + itemBounds.height;
        }

    }

    return returnData;
}

/**
 * Set the menu items on the menu, clears already-existing items and automatically sets positions
 * @param itemNames
 * @param itemsText
 */
void Menu::setItems(const std::vector<std::string> &itemNames, const std::vector<std::string> &itemsText) {

    // Clear the existing items if there are any
    for (int i = 0; i < items.size(); i++) {
        delete (items[i]);
        items.erase(items.begin() + i);
    }

    if (itemNames.size() != itemsText.size()) {
        throw MisuseException("setItems called with differing number of item names and items text.");
    }

    // Add the new items
    for (int i = 0; i < itemNames.size(); i++) {
        addButton(itemNames[i], itemsText[i]);
    }

    // Find the size of the largest menu item in each dimension
    sf::Vector2f largestSize = sf::Vector2f(0, 0);

    for (auto &currentItem : items) {

        if (currentItem->getSize().width > largestSize.x) {
            largestSize.x = currentItem->getSize().width;
        }

        if (currentItem->getSize().height > largestSize.y) {
            largestSize.y = currentItem->getSize().height;
        }
    }

    // Increase the largest size to place a gap between the objects
    largestSize.x += 32;
    largestSize.y += 32;

    // Position each item as needed
    for (int i = 0; i < items.size(); i++) {

        if (orientation == ORIENTATION_VERTICAL) {
            items[i]->setPosition(sf::Vector2f(0, i * (int) largestSize.y));
        } else {
            items[i]->setPosition(sf::Vector2f(i * (int) largestSize.x, 0));
        }

    }
}

/**
 * Sets the orientation of the menu, changes the keyboard buttons and drawing positions of items as they are added.
 * @param newOrientation
 */
void Menu::setOrientation(int newOrientation) {
    orientation = newOrientation;
    decrementEventName = orientation == ORIENTATION_HORIZONTAL ? "menu_left" : "menu_up";
    incrementEventName = orientation == ORIENTATION_HORIZONTAL ? "menu_right" : "menu_down";
}

/**
 * Set the position of the menu (acts as an offset for all of the objects within the menu)
 * @param newPosition
 */
void Menu::setPosition(sf::Vector2f newPosition) {
    position = newPosition;

    for (auto &item : items) {
        sf::Vector2f itemPosition = item->getPosition();
        item->setPosition(sf::Vector2f(position.x + itemPosition.x, position.y + itemPosition.y));
    }
}

/**
 * Get the current position of the menu
 * @return
 */
sf::Vector2f Menu::getPosition() {
    return position;
}

