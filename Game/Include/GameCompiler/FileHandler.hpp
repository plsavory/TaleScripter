#ifndef FILE_HANDLER_INCLUDED
#define FILE_HANDLER_INCLUDED

#include <fstream>
#include <nlohmann/json.hpp>
#include "Exceptions/ProjectBuilderException.hpp"
#include "Misc/Utils.hpp"
#include <vector>

using namespace nlohmann;

class FileHandler {
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
private:
    std::string currentFileBeingProcessed;
};

#endif