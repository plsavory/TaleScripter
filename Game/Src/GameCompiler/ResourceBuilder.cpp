#include <iostream>
#include <fstream>
#include <regex>
#include <vector>
#include <string>
#include "Misc/Utils.hpp"
#include "Database/DatabaseConnection.hpp"
#include "Database/TypeCaster.hpp"
#include "Misc/JsonHandler.hpp"
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
    processBackgroundTextures();
    processBackgrounds();
    processTextures();
    processSprites();
    processFonts();
    processMusic();
}

void ResourceBuilder::processBackgroundTextures() {
    std::cout << "Processing Background Textures..." << std::endl;
    std::string backgroundImagesDirectory = resourceDirectory;
    backgroundImagesDirectory.append("Backgrounds/");

    std::string backgroundImagesJsonFileName = backgroundImagesDirectory;
    backgroundImagesJsonFileName.append("BackgroundTextures.json");

    // Start parsing the JSON file
    json backgroundTextureJson = fHandler->parseJsonFile(backgroundImagesJsonFileName);

    int numberOfBackgroundTextures = 0;

    for (auto &element : backgroundTextureJson.items()) {

        json backgroundImage = element.value();

        std::string name;
        std::string fileName;

        // Some error handling...
        if (backgroundImage.find("name") == backgroundImage.end()) {
            throw ProjectBuilderException("Each background image must have a 'name' attribute");
        }

        if (backgroundImage.find("fileName") == backgroundImage.end()) {
            throw ProjectBuilderException("Each background image must have a 'fileName' attribute");
        }

        name = JsonHandler::getString(backgroundImage,"name");
        fileName = JsonHandler::getString(backgroundImage,"fileName");

        std::vector<std::string> columns = {"name", "filename"};
        std::vector<std::string> values = {name, fileName};
        std::vector<int> types = {DATA_TYPE_STRING, DATA_TYPE_STRING};

        int backgroundTextureIdFromInsert = resource->insert("background_textures", columns, values, types);

        numberOfBackgroundTextures++;

        if (backgroundImage.find("attributes") == backgroundImage.end()) {
            continue;
        }
    }
}

void ResourceBuilder::processBackgrounds() {

    std::cout << "Processing Backgrounds..." << std::endl;
    std::string backgroundImagesDirectory = resourceDirectory;
    backgroundImagesDirectory.append("Backgrounds/");

    std::string backgroundImagesJsonFileName = backgroundImagesDirectory;
    backgroundImagesJsonFileName.append("Backgrounds.json");

    // Start parsing the JSON file
    json backgroundJson = fHandler->parseJsonFile(backgroundImagesJsonFileName);

    int numberOfBackgrounds = 0;

    for (auto &element : backgroundJson.items()) {

        json background = element.value();

        std::string name;
        std::string enabled;

        // Some error handling...
        if (background.find("name") == background.end()) {
            throw ProjectBuilderException("Each background must have a 'name' attribute");
        }

        if (background.find("enabled") != background.end()) {
            enabled = TypeCaster::cast(JsonHandler::getBoolean(background, "enabled"));
        } else {
            enabled = "TRUE"; // If not stated otherwise, assume that it is enabled
        }

        name = JsonHandler::getString(background, "name");

        std::vector<std::string> columns = {"name", "enabled"};
        std::vector<std::string> values = {name, enabled};
        std::vector<int> types = {DATA_TYPE_STRING, DATA_TYPE_BOOLEAN};

        int backgroundImageIdFromInsert = resource->insert("backgrounds", columns, values, types);

        numberOfBackgrounds++;

        if (background.find("layers") == background.end()) {
            throw ProjectBuilderException(Utils::implodeString({"Background with name '", name, "' is missing a 'layers' attribute."}));
        }

        json layersJson = background["layers"];

        for (auto &layer : layersJson.items()) {
            processBackgroundLayer(backgroundImageIdFromInsert, layer.value());
        }

        continue;

        if (background.find("attributes") == background.end()) {
            continue;
        }

        // If we're here, we have some attributes that we need to apply to the background image
        {
            json attributeJson = background["attributes"];

            std::string backgroundImageId = std::to_string(backgroundImageIdFromInsert);


            if (attributeJson.find("enabled") != attributeJson.end()) {
                enabled = TypeCaster::cast(JsonHandler::getBoolean(attributeJson,"enabled"));
            }


        }
    }

    if (numberOfBackgrounds == 0) {
        std::cout << "WARNING: No background images were found in BackgroundImages.json, there may be a problem."
                  << std::endl;
    }

}

void ResourceBuilder::processBackgroundLayer(int backgroundId, json layerJson) {

    std::string maxHeight = "1280"; // TODO: Decide on a default for this
    std::string maxWidth = "720";
    std::string offsetLeft = "0";
    std::string offsetTop = "0";
    std::string horizontalScrollSpeed = "0";
    std::string verticalScrollSpeed = "0";
    std::string depth = "0";
    std::string isTiled = "FALSE";

    if (layerJson.find("maxHeight") != layerJson.end()) {
        maxHeight = TypeCaster::cast(JsonHandler::getInteger(layerJson, "maxHeight"));
    }

    if (layerJson.find("maxWidth") != layerJson.end()) {
        maxWidth = TypeCaster::cast(JsonHandler::getInteger(layerJson, "maxWidth"));
    }

    if (layerJson.find("offsetLeft") != layerJson.end()) {
        offsetLeft = TypeCaster::cast(JsonHandler::getInteger(layerJson, "offsetLeft"));
    }

    if (layerJson.find("offsetTop") != layerJson.end()) {
        offsetTop = TypeCaster::cast(JsonHandler::getInteger(layerJson, "offsetTop"));
    }

    if (layerJson.find("horizontalScrollSpeed") != layerJson.end()) {
        horizontalScrollSpeed = TypeCaster::cast(JsonHandler::getInteger(layerJson, "horizontalScrollSpeed"));
    }

    if (layerJson.find("verticalScrollSpeed") != layerJson.end()) {
        verticalScrollSpeed = TypeCaster::cast(JsonHandler::getInteger(layerJson, "verticalScrollSpeed"));
    }

    if (layerJson.find("depth") != layerJson.end()) {
        depth = TypeCaster::cast(JsonHandler::getInteger(layerJson, "depth"));
    }

    if (layerJson.find("backgroundTextureName") == layerJson.end()) {
        throw ProjectBuilderException("Background layer is missing a 'backgroundTextureName' attribute.");
    }

    if (layerJson.find("isTiled") != layerJson.end()) {
        isTiled = TypeCaster::cast(JsonHandler::getBoolean(layerJson, "isTiled"));
    }

    std::string backgroundTextureName = JsonHandler::getString(layerJson, "backgroundTextureName");

    // Ensure that the background texture exists before trying to use it (The fact that we use ID's enforces this anyway...).
    auto *backgroundTextureDataSet = new DataSet();

    resource->execute("SELECT * FROM background_textures WHERE name = ?", backgroundTextureDataSet, {backgroundTextureName}, {DatabaseConnection::TYPE_TEXT});

    if (!backgroundTextureDataSet->getRowCount()) {
        throw ProjectBuilderException(Utils::implodeString({"No background texture with name '", backgroundTextureName, "' could be found"}));
    }

    int backgroundTextureId = backgroundTextureDataSet->getRow(0)->getColumn("id")->getData()->asInteger();

    delete(backgroundTextureDataSet);

    std::vector<std::string> columns = {"background_id" ,"background_texture_id", "max_height", "max_width",
                                        "offset_left", "offset_top", "horizontal_scroll_speed", "vertical_scroll_speed", "depth", "is_tiled"};

    std::vector<std::string> values = {std::to_string(backgroundId), std::to_string(backgroundTextureId), maxHeight, maxWidth, offsetLeft, offsetTop, horizontalScrollSpeed, verticalScrollSpeed, depth, isTiled};

    std::vector<int> types = {DATA_TYPE_NUMBER, DATA_TYPE_NUMBER, DATA_TYPE_NUMBER, DATA_TYPE_NUMBER,
                              DATA_TYPE_NUMBER, DATA_TYPE_NUMBER, DATA_TYPE_NUMBER, DATA_TYPE_NUMBER, DATA_TYPE_NUMBER, DATA_TYPE_BOOLEAN};

    resource->insert("background_layers", columns, values, types);
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
