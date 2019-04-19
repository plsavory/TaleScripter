#ifndef FILE_HANDLER_INCLUDED
#define FILE_HANDLER_INCLUDED

#include <fstream>
#include <nlohmann/json.hpp>
#include "Exceptions/ProjectBuilderException.hpp"
#include "Exceptions/JsonParserException.hpp"
#include "Misc/Utils.hpp"
#include <vector>

using namespace nlohmann;

class JsonHandler {
public:
    json parseJsonFile(const std::string& fileName) {
        currentFileBeingProcessed = fileName;

        if (!Utils::fileExists(fileName)) {
            std::vector<std::string> errorVector = {
                    "Unable to find file with filename: ",
                    fileName
            };
            throw ProjectBuilderException(Utils::implodeString(errorVector));
        }

        std::ifstream stream(fileName);
        json jsonData = json::parse(stream);

        return jsonData;
    }

    std::string getLastReadFileName() {
        return currentFileBeingProcessed;
    }

    // These methods handle JSON type casts and are used to generate more helpful errors to allow the user to find the issue
    static bool getBoolean(json object, const std::string& fieldName) {

        bool value = false;
        try {
            value = object[fieldName].get<bool>();
        } catch (nlohmann::json::exception &e) {
            handleError(e, fieldName);
        }

        return value;
    }

    static int getInteger(json object, const std::string& fieldName) {

        int value = false;
        try {
            value = object[fieldName].get<int>();
        } catch (nlohmann::json::exception &e) {
            handleError(e, fieldName);
        }

        return value;
    }

    static double getDouble(json object, const std::string& fieldName) {

        double value = false;
        try {
            value = object[fieldName].get<double>();
        } catch (nlohmann::json::exception &e) {
            handleError(e, fieldName);
        }

        return value;
    }

    static std::string getString(json object, const std::string& fieldName) {
        std::string value;
        try {
            value = object[fieldName].get<std::string>();
        } catch (nlohmann::json::exception &e) {
            handleError(e, fieldName);
        }

        return value;
    }
private:
    std::string currentFileBeingProcessed;
    std::string currentFieldBeingProcessed;

    /**
     *
     * @param e
     * @param fieldName
     */
    static void handleError(nlohmann::json::exception &e, const std::string& fieldName) {

        std::vector<std::string> error = {
                "Unable to process property '", fieldName, "': ",
                e.what(), " - This is probably a data type mismatch"
        };

        throw JsonParserException(Utils::implodeString(error));
    }
};

#endif