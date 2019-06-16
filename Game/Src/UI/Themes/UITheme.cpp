//
// Created by Pete on 2019-06-15.
//

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "Misc/Utils.hpp"
#include "DatabaseConnection.hpp"
#include "BackgroundImageRenderer.hpp"
#include "ResourceManager.hpp"
#include "UI/Themes/UITheme.h"
#include "UI/UIConstants.h"
#include "MisuseException.hpp"

UITheme::UITheme(sf::RenderWindow *renderWindow, ResourceManager *rManager, DatabaseConnection *novel,
                 DataSetRow *data) {
    window = renderWindow;
    resourceManager = rManager;

    // Store all of the attribute types
    auto *dataSet = new DataSet();

    novel->executeQuery(Utils::implodeString(
            {"SELECT * FROM ui_theme_attributes WHERE ui_theme_id = ", data->getColumn("id")->getData()->asString()}),
                        dataSet);

    for (int i = 0; i < dataSet->getRowCount(); i++) {
        auto *newAttribute = new UIThemeAttribute(renderWindow, rManager, novel, dataSet->getRow(i));
        attributes.push_back(newAttribute);
    }

    // Check that we have all of the attributes that we want...
    for (int i = 0; i < 1; i++) {
        // This loop does not actually loop right now... (There is a reason for doing this in the future, really!)
        // TODO: Continue without the check if an attribute is optional
        if (attributes.size() < i + 1) {
            throw ResourceException(Utils::implodeString(
                    {"Required attribute type '", UIConstants::AttributeType::getTypeName(i), "' is missing"}));
        }
    }

    delete (dataSet);
}

UITheme::~UITheme() {
    for (auto &attribute : attributes) {
        delete (attribute);
    }
}

UIThemeAttribute *UITheme::getAttribute(int type) {

    for (auto &attribute : attributes) {
        if (attribute->getType() == type) {
            return attribute;
        }
    }

    throw ResourceException(
            Utils::implodeString({"No UI theme attribute of type ", std::to_string(type), " was found"}));
}

// Attribute functions
int UIThemeAttribute::getType() {
    return type;
}

UIThemeAttribute::UIThemeAttribute(sf::RenderWindow *renderWindow, ResourceManager *rManager, DatabaseConnection *novel,
                                   DataSetRow *data) {

    std::vector<std::string> fields = {"fill_gradient_id", "outline_gradient_id", "selected_gradient_id"};

    for (int i = 0; i < fields.size(); i++) {
        // Get all of the gradients that we need
        auto *dataSet = new DataSet();

        novel->executeQuery(Utils::implodeString({"SELECT * FROM ui_theme_gradients WHERE id = ",
                                                  data->getColumn(fields[i])->getData()->asString()}), dataSet);

        if (dataSet->getRowCount() == 0) {
            throw ResourceException(Utils::implodeString(
                    {"No gradient found linked to attribute ", UIConstants::AttributeType::getTypeName(
                            data->getColumn("ui_theme_attribute_type_id")->getData()->asInteger())}));
        }

        auto *newGradient = new UIThemeGradient(dataSet->getRow(0), novel);

        if (fields[i] == "fill_gradient_id") {
            fillGradient = newGradient;
        }

        if (fields[i] == "outline_gradient_id") {
            outlineGradient = newGradient;
        }

        if (fields[i] == "selected_gradient_id") {
            selectedGradient = newGradient;
        }

        delete (dataSet);
    }
}

UIThemeAttribute::~UIThemeAttribute() {

}

// Gradient functions
UIThemeGradient::UIThemeGradient(DataSetRow *data, DatabaseConnection *novel) {

    std::vector<std::string> fields = {"top_left_colour_id", "top_right_colour_id", "bottom_left_colour_id",
                                       "bottom_right_colour_id"};

    // Load all of the colours into memory
    for (int i = 0; i < fields.size(); i++) {

        auto *dataSet = new DataSet();

        colours.push_back(getColourFromDatabase(data->getColumn(fields[i])->getData()->asInteger(), novel));

        delete(dataSet);
    }
}

UIThemeGradient::~UIThemeGradient() {
    for (auto &colour : colours) {
        delete(colour);
    }
}

sf::Color* UIThemeGradient::getColourFromDatabase(int colourId, DatabaseConnection *novel) {

    auto *colourData = new DataSet();
    novel->executeQuery(Utils::implodeString({"SELECT * FROM colours WHERE id = ", std::to_string(colourId)}), colourData);

    if (colourData->getRowCount() == 0) {
        throw ResourceException(Utils::implodeString({"No colour with id '", std::to_string(colourId), "' was found"}));
    }

    auto *row = colourData->getRow(0);

    auto *colour = new sf::Color();

    for (int i = 0; i < row->getColumnCount(); i++) {

        int value = row->getColumn(i)->getData()->asInteger();

        switch (i) {
            case 0:
                continue; // Don't want to try and process the id column...
            case 1:
                colour->r = value;
                break;
            case 2:
                colour->g = value;
                break;
            case 3:
                colour->b = value;
                break;
            case 4:
                colour->a = value;
                break;
            default:
                throw MisuseException(Utils::implodeString({"Unknown colour index ", std::to_string(i)}));
        }

    }

    delete(colourData);
    return colour;

}