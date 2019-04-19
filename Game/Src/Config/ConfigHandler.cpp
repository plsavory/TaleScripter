#include "Misc/JsonHandler.hpp"
#include "Config/ConfigHandler.hpp"
#include "Misc/Utils.hpp"
#include <fstream>

ConfigHandler::ConfigHandler() {

    config = new Config();

    // Call setDefaults so that if the config file is missing, or if there are missing options we have a base config...
    config->setDefaults();

    // TODO: Provide an option somewhere to override the default config file path if need be
    readConfigFile();

}

ConfigHandler::~ConfigHandler() {
    delete(config);
}

void ConfigHandler::readConfigFile() {
    readConfigFile("config.json");
}

void ConfigHandler::readConfigFile(const std::string &fileName) {

    if (!Utils::fileExists(fileName)) {
        // We will later write the config file if this is the case.
        return;
    }

    std::ifstream stream(fileName);
    json configJsonData = json::parse(stream);
    config->setFromJson(configJsonData);

}

void ConfigHandler::writeConfigFile() {

}

void ConfigHandler::writeConfigFile(const std::string &fileName) {

}