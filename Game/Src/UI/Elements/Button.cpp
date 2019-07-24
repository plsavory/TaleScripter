//
// Created by Pete on 2019-06-09.
//

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "DatabaseConnection.hpp"
#include "BackgroundImageRenderer.hpp"
#include "ResourceManager.hpp"
#include "MouseHandler.hpp"
#include "UI/Elements/Button.h"

Button::Button(sf::RenderWindow *renderWindow, ResourceManager *rManager, MouseHandler *mHandler) {
    window = renderWindow;
    resourceManager = rManager;
    mouseHandler = mHandler;

    text = new sf::Text;
    text->setFont(*resourceManager->getFontManager()->getFont("story_font")->getFont());
    text->setCharacterSize(18);

    setName("unnamed");
    setText("Unnamed");

    selectedRectangle = nullptr;
}

Button::~Button() {
    delete(text);

    if (selectedRectangle) {
        delete(selectedRectangle);
    }
}

void Button::update(sf::Clock *gameTime) {

}

void Button::drawText() {
    window->draw(*text);
}

void Button::drawBack(bool selected) {

    if (selected) {
        window->draw(*selectedRectangle);
    }

}

void Button::setName(std::string newName) {
    name = newName;
}

std::string Button::getName() {
    return name;
}

void Button::setText(std::string newText) {
    text->setString(newText);
    text->setOrigin(text->getGlobalBounds().width/2, text->getGlobalBounds().height/2);
}

void Button::setPosition(sf::Vector2f newPosition) {

    position = newPosition;

    if (!selectedRectangle) {
        selectedRectangle = new sf::RectangleShape();
    }

    selectedRectangle->setSize(sf::Vector2f(100,50));
    selectedRectangle->setOrigin(selectedRectangle->getGlobalBounds().width/2, selectedRectangle->getGlobalBounds().height/2);
    selectedRectangle->setPosition(position.x, position.y);
    selectedRectangle->setFillColor(sf::Color(200,200,200,200));

    text->setPosition(position.x, position.y);
}

sf::Vector2f Button::getPosition() {
    return position;
}

sf::FloatRect Button::getSize() {
    return selectedRectangle->getGlobalBounds();
}

/**
 * Returns true if the mouse cursor has been clicked within the bounds of the button
 * @return
 */
bool Button::isClicked() {
    return handleMouseInput() && mouseHandler->isButtonClicked(sf::Mouse::Button::Left);
}

/**
 * Returns true if the mouse cursor is within the bounds of the button
 * @return
 */
bool Button::handleMouseInput() {
    sf::Vector2i mousePosition = mouseHandler->getMousePosition();
    return getSize().contains(mousePosition.x, mousePosition.y);
}