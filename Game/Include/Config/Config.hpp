#ifndef CONFIG_INCLUDED
#define CONFIG_INCLUDED

#include <nlohmann/json.hpp>

#include "Exceptions/ConfigurationException.hpp"
#include "Config/ConfigConstants.hpp"
#include "Misc/JsonHandler.hpp"

// TODO: Validate that the novel itself supports any given setting (e.g display modes due to sprite sizes)

struct Config {
public:
    Config() = default;

    ~Config() = default;

    /**
     * Set default values, used on every application launch
     */
    void setDefaults() {
        setDisplayMode(ConfigConstants::DISPLAY_MODE_WINDOWED);
        setFrameRate(ConfigConstants::FPS_60);
    }

    /**
     * Sets our configuration based on a decoded JSON string and throws the necessary exceptions if invalid values are found
     * @param pConfig
     */
    void setFromJson(json pConfig) {

        if (pConfig.find("displayMode") != pConfig.end()) {
            setDisplayMode(JsonHandler::getInteger(pConfig, "displayMode"));
        }

        if (pConfig.find("frameRate") != pConfig.end()) {
            setFrameRate(JsonHandler::getInteger(pConfig, "frameRate"));
        }

    };

    /**
     * Validate that we have a supported display mode
     * @param pDisplayMode
     */
    void setDisplayMode(int pDisplayMode) {

        switch (displayMode) {
            case ConfigConstants::DISPLAY_MODE_BORDERLESS:
            case ConfigConstants::DISPLAY_MODE_FULLSCREEN:
            case ConfigConstants::DISPLAY_MODE_WINDOWED:
                break;
            default:
                std::vector<std::string> error = {
                  "Unsupported display mode setting: ", std::to_string(pDisplayMode)
                };
                throw ConfigurationException(Utils::implodeString(error));
        }

        // TODO: Validate that the computer can display this mode
        displayMode = pDisplayMode;

    }

    /**
     * Validates that we have been given an allowed frame rate value
     * @param pFrameRate
     */
    void setFrameRate(int pFrameRate) {

        switch (pFrameRate) {
            case ConfigConstants::FPS_30:
            case ConfigConstants::FPS_60:
            case ConfigConstants::FPS_120:
            case ConfigConstants::FPS_144:
                break;
            default:
                std::vector<std::string> error = {
                        "Unsupported frame rate setting: ", std::to_string(pFrameRate)
                };
                throw ConfigurationException(Utils::implodeString(error));
        }

        frameRate = pFrameRate;
    }

    int getFrameRate() {
        return frameRate;
    }

    int getDisplayMode() {
        return displayMode;
    }

private:
    // Window settings
    int displayMode;
    int displayWidth;
    int displayHeight;
    int frameRate;
};

#endif