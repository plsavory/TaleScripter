/**
 * Game.cpp - Written by Peter L. Savory
 *
 * This class contains initialisation and main loop functions for the game.
 * It is immediately created and called at the start of the program, in EntryPoint.cpp.
 */

#include <iostream>
#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "Resource/ResourceManager.hpp"
#include "SpriteRenderer/SpriteRenderer.hpp"
#include "SFML/Graphics.hpp"
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
  resourceManager = new ResourceManager();
  spriteRenderer = new SpriteRenderer(window,resourceManager->getTextureManager());
  gameManager = new GameManager(window, resourceManager, spriteRenderer);

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
 * [Game::update Update loop, should be called once per frame]
 */
void Game::update(int gameTime) {
  resourceManager->update();
  gameManager->update();
  spriteRenderer->update();
}

/**
 * [Game::draw Draw loop, handle display here]
 */
void Game::draw() {
  gameManager->draw();
  spriteRenderer->draw();
}
