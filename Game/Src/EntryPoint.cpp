// EntryPoint.cpp - The entry point for the main TaleScripter-Runner executable
#include <iostream>
#include "Database/DatabaseConnection.hpp"
#include "VisualNovelEngine/Classes/Data/Novel.hpp"
#include "Base/Engine.hpp"
#include "Base/GameManager.hpp"
#include <thread>
#include "Base/Game.hpp"
#include "Misc/ParameterHandler.hpp"
#include "Exceptions/Exceptions.hpp"

int main(int argc, char* argv[])
{
  // Handle any command line parameters before starting the engine
  auto *parameterHandler = new ParameterHandler(argc, argv);

  if (parameterHandler->shouldExit()) {
    return 1;
  }

  // Entry point for the program
  try {
      Game *game = new Game();
      game->run();
  } catch (GeneralException &e) {
      // Exceptions should be caught here as a last resort, as the visual error handler will later be added.
      std::cout<<"Caught an exception: "<<e.what()<<std::endl;
      exit(EXIT_FAILURE);
  }
  catch (std::exception &e) {
      std::cout<<"Caught an exception: "<<e.what()<<" (Not very helpful, these exception types shouldn't be thrown...)"<<std::endl;
      exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}
