/*
  This class handles the parsing of a chapter .json file, and will create the appropriate records in the SQLite database for it
  This class will also handle sanity checks for errors which could make it into the game's database.
 */

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "Database/DatabaseConnection.hpp"
#include "GameCompiler/ChapterBuilder.hpp"
#include "Misc/Utils.hpp"
#include <vector>
#include <regex>
#include <fstream>

ChapterBuilder::ChapterBuilder(std::string fileName, DatabaseConnection *novelDb) {

  if (!Utils::fileExists(fileName)) {

    std::vector<std::string> error = {
      "Unable to process chapter, could not find file:",
      fileName
    };

    throw Utils::implodeString(error, std::string(" "), 0);
  }

  chapterFileName = fileName;
  novel = novelDb;

}

ChapterBuilder::~ChapterBuilder() {

}

void ChapterBuilder::process() {

  std::ifstream stream(chapterFileName);
  json chapterJson = json::parse(stream);

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
    title = chapterJson["title"];
  }

  std::cout<<"Processing chapter "<<title<<"..."<<std::endl;

  if (chapterJson.find("accessibleName") == chapterJson.end()) {
    accessibleName = title;
  } else {
    accessibleName = chapterJson["accessibleName"];
  }

  if (chapterJson.find("requirementId") == chapterJson.end()) {
    requirementId = "0";
  } else {
    requirementId = chapterJson["requirementId"];
  }

  if (chapterJson.find("hidden") == chapterJson.end()) {
    hidden = "FALSE";
  } else {
    hidden = chapterJson["hidden"];
  }

  if (chapterJson.find("description") == chapterJson.end()) {
    description = "";
  } else {
    description = chapterJson["description"];
  }

  std::vector<std::string> columns = {"title", "accessible_name", "description", "hidden", "requirement_id"};
  std::vector<std::string> values = {title, accessibleName, description, hidden, requirementId};
  std::vector<int> types = {DATA_TYPE_STRING, DATA_TYPE_STRING, DATA_TYPE_STRING, DATA_TYPE_BOOLEAN, DATA_TYPE_NUMBER};
  int chapterId = novel->insert("chapters", columns, values, types);

  // Process each scene
  if (chapterJson.find("scenes") == chapterJson.end()) {
    std::vector<std::string> errorMessageVector = {
      "No 'scenes' object was found on this chapter. (",
      accessibleName,
      ")"
    };

    throw Utils::implodeString(errorMessageVector,"", 0);
  }

  json scenes = chapterJson["scenes"];
  int numberOfScenes = 0;

  for (auto& scene : scenes.items()) {
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
    backgroundImageName = sceneJson["backgroundImageName"];
  } else {
    backgroundImageName = "nullptr";
  }

  if (sceneJson.find("backgroundColourId") != sceneJson.end()) {
    backgroundColourId = sceneJson["backgroundColourId"];
  } else {
    backgroundColourId = "nullptr";
  }

  if (sceneJson.find("startTransitionColourId") != sceneJson.end()) {
    startTransitionColourId = sceneJson["startTransitionColourId"];
  } else {
    startTransitionColourId = "nullptr";
  }

  if (sceneJson.find("endTransitionColourId") != sceneJson.end()) {
    endTransitionColourId = sceneJson["endTransitionColourId"];
  } else {
    endTransitionColourId = "nullptr";
  }

  if (sceneJson.find("startTransitionTypeId") != sceneJson.end()) {
    startTransitionTypeId = sceneJson["startTransitionTypeId"];
  } else {
    startTransitionTypeId = "nullptr";
  }

  if (sceneJson.find("endTransitionTypeId") != sceneJson.end()) {
    endTransitionTypeId = sceneJson["endTransitionTypeId"];
  } else {
    endTransitionTypeId = "nullptr";
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
    throw Utils::implodeString(errorVector," ", 0);
  }

  int numberOfSegments = 0;

  for (auto& segment : segments.items()) {
    numberOfSegments++;
    processSceneSegment(segment.value(), sceneId);
  }

  if (numberOfSegments == 0) {
    std::vector<std::string> errorVector = {
      "No segments were found linked to scene",
      std::to_string(sceneId),
      "on this chapter."
    };
    throw Utils::implodeString(errorVector," ", 0);
  }

}

void ChapterBuilder::processSceneSegment(json sceneSegmentJson, int sceneId) {

  std::string backgroundMusicName;
  std::string visualEffectName;

  if (sceneSegmentJson.find("music") != sceneSegmentJson.end()) {

    // TODO: Deal with music effects/transformations
    json musicJson = sceneSegmentJson["music"];

    backgroundMusicName = "nullptr";

    if (musicJson.find("name") != musicJson.end()) {
      // TODO: Validate that the music actually exists im the resource database when it is working
      backgroundMusicName = musicJson["name"];
    }
  }

  if (sceneSegmentJson.find("visualEffectName") != sceneSegmentJson.end()) {
    visualEffectName = sceneSegmentJson["visualEffectName"];
  } else {
    visualEffectName = "nullptr";
  }

  std::vector<std::string> columns = {"scene_id","background_music_name","visual_effect_name"};
  std::vector<std::string> values = {std::to_string(sceneId), backgroundMusicName, visualEffectName};
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
    throw Utils::implodeString(errorVector," ", 0);
  }

  int numberOfLines = 0;

  for (auto& line : lines.items()) {
    numberOfLines++;
    processLine(line.value(), sceneSegmentId);
  }

  if (numberOfLines == 0) {
    std::vector<std::string> errorVector = {
      "No lines were found linked to scene segment",
      std::to_string(sceneId),
      "on this chapter. (Doing this will be supported in the future when I get around to it)"
    };
    throw Utils::implodeString(errorVector," ", 0);
  }
}

void ChapterBuilder::processLine(json lineJson, int sceneSegmentId) {

  // TODO: Support multiple languages in one distribution, on second thought it might be better to do this at a chapter level.
  std::string languageId = "1";
  std::string overrideCharacterName = "nullptr";
  std::string characterId = "nullptr";
  std::string text = "nullptr";
  std::string characterStateGroupId = "nullptr";

  if (lineJson.find("characterName") != lineJson.end()) {
    // TODO: Validate that the character exists in the database
    overrideCharacterName = lineJson["characterName"];
  }

  if (lineJson.find("text") == lineJson.end()) {
    std::cout<<"Warning: a line in this chapter does not have any text associated with it."<<std::endl;
    text = "";
  } else {
    text = lineJson["text"];

    // TODO: Figure out how large the text will be, and warn the user if it is too long based on length and font size.
  }

  if (lineJson.find("characterId") != lineJson.end()) {

    if (overrideCharacterName != "nullptr") {
      std::cout<<"Warning: both characterId and characterName are present on the same line - characterName will override characterId."<<std::endl;
    } else {
      characterId = lineJson["characterId"];
    }

  }

  if (lineJson.find("spoken") != lineJson.end()) {
      if (lineJson["spoken"] == "TRUE" || lineJson["spoken"] == "true") {
        // There is probably a better way of doing this...
        std::string tempText = "\"";
        tempText.append(text);
        tempText.append("\"");
        text = tempText;
      }
  }

  // See if any character state changes are on this line, add them to the database if they are
  if (lineJson.find("characterStates") != lineJson.end()) {
    // Create a character state group
    // TODO: If one which looks the same already exists, use it rather than creating a new one.
    characterStateGroupId = std::to_string(novel->insert("character_state_groups"));

    json characterStates = lineJson["characterStates"];

    for (auto& element : characterStates.items()) {
      json characterState = element.value();

      std::string characterId = "";
      std::string spriteName;

      std::string characterSpriteId;


      if (characterState.find("characterId") == characterState.end()) {

        if (characterState.find("characterFirstName") != characterState.end()) {

          std::string characterFirstName = characterState["characterFirstName"];

          DataSet *dataSet = new DataSet();

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

            throw Utils::implodeString(error, "");
          }

          if (dataSet->getRowCount() > 1) {

            std::vector<std::string> error = {
              "Error adding character sprite to a line, multiple characters exist with name: ",
              characterFirstName,
              "exists."
            };

            throw Utils::implodeString(error, "");
          }

          characterId = dataSet->getRow(0)->getColumn("id")->getData();

        } else {
          throw "Each character state item must have a characterId or characterFirstName attribute";
        }

      } else {
        characterId = characterState["characterId"];
      }

      if (characterState.find("spriteName") == characterState.end()) {
        throw "Each character state item must have a spriteName";
      }

      spriteName = characterState["spriteName"];

      // TODO: Validate that a character exists with this ID (if it wasn't selected by name)

      DataSet *dataSet = new DataSet();

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

        throw Utils::implodeString(error, " ");
      }

      characterSpriteId = dataSet->getRow(0)->getColumn("id")->getData();

      std::vector<std::string> columns = {"character_sprite_id", "character_state_group_id"};
      std::vector<std::string> values = {characterSpriteId, characterStateGroupId};
      std::vector<int> types = {DATA_TYPE_NUMBER, DATA_TYPE_NUMBER};
      novel->insert("character_states", columns, values, types);
    }
  }

  std::vector<std::string> columns = {"scene_segment_id", "language_id", "character_id", "override_character_name", "text", "character_state_group_id"};
  std::vector<std::string> values = {std::to_string(sceneSegmentId), languageId, characterId, overrideCharacterName, text, characterStateGroupId};
  std::vector<int> types = {DATA_TYPE_NUMBER, DATA_TYPE_NUMBER, DATA_TYPE_NUMBER, DATA_TYPE_STRING, DATA_TYPE_STRING, DATA_TYPE_STRING};
  novel->insert("segment_lines", columns, values, types);

}
