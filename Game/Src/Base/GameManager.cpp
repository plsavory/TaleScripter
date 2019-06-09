// GameManager.cpp - Manages the current game control flow. Calls the appropriate objects depending on the current state of the game.

#include "Database/DatabaseConnection.hpp"
#include "VisualNovelEngine/Classes/Data/Novel.hpp"
#include "Base/Engine.hpp"
#include "UI/CommonUI.h"
#include "Base/GameManager.hpp"
#include <sstream>

GameManager::GameManager(Engine *enginePointer, const std::string &initialErrorMessage) {

    screenState = new ScreenState();

    engine = enginePointer;
    sf::RenderWindow *newWindow = engine->getWindow();

    if (!initialErrorMessage.empty()) {
        invokeErrorScreen(initialErrorMessage);
        return;
    }

    inputManager = engine->getInputManager();
    resourceManager = engine->getResourceManager();

    // Register any global input events that we need
    inputManager->bindKeyboardEvent("menu_left", "left", true);
    inputManager->bindKeyboardEvent("menu_right", "right", true);
    inputManager->bindKeyboardEvent("menu_up", "up", true);
    inputManager->bindKeyboardEvent("menu_down", "down", true);
    inputManager->bindKeyboardEvent("menu_next", "return", true);
    inputManager->bindKeyboardEvent("menu_back", "escape", true);

    commonUI = new CommonUI(newWindow, resourceManager);

    // Create other objects
    errorScreen = nullptr;
    titleScreen = nullptr;

    try {
        novel = new NovelData();
        engine->getCharacterSpriteRenderer()->initData(novel);

        std::string windowTitle = novel->getProjectInformation()->getGameTitle();
        newWindow->setTitle(windowTitle);

        // Create each game GameScreen
        novelScreen = new NovelScreen(engine, novel);
    } catch (GeneralException &e) {
        invokeErrorScreen(e);
    }

    gameTime = new sf::Clock();

}

GameManager::~GameManager() = default;

void GameManager::init() {

}

void GameManager::update() {

    if (!commonUI->isDoingNothing()) {
        commonUI->update(gameTime);
        gameTime->restart();
        return;
    }

    try {

        switch (screenState->getCurrentState()) {
            case ScreenState::STATE_NOVEL:
                novelScreen->update();
                return;
            case ScreenState::STATE_TITLE:
                titleScreen->update(gameTime);
            case ScreenState::STATE_ERROR:
                return;
            default:
                break;
        }

        // Start the game once initial resource loading has completed
        if (screenState->getCurrentState() == ScreenState::STATE_INIT && resourceManager->isQueueEmpty()) {
            screenState->changeState(ScreenState::STATE_TITLE);
        }

        handleScreenChanges();
        screenState->update();
        gameTime->restart();
    } catch (GeneralException &e) {
        invokeErrorScreen(e);
    }

}

void GameManager::draw() {

    try {
        switch (screenState->getCurrentState()) {
            case ScreenState::STATE_NOVEL:
                novelScreen->draw();
                return;
            case ScreenState::STATE_TITLE:
                titleScreen->draw();
                return;
            case ScreenState::STATE_ERROR:
                errorScreen->draw();
                return;
            default:
                break;
        }

        commonUI->draw(); // Draw common UI elements over the top of everything else
    } catch (GeneralException &e) {
        invokeErrorScreen(e);
    }
}

void GameManager::updateWindowPointers(sf::RenderWindow *windowPointer) {
    // TODO: Update the window pointer on every class which needs it.
}

void GameManager::handleScreenChanges() {

    // Don't do anything if the state hasn't changed.
    if (!screenState->hasStateChanged()) {
        return;
    }

    try {
        switch (screenState->getUpcomingState()) {
            case ScreenState::STATE_TITLE:
                if (!titleScreen) {
                    titleScreen = new TitleScreen(engine->getWindow(), novel->getNovelDatabase(), resourceManager, inputManager);
                }
                break;
            case ScreenState::STATE_NOVEL:
                novelScreen->start();
                break;
            default:
                break;
        }

        // Handle any class deletions or resource unloading that we need to do
        // TODO: Will be moved into a separate function to be called when the screen has faded out when we are doing that
        switch (screenState->getPreviousState()) {
            case ScreenState::STATE_TITLE:
                delete(titleScreen);
                titleScreen = nullptr;
                break;
            default:
                break;
        }
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
    screenState->changeState(ScreenState::STATE_ERROR);
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
    screenState->changeState(ScreenState::STATE_ERROR);
}