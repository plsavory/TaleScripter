//
// Created by Pete on 2019-06-09.
//

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "DatabaseConnection.hpp"
#include "BackgroundImageRenderer.hpp"
#include "ResourceManager.hpp"
#include "UI/Groups/MenuItem.h"

MenuItem::MenuItem(sf::RenderWindow *renderWindow, ResourceManager *rManager) {
    window = renderWindow;
    resourceManager = rManager;
    type = TYPE_NONE;

    // Set all of the possible UI elements that we can hold to nullptr so that we can detect when there is something stored in there.
    button = nullptr;
}

MenuItem::~MenuItem() {

    if (button) {
        delete(button);
    }

}

void MenuItem::update(sf::Clock *gameTime) {

    if (button) {
        button->update(gameTime);
    }

}

void MenuItem::drawFront() {

    if (button) {
        button->drawText();
    }

}

void MenuItem::drawBack(bool selected) {

    if (button) {
        button->drawBack(selected);
    }

}

void MenuItem::setAsButton(std::string name, std::string text, sf::Vector2i position) {

    typeSetValidation();

    button = new Button(window, resourceManager);
    button->setName(name);
    button->setText(text);
    button->setPosition(position);
    type = TYPE_BUTTON;
}

void MenuItem::typeSetValidation() {
    if (type != TYPE_NONE) {
        throw MisuseException("Tried to set the type of a menu item which already has a type");
    }
}

bool MenuItem::isSelected() {
    return false;
}

std::string MenuItem::getName() {
    switch (type) {
        case TYPE_BUTTON:
            return button->getName();
        default:
            throw MisuseException("Unknown menu item type");
    }
}