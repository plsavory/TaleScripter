//
// Created by Pete on 2019-06-15.
//

#include "DatabaseConnection.hpp"
#include <nlohmann/json.hpp>
#include "JsonHandler.hpp"
#include "ThemeBuilder.hpp"
#include "ProjectBuilderException.hpp"
#include "UI/UIConstants.h"
#include <iostream>

ThemeBuilder::ThemeBuilder(std::string fileName, DatabaseConnection *novelDb, DatabaseConnection *resourceDb,
                           JsonHandler *fileHandler) {

    // Ensure that the themes file actually exists
    if (!Utils::fileExists(fileName)) {
        throw ProjectBuilderException(Utils::implodeString({"Could not build UI themes, (file ", fileName," does not exist.)"}));
    }

    // Get all possible element types
    allPossibleElementTypes = UIUtils::getAllPossibleElementTypes();

    novel = novelDb;
    resource = resourceDb;

    std::cout<<"Processing Themes..."<<std::endl;

    json themesJson = fileHandler->parseJsonFile(fileName);

    for (auto &theme : themesJson) {
        processTheme(theme);
    }
}

ThemeBuilder::~ThemeBuilder() {

}

void ThemeBuilder::processTheme(const json &theme) {

    if (theme.find("name") == theme.end()) {
        throw ProjectBuilderException("Couldn't process theme: each theme must have a 'name' property");
    }

    if (theme.find("elementTextures") == theme.end()) {
        throw ProjectBuilderException("Couldn't process theme: each theme must have an 'elementTextures' property");
    }

    std::string name = JsonHandler::getString(theme, "name");

    // Insert the theme into the database
    std::vector<std::string> columns = {"name"};
    std::vector<std::string> data = {name};
    std::vector<int> types = {DATA_TYPE_STRING};
    int themeId = novel->insert("ui_themes", columns, data, types);

    // Process each possible element inside this object
    auto &elements = theme["elementTextures"];
    for (auto &item : allPossibleElementTypes) {

        if (elements.find(item->getName()) == elements.end()) {

            if (item->isRequired()) {
                throw ProjectBuilderException(Utils::implodeString({"Required UI element type '", item->getName(), "' was not found linked to theme", name}));
            } else {
                continue;
                // TODO: Set a default style/image for the missing element if it does not exist (if there are any that need this)
            }

        } else {
            processElement(themeId, item->getName(), elements[item->getName()]);
        }
    }

    // Process individual screens that need it
    if (theme.find("novelScreen") == theme.end()) {
        throw ProjectBuilderException(Utils::implodeString({"Required attribute 'novelScreen' is missing on theme ", name}));
    }

    processNovelScreen(name, themeId, theme["novelScreen"]);
}

void ThemeBuilder::processNovelScreenAttributes(int themeId) {

    auto *dataSet = new DataSet();

    novel->executeQuery(Utils::implodeString({"SELECT * FROM novel_screen_attributes WHERE ui_theme_id = ", std::to_string(themeId)}));


}

/**
 * Processes a gradient and stores it in the database
 * @param - gradient json data containing gradient information
 * @return - the id of the gradient in the database
 */
int ThemeBuilder::processGradient(const json &gradient) {

    // Process each required colour
    std::vector<std::string> requiredColours = {"topLeftColour", "topRightColour", "bottomLeftColour", "bottomRightColour"};
    std::vector<std::string> columns = {"top_left_colour_id", "top_right_colour_id", "bottom_left_colour_id", "bottom_right_colour_id"};
    std::vector<std::string> values;
    std::vector<int> types;

    for (auto & colour : requiredColours) {

        if (gradient.find(colour) == gradient.end()) {
            throw ProjectBuilderException(Utils::implodeString({"A '", colour, "' parameter is required for a gradient"}));
        }

        // Ensure that the colour is in the correct ('RGB(RRR,GGG,BBB,AAA)' format)
        int newColourId = processColour(gradient[colour]);

        types.push_back(DATA_TYPE_NUMBER);
        values.push_back(std::to_string(newColourId));
    }

    // Write the gradient to the database and return the id
    return novel->insert("ui_theme_gradients", columns, values, types);

}

/**
 * Processes a colour, requires a JSON object of {"r":0, "g": 0, "b": 0, "a": 0)
 * @param colourData
 * @return
 */
int ThemeBuilder::processColour(const json &colourData) {
    std::vector<std::string> requiredValues = {"r","g","b","a"};

    std::vector<std::string> colourValues;

    for (auto &currentValue : requiredValues) {

        // Data validation
        if (colourData.find(currentValue) == colourData.end()) {
            throw ProjectBuilderException(Utils::implodeString({"A ', ", currentValue, "' attribute is required for a colour."}));
        }

        int colourValue = 0;

        try {
            colourValue = JsonHandler::getInteger(colourData, currentValue);
        } catch (std::exception &e) {
            throw ProjectBuilderException(Utils::implodeString({"Colour value for ", currentValue, " is invalid (", colourData[currentValue], "): must be an integer."}));
        }

        if (colourValue < 0 || colourValue > 255) {
            throw ProjectBuilderException(Utils::implodeString({"Colour value '", std::to_string(colourValue), "' is invalid, must be between 0 and 255"}));
        }

        // Store the portion of the colour to store in the database (Relies on the order of the strings used by the for loop being in the correct order)
        colourValues.push_back(std::to_string(colourValue));

    }

    // Write the colour to the database and return the id
    return novel->insert("colours", requiredValues, colourValues, {DATA_TYPE_NUMBER, DATA_TYPE_NUMBER, DATA_TYPE_NUMBER, DATA_TYPE_NUMBER});
}

/**
 *
 * @param element
 * @return id in the database of the element
 */
int ThemeBuilder::processElement(int themeId, const std::string &name, const json &element) {

    // Insert the element into the database
    std::vector<std::string> data = {std::to_string(themeId), name};
    int elementId = novel->insert("ui_theme_elements", {"ui_theme_id", "name"}, data, {DATA_TYPE_NUMBER, DATA_TYPE_STRING});

    if (element.find("textureName") != element.end()) {
        processElementTexture(elementId, JsonHandler::getString(element, "textureName"));
        return elementId;
    }

    if (element.find("textureNames") != element.end()) {

        // Store each texture -> element link in the database, just makes things a little nicer for those who edit the json files.
        for (auto &item : element["textureNames"]) {
            processElementTexture(elementId, item);
        }

        return elementId;
    }

    throw ProjectBuilderException(Utils::implodeString({"Element ", name, "must have either a 'textureName' or 'textureNames' property"}));

}

/**
 * Insert an element texture in to the database, doesn't return anything as we don't care what its id is at this point.
 * @param elementId
 * @param textureName
 */
void ThemeBuilder::processElementTexture(int elementId, const std::string &textureName) {

    // Determine that the texture exists in the resource database's textures table and get its id
    auto *dataSet = new DataSet();
    resource->executeQuery(Utils::implodeString({"SELECT id FROM textures WHERE name = '", textureName, "';"}), dataSet);

    if (dataSet->getRowCount() == 0) {
        throw ProjectBuilderException(Utils::implodeString({"No texture named '", textureName, "' was found in the resource database."}));
    }

    // Insert the link into the database
    std::vector<std::string> data = {std::to_string(elementId), dataSet->getRow(0)->getColumn("id")->getData()->asString()};
    novel->insert("ui_theme_element_textures", {"ui_theme_element_id", "texture_id"}, data, {DATA_TYPE_NUMBER, DATA_TYPE_NUMBER});

}

void ThemeBuilder::processNovelScreen(const std::string &themeName,int themeId, json novelScreenJson) {

    if (novelScreenJson.find("attributes") == novelScreenJson.end()) {
        throw ProjectBuilderException(Utils::implodeString({"Missing attribute 'attributes' on NovelScreen for theme ", themeName}));
    }

    auto attributes = novelScreenJson["attributes"];

    std::vector<DataAttributeGroup*> possibleGroups = UIUtils::getAllPossibleNovelScreenAttributeGroups();

    for (auto & currentGroup : possibleGroups) {

        // Store the group in the database
        std::vector<std::string> columns = {"name", "ui_theme_id"};
        std::vector<std::string> data = {currentGroup->getName(), std::to_string(themeId)};
        std::vector<int> types = {DATA_TYPE_STRING, DATA_TYPE_NUMBER};

        int groupId = novel->insert("novel_screen_attribute_groups", columns, data, types);

        bool groupFoundInJson = attributes.find(currentGroup->getName()) != attributes.end();

        // Print a message saying we are setting default types if the group does not exist
        if (!groupFoundInJson) {
            std::cout<<"NOTICE: Attribute group "<<currentGroup->getName()<<" does not exist, setting default values..."<<std::endl;
        }

        auto groupJson = groupFoundInJson ? attributes[currentGroup->getName()] : json();

        for (auto & currentAttribute : currentGroup->getAcceptedAttributes()) {

            bool attributeFoundInJson = groupJson.find(currentAttribute->getName()) != groupJson.end();

            if (!groupFoundInJson || !attributeFoundInJson) {
                std::cout<<"NOTICE: "<<currentGroup->getName()<<"\\"<<currentAttribute->getName()<<" attribute not found, setting default ("<<currentAttribute->getDefaultValue()<<")"<<std::endl;
            };

            // Insert the value in to the database
            std::string value = attributeFoundInJson ? std::to_string(JsonHandler::getInteger(groupJson, currentAttribute->getName())) : currentAttribute->getDefaultValue();
            std::vector<std::string> columns = {"novel_screen_attribute_group_id", "name", "value"};
            std::vector<std::string> data = {std::to_string(groupId), currentAttribute->getName(), value};
            std::vector<int> types = {DATA_TYPE_NUMBER, DATA_TYPE_STRING, DATA_TYPE_STRING};
            novel->insert("novel_screen_attributes", columns, data, types);
        }
    }

}