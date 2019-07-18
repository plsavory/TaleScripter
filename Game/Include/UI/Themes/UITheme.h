//
// Created by Pete on 2019-06-15.
//

#ifndef TALESCRIPTER_UI_THEME_H
#define TALESCRIPTER_UI_THEME_H

#include "UI/UIUtils.hpp"

class UIElementTexture {
public:
    UIElementTexture(DataSetRow *data, DatabaseConnection *novel, ResourceManager *rManager);
    ~UIElementTexture();
    Texture* getTexture();
private:
    int id;
    Texture *texture;
};

class UIElement {
public:
    UIElement(DataSetRow *data, DatabaseConnection *novel, ResourceManager *rManager);
    ~UIElement();
    std::string getName();
    UIElementTexture* getTextureByHorizontalSize(int width);
    UIElementTexture* getTextureByVerticalSize(int height);
private:
    int id;
    std::string name;
    std::vector<UIElementTexture*> uiElementTextures;
};

class UITheme {
public:

    /**
     *
     * @param renderWindow an SFML RenderWindow object
     * @param rManager a ResourceManager object
     * @param data a row of data from the ui_themes table in the database
     */
    UITheme(sf::RenderWindow *renderWindow, ResourceManager *rManager, DatabaseConnection *novel, DataSetRow *data);

    ~UITheme();

    UIElement* getElement(const std::string& name);

private:
    sf::RenderWindow *window;
    ResourceManager *resourceManager;
    std::vector<UIElement*> elements;
};

#endif //TALESCRIPTER_UITHEME_H
