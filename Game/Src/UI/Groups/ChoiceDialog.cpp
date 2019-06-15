//
// Created by Pete on 2019-06-11.
//

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "DatabaseConnection.hpp"
#include "BackgroundImageRenderer.hpp"
#include "ResourceManager.hpp"
#include "UI/Groups/ChoiceDialog.h"

ChoiceDialog::ChoiceDialog(sf::RenderWindow *renderWindow, ResourceManager *rManager, const std::string &text,
                           const std::vector<std::string> &optionNames, const std::vector<std::string> &optionText, InputManager *inputManager) {

    window = renderWindow;
    resourceManager = rManager;
    state = STATE_ACTIVE;

    // Create the dialog text
    textObject = new sf::Text();
    textObject->setFont(*resourceManager->getFontManager()->getFont("story_font")->getFont());
    textObject->setCharacterSize(18);
    textObject->setPosition(0, 0);
    textObject->setFillColor(sf::Color::White);
    textObject->setString(text);

    bounds.left = ((float)1280/2-(textObject->getGlobalBounds().width/2))-16;

    // Create and populate the dialog menu
    menu = new Menu(renderWindow, resourceManager, inputManager);
    menu->setOrientation(Menu::ORIENTATION_VERTICAL);
    menu->setItems(optionNames, optionText);

    // Calculate our position/size
    sf::FloatRect menuBounds = menu->getGlobalBounds();

    // Position the menu and text
    menu->setPosition(sf::Vector2f(((float)1280/2)-(menuBounds.width/2), ((float)720/2) - (menuBounds.height/2)));
    textObject->setPosition(((float)1280/2)-(textObject->getGlobalBounds().width/2), menu->getPosition().y - 64);
}

ChoiceDialog::~ChoiceDialog() {
    delete(menu);
    delete(textObject);
}

void ChoiceDialog::update(sf::Clock *gameTime) {

    if (state != STATE_ACTIVE) {
        return;
    }

    menu->update(gameTime);

    if (!menu->getSelectedItemName().empty()) {
        state = STATE_INACTIVE;
    }
}

void ChoiceDialog::draw() {
    menu->draw();
    window->draw(*textObject);
}

std::string ChoiceDialog::getSelectedItem() {
    return menu->getSelectedItemName();
}

void ChoiceDialog::markForDeletion() {
    state = STATE_DELETED;
}

bool ChoiceDialog::needsDeleting() {
    return state == STATE_DELETED;
}

int ChoiceDialog::getState() {
    return state;
}