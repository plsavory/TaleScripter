/*
  TestScreen.cpp - Test any random things here. Keyboard shortcut should be enabled to jump to the test screen
 */


#include "Database/DatabaseConnection.hpp"
#include "Base/Engine.hpp"
#include "GameScreens/TestScreen.hpp"
#include <iostream>


TestScreen::TestScreen(Engine *enginePointer) {
  engine = enginePointer;
  window = engine->getWindow();
  spriteRenderer = engine->getSpriteRenderer();
  resourceManager = engine->getResourceManager();
  textRenderer = engine->getTextRenderer();
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
