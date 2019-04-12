//
// Created by Pete on 2019-04-10.
//

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "BackgroundRenderer/BackgroundOverlay.hpp"

BackgroundOverlay::BackgroundOverlay(sf::RenderWindow *targetWindow) {
    window = targetWindow;
    overlay = new sf::RectangleShape();
    colour = new sf::Color();

    // Colour default values
    colour->a = 0;
    colour->r = 0;
    colour->g = 0;
    colour->b = 0;

    overlay->setPosition(0,0);
    overlay->setSize(sf::Vector2f(1280,720));

    foreground = true;

}

BackgroundOverlay::~BackgroundOverlay() {
    delete(overlay);
};

void BackgroundOverlay::update() {

}

void BackgroundOverlay::draw() {
    window->draw(*overlay);
}

void BackgroundOverlay::setAlpha(int alpha) {
    colour->a = alpha;
    overlay->setFillColor(*colour);
}

int BackgroundOverlay::getAlpha() {
    return colour->a;
}

void BackgroundOverlay::setColour(sf::Color newColour) {
    colour->r = newColour.r;
    colour->g = newColour.g;
    colour->b = newColour.b;
    overlay->setFillColor(*colour);
}

sf::Color BackgroundOverlay::getColour() {
    return *colour;
}

bool BackgroundOverlay::isInForeground() {
    return foreground;
}