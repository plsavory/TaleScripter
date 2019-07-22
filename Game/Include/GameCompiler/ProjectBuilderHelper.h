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
    int processMenu(const json& menuJson) {

        // Validate that the menu has the required types and items
        if (!menuJson.is_object()) {
            throw ProjectBuilderException("Menu data must be a json object");
        }

        if (menuJson.find("keyboardInputLayout") == menuJson.end()) {
            throw ProjectBuilderException("Required element 'keyboardInputLayout' is missing.");
        }

        if (!menuJson["keyboardInputLayout"].is_string()) {
            throw ProjectBuilderException("Item 'keyboardInputLayout' must be a string containing values 'horizontal' or 'vertical'");
        }

        std::string keyboardInputLayout = JsonHandler::getString(menuJson, "keyboardInputLayout");
        if (!(keyboardInputLayout == "vertical" || keyboardInputLayout == "horizontal")) {
            throw ProjectBuilderException("Item 'keyboardInputLayout' must contain values 'horizontal' or 'vertical'");
        }

        if (menuJson.find("options") == menuJson.end()) {
            throw ProjectBuilderException("Required array 'options' is missing.");
        }

        if (!menuJson["options"].is_array()) {
            throw ProjectBuilderException("Attribute 'options' must be an array.");
        }

        // Store the menu
        std::vector<std::string> columns = {"heading_text", "orientation"};
        std::vector<std::string> values = {"", keyboardInputLayout};
        std::vector<int> types = {DATA_TYPE_STRING, DATA_TYPE_STRING};
        int menuId = novel->insert("menus", columns, values, types);

        // Process each option and store it
        for(auto &option : menuJson["options"]) {
            processMenuOption(option, menuId);
        }

        return 0;
    };
private:
    int processMenuOption(const json &menuOptionJson, int menuId) {

        if (menuOptionJson.find("action") == menuOptionJson.end()) {
            throw ProjectBuilderException("Required attribute 'action' is missing.");
        }

        std::string text;
        std::string textureName;
        std::string action;
        int textureId = 0;

        if (menuOptionJson.find("text") != menuOptionJson.end()) {
            if (!menuOptionJson["text"].is_null()) {
                text = JsonHandler::getString(menuOptionJson, "text");
            }
        }

        if (menuOptionJson.find("textureName") != menuOptionJson.end()) {
            if (!menuOptionJson["textureName"].is_null()) {
                textureName = JsonHandler::getString(menuOptionJson, "textureName");
            }
        }

        if (menuOptionJson.find("action") != menuOptionJson.end()) {
            if (!menuOptionJson["action"].is_null()) {
                action = JsonHandler::getString(menuOptionJson, "action");
            }
        }

        if (menuOptionJson.find("xPosition") == menuOptionJson.end()) {
            throw ProjectBuilderException("Required attribute 'xPosition' is missing");
        }

        if (menuOptionJson.find("yPosition") == menuOptionJson.end()) {
            throw ProjectBuilderException("Required attribute 'yPosition' is missing");
        }

        if (!textureName.empty()) {
            auto *textureDataSet = new DataSet();
            resource->executeQuery(Utils::implodeString({"SELECT * FROM textures WHERE name = '", textureName ,"';"}), textureDataSet);

            if (textureDataSet->getRowCount() == 0) {
                throw ProjectBuilderException(Utils::implodeString({"No texture with name '", textureName,"' exists."}));
            }

            textureId = textureDataSet->getRow(0)->getColumn("id")->getData()->asInteger();
            delete(textureDataSet);
        }

        if (text.empty() && textureName.empty()) {
            throw ProjectBuilderException("Either attribute 'text' or 'imageName' should exist and be populated.");
        }

        std::string xPosition = std::to_string(JsonHandler::getInteger(menuOptionJson, "xPosition"));
        std::string yPosition = std::to_string(JsonHandler::getInteger(menuOptionJson, "yPosition"));

        std::vector<std::string> columns = {"x_position", "y_position", "text", "texture_id", "action", "menu_id"};
        std::vector<std::string> values = {xPosition, yPosition, text, std::to_string(textureId), action, std::to_string(menuId)};
        std::vector<int> types = {DATA_TYPE_NUMBER, DATA_TYPE_NUMBER, DATA_TYPE_STRING, DATA_TYPE_NUMBER, DATA_TYPE_STRING, DATA_TYPE_NUMBER};
        return novel->insert("menu_options", columns, values, types);
    };
    DatabaseConnection *novel;
    DatabaseConnection *resource;
};

#endif //TALESCRIPTER_PROJECTBUILDERHELPER_H
