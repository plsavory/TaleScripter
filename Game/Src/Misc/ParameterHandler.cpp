#include <iostream>
#include "Misc/ParameterHandler.hpp"
#include "Misc/ProjectInfo.hpp"
#include "Misc/Utils.hpp"

ParameterHandler::ParameterHandler(int argc, char* argv[]) {

  shouldExitProgram = false;

  for (int i = 0; i < argc; i++) {

    // Do no more processing if we should quit
    if (shouldExitProgram) {
      return;
    }

    std::string parameter(argv[i]);

    if (parameter == "-v") {
      printVersionInformation();
    }

  }

}

ParameterHandler::~ParameterHandler() {

}

bool ParameterHandler::shouldExit() {
  return shouldExitProgram;
}

void ParameterHandler::printVersionInformation() {
  // TODO: Allow the ability to compile with a mode to hide the extra version information
  std::cout<<Utils::getVersionString(true)<<std::endl;

  #ifdef DISPLAY_LICENCE
  std::cout<<std::endl;
  printLicenceInformation();
  #endif

  shouldExitProgram = true;
}

void ParameterHandler::printLicenceInformation() {
  std::string licenceInformation("\n\n");

  std::cout<<"TaleScripter version "<<PROJECT_VERSION<<", Copyright (C) 2019 Peter L. Savory"<<
  std::endl<<"TaleScripter comes with ABSOLUTELY NO WARRANTY;"<<
  std::endl<<"This is free software, and you are welcome to redistribute it"<<
  std::endl<<"under certain conditions;"<<std::endl<<std::endl<<
  "The TaleScripter licence applies only to this executable file and its source code;"<<std::endl<<
  "Game data, scripts and assets are property of the copyright holder as named in their own licence."<<std::endl;
}
