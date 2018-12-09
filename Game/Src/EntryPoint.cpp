// Dev.cpp - A test suite for Tiler during development
#include <iostream>
#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Resource/ResourceManager.hpp"
#include "Base/GameManager.hpp"
#include "Base/Game.hpp"

int main()
{
  // Entry point for the program

  Game *game = new Game();

  game->run();

  return 1;
}
