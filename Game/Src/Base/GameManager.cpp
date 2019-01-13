// GameManager.cpp - Manages the current game control flow. Calls the appropriate objects depending on the current state of the game.

#include "SFML/Graphics.hpp"
#include "Database/DatabaseConnection.hpp"
#include "BackgroundRenderer/BackgroundImageRenderer.hpp"
#include "Resource/ResourceManager.hpp"
#include "Base/Renderers.hpp"
#include "Input/InputManager.hpp"
#include "VisualNovelEngine/Classes/Data/Novel.hpp"
#include "Base/GameManager.hpp"
#include <sstream>

GameManager::GameManager(sf::RenderWindow *window, ResourceManager *rManager, SpriteRenderer *sRenderer, TextRenderer *tRenderer, InputManager *iManager, BackgroundImageRenderer *bgRenderer) {
  currentGameState = GameState::Init;
  inputManager = iManager;
  resourceManager = rManager;

  // Create other objects
  novel = new NovelData();

  std::string windowTitle = novel->getProjectInformation()->getGameTitle();

  window->setTitle(windowTitle);

  // Create each game GameScreen
  testScreen = new TestScreen(window, rManager, sRenderer, tRenderer);
  novelScreen = new NovelScreen(window, rManager, sRenderer, tRenderer, iManager, novel, bgRenderer);
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
    case GameState::Test:
    testScreen->update();
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
    case GameState::Test:
    testScreen->draw();
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
