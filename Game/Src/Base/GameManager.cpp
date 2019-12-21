// GameManager.cpp - Manages the current game control flow. Calls the appropriate objects depending on the current state of the game.

#include "Database/DatabaseConnection.hpp"
#include "VisualNovelEngine/Classes/Data/Novel.hpp"
#include "Base/Engine.hpp"
#include "UI/Themes/UIThemeManager.h"
#include "ScreenState.h"
#include "UI/CommonUI.h"
#include "Base/GameManager.hpp"
#include <sstream>

GameManager::GameManager(Engine *enginePointer, const std::string &initialErrorMessage) {

    screenState = new ScreenState();

    engine = enginePointer;
    sf::RenderWindow *newWindow = engine->getWindow();

    gameTime = new sf::Clock();

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

    // Create other objects
    errorScreen = nullptr;
    titleScreen = nullptr;

    try {
        novel = new NovelData(engine->getGameSaveManager()->getSaveDb());
        engine->getCharacterSpriteRenderer()->initData(novel);

        // Initialise the UI
        uiThemeManager = new UIThemeManager(engine->getWindow(), resourceManager, novel->getNovelDatabase());
        commonUI = new CommonUI(newWindow, resourceManager, inputManager, uiThemeManager, engine->getGameSaveManager(), screenState);


        std::string windowTitle = novel->getProjectInformation()->getGameTitle();
        newWindow->setTitle(windowTitle);

        // Create each game GameScreen
        novelScreen = new NovelScreen(engine, novel, commonUI);
    } catch (GeneralException &e) {
        invokeErrorScreen(e);
    }


}

GameManager::~GameManager() = default;

void GameManager::init() {

}

void GameManager::update() {

    try {

        if (screenState->getCurrentState() != ScreenState::STATE_ERROR && !commonUI->isDoingNothing()) {
            commonUI->update(gameTime);
            gameTime->restart();
            return;
        }

        switch (screenState->getCurrentState()) {
            case ScreenState::STATE_NOVEL:
                novelScreen->update();
                break;
            case ScreenState::STATE_TITLE:
                titleScreen->update(gameTime);
                break;
            case ScreenState::STATE_ERROR:
                gameTime->restart();
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
                break;
            case ScreenState::STATE_TITLE:
                titleScreen->draw();
                break;
            case ScreenState::STATE_ERROR:
                errorScreen->draw();
                return;
            default:
                break;
        }
    } catch (GeneralException &e) {
        invokeErrorScreen(e);
    }
}

/**
 * Draws overlay UI elements (Pop-up menus and load/save game data menus)
 */
void GameManager::drawCommonUI() {

    if (screenState->getCurrentState() == ScreenState::STATE_ERROR) {
        // Don't want to draw these menus over the top of an error message...
        return;
    }

    commonUI->draw();
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
                    titleScreen = new TitleScreen(engine, novel->getNovelDatabase(), resourceManager,
                                                  inputManager, screenState, commonUI);

                    titleScreen->start();
                }
                break;
            case ScreenState::STATE_NOVEL:
                novelScreen->start();
                break;
            case ScreenState::STATE_NOVEL_LOAD:
                novelScreen->start(engine->getGameSaveManager()->getLoadedSave());
                screenState->setState(ScreenState::STATE_NOVEL);
                break;
            default:
                break;
        }

        // Handle any class deletions or resource unloading that we need to do
        // TODO: Will be moved into a separate function to be called when the screen has faded out when we are doing that
        switch (screenState->getPreviousState()) {
            case ScreenState::STATE_TITLE:
                delete (titleScreen);
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
    handleScreenChanges();
    screenState->update();
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
    handleScreenChanges();
    screenState->update();
}