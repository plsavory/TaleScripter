/**
 * Game.cpp - Written by Peter L. Savory
 *
 * This class contains initialisation and main loop functions for the game.
 * It is immediately created and called at the start of the program, in EntryPoint.cpp.
 */

#include <iostream>
#include "Database/DatabaseConnection.hpp"
#include "VisualNovelEngine/Classes/Data/Novel.hpp"
#include "Base/Engine.hpp"
#include "Base/GameManager.hpp"
#include <thread>
#include <chrono>
#include "Base/Game.hpp"

/**
 * [Game::Game Class constructor - Initialise variables here]
 */
Game::Game() = default;

/**
 * [Game Destructor - Do anything we need to do before the program ends]
 */
Game::~Game() {
    delete (gameManager);
    delete (engine);
}

/**
 * [Game::run Initialise and run the game]
 */
void Game::run() {
    // TODO: Load from config file

    int frameRateLimit = 60;

    // Initialise SFML
    window = new sf::RenderWindow(sf::VideoMode(1280, 720), gameTitle, sf::Style::Default);
    window->setFramerateLimit(frameRateLimit);

    // Create the main engine object(s)
    engine = new Engine(window);
    std::string errorMessage = engine->getErrorMessage();

    // We throw exceptions when attempting to load resources, so store it so that we can get to the graphical error screen
    try {
        if (errorMessage.empty()) {
            engine->getResourceManager()->loadResourcesFromDatabase();
        }
    } catch (GeneralException &e) {
        errorMessage = e.what();
    }

    // Create ResourceManager to spin up the resource loading thread
    inputManager = engine->getInputManager();
    backgroundOverlay = engine->getBackgroundOverlay();
    backgroundImageRenderer = engine->getBackgroundImageRenderer();
    resourceManager = engine->getResourceManager();
    spriteRenderer = engine->getSpriteRenderer();
    textRenderer = engine->getTextRenderer();
    characterSpriteRenderer = engine->getCharacterSpriteRenderer();
    backgroundTransitionRenderer = engine->getBackgroundTransitionRenderer();

    gameManager = new GameManager(engine, errorMessage);

    sf::Clock updateClock;

#ifdef MULTITHREADED_RENDERING
    // Launch the rendering thread if that compile option is enabled
    window->setActive(false);
    renderingThread = new std::thread(&Game::renderingThreadFunction, this);
    renderingThread->detach(); // Launch the thread
#endif

    // Enter the main loop
    while (window->isOpen()) {

        // Check for window events and handle them
        sf::Event event;

        while (window->pollEvent(event)) {

            switch (event.type) {
                case sf::Event::Closed:
                    window->close();
                    break;
                default:
                    break;
            }

        }

        // Update the game state 60 times a second
        update(updateClock.getElapsedTime().asMilliseconds());

#ifndef MULTITHREADED_RENDERING
        // Draw the game
        draw();
        window->display();
#else
        // Sleep the main thread for the time that one frame should happen
        std::this_thread::sleep_for(std::chrono::milliseconds(1000/frameRateLimit));
#endif
    }

}

/**
 * [Game::update Update loop]
 */
void Game::update(int gameTime) {

    // If the engine didn't start properly, don't try to update anything
    if (!engine->getErrorMessage().empty()) {
        return;
    }

    inputManager->update();
    resourceManager->update();
    gameManager->update();
    backgroundImageRenderer->update();
    spriteRenderer->update();
    textRenderer->update();
    backgroundTransitionRenderer->update();
    characterSpriteRenderer->update();
    backgroundOverlay->update();

    // Don't respond to input when the window isn't in focus
    inputManager->setEnabled(window->hasFocus());
}

/**
 * [Game::draw Draw loop, handle display here]
 */
void Game::draw() {
    // TODO: Handle priorities for different types of rendering
    // Clear the window
    window->clear(*backgroundImageRenderer->getBackgroundColour());

    gameManager->draw();

    // We don't want to update anything beyond here if the engine didn't start properly
    if (!engine->getErrorMessage().empty()) {
        return;
    }

    backgroundImageRenderer->draw();

    if (!backgroundOverlay->isInForeground()) {
        backgroundOverlay->draw();
    }
    characterSpriteRenderer->draw(); // Probably not needed. TODO: Remove this when this is determined.
    spriteRenderer->draw();
    textRenderer->draw();

    if (backgroundOverlay->isInForeground()) {
        backgroundOverlay->draw();
    }
}

#ifdef MULTITHREADED_RENDERING

void Game::renderingThreadFunction() {
  window->setActive(true);
  window->setFramerateLimit(60);

  while (window->isOpen()) {
    draw();
    window->display();
  }
}

#endif
