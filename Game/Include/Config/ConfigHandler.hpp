#ifndef CONFIG_HANDLER_INCLUDED
#define CONFIG_HANDLER_INCLUDED

#include "Config/Config.hpp"

// This class will handle reading and writing to the configuration file.

class ConfigHandler {
public:
    ConfigHandler();

    ~ConfigHandler();

    Config* getConfig() {
        return config;
    }

private:

    /**
     * Calls readConfigFile with a default file name
     */
    void readConfigFile();

    /**
     * Read's the user's config from the config file and stores it within the properties on this class.
     * @param fileName
     */
    void readConfigFile(const std::string &fileName);

    /**
     * Calls the other writeConfigFile method with a default file name.
     */
    void writeConfigFile();

    /**
     * Writes the current configuration to the config file.
     * Take a file name in the case of the user overriding it somewhere.
     * @param fileName
     */
    void writeConfigFile(const std::string &fileName);

    /**
     * The actual config object, will hold all of the current configuration properties
     */
    Config *config;

};

#endif