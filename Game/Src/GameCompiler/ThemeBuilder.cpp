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

    novel = novelDb;
    resource = resourceDb;

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

    if (theme.find("data") == theme.end()) {
        throw ProjectBuilderException("Couldn't process theme: each theme must have a 'data' property");
    }

    std::string name = JsonHandler::getString(theme, "name");

    // Insert the theme into the database
    std::vector<std::string> columns = {"name"};
    std::vector<std::string> data = {name};
    std::vector<int> types = {DATA_TYPE_STRING};
    int themeId = novel->insert("ui_themes", columns, data, types);

    // Process each element inside this object
    std::vector<bool> processedTypes;

    for (int i = 0; i<UIConstants::AttributeType::getTypeNames().size(); i++) {
        processedTypes.push_back(false);
    }

    for (int i = 0; i<UIConstants::AttributeType::getTypeNames().size(); i++) {

        if (theme["data"].find(UIConstants::AttributeType::getTypeName(i)) == theme["data"].end()) {
            continue;
        }

        std::string attributeName = UIConstants::AttributeType::getTypeName(i);
        processAttribute(theme["data"][attributeName], themeId, attributeName, i+1);
        processedTypes[i] = true;
    }

    // Missing type checking
    for (int i = 0; i < processedTypes.size(); i++) {
        switch (i) {
            // Put required values here
            case UIConstants::AttributeType::UIContainer:
                if (!processedTypes[i]) {
                    throw ProjectBuilderException(Utils::implodeString({"An attribute type '", UIConstants::AttributeType::getTypeName(i), " is required."}));
                }
                break;
            default:
                throw ProjectBuilderException("Unknown attribute type");
        }
    }
}

void ThemeBuilder::processAttribute(const json &attribute, int themeId, const std::string &attributeName, int attributeId) {

    // Insert the gradients which will be used for this attribute
    std::vector<std::string> requiredGradients = {"fill", "outline", "selected"};
    std::vector<std::string> columns = {"ui_theme_id", "ui_theme_attribute_type_id", "fill_gradient_id", "outline_gradient_id", "selected_gradient_id"};
    std::vector<std::string> values = {std::to_string(themeId), std::to_string(attributeId)};
    std::vector<int> types = {DATA_TYPE_NUMBER, DATA_TYPE_NUMBER};

    for (auto &gradientName : requiredGradients) {

        if (attribute.find(gradientName) == attribute.end()) {
            throw ProjectBuilderException(Utils::implodeString({"A '", gradientName, "' parameter is required a ", attributeName, " attribute."}));
        }

        // Store each gradient in the database and record the id's
        int newGradientId = processGradient(attribute[gradientName]);
        values.push_back(std::to_string(newGradientId));
        types.push_back(DATA_TYPE_NUMBER);
    };

    // Write the attribute to the database
    novel->insert("ui_theme_attributes", columns, values, types);
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