// GameManager.cpp - Manages the current game control flow. Calls the appropriate objects depending on the current state of the game.

#include "SFML/Graphics.hpp"
#include "Base/GameManager.hpp"

GameManager::GameManager(sf::RenderWindow *window) {
  currentGameState = GameState::Test;

  // Create each game GameScreen
  testScreen = new TestScreen(window);
}

GameManager::~GameManager() {

}

void GameManager::init() {

}

void GameManager::update() {

  switch (currentGameState) {
    case GameState::Test:
    testScreen->update();
    break;
    default:
    break;
  }

}

void GameManager::draw() {
  switch (currentGameState) {
    case GameState::Test:
    testScreen->draw();
    break;
    default:
    break;
  }
}

void GameManager::updateWindowPointers(sf::RenderWindow *windowPointer) {
  // TODO: Update the window pointer on every class which needs it.
}
