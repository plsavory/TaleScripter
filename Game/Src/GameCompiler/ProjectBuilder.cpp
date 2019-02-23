#include <iostream>
#include "Database/DatabaseConnection.hpp"
#include "GameCompiler/ProjectBuilder.hpp"
#include <fstream>
#include "nlohmann/json.hpp"
#include "Misc/Utils.hpp"

using json = nlohmann::json;

#define VERBOSE_PROJECT_BUILDER_MESSAGES

/**
 * [ProjectBuilder::ProjectBuilder Load the JSON file and report any errors]
 * @param fileName [File name to the project.json file]
 */
ProjectBuilder::ProjectBuilder(std::string fileName, DatabaseConnection *novelDb, DatabaseConnection *resourceDb) {

  // Handled previously, but check again in case this class is ever used anywhere else.
  if (!Utils::fileExists(fileName)) {
    throw "ProjectBuilder: Unable to find project file.";
  }

  projectFileName = fileName;
  novel = novelDb;
  resource = resourceDb;
}

ProjectBuilder::~ProjectBuilder() {

}

bool ProjectBuilder::process() {

  // Open the project.json file and parse it
  #ifdef VERBOSE_PROJECT_BUILDER_MESSAGES
  std::cout<<"Opening project.json..."<<std::endl;
  #endif

  std::ifstream stream(projectFileName);
  json projectJson = json::parse(stream);

  // Find the appropriate elements and store them 0 throwing errors where neccesary
  if (projectJson.find("title") == projectJson.end()) {
    throw "ProjectBuilder: No 'title' element found in project.json";
  }

  std::string projectTitle = projectJson["title"];

  bool storeCredits;

  if (projectJson.find("credits") == projectJson.end()) {
    storeCredits = false;
  } else {

    json credits = projectJson["credits"];

    for (auto& element : credits.items()) {
      std::cout<<element<<std::endl;
    }
  }

  #ifdef VERBOSE_PROJECT_BUILDER_MESSAGES
  std::cout<<"Project Title: "<<projectTitle<<std::endl;
  #endif

}
