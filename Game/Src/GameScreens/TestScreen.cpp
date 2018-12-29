#include "SFML/Graphics.hpp"
#include "Database/DatabaseConnection.hpp"
#include "Resource/ResourceManager.hpp"
#include "GameScreens/TestScreen.hpp"
#include <iostream>

TestScreen::TestScreen(sf::RenderWindow *windowPointer, ResourceManager *rManager, SpriteRenderer *sRenderer) {
  window = windowPointer;
  spriteRenderer = sRenderer;
  resourceManager = rManager;

  // Load an image as a test
  resourceManager->loadTexture("resource\\uzume.jpg", "uzume");
  resourceManager->loadTexture("resource\\uzume2.jpg", "uzume2");
  resourceManager->loadTexture("resource\\natsuki.jpg", "natsuki");
  resourceManager->loadTexture("resource\\monika.png", "monika");
  spriteRenderer->addSprite("uzume", "uzume", 2);
  spriteRenderer->addSprite("uzume2", "uzume2", 1);
  spriteRenderer->addSprite("natsuki", "natsuki", 9);
  spriteRenderer->addSprite("monika", "monika", 0);

  // Play some music as a test
  resourceManager->getMusicManager()
    ->playAudioStream("main_theme");

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
