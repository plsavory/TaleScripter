//
// Created by Pete on 2019-06-15.
//

#ifndef TALESCRIPTER_THEME_BUILDER_HPP
#define TALESCRIPTER_THEME_BUILDER_HPP

class ThemeBuilder {
public:
    ThemeBuilder(std::string fileName, DatabaseConnection *novelDb, DatabaseConnection *resourceDb, JsonHandler *fileHandler);
    ~ThemeBuilder();
private:
    void processTheme(const json &theme);
    void processAttribute(const json &attribute, int themeId, const std::string &attributeName, int attributeId);
    int processGradient(const json &gradient);
    int processColour(const json &colourData);
    static void populateThemeAttributeTypeTable();
    DatabaseConnection *novel;
    DatabaseConnection *resource;
};

#endif //TALESCRIPTER_THEME_BUILDER_HPP
