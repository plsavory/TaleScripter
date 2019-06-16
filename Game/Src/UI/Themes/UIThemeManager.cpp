//
// Created by Pete on 2019-06-15.
//

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "Misc/Utils.hpp"
#include "DatabaseConnection.hpp"
#include "BackgroundImageRenderer.hpp"
#include "ResourceManager.hpp"
#include "UI/Themes/UIThemeManager.h"
#include "MisuseException.hpp"

UIThemeManager::UIThemeManager(sf::RenderWindow *renderWindow, ResourceManager *rManager,
                               DatabaseConnection *novelDB) {
    // Copy the pointers...
    window = renderWindow;
    resourceManager = rManager;
    activeTheme = 0;
    novel = novelDB;

    // Load the UI themes from the database
    auto *dataSet = new DataSet();
    novelDB->executeQuery("SELECT * FROM ui_themes;", dataSet);

    if (dataSet->getRowCount() == 0) {
        throw ResourceException("No UI themes exist in the database");
    }

    for (int i = 0; i < dataSet->getRowCount(); i++) {
        auto newTheme = new UITheme(window, resourceManager, novelDB, dataSet->getRow(i));
        themes.push_back(newTheme);
    }

    delete(dataSet);
}

UIThemeManager::~UIThemeManager() {
    // Delete all of the themes from memory
    for (auto &theme : themes) {
        delete (theme);
    }
}

UITheme *UIThemeManager::getActiveTheme() {

    if (activeTheme > themes.size() - 1) {
        throw MisuseException(Utils::implodeString(
                {"Attempted to use a theme that has not been loaded (", std::to_string(activeTheme), ")"}));
    }

    return themes[activeTheme];

}