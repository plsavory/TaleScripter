//
// Created by Pete on 2019-06-15.
//

#ifndef TALESCRIPTER_THEME_BUILDER_HPP
#define TALESCRIPTER_THEME_BUILDER_HPP

#include "UI/UIUtils.hpp"

class ThemeBuilder {
public:
    ThemeBuilder(std::string fileName, DatabaseConnection *novelDb, DatabaseConnection *resourceDb, JsonHandler *fileHandler);
    ~ThemeBuilder();
private:
    void processTheme(const json &theme);
    int processElement(int themeId, const std::string &name, const json &element);
    void processElementTexture(int elementId, const std::string &textureName);
    void processAttribute(const json &attribute, int themeId, const std::string &attributeName, int attributeId);
    int processGradient(const json &gradient);
    int processColour(const json &colourData);
    static std::vector<std::vector<std::string>> getUiElementTypes;
    DatabaseConnection *novel;
    DatabaseConnection *resource;
    std::vector<UIConstants::ElementType*> allPossibleElementTypes;
};

#endif //TALESCRIPTER_THEME_BUILDER_HPP
