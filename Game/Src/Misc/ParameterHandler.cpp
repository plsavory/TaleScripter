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
  std::string licenceInformation("TaleScripter version 0.0.1, Copyright (C) 2019 Peter L. Savory\nTaleScripter comes with ABSOLUTELY NO WARRANTY;\nThis is free software, and you are welcome to redistribute it\nunder certain conditions;");

  std::cout<<licenceInformation<<std::endl;
}
