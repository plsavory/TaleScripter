//
// Created by Pete on 2019-06-09.
//

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "DatabaseConnection.hpp"
#include "BackgroundImageRenderer.hpp"
#include "MouseHandler.hpp"
#include "ResourceManager.hpp"
#include "UI/Groups/MenuItem.h"

MenuItem::MenuItem(sf::RenderWindow *renderWindow, ResourceManager *rManager, MouseHandler *mHandler, sf::Vector2f itemSize) {
    window = renderWindow;
    resourceManager = rManager;
    type = TYPE_NONE;
    mouseHandler = mHandler;
    size = itemSize;

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

void MenuItem::setAsButton(std::string name, std::string text, sf::Vector2f position) {

    typeSetValidation();

    button = new Button(window, resourceManager, mouseHandler, compact);
    button->setName(name);
    button->setText(text);
    button->setPosition(position);

    if (size != sf::Vector2f(0,0)) {
        button->setSize(size);
    } else {
        button->setSize();
    }

    type = TYPE_BUTTON;
}

void MenuItem::typeSetValidation() {
    if (type != TYPE_NONE) {
        throw MisuseException("Tried to set the type of a menu item which already has a type");
    }
}

bool MenuItem::isClicked() {
    switch (type) {
        case TYPE_BUTTON:
            return button->isClicked();
        default:
            throw MisuseException("Unknown menu item type");
    }
}

std::string MenuItem::getName() {
    switch (type) {
        case TYPE_BUTTON:
            return button->getName();
        default:
            throw MisuseException("Unknown menu item type");
    }
}

sf::FloatRect MenuItem::getSize() {
    // TODO: Handle other types
    if (type != TYPE_BUTTON) {
        throw MisuseException("Unimplemented MenuItem type");
    }
    return button->getSize();
}

void MenuItem::setPosition(sf::Vector2f position) {
    button->setPosition(position);

    if (size != sf::Vector2f(0,0)) {
        button->setSize(size);
    } else {
        button->setSize();
    }
}

sf::Vector2f MenuItem::getPosition() {
    switch (type) {
        case TYPE_BUTTON:
            return button->getPosition();
        default:
            throw MisuseException("Unknown menu item type");
    }
}

sf::FloatRect MenuItem::getGlobalBounds() {

    switch (type) {
        case TYPE_BUTTON:
            return button->getSize();
        default:
            throw MisuseException("Unknown menu item type");
    }
}

bool MenuItem::handleMouseInput() {
    switch (type) {
        case TYPE_BUTTON:
            return button->handleMouseInput();
        default:
            throw MisuseException("Unknown menu item type");
    }
}