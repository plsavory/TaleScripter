//
// Created by Pete on 2019-06-15.
//

#ifndef TALESCRIPTER_UI_THEME_H
#define TALESCRIPTER_UI_THEME_H

#include "UI/UIUtils.hpp"

class NovelScreenCharacterNameDisplay {
public:
    NovelScreenCharacterNameDisplay(DataSetRow *groupRow, DatabaseConnection *novel);

    // We don't want any of these to be settable (at the moment)
    int getFramePositionX() {return framePositionX;};
    int getFramePositionY() {return framePositionY;};
    int getFrameFillAlpha() {return frameFillAlpha;};
    int getFrameBorderAlpha() {return frameBorderAlpha;};
    int getTextOffsetX() {return textOffsetX;};
    int getTextOffsetY() {return textOffsetY;};
private:
    int framePositionX;
    int framePositionY;
    int frameFillAlpha;
    int frameBorderAlpha;
    int textOffsetX;
    int textOffsetY;
};

class NovelScreenTextDisplay {
public:
    NovelScreenTextDisplay(DataSetRow *groupRow, DatabaseConnection *novel);

    // We don't want any of these to be settable (at the moment)
    int getFramePositionX() {return framePositionX;};
    int getFramePositionY() {return framePositionY;};
    int getFrameFillAlpha() {return frameFillAlpha;};
    int getFrameBorderAlpha() {return frameBorderAlpha;};
    int getTextOffsetX() {return textOffsetX;};
    int getTextOffsetY() {return textOffsetY;};
    int getMaxTextWidth() {return maxTextWidth;}
private:
    int framePositionX;
    int framePositionY;
    int frameFillAlpha;
    int frameBorderAlpha;
    int textOffsetX;
    int textOffsetY;
    int maxTextWidth;
};

class NovelScreenThemeData {
public:
    NovelScreenThemeData(int themeId, DatabaseConnection *novel);
    NovelScreenTextDisplay* getTextDisplayData() {
        return novelScreenTextDisplay;
    }
    NovelScreenCharacterNameDisplay* getNovelScreenCharacterNameDisplay() {
        return novelScreenCharacterNameDisplay;
    }
private:
    NovelScreenTextDisplay *novelScreenTextDisplay;
    NovelScreenCharacterNameDisplay *novelScreenCharacterNameDisplay;
};

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

    NovelScreenThemeData* getNovelScreenThemeData() {
        return novelScreenThemeData;
    }

private:
    sf::RenderWindow *window;
    ResourceManager *resourceManager;
    std::vector<UIElement*> elements;
    NovelScreenThemeData *novelScreenThemeData;
};

#endif //TALESCRIPTER_UITHEME_H
