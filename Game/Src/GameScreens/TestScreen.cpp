#include "SFML/Graphics.hpp"
#include "Resource/ResourceManager.hpp"
#include "GameScreens/TestScreen.hpp"
#include <iostream>

TestScreen::TestScreen(sf::RenderWindow *windowPointer, ResourceManager *rManager, SpriteRenderer *sRenderer) {
  window = windowPointer;
  spriteRenderer = sRenderer;

  // Load an image as a test
  rManager->loadTexture("testassets\\uzume.jpg", "uzume");
  spriteRenderer->addSprite("uzume", "uzume", 1);

}

TestScreen::~TestScreen() {

}

void TestScreen::init(sf::RenderWindow *windowPointer) {
  window = windowPointer;
}

void TestScreen::update() {

}

void TestScreen::draw() {
  
}
