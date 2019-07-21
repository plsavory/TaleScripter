//
// Created by Pete on 2019-07-21.
//

#ifndef TALESCRIPTER_SCREEN_BUILDER_HPP
#define TALESCRIPTER_SCREEN_BUILDER_HPP

class ScreenBuilder {
public:
    ScreenBuilder(DatabaseConnection *novelDb, DatabaseConnection *resourceDb, JsonHandler *fHandler, ProjectBuilderHelper *pbHelper);
    ~ScreenBuilder();
    void processTitleScreens(const std::string &fileName);
private:
    DatabaseConnection *novel;
    DatabaseConnection *resource;
    JsonHandler *fileHandler;
    ProjectBuilderHelper *projectBuilderHelper;
    void processTitleScreen(json titleScreenJson);
};

#endif //TALESCRIPTER_SCREENBUILDER_HPP
