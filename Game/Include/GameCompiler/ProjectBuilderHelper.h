//
// Created by Pete on 2019-07-21.
//

#ifndef TALESCRIPTER_PROJECT_BUILDER_HELPER_H
#define TALESCRIPTER_PROJECT_BUILDER_HELPER_H

using namespace nlohmann;

/**
 * Stores generic functions which will be used throughout multiple places of the ProjectBuilder.
 */
class ProjectBuilderHelper {
public:
    ProjectBuilderHelper(DatabaseConnection *novelDb, DatabaseConnection *resourceDb) {
        novel = novelDb;
        resource = resourceDb;
    }
    int processMenu(json menuJson) {
        return 0;
    };
private:
    DatabaseConnection *novel;
    DatabaseConnection *resource;
};

#endif //TALESCRIPTER_PROJECTBUILDERHELPER_H
