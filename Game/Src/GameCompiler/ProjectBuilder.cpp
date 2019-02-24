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

  std::string* contributors[100];

  if (projectJson.find("credits") == projectJson.end()) {
    storeCredits = false;
  } else {

    json credits = projectJson["contributors"];

    // Store all of the names in the Credits array. TODO: Create a more comprehensive solution for game ending cvredits later
    for (int i = 0; i < 100; i++) {
      contributors[i] = NULL;
    }

    int contributorsCount = -1;
    for (auto& element : credits.items()) {
      contributors[++contributorsCount] = new std::string(Utils::removeQuotationsFromString(element.value()));
    }
  }

  std::string versionNumber;
  if (projectJson.find("version_number") == projectJson.end()) {
    versionNumber = "1.0";
  } else {
    versionNumber = projectJson["version_number"];
  }

  // TODO: Store the contributors in the database when we have an appropriate location for it

  std::string buildDate;

  if (projectJson.find("date") == projectJson.end()) {
    buildDate = projectJson["date"];
  } else {
    buildDate = "NOW()";
  }

  // Write the extracted data to the database
  std::vector<std::string> columns = {"game_title", "author_name", "version_number", "text_display_type"};

  std::vector<std::string> values = {
    projectTitle,
    storeCredits ? *contributors[0] : std::string("Unknown"),
    versionNumber,
    std::string("1") // TODO: Implement this
  };

  // This didn't differ as much as I thought it would...
  std::vector<int> types = {
    DATA_TYPE_STRING,
    DATA_TYPE_STRING,
    DATA_TYPE_STRING,
    DATA_TYPE_STRING
  };

  novel->insert("game_information", columns, values, types);

}
