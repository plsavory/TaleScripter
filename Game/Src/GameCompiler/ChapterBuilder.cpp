/*
  This class handles the parsing of a chapter .json file, and will create the appropriate records in the SQLite database for it
  This class will also handle sanity checks for errors which could make it into the game's database.
 */

#include <nlohmann/json.hpp>

using json = nlohmann::json;

#include "Database/DatabaseConnection.hpp"
#include "Misc/JsonHandler.hpp"
#include "GameCompiler/ChapterBuilder.hpp"
#include "Exceptions/ProjectBuilderException.hpp"
#include "Exceptions/JsonParserException.hpp"
#include "Misc/Utils.hpp"
#include <vector>
#include <regex>
#include <fstream>
#include "Database/TypeCaster.hpp"

ChapterBuilder::ChapterBuilder(const std::string &fileName, DatabaseConnection *novelDb, JsonHandler *fileHandler) {

    if (!Utils::fileExists(fileName)) {

        std::vector<std::string> error = {
                "Unable to process chapter, could not find file:",
                fileName
        };

        throw ProjectBuilderException(Utils::implodeString(error));
    }

    fHandler = fileHandler;
    chapterFileName = fileName;
    novel = novelDb;

}

ChapterBuilder::~ChapterBuilder() = default;

void ChapterBuilder::process() {

    json chapterJson = fHandler->parseJsonFile(chapterFileName);

    // Attributes that we need to store...
    std::string title;
    std::string accessibleName;
    std::string hidden;
    std::string requirementId;
    std::string description;

    // Validate that we have everything we need. Substitute or throw errors where needed.
    if (chapterJson.find("title") == chapterJson.end()) {
        title = "Unnamed Chapter";
    } else {
        title = JsonHandler::getString(chapterJson,"title");
    }

    std::cout << "Processing chapter " << title << "..." << std::endl;

    if (chapterJson.find("accessibleName") == chapterJson.end()) {
        accessibleName = title;
    } else {
        accessibleName = JsonHandler::getString(chapterJson,"accessibleName");
    }

    if (chapterJson.find("requirementId") == chapterJson.end()) {
        requirementId = "0";
    } else {
        requirementId = TypeCaster::cast(JsonHandler::getInteger(chapterJson, "requirementId"));
    }

    if (chapterJson.find("hidden") == chapterJson.end()) {
        hidden = "FALSE";
    } else {
        hidden = TypeCaster::cast(JsonHandler::getBoolean(chapterJson,"hidden"));
    }

    if (chapterJson.find("description") == chapterJson.end()) {
        description = "";
    } else {
        description = JsonHandler::getString(chapterJson,"description");
    }

    std::vector<std::string> columns = {"title", "accessible_name", "description", "hidden", "requirement_id"};
    std::vector<std::string> values = {title, accessibleName, description, hidden, requirementId};
    std::vector<int> types = {DATA_TYPE_STRING, DATA_TYPE_STRING, DATA_TYPE_STRING, DATA_TYPE_BOOLEAN,
                              DATA_TYPE_NUMBER};
    int chapterId = novel->insert("chapters", columns, values, types);

    // Process each scene
    if (chapterJson.find("scenes") == chapterJson.end()) {
        std::vector<std::string> errorMessageVector = {
                "No 'scenes' object was found on this chapter. (",
                accessibleName,
                ")"
        };

        throw ProjectBuilderException(Utils::implodeString(errorMessageVector));
    }

    json scenes = chapterJson["scenes"];
    int numberOfScenes = 0;

    for (auto &scene : scenes.items()) {
        numberOfScenes++;
        processScene(scene.value(), chapterId);
    }
}

void ChapterBuilder::processScene(json sceneJson, int chapterId) {
    // TODO: Surely a lot more can be attached to this object, I need to look at the database structure and decide what at some point.
    // TODO: Once we're processing the resource database, throw an error if this image does not exist
    std::string backgroundImageName;
    std::string backgroundColourId;
    std::string startTransitionColourId;
    std::string endTransitionColourId;
    std::string startTransitionTypeId;
    std::string endTransitionTypeId;

    // Could use ternaries for this, but this is easier to follow I suppose.
    if (sceneJson.find("backgroundImageName") != sceneJson.end()) {
        backgroundImageName = JsonHandler::getString(sceneJson,"backgroundImageName");
    } else {
        backgroundImageName = "NULL";
    }

    if (sceneJson.find("backgroundColourId") != sceneJson.end()) {
        backgroundColourId = TypeCaster::cast(JsonHandler::getInteger(sceneJson, "backgroundColourId"));
    } else {
        backgroundColourId = "NULL";
    }

    if (sceneJson.find("startTransitionColourId") != sceneJson.end()) {
        startTransitionColourId = TypeCaster::cast(JsonHandler::getInteger(sceneJson, "startTransitionColourId"));
    } else {
        startTransitionColourId = "NULL";
    }

    if (sceneJson.find("endTransitionColourId") != sceneJson.end()) {
        endTransitionColourId = TypeCaster::cast(JsonHandler::getInteger(sceneJson, "endTransitionColourId"));
    } else {
        endTransitionColourId = "NULL";
    }

    if (sceneJson.find("startTransitionTypeId") != sceneJson.end()) {
        startTransitionTypeId = TypeCaster::cast(JsonHandler::getInteger(sceneJson, "startTransitionTypeId"));
    } else {
        startTransitionTypeId = "NULL";
    }

    if (sceneJson.find("endTransition") != sceneJson.end()) {
        std::string endTransitionType = JsonHandler::getString(sceneJson,"endTransition");

        std::vector<std::string> acceptedValues = {
                "none", "morph", "fade"
        };

        if (!Utils::isAcceptedValue(acceptedValues, endTransitionType, true)) {
            std::vector<std::string> error = {
                "Unknown endTransition type '", endTransitionType, "' \n",
                "Accepted values: 'fade', 'morph'"
            };
            throw ProjectBuilderException(Utils::implodeString(error));
        }

        endTransitionType = Utils::strToLower(endTransitionType);

        if (endTransitionType == "fade") {
            endTransitionTypeId = "2";
        } else if (endTransitionType == "morph") {
            endTransitionTypeId = "3";
        } else {
            endTransitionTypeId = "NULL";
        }
    } else {
        endTransitionTypeId = "NULL";
    }

    // Insert the data into the database...
    std::vector<std::string> columns = {
            "chapter_id",
            "background_image_name",
            "background_colour_id",
            "start_transition_colour_id",
            "end_transition_colour_id",
            "start_transition_type_id",
            "end_transition_type_id"
    };

    std::vector<std::string> values = {
            std::to_string(chapterId),
            backgroundImageName,
            backgroundColourId,
            startTransitionColourId,
            endTransitionColourId,
            startTransitionTypeId,
            endTransitionTypeId
    };

    std::vector<int> types = {
            DATA_TYPE_NUMBER,
            DATA_TYPE_STRING,
            DATA_TYPE_NUMBER,
            DATA_TYPE_NUMBER,
            DATA_TYPE_NUMBER,
            DATA_TYPE_NUMBER,
            DATA_TYPE_NUMBER
    };

    int sceneId = novel->insert("scenes", columns, values, types);

    // Process each scene segment
    json segments;

    if (sceneJson.find("segments") != sceneJson.end()) {
        segments = sceneJson["segments"];
    } else {
        std::vector<std::string> errorVector = {
                "No 'segments' attribute was found linked to scene",
                std::to_string(sceneId),
                "on this chapter."
        };
        throw ProjectBuilderException(Utils::implodeString(errorVector));
    }

    int numberOfSegments = 0;

    for (auto &segment : segments.items()) {
        numberOfSegments++;
        processSceneSegment(segment.value(), sceneId);
    }

    if (numberOfSegments == 0) {
        std::vector<std::string> errorVector = {
                "No segments were found linked to scene",
                std::to_string(sceneId),
                "on this chapter."
        };
        throw ProjectBuilderException(Utils::implodeString(errorVector));
    }

}

void ChapterBuilder::processSceneSegment(json sceneSegmentJson, int sceneId) {

    std::string musicPlaybackRequestId = "NULL";
    std::string visualEffectName;

    if (sceneSegmentJson.find("music") != sceneSegmentJson.end()) {

        // TODO: Deal with music effects/transformations
        json musicJson = sceneSegmentJson["music"];

        // Process a music playback request as well as its metadata
        {
            std::string backgroundMusicName = "NULL";

            if (musicJson.find("name") != musicJson.end()) {
                // TODO: Validate that the music actually exists in the resource database when it is working
                backgroundMusicName = JsonHandler::getString(musicJson,"name");
            }

            // We only want to bother adding the rest to the database if there's any music to actually play
            std::string musicPlaybackRequestMetadataId = "NULL";

            if (backgroundMusicName != "NULL") {
                // Create the metadata entry
                {
                    std::string pitch = "NULL";
                    std::string speed = "NULL";
                    std::string loop = "NULL";
                    std::string volume = "NULL";
                    std::string startTime = "NULL";
                    std::string endTime = "NULL";
                    std::string muted = "NULL";

                    if (musicJson.find("pitch") != musicJson.end()) {
                        pitch = TypeCaster::cast(JsonHandler::getDouble(musicJson, "pitch"));

                        // 0 causes problems and higher than 5 is speaker-busting. Higher than 5 can work, but it is appalling so I am stopping it here.
                        if (std::stof(pitch) < 0.1 || std::stof(pitch) > 5) {
                            std::vector<std::string> error = {
                                    "Value '", pitch , "' for pitch is out of range (Must be between 0.1 and 5)"
                            };

                            throw ProjectBuilderException(Utils::implodeString(error));
                        }

                    }

                    // TODO: Validate that these values are in range

                    if (musicJson.find("loop") != musicJson.end()) {
                        loop = TypeCaster::cast(JsonHandler::getBoolean(musicJson, "loop"));
                    }

                    if (musicJson.find("muted") != musicJson.end()) {
                        muted = TypeCaster::cast(JsonHandler::getBoolean(musicJson,"muted"));
                    }

                    if (musicJson.find("volume") != musicJson.end()) {
                        volume = TypeCaster::cast(JsonHandler::getInteger(musicJson,"volume"));

                        if (std::stoi(volume) < 0 || std::stoi(volume) > 100) {
                            std::vector<std::string> error = {
                                    "Value '", volume , "' for volume is out of range (Must be between 1 and 100) - To mute the audio, use the 'mute' attribute instead (it's a boolean)"
                            };

                            throw ProjectBuilderException(Utils::implodeString(error));
                        }
                    }

                    if (musicJson.find("startTime") != musicJson.end()) {
                        startTime = TypeCaster::cast(JsonHandler::getInteger(musicJson,"startTime"));
                        // TODO: Implement this
                        throw ProjectBuilderException("Music startTime is currently unimplemented");
                    }

                    if (musicJson.find("endTime") != musicJson.end()) {
                        endTime = TypeCaster::cast(JsonHandler::getInteger(musicJson,"endTime"));
                        // TODO: Implement this
                        throw ProjectBuilderException("Music endTime is currently unimplemented");
                    }

                    std::vector<std::string> values = {pitch, speed, loop, volume, startTime, endTime, muted};
                    std::vector<std::string> columns = {"pitch", "speed", "loop", "volume", "startTime", "endTime", "muted"};
                    std::vector<int> types = {DATA_TYPE_NUMBER, DATA_TYPE_NUMBER, DATA_TYPE_BOOLEAN, DATA_TYPE_NUMBER, DATA_TYPE_NUMBER, DATA_TYPE_NUMBER, DATA_TYPE_BOOLEAN};
                    int newId = novel->insert("music_playback_request_metadata", columns, values,
                                                                   types);

                    musicPlaybackRequestMetadataId = std::to_string(newId);
                }
            }

            // Create the background music request
            std::vector<std::string> columns = {"music_name", "music_playback_request_metadata_id"};
            std::vector<std::string> values = {backgroundMusicName, musicPlaybackRequestMetadataId};
            std::vector<int> types = {DATA_TYPE_STRING, DATA_TYPE_NUMBER};
            int newId = novel->insert("music_playback_requests", columns, values, types);
            musicPlaybackRequestId = std::to_string(newId);
        }

    }

    if (sceneSegmentJson.find("visualEffectName") != sceneSegmentJson.end()) {
        visualEffectName = JsonHandler::getString(sceneSegmentJson,"visualEffectName");
    } else {
        visualEffectName = "NULL";
    }

    std::vector<std::string> columns = {"scene_id", "music_playback_request_id", "visual_effect_name"};
    std::vector<std::string> values = {std::to_string(sceneId), musicPlaybackRequestId, visualEffectName};
    std::vector<int> types = {DATA_TYPE_NUMBER, DATA_TYPE_STRING, DATA_TYPE_STRING};

    int sceneSegmentId = novel->insert("scene_segments", columns, values, types);

    // Process each line
    json lines;

    if (sceneSegmentJson.find("lines") != sceneSegmentJson.end()) {
        lines = sceneSegmentJson["lines"];
    } else {
        std::vector<std::string> errorVector = {
                "No 'lines' attribute was found linked to scene segment",
                std::to_string(sceneSegmentId),
                "on this chapter."
        };
        throw ProjectBuilderException(Utils::implodeString(errorVector));
    }

    int numberOfLines = 0;

    for (auto &line : lines.items()) {
        numberOfLines++;
        processLine(line.value(), sceneSegmentId);
    }

    if (numberOfLines == 0) {
        std::vector<std::string> errorVector = {
                "No lines were found linked to scene segment",
                std::to_string(sceneId),
                "on this chapter. (Doing this will be supported in the future when I get around to it)"
        };
        throw ProjectBuilderException(Utils::implodeString(errorVector));
    }
}

void ChapterBuilder::processLine(json lineJson, int sceneSegmentId) {

    // TODO: Support multiple languages in one distribution, on second thought it might be better to do this at a chapter level.
    std::string languageId = "1";
    std::string overrideCharacterName = "NULL";
    std::string characterId = "NULL";
    std::string text = "NULL";
    std::string characterStateGroupId = "NULL";

    if (lineJson.find("characterName") != lineJson.end()) {
        // TODO: Validate that the character exists in the database
        overrideCharacterName = JsonHandler::getString(lineJson,"characterName");
    }

    if (lineJson.find("text") == lineJson.end()) {
        // TODO: Make this work - it could be used for chaining together character sprites
        std::cout << "Warning: a line in this chapter does not have any text associated with it." << std::endl;
        text = "";
    } else {
        text = JsonHandler::getString(lineJson,"text");

        // TODO: Figure out how large the text will be, and warn the user if it is too long based on length and font size.
    }

    if (lineJson.find("characterId") != lineJson.end()) {

        if (overrideCharacterName != "NULL") {
            std::cout
                    << "Warning: both characterId and characterName are present on the same line - characterName will override characterId."
                    << std::endl;
        } else {
            characterId = TypeCaster::cast(JsonHandler::getInteger(lineJson, "characterId"));
        }

    }

    if (lineJson.find("spoken") != lineJson.end()) {
        if (JsonHandler::getBoolean(lineJson, "spoken")) {
            text = Utils::implodeString(std::vector<std::string> {
               "\"",text,"\""
            });
        }
    }

    // See if any character state changes are on this line, add them to the database if they are
    if (lineJson.find("characterStates") != lineJson.end()) {
        // Create a character state group
        // TODO: If one which looks the same already exists, use it rather than creating a new one.
        characterStateGroupId = std::to_string(novel->insert("character_state_groups"));

        json characterStates = lineJson["characterStates"];

        for (auto &element : characterStates.items()) {
            json characterState = element.value();

            std::string characterId;
            std::string spriteName;

            std::string characterSpriteId;


            if (characterState.find("characterId") == characterState.end()) {

                if (characterState.find("characterFirstName") != characterState.end()) {

                    std::string characterFirstName = JsonHandler::getString(characterState,"characterFirstName");

                    auto *dataSet = new DataSet();

                    std::vector<std::string> query = {
                            "SELECT * FROM characters WHERE first_name = '",
                            characterFirstName,
                            "';"
                    };

                    novel->executeQuery(Utils::implodeString(query, ""), dataSet);

                    if (dataSet->getRowCount() == 0) {
                        std::vector<std::string> error = {
                                "Error adding character sprite to a line, no character with name: ",
                                characterFirstName,
                                " exists."
                        };

                        throw ProjectBuilderException(Utils::implodeString(error));
                    }

                    if (dataSet->getRowCount() > 1) {

                        std::vector<std::string> error = {
                                "Error adding character sprite to a line, multiple characters exist with name: ",
                                characterFirstName,
                                "exists."
                        };

                        throw ProjectBuilderException(Utils::implodeString(error));
                    }

                    characterId = dataSet->getRow(0)->getColumn("id")->getRawData();

                } else {
                    throw ProjectBuilderException(
                            "Each character state item must have a characterId or characterFirstName attribute");
                }

            } else {
                characterId = TypeCaster::cast(JsonHandler::getInteger(characterState,"characterId"));
            }

            if (characterState.find("spriteName") == characterState.end()) {
                throw ProjectBuilderException("Each character state item must have a spriteName");
            }

            spriteName = JsonHandler::getString(characterState,"spriteName");

            // TODO: Validate that a character exists with this ID (if it wasn't selected by name)

            auto *dataSet = new DataSet();

            std::vector<std::string> query = {
                    "SELECT * FROM character_sprites WHERE character_id = ",
                    characterId,
                    " AND name = '",
                    spriteName,
                    "';"
            };

            novel->executeQuery(Utils::implodeString(query, ""), dataSet);

            if (dataSet->getRowCount() == 0) {
                std::vector<std::string> error = {
                        "Could not link character sprite to line: no character sprite linked to character",
                        characterId,
                        "with sprite name",
                        spriteName,
                        "was found."
                };

                throw ProjectBuilderException(Utils::implodeString(error));
            }

            characterSpriteId = dataSet->getRow(0)->getColumn("id")->getRawData();

            std::vector<std::string> columns = {"character_sprite_id", "character_state_group_id"};
            std::vector<std::string> values = {characterSpriteId, characterStateGroupId};
            std::vector<int> types = {DATA_TYPE_NUMBER, DATA_TYPE_NUMBER};
            novel->insert("character_states", columns, values, types);
        }
    }

    std::vector<std::string> columns = {"scene_segment_id", "language_id", "character_id", "override_character_name",
                                        "text", "character_state_group_id"};
    std::vector<std::string> values = {std::to_string(sceneSegmentId), languageId, characterId, overrideCharacterName,
                                       text, characterStateGroupId};
    std::vector<int> types = {DATA_TYPE_NUMBER, DATA_TYPE_NUMBER, DATA_TYPE_NUMBER, DATA_TYPE_STRING, DATA_TYPE_STRING,
                              DATA_TYPE_STRING};
    novel->insert("segment_lines", columns, values, types);

}
