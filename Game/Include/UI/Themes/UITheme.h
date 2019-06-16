//
// Created by Pete on 2019-06-15.
//

#ifndef TALESCRIPTER_UI_THEME_H
#define TALESCRIPTER_UI_THEME_H

class UITheme {
public:

    /**
     *
     * @param renderWindow an SFML RenderWindow object
     * @param rManager a ResourceManager object
     * @param data a row of data from the ui_themes table in the database
     */
    UITheme(sf::RenderWindow *renderWindow, ResourceManager *rManager, DataSetRow data);

    ~UITheme();

private:
    sf::RenderWindow *window;
    ResourceManager *resourceManager;
};

#endif //TALESCRIPTER_UITHEME_H
