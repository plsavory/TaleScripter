#include "SFML/Graphics.hpp"
#include "Resource/ResourceManager.hpp"
#include "GameScreens/TestScreen.hpp"
#include <iostream>

TestScreen::TestScreen(sf::RenderWindow *windowPointer, ResourceManager *rManager) {
  window = windowPointer;

  spriteRenderer = new SpriteRenderer(window);
  sprite = new Sprite(window);

  // Load an image as a test
  rManager->loadTexture("testassets\\uzume.jpg", "uzume");

  /*
    Without this call to rManager-Update() being here, the image will not display.
    This is because we are setting an image on something which hasn't loaded yet.
    TODO: Load a texture into a sprite using the SpriteHandler class only once it has loaded
   */
  rManager->update();

  sprite->setImage(rManager->getTexture("uzume")->texture);
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
