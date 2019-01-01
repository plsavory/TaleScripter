/**
 * Game.cpp - Written by Peter L. Savory
 *
 * This class contains initialisation and main loop functions for the game.
 * It is immediately created and called at the start of the program, in EntryPoint.cpp.
 */

#include <iostream>
#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Database/DatabaseConnection.hpp"
#include "Resource/ResourceManager.hpp"
#include "SpriteRenderer/SpriteRenderer.hpp"
#include "TextRenderer/TextRenderer.hpp"
#include "SFML/Graphics.hpp"
#include "Input/InputManager.hpp"
#include "VisualNovelEngine/Classes/Data/Novel.hpp"
#include "Base/GameManager.hpp"
#include "Base/Game.hpp"

/**
 * [Game::Game Class constructor - Initialise variables here]
 */
Game::Game() {

}

/**
 * [Game Destructor - Do anything we need to do before the program ends]
 */
Game::~Game() {
  delete(gameManager);
  delete(resourceManager);
  delete(spriteRenderer);
  delete(textRenderer);
  delete(inputManager);
}

/**
 * [Game::run Initialise and run the game]
 */
void Game::run() {
  // TODO: Load config file

  // Initialise SFML
  window = new sf::RenderWindow(sf::VideoMode(1280,720), gameTitle, sf::Style::Close);
  window->setFramerateLimit(60);

  // Create ResourceManager to spin up the resource loading thread
  inputManager = new InputManager();
  resourceManager = new ResourceManager();
  spriteRenderer = new SpriteRenderer(window,resourceManager->getTextureManager());
  textRenderer = new TextRenderer(window, resourceManager->getFontManager());
  gameManager = new GameManager(window, resourceManager, spriteRenderer, textRenderer, inputManager);

  sf::Clock updateClock;

  // Enter the main loop
  while (window->isOpen()) {

    // Check for window events and handle them
    sf::Event event;

    while (window->pollEvent(event)) {

      switch(event.type) {
        case sf::Event::Closed:
        window->close();
        break;
        default:
        break;
      }

    }

    // Update the game state 60 times a second
    update(updateClock.getElapsedTime().asMilliseconds());

    // Clear the window
    window->clear(sf::Color::Black);

    // Draw the game
    draw();

    // Update the Window
    window->display();
  }

}

/**
 * [Game::update Update loop]
 */
void Game::update(int gameTime) {
  inputManager->update();
  resourceManager->update();
  gameManager->update();
  spriteRenderer->update();
  textRenderer->update();
}

/**
 * [Game::draw Draw loop, handle display here]
 */
void Game::draw() {
  // TODO: Handle priorities for different types of rendering
  gameManager->draw();
  spriteRenderer->draw();
  textRenderer->draw();
}
