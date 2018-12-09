#include "SFML/Graphics.hpp"
#include "GameScreens/TestScreen.hpp"
#include <iostream>

TestScreen::TestScreen(sf::RenderWindow *windowPointer) {
  window = windowPointer;

  spriteRenderer = new SpriteRenderer(window);
  sprite = new Sprite(window);

  testImage = new sf::Texture();
  testImage->loadFromFile("testassets\\uzume.jpg");

  sprite->setImage(testImage);
  sprite->setPosition(0,0);
}

TestScreen::~TestScreen() {

}

void TestScreen::init(sf::RenderWindow *windowPointer) {
  window = windowPointer;
}

void TestScreen::update() {

}

void TestScreen::draw() {
  sprite->draw();
  spriteRenderer->draw();
}
