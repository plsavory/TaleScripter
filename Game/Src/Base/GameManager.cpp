// GameManager.cpp - Manages the current game control flow. Calls the appropriate objects depending on the current state of the game.

#include "SFML/Graphics.hpp"
#include "Base/GameManager.hpp"

GameManager::GameManager(sf::RenderWindow *window) {
  currentGameState = GameState::TestHarness;
}

GameManager::~GameManager() {

}

void GameManager::init() {

}

void GameManager::update() {

  switch (currentGameState) {
    default:
    break;
  }

}

void GameManager::draw() {
  switch (currentGameState) {
    default:
    break;
  }
}

void GameManager::updateWindowPointers(sf::RenderWindow *windowPointer) {
  // TODO: Update the window pointer on every class which needs it.
}
