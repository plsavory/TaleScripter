//
// Created by Pete on 2019-06-15.
//

#ifndef TALESCRIPTER_UI_THEME_H
#define TALESCRIPTER_UI_THEME_H

class UIThemeGradient {
public:
    UIThemeGradient(DataSetRow *data, DatabaseConnection *novel);
    ~UIThemeGradient();
    static const int COLOUR_TOP_LEFT = 0;
    static const int COLOUR_TOP_RIGHT = 1;
    static const int COLOUR_BOTTOM_LEFT = 2;
    static const int COLOUR_BOTTOM_RIGHT = 3;
private:
    std::vector<sf::Color*> colours;
    sf::Color* getColourFromDatabase(int colourId, DatabaseConnection *novel);
};

class UIThemeAttribute {
public:
    UIThemeAttribute(sf::RenderWindow *renderWindow, ResourceManager *rManager, DatabaseConnection *novel, DataSetRow *data);
    ~UIThemeAttribute();
    int getType();
private:
    sf::RenderWindow *window;
    ResourceManager *resourceManager;
    int type;
    UIThemeGradient *fillGradient;
    UIThemeGradient *outlineGradient;
    UIThemeGradient *selectedGradient;
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

    UIThemeAttribute* getAttribute(int type);

private:
    sf::RenderWindow *window;
    ResourceManager *resourceManager;
    std::vector<UIThemeAttribute*> attributes;
};

#endif //TALESCRIPTER_UITHEME_H
