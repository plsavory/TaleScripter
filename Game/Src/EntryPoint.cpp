// EntryPoint.cpp - The entry point for the main TaleScripter-Runner executable
#include <iostream>
#include "Database/DatabaseConnection.hpp"
#include "VisualNovelEngine/Classes/Data/Novel.hpp"
#include "Base/Engine.hpp"
#include "Base/GameManager.hpp"
#include <thread>
#include "Base/Game.hpp"
#include "Misc/ParameterHandler.hpp"

int main(int argc, char* argv[])
{
  // Handle any command line parameters before starting the engine
  ParameterHandler *parameterHandler = new ParameterHandler(argc, argv);

  if (parameterHandler->shouldExit()) {
    return 1;
  }

  // Entry point for the program
  Game *game = new Game();

  game->run();

  return 1;
}
