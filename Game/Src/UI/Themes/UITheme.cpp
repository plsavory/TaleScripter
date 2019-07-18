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
#include "MisuseException.hpp"

UITheme::UITheme(sf::RenderWindow *renderWindow, ResourceManager *rManager, DatabaseConnection *novel,
                 DataSetRow *data) {
    window = renderWindow;
    resourceManager = rManager;

    // Store all of the attribute types
    auto *dataSet = new DataSet();

    novel->executeQuery(Utils::implodeString(
            {"SELECT * FROM ui_theme_elements WHERE ui_theme_id = ", data->getColumn("id")->getData()->asString()}),
                        dataSet);

    // Ensure that all possible element types which are required actually exist in the theme
    std::vector<ElementType*> types = UIUtils::getAllPossibleElementTypes();

    for (auto &type : types) {
        auto *existingRow = dataSet->findRow("name", type->getName());

        if (!existingRow) {
            if (type->isRequired()) {
                throw MisuseException(Utils::implodeString({"Required UI theme element '", type->getName(), "' is missing."}));
            } else {
                // TODO: Load some kind of default style, might not be needed if this is handled by GameCompiler
                continue;
            }
        }

        // Fetch the element's data, store its graphics pointers and store it against the theme (handled by the object we're newing up here).
        auto *newElement = new UIElement(existingRow, novel, rManager);
        elements.push_back(newElement);
    }

    // TODO: Process any custom elements when this is supported.

    // Process the theme data for the NovelScreen
    novelScreenThemeData = new NovelScreenThemeData(data->getColumn("id")->getData()->asInteger(), novel);
    delete (dataSet);
}

UIElement* UITheme::getElement(const std::string& name) {

    for (auto & element : elements) {
        if (element->getName() == name) {
            return element;
        }
    }

    return nullptr;
}

UITheme::~UITheme() {
    for (auto & item : elements) {
        delete(item);
    }
}

/**
 * Loads UI element data
 * @param data - A DataSet object containing a row from the ui_theme_elements table.
 */
UIElement::UIElement(DataSetRow *data, DatabaseConnection *novel, ResourceManager *rManager) {
    id = data->getColumn("id")->getData()->asInteger();
    name = data->getColumn("name")->getData()->asString();

    auto *dataSet = new DataSet();
    novel->executeQuery(Utils::implodeString({"SELECT * FROM ui_theme_element_textures WHERE ui_theme_element_id = ", std::to_string(id)}), dataSet);

    if (dataSet->getRowCount() == 0) {
        throw MisuseException(Utils::implodeString({"UI element '", name, "' does not have any textures linked to it."}));
    }

    for (auto &row : dataSet->getRows()) {
        auto *newUIElementTexture = new UIElementTexture(row, novel, rManager);
        uiElementTextures.push_back(newUIElementTexture);
    }
}

UIElement::~UIElement() {
    for (auto &item : uiElementTextures) {
        delete(item);
    }
}

std::string UIElement::getName() {
    return name;
}

/**
 * @param width - the desired width
 * @return
 */
UIElementTexture* UIElement::getTextureByHorizontalSize(int width) {

    std::vector<UIElementTexture*> hLargestFirstTextures;

    // Arrange them all largest-first
    std::vector<UIElementTexture*> processedTextures = uiElementTextures;

    while (hLargestFirstTextures.size() != uiElementTextures.size()) {

        int largestX = 0;
        UIElementTexture *largestTexture = nullptr;

        int largestTextureIndex = 0;

        // Get the largest texture size...
        for (int i = 0; i < processedTextures.size(); i++) {

            UIElementTexture *item = processedTextures[i];

            if (item->getTexture()->getTexture()->getSize().x > largestX) {
                largestX = item->getTexture()->getTexture()->getSize().x;
                largestTexture = item;
                largestTextureIndex = i;
            }
        }

        hLargestFirstTextures.push_back(largestTexture);
        processedTextures.erase(processedTextures.begin() + largestTextureIndex);
    }

    // Out of the arranged textures, select the latest that is large enough to accommodate the given width
    UIElementTexture *largestFittingTexture;
    largestFittingTexture = processedTextures[0]; // We always want to return the largest unless a smaller one is large enough to accomodate

    for (auto &texture : processedTextures) {
        if (texture->getTexture()->getTexture()->getSize().x < width) {
            break;
        }
        largestFittingTexture = texture;
    }

    return largestFittingTexture;
}

/**
 * @param height - the height that we need
 * @return
 */
UIElementTexture* UIElement::getTextureByVerticalSize(int height) {

    std::vector<UIElementTexture*> vLargestFirstTextures;

    // Arrange them all largest-first
    std::vector<UIElementTexture*> processedTextures = uiElementTextures;

    while (vLargestFirstTextures.size() != uiElementTextures.size()) {

        int largestY = 0;
        UIElementTexture *largestTexture = nullptr;

        int largestTextureIndex = 0;

        // Get the largest texture size...
        for (int i = 0; i < processedTextures.size(); i++) {

            UIElementTexture *item = processedTextures[i];

            if (item->getTexture()->getTexture()->getSize().y > largestY) {
                largestY = item->getTexture()->getTexture()->getSize().y;
                largestTexture = item;
                largestTextureIndex = i;
            }
        }

        vLargestFirstTextures.push_back(largestTexture);
        processedTextures.erase(processedTextures.begin() + largestTextureIndex);
    }

    // Out of the arranged textures, select the latest that is large enough to accommodate the given width
    UIElementTexture *largestFittingTexture;
    largestFittingTexture = processedTextures[0]; // We always want to return the largest unless a smaller one is large enough to accomodate

    for (auto &texture : processedTextures) {
        if (texture->getTexture()->getTexture()->getSize().y < height) {
            break;
        }
        largestFittingTexture = texture;
    }

    return largestFittingTexture;
}

UIElementTexture::UIElementTexture(DataSetRow *data, DatabaseConnection *novel, ResourceManager *rManager) {
    int textureId = data->getColumn("texture_id")->getData()->asInteger()-1; // Database is 1-indexed, memory is not.
    Texture* foundTexture = rManager->getTexture(textureId);

    if (!foundTexture) {
        throw ResourceException(Utils::implodeString({"No texture with id ", std::to_string(textureId), " was found"}));
    }

    texture = foundTexture;
}

UIElementTexture::~UIElementTexture() {
    // TODO: Unload any textures used by this item
}

Texture* UIElementTexture::getTexture() {
    return texture;
}

/**
 * Fetches all of the NovelScreen information for a given theme
 * @param themeId - the id of the theme in the database
 */
NovelScreenThemeData::NovelScreenThemeData(int themeId, DatabaseConnection *novel) {

    // It is required that every piece of data for this exists in the database and has values, so it's ok to fetch them directly.
    auto *dataSet = new DataSet();

    novel->executeQuery(Utils::implodeString({"SELECT * FROM novel_screen_attribute_groups WHERE ui_theme_id = ", std::to_string(themeId)}), dataSet);

    if (dataSet->getRowCount() == 0) {
        throw DataSetException(Utils::implodeString({"No novel screen attribute groups were found for theme ", std::to_string(themeId)}));
    }

    auto *novelTextDisplayRow = dataSet->findRow("name", "textDisplay");

    if (!novelTextDisplayRow) {
        throw DataSetException(Utils::implodeString({"No 'textDisplay' attribute group was found for theme ", std::to_string(themeId)}));
    }

    novelScreenTextDisplay = new NovelScreenTextDisplay(novelTextDisplayRow, novel);

    delete(dataSet);

}

NovelScreenTextDisplay::NovelScreenTextDisplay(DataSetRow *groupRow, DatabaseConnection *novel) {

    int groupId = groupRow->getColumn("id")->getData()->asInteger();

    // Find the attributes that we care about, throwing errors if they don't exist.
    auto *dataSet = new DataSet();
    novel->executeQuery(Utils::implodeString({"SELECT * FROM novel_screen_attributes WHERE novel_screen_attribute_group_id = ", std::to_string(groupId)}), dataSet);
    framePositionX = dataSet->findRow("name", "framePositionX", true)->getColumn("value")->getData()->asInteger();
    framePositionY = dataSet->findRow("name", "framePositionY", true)->getColumn("value")->getData()->asInteger();
    frameFillAlpha = dataSet->findRow("name", "frameFillAlpha", true)->getColumn("value")->getData()->asInteger();
    frameBorderAlpha = dataSet->findRow("name", "frameBorderAlpha", true)->getColumn("value")->getData()->asInteger();
    textOffsetX = dataSet->findRow("name", "textOffsetX", true)->getColumn("value")->getData()->asInteger();
    textOffsetY = dataSet->findRow("name", "textOffsetY", true)->getColumn("value")->getData()->asInteger();
    maxTextWidth = dataSet->findRow("name", "maxTextWidth", true)->getColumn("value")->getData()->asInteger();

}