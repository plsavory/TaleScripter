// GameScreen.cpp - Contains function definitions which will be common on each Game State/screen

#include "SFML/Graphics.hpp"
#include "Base/GameScreen.hpp"

/**
 * [GameScreen::updateWindowPointer if the window is destroyed and re-created, our pointer might need updating.]
 * @param windowPointer [A pointer to a RenderWindow]
 */
void GameScreen::updateWindowPointer(sf::RenderWindow *windowPointer) {
  window = windowPointer;
}
