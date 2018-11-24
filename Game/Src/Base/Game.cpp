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
#include "Base/GameManager.hpp"
#include "Base/Game.hpp"

/**
 * [Game::Game Class constructor - Initialise variables here]
 */
Game::Game() {
  gameManager = new GameManager(window);
}

/**
 * [Game Destructor - Do anything we need to do before the program ends]
 */
Game::~Game() {
  delete(gameManager);
}

/**
 * [Game::run Initialise and run the game]
 */
void Game::run() {
  // TODO: Load config file

  // Initialise SFML
  window = new sf::RenderWindow(sf::VideoMode(1280,720), gameTitle, sf::Style::Close);
  window->setFramerateLimit(60);

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
    if (updateClock.getElapsedTime().asMilliseconds() >= 16) {
      update();
      updateClock.restart();
    }

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
void Game::update() {
  gameManager->update();
}

/**
 * [Game::draw Draw loop, handle display here]
 */
void Game::draw() {
  gameManager->draw();
}
