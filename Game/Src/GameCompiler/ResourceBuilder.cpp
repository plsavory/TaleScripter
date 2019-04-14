#include <iostream>
#include <fstream>
#include <regex>
#include <vector>
#include <string>
#include "Misc/Utils.hpp"
#include "Database/DatabaseConnection.hpp"
#include "Database/TypeCaster.hpp"
#include "GameCompiler/JsonHandler.hpp"
#include "GameCompiler/ResourceBuilder.hpp"
#include "Exceptions/ProjectBuilderException.hpp"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * [ResourceBuilder::ResourceBuilder Store all of the parameters which we need]
 * @param resourceDb       [An instance of DatabaseConnection pointing to a resource database]
 * @param projectDirectory [The working directory of the project]
 */
ResourceBuilder::ResourceBuilder(DatabaseConnection *resourceDb, const std::string& projectDirectory,
                                 JsonHandler *fileHandler) {
    resource = resourceDb;

    resourceDirectory = projectDirectory;
    resourceDirectory.append("resource/");

    assetDirectory = projectDirectory;
    assetDirectory.append("assets/");

    fHandler = fileHandler;

}

ResourceBuilder::~ResourceBuilder() = default;

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
    std::cout << "Processing Background Images..." << std::endl;
    std::string backgroundImagesDirectory = resourceDirectory;
    backgroundImagesDirectory.append("BackgroundImages/");

    std::string backgroundImagesJsonFileName = backgroundImagesDirectory;
    backgroundImagesJsonFileName.append("BackgroundImages.json");

    // Start parsing the JSON file
    json backgroundImageJson = fHandler->parseJsonFile(backgroundImagesJsonFileName);

    int numberOfBackgroundImages = 0;

    for (auto &element : backgroundImageJson.items()) {

        json backgroundImage = element.value();

        std::string name;
        std::string fileName;
        std::string enabled;

        // Some error handling...
        if (backgroundImage.find("name") == backgroundImage.end()) {
            throw ProjectBuilderException("Each background image must have a 'name' attribute");
        }

        if (backgroundImage.find("fileName") == backgroundImage.end()) {
            throw ProjectBuilderException("Each background image must have a 'fileName' attribute");
        }

        if (backgroundImage.find("enabled") != backgroundImage.end()) {
            enabled = TypeCaster::cast(JsonHandler::getBoolean(backgroundImage,"enabled"));
        } else {
            enabled = "TRUE"; // If not stated otherwise, assume that it is enabled
        }

        name = JsonHandler::getString(backgroundImage,"name");
        fileName = JsonHandler::getString(backgroundImage,"fileName");

        std::vector<std::string> columns = {"name", "filename", "enabled"};
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
                enabled = TypeCaster::cast(JsonHandler::getBoolean(attributeJson,"enabled"));
            }

            if (attributeJson.find("maxHeight") != attributeJson.end()) {
                maxHeight = TypeCaster::cast(JsonHandler::getInteger(attributeJson,"maxHeight"));
            }

            if (attributeJson.find("maxWidth") != attributeJson.end()) {
                maxWidth = TypeCaster::cast(JsonHandler::getInteger(attributeJson,"maxWidth"));
            }

            if (attributeJson.find("offsetLeft") != attributeJson.end()) {
                offsetLeft = TypeCaster::cast(JsonHandler::getInteger(attributeJson,"offsetLeft"));
            }

            if (attributeJson.find("offsetTop") != attributeJson.end()) {
                offsetTop = TypeCaster::cast(JsonHandler::getInteger(attributeJson,"offsetTop"));
            }

            std::vector<std::string> columns = {"background_image_id", "enabled", "max_height", "max_width",
                                                "offset_left", "offset_top"};
            std::vector<std::string> values = {backgroundImageId, enabled, maxHeight, maxWidth, offsetLeft, offsetTop};
            std::vector<int> types = {DATA_TYPE_NUMBER, DATA_TYPE_BOOLEAN, DATA_TYPE_NUMBER, DATA_TYPE_NUMBER,
                                      DATA_TYPE_NUMBER, DATA_TYPE_NUMBER};

            resource->insert("background_image_attributes", columns, values, types);
        }
    }

    if (numberOfBackgroundImages == 0) {
        std::cout << "WARNING: No background images were found in BackgroundImages.json, there may be a problem."
                  << std::endl;
    }

}

void ResourceBuilder::processTextures() {
    std::cout << "Processing Textures..." << std::endl;

    std::string imagesDirectory = resourceDirectory;
    imagesDirectory.append("textures/");

    std::string textureJsonFileName = imagesDirectory;
    textureJsonFileName.append("Textures.json");

    // Start parsing the JSON file
    // TODO: Make a function for handling this for all resource types rather than duplicating the code.
    json textureJson = fHandler->parseJsonFile(textureJsonFileName);

    int numberOfTextureEntries = 0;

    for (auto &element : textureJson.items()) {

        json texture = element.value();

        std::string name;
        std::string fileName;
        std::string enabled;

        // Some error handling...
        if (texture.find("name") == texture.end()) {
            throw ProjectBuilderException("Each texture must have a 'name' attribute");
        }

        if (texture.find("fileName") == texture.end()) {
            throw ProjectBuilderException("Each texture must have a 'fileName' attribute");
        }

        if (texture.find("enabled") != texture.end()) {
            enabled = TypeCaster::cast(JsonHandler::getBoolean(texture,"enabled"));
        } else {
            enabled = "TRUE"; // If not stated otherwise, assume that it is enabled
        }

        name = JsonHandler::getString(texture,"name");
        fileName = JsonHandler::getString(texture,"fileName");

        std::vector<std::string> columns = {"name", "filename", "enabled"};
        std::vector<std::string> values = {name, fileName, enabled};
        std::vector<int> types = {DATA_TYPE_STRING, DATA_TYPE_STRING, DATA_TYPE_BOOLEAN};

        resource->insert("textures", columns, values, types);

        numberOfTextureEntries++;
    }
}

void ResourceBuilder::processSprites() {

}

void ResourceBuilder::processFonts() {

    std::cout << "Processing Fonts..." << std::endl;

    std::string fontDirectory = resourceDirectory;
    fontDirectory.append("Fonts/");

    std::string fontJsonFileName = fontDirectory;
    fontJsonFileName.append("Fonts.json");

    // Start parsing the JSON file
    json fontJson = fHandler->parseJsonFile(fontJsonFileName);

    int numberOfFontEntries = 0;

    for (auto &element : fontJson.items()) {

        json font = element.value();

        std::string name;
        std::string fileName;
        std::string enabled;

        // Some error handling...
        if (font.find("name") == font.end()) {
            throw ProjectBuilderException("Each font must have a 'name' attribute");
        }

        if (font.find("fileName") == font.end()) {
            throw ProjectBuilderException("Each font must have a 'fileName' attribute");
        }

        if (font.find("enabled") != font.end()) {
            enabled = TypeCaster::cast(JsonHandler::getBoolean(font,"enabled"));
        } else {
            enabled = "TRUE"; // If not stated otherwise, assume that it is enabled
        }

        name = JsonHandler::getString(font,"name");
        fileName = JsonHandler::getString(font,"fileName");

        std::vector<std::string> columns = {"name", "filename", "enabled"};
        std::vector<std::string> values = {name, fileName, enabled};
        std::vector<int> types = {DATA_TYPE_STRING, DATA_TYPE_STRING, DATA_TYPE_BOOLEAN};

        resource->insert("fonts", columns, values, types);

        numberOfFontEntries++;
    }

    if (numberOfFontEntries == 0) {
        std::cout << "WARNING: No fonts were found in BackgroundImages.json, there may be a problem." << std::endl;
    }
}

void ResourceBuilder::processMusic() {

    std::cout << "Processing Music..." << std::endl;

    std::string musicDirectory = resourceDirectory;
    musicDirectory.append("Music/");

    std::string musicJsonFileName = musicDirectory;
    musicJsonFileName.append("Music.json");

    // Start parsing the JSON file
    json musicJson = fHandler->parseJsonFile(musicJsonFileName);

    int numberOfMusicEntries = 0;

    for (auto &element : musicJson.items()) {

        json music = element.value();

        std::string name;
        std::string fileName;
        std::string enabled;

        // Some error handling...
        if (music.find("name") == music.end()) {
            throw ProjectBuilderException("Each music track must have a 'name' attribute");
        }

        if (music.find("fileName") == music.end()) {
            throw ProjectBuilderException("Each music track must have a 'fileName' attribute");
        }

        if (music.find("enabled") != music.end()) {
            enabled = TypeCaster::cast(JsonHandler::getBoolean(music,"enabled"));
        } else {
            enabled = "TRUE"; // If not stated otherwise, assume that it is enabled
        }

        name = JsonHandler::getString(music,"name");
        fileName = JsonHandler::getString(music,"fileName");

        std::vector<std::string> columns = {"name", "filename", "enabled"};
        std::vector<std::string> values = {name, fileName, enabled};
        std::vector<int> types = {DATA_TYPE_STRING, DATA_TYPE_STRING, DATA_TYPE_BOOLEAN};

        resource->insert("music", columns, values, types);

        numberOfMusicEntries++;

    }

    if (numberOfMusicEntries == 0) {
        std::cout << "WARNING: No music tracks were found in Music.json, there may be a problem." << std::endl;
    }

}
