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