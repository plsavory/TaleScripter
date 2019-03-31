#include <iostream>
#include <fstream>
#include <regex>
#include <vector>
#include <string>
#include "Misc/Utils.hpp"
#include "Database/DatabaseConnection.hpp"
#include "GameCompiler/ResourceBuilder.hpp"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

/**
 * [ResourceBuilder::ResourceBuilder Store all of the parameters which we need]
 * @param resourceDb       [An instance of DatabaseConnection pointing to a resource database]
 * @param projectDirectory [The working directory of the project]
 */
ResourceBuilder::ResourceBuilder(DatabaseConnection *resourceDb, std::string projectDirectory) {
  resource = resourceDb;

  resourceDirectory = projectDirectory;
  resourceDirectory.append("resource/");

  assetDirectory = projectDirectory;
  assetDirectory.append("assets/");

}

ResourceBuilder::~ResourceBuilder() {

}

/**
 * [ResourceBuilder::process Process each type of resource]
 */
void ResourceBuilder::process() {

  // Process each type of resource
  processBackgroundImages();
  processTextures();
  processSprites();
  processFonts();
  processMusic();
}

void ResourceBuilder::processBackgroundImages() {
  std::cout<<"Processing Background Images..."<<std::endl;
  std::string backgroundImagesDirectory = resourceDirectory;
  backgroundImagesDirectory.append("BackgroundImages/");

  std::string backgroundImagesJsonFileName = backgroundImagesDirectory;
  backgroundImagesJsonFileName.append("BackgroundImages.json");

  if (!Utils::fileExists(backgroundImagesJsonFileName)) {
    std::vector<std::string> errorVector = {"Could not locate background images, unable to find file: ", backgroundImagesJsonFileName};
    throw Utils::implodeString(errorVector, "",0);
  }

  // Start parsing the JSON file
  std::ifstream stream(backgroundImagesJsonFileName);
  json backgroundImageJson = json::parse(stream);

  int numberOfBackgroundImages = 0;

  for (auto& element : backgroundImageJson.items()) {

    json backgroundImage = element.value();

    std::string name;
    std::string fileName;
    std::string enabled;

    // Some error handling...
    if (backgroundImage.find("name") == backgroundImage.end()) {
      throw "Each background image must have a 'name' attribute";
    }

    if (backgroundImage.find("fileName") == backgroundImage.end()) {
      throw "Each background image must have a 'fileName' attribute";
    }

    if (backgroundImage.find("enabled") != backgroundImage.end()) {
      enabled = backgroundImage["enabled"];
    } else {
      enabled = "TRUE"; // If not stated otherwise, assume that it is enabled
    }

    name = backgroundImage["name"];
    fileName = backgroundImage["fileName"];

    std::vector<std::string> columns = {"name", "filename","enabled"};
    std::vector<std::string> values = {name, fileName, enabled};
    std::vector<int> types = {DATA_TYPE_STRING, DATA_TYPE_STRING, DATA_TYPE_BOOLEAN};

    int backgroundImageIdFromInsert = resource->insert("background_images", columns, values, types);

    numberOfBackgroundImages++;

    if (backgroundImage.find("attributes") == backgroundImage.end()) {
      continue;
    }

    // If we're here, we have some attributes that we need to apply to the background image
    {
      json attributeJson = backgroundImage["attributes"];

      std::string backgroundImageId = std::to_string(backgroundImageIdFromInsert);
      std::string enabled = "TRUE";
      std::string maxHeight = "1920"; // TODO: Decide on a default for this
      std::string maxWidth = "1080";
      std::string offsetLeft = "0";
      std::string offsetTop = "0";

      if (attributeJson.find("enabled") != attributeJson.end()) {
        enabled = attributeJson["enabled"];
      }

      if (attributeJson.find("maxHeight") != attributeJson.end()) {
        maxHeight = attributeJson["maxHeight"];
      }

      if (attributeJson.find("maxWidth") != attributeJson.end()) {
        maxWidth = attributeJson["maxWidth"];
      }

      if (attributeJson.find("offsetLeft") != attributeJson.end()) {
        offsetLeft = attributeJson["offsetLeft"];
      }

      if (attributeJson.find("offsetTop") != attributeJson.end()) {
        offsetTop = attributeJson["offsetTop"];
      }

      std::vector<std::string> columns = {"background_image_id", "enabled", "max_height", "max_width", "offset_left", "offset_top"};
      std::vector<std::string> values = {backgroundImageId, enabled, maxHeight, maxWidth, offsetLeft, offsetTop};
      std::vector<int> types = {DATA_TYPE_NUMBER, DATA_TYPE_BOOLEAN, DATA_TYPE_NUMBER, DATA_TYPE_NUMBER, DATA_TYPE_NUMBER, DATA_TYPE_NUMBER};

      resource->insert("background_image_attributes", columns, values, types);
    }
  }

  if (numberOfBackgroundImages == 0) {
    std::cout<<"WARNING: No background images were found in BackgroundImages.json, there may be a problem."<<std::endl;
  }

}

void ResourceBuilder::processTextures() {
  std::cout<<"Processing Textures..."<<std::endl;

  std::string imagesDirectory = resourceDirectory;
  imagesDirectory.append("textures/");

  std::string textureJsonFileName = imagesDirectory;
  textureJsonFileName.append("Textures.json");

  if (!Utils::fileExists(textureJsonFileName)) {
    std::vector<std::string> errorVector = {"Could not locate textures, unable to find file: ", textureJsonFileName};
    throw Utils::implodeString(errorVector, "",0);
  }

  // Start parsing the JSON file
  // TODO: Make a function for handling this for all resource types rather than duplicating the code.
  std::ifstream stream(textureJsonFileName);
  json textureJson = json::parse(stream);

  int numberOfTextureEntries = 0;

  for (auto& element : textureJson.items()) {

    json texture = element.value();

    std::string name;
    std::string fileName;
    std::string enabled;

    // Some error handling...
    if (texture.find("name") == texture.end()) {
      throw "Each texture must have a 'name' attribute";
    }

    if (texture.find("fileName") == texture.end()) {
      throw "Each texture must have a 'fileName' attribute";
    }

    if (texture.find("enabled") != texture.end()) {
      enabled = texture["enabled"];
    } else {
      enabled = "TRUE"; // If not stated otherwise, assume that it is enabled
    }

    name = texture["name"];
    fileName = texture["fileName"];

    std::vector<std::string> columns = {"name", "filename","enabled"};
    std::vector<std::string> values = {name, fileName, enabled};
    std::vector<int> types = {DATA_TYPE_STRING, DATA_TYPE_STRING, DATA_TYPE_BOOLEAN};

    resource->insert("textures", columns, values, types);

    numberOfTextureEntries++;
  }
}

void ResourceBuilder::processSprites() {

}

void ResourceBuilder::processFonts() {

  std::cout<<"Processing Fonts..."<<std::endl;

  std::string fontDirectory = resourceDirectory;
  fontDirectory.append("Fonts/");

  std::string fontJsonFileName = fontDirectory;
  fontJsonFileName.append("Fonts.json");

  if (!Utils::fileExists(fontJsonFileName)) {
    std::vector<std::string> errorVector = {"Could not locate fonts, unable to find file: ", fontJsonFileName};
    throw Utils::implodeString(errorVector, "",0);
  }

  // Start parsing the JSON file
  std::ifstream stream(fontJsonFileName);
  json fontJson = json::parse(stream);

  int numberOfFontEntries = 0;

  for (auto& element : fontJson.items()) {

    json font = element.value();

    std::string name;
    std::string fileName;
    std::string enabled;

    // Some error handling...
    if (font.find("name") == font.end()) {
      throw "Each font must have a 'name' attribute";
    }

    if (font.find("fileName") == font.end()) {
      throw "Each font must have a 'fileName' attribute";
    }

    if (font.find("enabled") != font.end()) {
      enabled = font["enabled"];
    } else {
      enabled = "TRUE"; // If not stated otherwise, assume that it is enabled
    }

    name = font["name"];
    fileName = font["fileName"];

    std::vector<std::string> columns = {"name", "filename","enabled"};
    std::vector<std::string> values = {name, fileName, enabled};
    std::vector<int> types = {DATA_TYPE_STRING, DATA_TYPE_STRING, DATA_TYPE_BOOLEAN};

    resource->insert("fonts", columns, values, types);

    numberOfFontEntries++;
  }

  if (numberOfFontEntries == 0) {
    std::cout<<"WARNING: No fonts were found in BackgroundImages.json, there may be a problem."<<std::endl;
  }
}

void ResourceBuilder::processMusic() {

  std::cout<<"Processing Music..."<<std::endl;

  std::string musicDirectory = resourceDirectory;
  musicDirectory.append("Music/");

  std::string musicJsonFileName = musicDirectory;
  musicJsonFileName.append("Music.json");

  if (!Utils::fileExists(musicJsonFileName)) {
    std::vector<std::string> errorVector = {"Could not locate music, unable to find file: ", musicJsonFileName};
    throw Utils::implodeString(errorVector, "",0);
  }

  // Start parsing the JSON file
  std::ifstream stream(musicJsonFileName);
  json musicJson = json::parse(stream);

  int numberOfMusicEntries = 0;

  for (auto& element : musicJson.items()) {

    json music = element.value();

    std::string name;
    std::string fileName;
    std::string enabled;

    // Some error handling...
    if (music.find("name") == music.end()) {
      throw "Each music track must have a 'name' attribute";
    }

    if (music.find("fileName") == music.end()) {
      throw "Each music track must have a 'fileName' attribute";
    }

    if (music.find("enabled") != music.end()) {
      enabled = music["enabled"];
    } else {
      enabled = "TRUE"; // If not stated otherwise, assume that it is enabled
    }

    name = music["name"];
    fileName = music["fileName"];

    std::vector<std::string> columns = {"name", "filename","enabled"};
    std::vector<std::string> values = {name, fileName, enabled};
    std::vector<int> types = {DATA_TYPE_STRING, DATA_TYPE_STRING, DATA_TYPE_BOOLEAN};

    resource->insert("music", columns, values, types);

    numberOfMusicEntries++;

  }

  if (numberOfMusicEntries == 0) {
    std::cout<<"WARNING: No music tracks were found in Music.json, there may be a problem."<<std::endl;
  }

}
