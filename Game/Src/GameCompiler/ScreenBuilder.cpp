//
// Created by Pete on 2019-07-21.
//

#include "DatabaseConnection.hpp"
#include <nlohmann/json.hpp>
#include "JsonHandler.hpp"
#include "ProjectBuilderHelper.h"
#include "ScreenBuilder.hpp"
#include "ProjectBuilderException.hpp"
#include "UI/UIConstants.h"
#include <iostream>

ScreenBuilder::ScreenBuilder(DatabaseConnection *novelDb, DatabaseConnection *resourceDb,
                             JsonHandler *fHandler, ProjectBuilderHelper *pbHelper) {
    novel = novelDb;
    resource = resourceDb;
    fileHandler = fHandler;
    projectBuilderHelper = pbHelper;
}

ScreenBuilder::~ScreenBuilder() {

}

void ScreenBuilder::processTitleScreens(const std::string &fileName) {

    std::cout<<"Processing title screens..."<<std::endl;

    // Validate that the given file exists
    if (!Utils::fileExists(fileName)) {
        throw ProjectBuilderException(Utils::implodeString({"Unable to locate required file: ", fileName}));
    }

    // Parse the json file
    json titleScreens = fileHandler->parseJsonFile(fileName);

    if (!titleScreens.is_array()) {
        throw ProjectBuilderException(Utils::implodeString({fileName, " must contain an array of title screen json objects"}));
    }

    // Process every possible title screen
    for (auto &titleScreenJson : titleScreens) {
        processTitleScreen(titleScreenJson);
    }
}

/**
 * Reads the json data for a single title screen and stores it in the database
 * @param titleScreenJson
 */
void ScreenBuilder::processTitleScreen(json titleScreenJson) {

    // Validate that we actually have a json object.
    if (!titleScreenJson.is_object()) {

        if (titleScreenJson.is_array()) {
            throw ProjectBuilderException("Title screen data must be a json object, instead it is an array.");
        }

        throw ProjectBuilderException("Title screen data must be a json object, instead it is a primitive type.");
    }

    int backgroundId;
    int backgroundMusicId;

    // Validate that we have the required fields with the correct types, and store them.
    if (titleScreenJson.find("backgroundImageName") == titleScreenJson.end()) {
        throw ProjectBuilderException("Required value 'backgroundImageName' is missing");
    }

    if (!titleScreenJson["backgroundImageName"].is_string()) {
        throw ProjectBuilderException("Required value 'backgroundImageName' must be a string.");
    }

    if (titleScreenJson.find("backgroundMusicName") == titleScreenJson.end()) {
        throw ProjectBuilderException("Required value 'backgroundImageName' is missing");
    }

    if (!titleScreenJson["backgroundMusicName"].is_string()) {
        throw ProjectBuilderException("Required value 'backgroundMusicName' must be a string.");
    }

    if (titleScreenJson.find("layout") == titleScreenJson.end()) {
        throw ProjectBuilderException("Required value 'layout' is missing");
    }

    if (!titleScreenJson["layout"].is_object()) {
        throw ProjectBuilderException("Required value 'layout' must be a json object.");
    }

    // TODO Store the menu and options inside the database
    int menuId = projectBuilderHelper->processMenu(titleScreenJson["layout"]["menu"]);

    // Validate that the background image and music exist and get their id.
    auto *imageDataSet = new DataSet(); auto *musicDataSet = new DataSet();

    resource->executeQuery(Utils::implodeString({"SELECT * FROM backgrounds WHERE name = '", JsonHandler::getString(titleScreenJson,"backgroundImageName"), "'"}), imageDataSet);

    if (imageDataSet->getRowCount() == 0) {
        throw ProjectBuilderException(Utils::implodeString({"No background image named '", JsonHandler::getString(titleScreenJson, "backgroundImageName"), "' exists."}));
    }

    backgroundId = imageDataSet->getRow(0)->getColumn("id")->getData()->asInteger();
    delete(imageDataSet);

    resource->executeQuery(Utils::implodeString({"SELECT * FROM music WHERE name = '", JsonHandler::getString(titleScreenJson,"backgroundMusicName"), "'"}), musicDataSet);

    if (musicDataSet->getRowCount() == 0) {
        throw ProjectBuilderException(Utils::implodeString({"No music track named '", JsonHandler::getString(titleScreenJson, "backgroundMusicName"), "' exists."}));
    }

    backgroundMusicId = musicDataSet->getRow(0)->getColumn("id")->getData()->asInteger();
    delete(musicDataSet);

    // Store the title screen in the database
    std::vector<std::string> columns = {"background_id", "background_music_id", "menu_id"};
    std::vector<std::string> values = {std::to_string(backgroundId), std::to_string(backgroundMusicId), std::to_string(menuId)};
    std::vector<int> types = {DATA_TYPE_NUMBER, DATA_TYPE_NUMBER, DATA_TYPE_NUMBER};
    novel->insert("title_screens", columns, values, types);
}