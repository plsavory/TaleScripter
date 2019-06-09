//
// Created by Pete on 2019-06-09.
//

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "DatabaseConnection.hpp"
#include "BackgroundImageRenderer.hpp"
#include "ResourceManager.hpp"
#include "UI/Elements/Button.h"

Button::Button(sf::RenderWindow *renderWindow, ResourceManager *rManager) {
    window = renderWindow;
    resourceManager = rManager;

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

void Button::setPosition(sf::Vector2i newPosition) {

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