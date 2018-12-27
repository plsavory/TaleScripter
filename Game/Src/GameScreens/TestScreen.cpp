#include "SFML/Graphics.hpp"
#include "Resource/ResourceManager.hpp"
#include "GameScreens/TestScreen.hpp"
#include <iostream>

TestScreen::TestScreen(sf::RenderWindow *windowPointer, ResourceManager *rManager, SpriteRenderer *sRenderer) {
  window = windowPointer;
  spriteRenderer = sRenderer;
  resourceManager = rManager;

  // Load an image as a test
  resourceManager->loadTexture("testassets\\uzume.jpg", "uzume");
  resourceManager->loadTexture("testassets\\uzume2.jpg", "uzume2");
  resourceManager->loadTexture("testassets\\natsuki.jpg", "natsuki");
  resourceManager->loadTexture("testassets\\monika.png", "monika");
  spriteRenderer->addSprite("uzume", "uzume", 1);
  spriteRenderer->addSprite("uzume2", "uzume2", 0);
  spriteRenderer->addSprite("natsuki", "natsuki", 9);
  spriteRenderer->addSprite("monika", "monika", 6);

  // Play some music as a test
  resourceManager->getMusicManager()
    ->addStream("testfile","testassets\\02.ogg");

  resourceManager->getMusicManager()
    ->playAudioStream("testfile");

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
