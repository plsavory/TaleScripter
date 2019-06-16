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

UITheme::UITheme(sf::RenderWindow *renderWindow, ResourceManager *rManager, DataSetRow data) {
    window = renderWindow;
    resourceManager = rManager;
}

UITheme::~UITheme() {

}