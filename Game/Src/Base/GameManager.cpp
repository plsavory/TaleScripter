// GameManager.cpp - Manages the current game control flow. Calls the appropriate objects depending on the current state of the game.

#include "Database/DatabaseConnection.hpp"
#include "VisualNovelEngine/Classes/Data/Novel.hpp"
#include "Base/Engine.hpp"
#include "Base/GameManager.hpp"
#include <sstream>

GameManager::GameManager(Engine *enginePointer, const std::string &initialErrorMessage) {
    currentGameState = GameState::Init;

    engine = enginePointer;
    sf::RenderWindow *window = engine->getWindow();

    if (!initialErrorMessage.empty()) {
        invokeErrorScreen(initialErrorMessage);
        return;
    }

    inputManager = engine->getInputManager();
    resourceManager = engine->getResourceManager();

    // Create other objects
    errorScreen = nullptr;

    try {
        novel = new NovelData();
        engine->getCharacterSpriteRenderer()->initData(novel);

        std::string windowTitle = novel->getProjectInformation()->getGameTitle();
        window->setTitle(windowTitle);

        // Create each game GameScreen
        novelScreen = new NovelScreen(engine, novel);
    } catch (GeneralException &e) {
        invokeErrorScreen(e);
    }

}

GameManager::~GameManager() = default;

void GameManager::init() {

}

void GameManager::update() {

    try {

        switch (currentGameState) {
            case GameState::Novel:
                novelScreen->update();
                return;
            case GameState::ExceptionCaught:
                return;
            default:
                break;
        }

        // Start the game once initial resource loading has completed
        if (currentGameState == GameState::Init && resourceManager->isQueueEmpty()) {
            changeScreen(GameState::Novel);
        }
    } catch (GeneralException &e) {
        invokeErrorScreen(e);
    }

}

void GameManager::draw() {

    try {
        switch (currentGameState) {
            case GameState::Novel:
                novelScreen->draw();
                return;
            case GameState::ExceptionCaught:
                errorScreen->draw();
                return;
            default:
                break;
        }
    } catch (GeneralException &e) {
        invokeErrorScreen(e);
    }
}

void GameManager::updateWindowPointers(sf::RenderWindow *windowPointer) {
    // TODO: Update the window pointer on every class which needs it.
}

void GameManager::changeScreen(GameState newState) {

    try {
        switch (newState) {
            case GameState::Novel:
                novelScreen->start();
                break;
            default:
                break;
        }

        currentGameState = newState;
    } catch (GeneralException &e) {
        invokeErrorScreen(e);
    }

}

/**
 * Switches to the graphical error screen when an error is passed to it
 *
 * @param e a GeneralException to grab the error information from
 */
void GameManager::invokeErrorScreen(GeneralException &e) {
    // Attempt to display a graphical error screen
    errorScreen = new ErrorScreen(engine->getWindow());
    errorScreen->start(e);
    currentGameState = GameState::ExceptionCaught;
}

/**
 * Switches to the graphical error screen when an error is passed to it
 *
 * @param message an error message string
 */
void GameManager::invokeErrorScreen(const std::string &message) {
    // Attempt to display a graphical error screen
    errorScreen = new ErrorScreen(engine->getWindow());
    errorScreen->start(message);
    currentGameState = GameState::ExceptionCaught;
}