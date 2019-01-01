// GameManager.cpp - Manages the current game control flow. Calls the appropriate objects depending on the current state of the game.

#include "SFML/Graphics.hpp"
#include "Database/DatabaseConnection.hpp"
#include "Resource/ResourceManager.hpp"
#include "Base/Renderers.hpp"
#include "Input/InputManager.hpp"
#include "VisualNovelEngine/Classes/Data/Novel.hpp"
#include "Base/GameManager.hpp"

GameManager::GameManager(sf::RenderWindow *window, ResourceManager *rManager, SpriteRenderer *sRenderer, TextRenderer *tRenderer, InputManager *iManager) {
  currentGameState = GameState::Init;
  inputManager = iManager;

  // Create other objects
  novel = new NovelData();
  
  // Create each game GameScreen
  testScreen = new TestScreen(window, rManager, sRenderer, tRenderer);
  novelScreen = new NovelScreen(window, rManager, sRenderer, tRenderer, iManager, novel);

  changeScreen(GameState::Novel);
}

GameManager::~GameManager() {

}

void GameManager::init() {

}

void GameManager::update() {

  switch (currentGameState) {
    case GameState::Novel:
    novelScreen->update();
    break;
    case GameState::Test:
    testScreen->update();
    break;
    default:
    break;
  }

}

void GameManager::draw() {
  switch (currentGameState) {
    case GameState::Novel:
    novelScreen->draw();
    break;
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

void GameManager::changeScreen(GameState newState) {

  switch (newState) {
    case GameState::Novel:
    novelScreen->start();
    break;
    default:
    break;
  }

  currentGameState = newState;

}
