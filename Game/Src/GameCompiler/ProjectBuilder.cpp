#include <iostream>
#include "Database/DatabaseConnection.hpp"
#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include "GameCompiler/ProjectBuilder.hpp"
#include "GameCompiler/ResourceBuilder.hpp"
#include "GameCompiler/ChapterBuilder.hpp"
#include <fstream>
#include <regex>
#include "Misc/Utils.hpp"

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

void ProjectBuilder::process() {

  // Open the project.json file and parse it
  #ifdef VERBOSE_PROJECT_BUILDER_MESSAGES
  std::cout<<"Opening project.json..."<<std::endl;
  #endif

  // Get the project directory
  std::regex regex("project\\.json$");
  std::string projectDirectory = std::regex_replace(projectFileName, regex, "");
  projectPath = projectDirectory;


  // Process the novel's resources
  ResourceBuilder *resourceBuilder = new ResourceBuilder(resource, projectDirectory);
  resourceBuilder->process();

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

  // Process each chapter
  if (projectJson.find("chapters") == projectJson.end()) {
    throw "No 'chapters' attribute found in project.json";
  }

  // Process all of the characters
  processCharacters();

  json chapters = projectJson["chapters"];
  int numberOfChapters = 0;

  for (auto& chapter : chapters.items()) {
    numberOfChapters++;

    // Get the path to the chapter file
    std::vector<std::string> explodedFilePath = {
      projectDirectory,
      Utils::removeQuotationsFromString(chapter.value())
    };

    std::string chapterFilePath = Utils::implodeString(explodedFilePath, "", 0);

    ChapterBuilder *chapterBuilder = new ChapterBuilder(chapterFilePath, novel);
    chapterBuilder->process();
    delete(chapterBuilder);
  }

  if (numberOfChapters == 0) {
    throw "No chapters were listed to be processed in the 'chapters' attribute of project.json.";
  }
}

void ProjectBuilder::processCharacters() {
  std::cout<<"Processing characters..."<<std::endl;
  std::string characterJsonFileName = projectPath;
  characterJsonFileName.append("Characters/Characters.json");

  if (!Utils::fileExists(characterJsonFileName)) {
    std::cout<<"WARNING: No Characters.json file found in characters directory, certain features will not work."<<std::endl;
    return;
  }

  int numberOfCharacters = 0;

  std::ifstream stream(characterJsonFileName);
  json charactersJson = json::parse(stream);

  for (auto& datum : charactersJson.items()) {
    json character = datum.value();

    numberOfCharacters++;

    std::string characterId;
    std::string firstName;
    std::string surname = "";
    std::string bio = "";
    std::string age = "0";
    std::string showOnCharacterMenu = "TRUE";

    if (character.find("firstName") == character.end()) {
      throw "A character must have a firstName.";
    }

    if (character.find("characterId") == character.end()) {
      throw "A character must have a characterId (It should be numeric)";
    }

    characterId = character["characterId"];
    firstName = character["firstName"];

    if (character.find("surname") != character.end()) {
      surname = character["surname"];
    }

    if (character.find("bio") != character.end()) {
      bio = character["bio"];
    }

    if (character.find("age") != character.end()) {
      age = character["age"];
    }

    if (character.find("showOnCharacterMenu") != character.end()) {
      showOnCharacterMenu = character["showOnCharacterMenu"];
    }

    // Check if a character with this ID already exists
    std::vector<std::string> query = {
      "SELECT * FROM characters WHERE id = ",
      characterId,
      ";"
    };

    std::string queryString = Utils::implodeString(query, "");

    DataSet *dataSet = new DataSet();

    novel->executeQuery(queryString, dataSet);

    if (dataSet->getRowCount() > 0) {
      std::string existingCharacterName = dataSet->getRow(0)->getColumn("first_name")->getData();
      std::vector<std::string> error = {"Character ID's must be unique, character '", firstName, "' conflicts with character '", existingCharacterName, "'"};
      throw Utils::implodeString(error, "");
    }

    std::vector<std::string> columns = {"id", "first_name", "surname", "bio", "age", "showOnCharacterMenu"};
    std::vector<std::string> values = {characterId, firstName, surname, bio, age, showOnCharacterMenu};
    std::vector<int> types = {DATA_TYPE_NUMBER, DATA_TYPE_STRING, DATA_TYPE_STRING, DATA_TYPE_STRING, DATA_TYPE_NUMBER, DATA_TYPE_BOOLEAN};

    novel->insert("characters", columns, values, types);

    // Process character sprites and do some validation while we're at it
    if (character.find("sprites") != character.end()) {
      json characterSprites = character["sprites"];

      std::string textureId = "";
      std::string name = "";

      for (auto& element : characterSprites.items()) {
        json characterSprite = element.value();

        if (characterSprite.find("textureName") == characterSprite.end()) {
          std::vector<std::string> errorMessage = {
            "Could not process character ",
            firstName,
            ": A character sprite must have a textureName attribute linked to it."
          };

          throw Utils::implodeString(errorMessage, "");
        }

        if (characterSprite.find("name") == characterSprite.end()) {
          std::vector<std::string> errorMessage = {
            "Could not process character ",
            firstName,
            ": A character sprite must have a name attribute linked to it."
          };

          throw Utils::implodeString(errorMessage, "");
        }

        name = characterSprite["name"];

        {
          std::vector<std::string> query = {
            "SELECT id FROM textures WHERE name = '",
            characterSprite["textureName"],
            "';"
          };

          DataSet *dataSet = new DataSet();
          resource->executeQuery(Utils::implodeString(query, ""), dataSet);

          if (dataSet->getRowCount() == 0) {
            std::vector<std::string> errorMessage = {
              "Could not process character ",
              firstName,
              ": No texture named ",
              characterSprite["textureName"],
              "exists."
            };

            throw Utils::implodeString(errorMessage, "");
          }
          
          textureId = dataSet->getRow(0)->getColumn("id")->getData();

          delete(dataSet);
        }

        {
          // Validate that no two sprites linked to the same character have the same name
          std::vector<std::string> query = {
            "SELECT * FROM character_sprites WHERE character_id = ",
            characterId,
            " AND name = '",
            characterSprite["name"],
            "';"
          };

          DataSet *dataSet = new DataSet();
          novel->executeQuery(Utils::implodeString(query, ""), dataSet);

          if (dataSet->getRowCount() > 0) {
            std::vector<std::string> errorMessage = {
              "Could not process character ",
              firstName,
              ": There are multiple sprites with the same name (",
              characterSprite["name"],
              ") linked to this character."
            };

            throw Utils::implodeString(errorMessage, "");
          }
        }

        // If we're here, all validation has passed so write to the database.
        std::vector<std::string> columns = {"character_id", "name", "texture_id"};
        std::vector<std::string> values = {characterId, name, textureId};
        std::vector<int> types = {DATA_TYPE_NUMBER, DATA_TYPE_STRING, DATA_TYPE_NUMBER};

        novel->insert("character_sprites", columns, values, types);
      }
    }
  }

}
