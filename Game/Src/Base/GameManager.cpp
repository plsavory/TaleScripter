// GameManager.cpp - Manages the current game control flow. Calls the appropriate objects depending on the current state of the game.

#include "Database/DatabaseConnection.hpp"
#include "VisualNovelEngine/Classes/Data/Novel.hpp"
#include "Base/Engine.hpp"
#include "Base/GameManager.hpp"
#include <sstream>

GameManager::GameManager(Engine *enginePointer) {
  currentGameState = GameState::Init;

  engine = enginePointer;
  sf::RenderWindow *window = engine->getWindow();
  inputManager = engine->getInputManager();
  resourceManager = engine->getResourceManager();

  // Create other objects
  novel = new NovelData();
  engine->getCharacterSpriteRenderer()->initData(novel);

  std::string windowTitle = novel->getProjectInformation()->getGameTitle();
  window->setTitle(windowTitle);

  // Create each game GameScreen
  novelScreen = new NovelScreen(engine, novel);
}

GameManager::~GameManager() {

}

void GameManager::init() {

}

void GameManager::update() {

  switch (currentGameState) {
    case GameState::Novel:
    novelScreen->update();
    return;
    default:
    break;
  }

  // Start the game once initial resource loading has completed
  if (currentGameState == GameState::Init && resourceManager->isQueueEmpty()) {
    changeScreen(GameState::Novel);
  }

}

void GameManager::draw() {
  switch (currentGameState) {
    case GameState::Novel:
    novelScreen->draw();
    return;
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
