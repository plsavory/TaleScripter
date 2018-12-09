#include <iostream>
#include "SFML/Graphics.hpp"
#include "SpriteRenderer/Sprite.hpp"

Sprite::Sprite(sf::RenderWindow *window) {
  mySprite = new sf::Sprite();
  displayWindow = window;
}

Sprite::~Sprite() {
  delete mySprite;
}

bool Sprite::setImage(sf::Texture *image) {
  myImage = image;
  mySprite->setTexture(*myImage);
  return true;
}

void Sprite::setPosition(int x, int y) {
  // sf::Vector2f newPosition = new sf::Vector2f((float)x,(float)y);
  mySprite->setPosition(sf::Vector2f((float)x,(float)y));
}

void Sprite::update() {

}

void Sprite::draw() {
  displayWindow->draw(*mySprite);
}
