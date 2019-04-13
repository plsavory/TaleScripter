#include <iostream>
#include "Misc/Utils.hpp"
#include "Database/DatabaseConnection.hpp"
#include "VisualNovelEngine/Classes/Data/Novel.hpp"
#include <sstream>
#include <Exceptions/ResourceException.hpp>

NovelData::NovelData() {

    for (int i = 0; i < MAX_CHARACTERS; i++) {
        character[i] = nullptr;
    }

    chapterCount = 0;
    loadFromDatabase();
    start();
}

/**
 * [NovelData::start Start the novel from the start of the game]
 */
void NovelData::start() {
    start(0, -1, -1, -1);
}

/**
 * [NovelData::start Start the novel from the given position]
 */
void NovelData::start(int cChapter, int cScene, int cSceneSegment, int cSceneSegmentLine) {

    currentChapter = cChapter;
    currentScene = cScene;
    currentSceneSegment = cSceneSegment;
    currentSceneSegmentLine = cSceneSegmentLine; // Game hasn't started yet, first line has id of 0

}

/**
 * [getNextAction Returns the type of action that should be taken next]
 * @return [description]
 */
AdvanceState NovelData::getNextAction() {

    if (currentSceneSegmentLine == getCurrentSceneSegment()->getLineCount() - 1) {

        if (currentSceneSegment == getCurrentScene()->getSegmentCount() - 1) {

            if (currentScene == getCurrentChapter()->getSceneCount() - 1) {
                return AdvanceState::ChapterEnd;
            }

            return AdvanceState::SceneEnd;
        }

        return AdvanceState::SceneSegmentEnd;
    }

    return AdvanceState::NextLine;
}

NovelSceneSegment *NovelData::getCurrentSceneSegment() {
    return chapter[currentChapter]->getScene(currentScene)->getSceneSegment(currentSceneSegment);
}

NovelScene *NovelData::getCurrentScene() {
    return chapter[currentChapter]->getScene(currentScene);
}

NovelChapter *NovelData::getCurrentChapter() {
    return chapter[currentChapter];
}

void NovelData::loadFromDatabase() {

    if (!Utils::fileExists("db/novel")) {
        throw GeneralException("Error: Unable to find novel database file");
    }

    // Empty the character array
    for (int i = 0; i < MAX_CHARACTERS; i++) {
        if (character[i]) {
            delete (character[i]);
            character[i] = nullptr;
        }
    }

    novelDb = new DatabaseConnection("novel");

    // Load all of the characters
    DataSet *characterData = new DataSet();

    novelDb->executeQuery("SELECT * FROM characters;", characterData);

    for (int i = 0; i < characterData->getRowCount(); i++) {

        if (i >= MAX_CHARACTERS) {
            break;
        }

        {
            int id = 0;
            std::string firstName("");
            std::string surname("");
            std::string bio("");
            std::string age("");
            bool showOnCharacterMenu = false;

            if (characterData->getRow(i)->doesColumnExist("id")) {
                id = characterData->getRow(i)->getColumn("id")->getData()->asInteger();
            }

            if (characterData->getRow(i)->doesColumnExist("first_name")) {
                firstName = characterData->getRow(i)->getColumn("first_name")->getRawData();
            }

            if (characterData->getRow(i)->doesColumnExist("surname")) {
                surname = characterData->getRow(i)->getColumn("surname")->getRawData();
            }

            if (characterData->getRow(i)->doesColumnExist("bio")) {
                bio = characterData->getRow(i)->getColumn("bio")->getRawData();
            }

            if (characterData->getRow(i)->doesColumnExist("age")) {
                age = characterData->getRow(i)->getColumn("age")->getRawData();
            }

            if (characterData->getRow(i)->doesColumnExist("showOnCharacterMenu")) {
                std::string comparison = characterData->getRow(i)->getColumn("showOnCharacterMenu")->getRawData();
                showOnCharacterMenu = (comparison == "TRUE" || comparison == "true");
            }

            character[i] = new Character(id, firstName, surname, bio, age, showOnCharacterMenu, novelDb);
        }
    }

    for (int i = 0; i < MAX_CHAPTERS; i++) {
        chapter[i] = nullptr;
    }

    // Load project information from Database
    projectInformation = new ProjectInformation(novelDb);

    DataSet *chapterData = new DataSet();

    novelDb->executeQuery("SELECT * FROM chapters", chapterData);

    for (int i = 0; i < chapterData->getRowCount(); i++) {

        if (!chapterData->getRow(i)->doesColumnExist("id")) {
            continue;
        }

        if (!chapterData->getRow(i)->doesColumnExist("title")) {
            continue;
        }

        chapter[i] = new NovelChapter(novelDb,
                                      chapterData->getRow(i)->getColumn("title")->getData()->getRawData(),
                                      chapterData->getRow(i)->getColumn("id")->getData()->asInteger(),
                                      character);

        chapterCount++;
    }

    // Cleanup
    delete (characterData);
    delete (chapterData);
}

NovelData::~NovelData() {
    delete novelDb;

    for (int i = 0; i < MAX_CHAPTERS; i++) {
        if (chapter[i]) {
            delete (chapter[i]);
        }
    }
}

NovelSceneSegmentLine *NovelData::getNextLine() {
    return getCurrentSceneSegment()->getLine(++currentSceneSegmentLine);
}

NovelSceneSegment *NovelData::advanceToNextSegment() {
    currentSceneSegmentLine = -1; // Reset which line we're on

    if (getCurrentScene()->getSegmentCount() == 0) {
        // TODO: Allow scenes with no segments as a background image transition between multiple places
        std::cout << "Error: scene " << getCurrentScene()->getId() << " has no scene segments" << std::endl;
    }
    return getCurrentScene()->getSceneSegment(++currentSceneSegment);
}

NovelScene *NovelData::advanceToNextScene() {
    currentSceneSegment = -1;
    currentScene++;
    return getCurrentScene();
}

ProjectInformation *NovelData::getProjectInformation() {
    return projectInformation;
}

Character *NovelData::getCharacter(int id) {
    return character[id];
}

// Chapter-specific stuff
NovelChapter::NovelChapter(DatabaseConnection *db, std::string chapterTitle, int chapterId, Character *character[]) {
    title = chapterTitle;
    id = chapterId;
    sceneCount = 0;

#ifdef DEBUG_NOVEL_DATA
    std::cout<<"Adding chapter "<<id<<" '"<<title<<"'"<<std::endl;
#endif

    for (int i = 0; i < MAX_SCENES; i++) {
        scene[i] = nullptr;
    }

    // Get all of the scenes within the chapter
    auto *sceneData = new DataSet();

    std::ostringstream ss;

    ss << "SELECT * FROM scenes WHERE chapter_id = " << id << ";";

    db->executeQuery(ss.str(), sceneData);

    for (int i = 0; i < sceneData->getRowCount(); i++) {

        if (!sceneData->getRow(i)->doesColumnExist("id")) {
            continue;
        }

        std::string backgroundImageName("");

        if (sceneData->getRow(i)->doesColumnExist("background_image_name")) {
            backgroundImageName = sceneData->getRow(i)->getColumn("background_image_name")->getRawData();
        }

        int backgroundColourId = 0;
        int startTransitionColourId = 0;
        int endTransitionColourId = 0;

        if (sceneData->getRow(i)->doesColumnExist("background_colour_id")) {
            backgroundColourId = sceneData->getRow(i)->getColumn("background_colour_id")->getData()->asInteger();
        }

        if (sceneData->getRow(i)->doesColumnExist("start_transition_colour_id")) {
            startTransitionColourId = sceneData->getRow(i)->getColumn("start_transition_colour_id")->getData()->asInteger();
        }

        if (sceneData->getRow(i)->doesColumnExist("end_transition_colour_id")) {
            endTransitionColourId = sceneData->getRow(i)->getColumn("end_transition_colour_id")->getData()->asInteger();
        }

        scene[i] = new NovelScene(db,
                                  sceneData->getRow(i)->getColumn("id")->getData()->asInteger(),
                                  backgroundImageName,
                                  backgroundColourId,
                                  startTransitionColourId,
                                  endTransitionColourId,
                                  character);
        sceneCount++;
    }

    delete sceneData;
}

NovelChapter::~NovelChapter() {
    for (int i = 0; i < MAX_SCENES; i++) {
        if (scene[i]) {
            delete scene[i];
        }
    }
}

int NovelChapter::getId() {
    return id;
}

std::string NovelChapter::getTitle() {
    return title;
}

NovelScene *NovelChapter::getScene(int id) {
    return scene[id];
}

int NovelChapter::getSceneCount() {
    return sceneCount;
}

// Scene-specific stuff
NovelScene::NovelScene(DatabaseConnection *db, int sId, std::string bgImage, int bgColourId, int strColourId,
                       int etrColourId, Character *character[]) {
    id = sId;
    backgroundImage = bgImage;
    backgroundColourId = bgColourId;
    startTransitionColourId = strColourId;
    endTransitionColourId = etrColourId;

    segmentCount = 0;

    for (int i = 0; i < MAX_SEGMENTS; i++) {
        segment[i] = nullptr;
    }

#ifdef DEBUG_NOVEL_DATA
    std::cout<<"Adding scene "<<id<<std::endl;
#endif

    DataSet *sceneSegmentData = new DataSet();

    std::ostringstream ss;

    ss << "SELECT * FROM scene_segments WHERE scene_id = " << id << ";";

    db->executeQuery(ss.str(), sceneSegmentData);

    for (int i = 0; i < sceneSegmentData->getRowCount(); i++) {

        if (!sceneSegmentData->getRow(i)->doesColumnExist("id")) {
            continue;
        }

        std::string visualEffectName = sceneSegmentData->getRow(i)->doesColumnExist("visual_effect_name")
                                       ? sceneSegmentData->getRow(i)->getColumn("visual_effect_name")->getRawData() : "";

        segment[i] = new NovelSceneSegment(db,
                                           sceneSegmentData->getRow(i)->getColumn("id")->getData()->asInteger(),
                                           visualEffectName,
                                           character,
                                           sceneSegmentData->getRow(i)->getColumn(
                                                   "music_playback_request_id")->getData()->asInteger());

        segmentCount++;
    }

    delete sceneSegmentData;

}

NovelScene::~NovelScene() {
    for (int i = 0; i < MAX_SEGMENTS; i++) {
        if (segment[i]) {
            delete segment[i];
        }
    }
}

NovelSceneSegment *NovelScene::getSceneSegment(int id) {
    return segment[id];
}

int NovelScene::getSegmentCount() {
    return segmentCount;
}

int NovelScene::getId() {
    return id;
}

std::string NovelScene::getBackgroundImageName() {
    return backgroundImage;
}

int NovelScene::getStartTransitionColourId() {

    // Check for null value
    if (!startTransitionColourId) {
        return 0;
    }

    return startTransitionColourId;
}

int NovelScene::getEndTransitionColourId() {

    // Check for null value
    if (!endTransitionColourId) {
        return 0;
    }

    return endTransitionColourId;
}

// Segment-specific stuff
NovelSceneSegment::NovelSceneSegment(DatabaseConnection *db, int ssId,
                                     std::string ssVisualEffectName, Character *character[], int musicPlaybackRequestId) {
    id = ssId;
    visualEffectName = ssVisualEffectName;
    lineCount = 0;
    musicPlaybackRequest = nullptr;

    for (auto & currentLine : line) {
        currentLine = nullptr;
    }

#ifdef DEBUG_NOVEL_DATA
    std::cout<<"Added scene segment "<<id<<std::endl;
#endif

    // Get the music playback request if there is one
    auto *musicPlaybackRequestData = new DataSet();

    if (musicPlaybackRequestId) {
        std::vector<std::string> query = {
                "SELECT * FROM music_playback_requests WHERE id = ",
                std::to_string(musicPlaybackRequestId), ";"
        };

        db->executeQuery(Utils::implodeString(query), musicPlaybackRequestData);
    }

    if (musicPlaybackRequestData->getRowCount() == 1) {

        // We have a music playback request, so let's add it
        int playbackRequestId = musicPlaybackRequestData->getRow(0)->getColumn("id")->getData()->asInteger();
        std::string musicName = musicPlaybackRequestData->getRow(0)->getColumn("music_name")->getRawData();
        int musicPlaybackRequestMetadataId = musicPlaybackRequestData->getRow(0)->getColumn("music_playback_request_metadata_id")->getData()->asInteger();
        musicPlaybackRequest = new MusicPlaybackRequest(db, playbackRequestId, musicName, musicPlaybackRequestMetadataId);
    }

    delete(musicPlaybackRequestData);

    // Get all of the lines for this scene segment
    DataSet *lineData = new DataSet();

    std::ostringstream ss;

    ss << "SELECT * FROM segment_lines WHERE scene_segment_id = " << id << ";";

    db->executeQuery(ss.str(), lineData);

    for (int i = 0; i < lineData->getRowCount(); i++) {

        int characterStateGroupId = 0;

        if (!lineData->getRow(i)->doesColumnExist("id")) {
            continue;
        }

        if (!lineData->getRow(i)->doesColumnExist("text")) {
            continue;
        }

        if (lineData->getRow(i)->doesColumnExist("character_state_group_id")) {
            characterStateGroupId = lineData->getRow(i)->getColumn("character_state_group_id")->getData()->asInteger();
        }

        int characterId = lineData->getRow(i)->getColumn("character_id")->getData()->asInteger();
        std::string overrideCharacterName = "";

        if (lineData->getRow(i)->doesColumnExist("override_character_name")) {
            overrideCharacterName = lineData->getRow(i)->getColumn("override_character_name")->getRawData();
        }

        line[i] = new NovelSceneSegmentLine(db,
                                            lineData->getRow(i)->getColumn("id")->getData()->asInteger(),
                                            characterId,
                                            lineData->getRow(i)->getColumn("text")->getRawData(),
                                            characterStateGroupId,
                                            overrideCharacterName,
                                            character
        );

        lineCount++;
    }

    delete lineData;
}

NovelSceneSegment::~NovelSceneSegment() {
    for (int i = 0; i < MAX_LINES; i++) {
        if (line[i]) {
            delete line[i];
        }
    }
}

int NovelSceneSegment::getLineCount() {
    return lineCount;
}

NovelSceneSegmentLine *NovelSceneSegment::getLine(int id) {
    return line[id];
}

std::string NovelSceneSegment::getBackgroundMusicName() {
    return backgroundMusicName;
}

// Line-specific stuff
NovelSceneSegmentLine::NovelSceneSegmentLine(DatabaseConnection *db, int sslId, int sslCharacterId, std::string sslText,
                                             int sslCharacterStateGroupId, std::string sslOverrideCharacterName,
                                             Character *character[]) {
    id = sslId;
    characterId = sslCharacterId;
    overrideCharacterName = sslOverrideCharacterName;

    text = sslText;
    characterStateGroup = nullptr;

#ifdef DEBUG_NOVEL_DATA
    std::cout<<"Added line \""<<text<<"\""<<std::endl;
#endif

    // Find if we have a character sprite group attached to this line
    if (sslCharacterStateGroupId == 0) {
        return;
    }

    std::vector<std::string> characterStateGroupQuery = {
            "SELECT * FROM character_state_groups WHERE id = ",
            std::to_string(sslCharacterStateGroupId),
            ";"
    };

    DataSet *dataSet = new DataSet();

    db->executeQuery(Utils::implodeString(characterStateGroupQuery), dataSet);

    if (dataSet->getRowCount() > 0) {
        characterStateGroup = new CharacterStateGroup(dataSet->getRow(0)->getColumn("id")->getData()->asInteger(), db,
                                                      character);
    }

}

NovelSceneSegmentLine::~NovelSceneSegmentLine() {

}

std::string NovelSceneSegmentLine::getText() {
    return text;
}

int NovelSceneSegmentLine::getCharacterId() {
    if (characterId) {
        return characterId;
    } else {
        return 0;
    }
}

std::string NovelSceneSegmentLine::getOverrideCharacterName() {
    return overrideCharacterName;
}

CharacterStateGroup *NovelSceneSegmentLine::getCharacterStateGroup() {
    return characterStateGroup;
}

// ProjectInformation specific things
ProjectInformation::ProjectInformation(DatabaseConnection *db) {

    // Initial values
    gameTitle = "Unnamed TaleScripter Project";
    authorName = "Unknown";
    versionNumber = "Unknown";

    // Gets information about the project and stores it
    DataSet *projectInformationDataSet = new DataSet();

    db->executeQuery("SELECT * FROM game_information LIMIT 1;", projectInformationDataSet);

    if (projectInformationDataSet->getRowCount() == 0) {
        return;
    }

    if (!projectInformationDataSet->getRow(0)) {
        return;
    }

    if (projectInformationDataSet->getRow(0)->doesColumnExist("game_title")) {
        gameTitle = projectInformationDataSet->getRow(0)->getColumn("game_title")->getRawData();
    }

    if (projectInformationDataSet->getRow(0)->doesColumnExist("author_name")) {
        authorName = projectInformationDataSet->getRow(0)->getColumn("author_name")->getRawData();
    }

    if (projectInformationDataSet->getRow(0)->doesColumnExist("version_number")) {
        versionNumber = projectInformationDataSet->getRow(0)->getColumn("version_number")->getRawData();
    }

    delete projectInformationDataSet;

}

// Character sprite group stuff
CharacterStateGroup::CharacterStateGroup(int myId, DatabaseConnection *db, Character *character[]) {
    id = myId;

    DataSet *dataSet = new DataSet();

    std::vector<std::string> query = {
            "SELECT * FROM character_states WHERE character_state_group_id = ",
            std::to_string(id),
            ";"
    };

    db->executeQuery(Utils::implodeString(query), dataSet);

    int numberOfStates = dataSet->getRowCount();

    if (numberOfStates > 0) {

        for (int i = 0; i < numberOfStates; i++) {
            characterState.push_back(
                    new CharacterState(dataSet->getRow(i)->getColumn("id")->getData()->asInteger(), db, character));
        }

    }

    delete (dataSet);
}

CharacterStateGroup::~CharacterStateGroup() {

}

std::vector<CharacterState *> CharacterStateGroup::getCharacterStates() {
    return characterState;
}

// Character state stuff
CharacterState::CharacterState(int myId, DatabaseConnection *db, Character *character[]) {
    id = myId;

    std::vector<std::string> query = {
            "SELECT * FROM character_states WHERE id = ",
            std::to_string(id),
            ";"
    };

    DataSet *dataSet = new DataSet();

    db->executeQuery(Utils::implodeString(query), dataSet);

    std::string characterSpriteId = dataSet->getRow(0)->getColumn("character_sprite_id")->getRawData();

    if (dataSet->getRowCount() > 0) {

        std::vector<std::string> query = {
                "SELECT * FROM character_sprites WHERE id = ",
                characterSpriteId,
                ";"
        };

        auto *dataSet = new DataSet();

        db->executeQuery(Utils::implodeString(query), dataSet);

        if (dataSet->getRowCount() == 0) {
            std::vector<std::string> error = {
                    "Unable to find a character sprite with id ",
                    characterSpriteId
            };

            throw ResourceException(Utils::implodeString(error));
        }

        int characterId = dataSet->getRow(0)->getColumn("character_id")->getData()->asInteger();
        std::string characterSpriteName = dataSet->getRow(0)->getColumn("name")->getRawData();

        characterSprite = character[characterId - 1]->getSprite(characterSpriteName);

        if (!characterSprite) {
            std::vector<std::string> error = {
                    "Could not find sprite for character ",
                    std::to_string(characterId),
                    " with name: ",
                    characterSpriteName
            };

            throw ResourceException(Utils::implodeString(error));
        }

        delete (dataSet);

    } else {
        std::vector<std::string> error = {
                "No character state with id ",
                std::to_string(id),
                " could be found."
        };

        throw DataSetException(Utils::implodeString(error));
    }

    delete (dataSet);
}

CharacterState::~CharacterState() {

}

// Music playback request stuff
MusicPlaybackRequest::MusicPlaybackRequest(DatabaseConnection *db, int myId, std::string myMusicName, int musicPlaybackRequestMetadataId) {
    id = myId;
    musicName = myMusicName;
    metadata = nullptr;

    auto *musicPlaybackRequestMetadataSet = new DataSet();

    std::vector<std::string> query = {
            "SELECT * FROM music_playback_request_metadata WHERE id = ",
            std::to_string(musicPlaybackRequestMetadataId), ";"
    };

    db->executeQuery(Utils::implodeString(query), musicPlaybackRequestMetadataSet);

    if (musicPlaybackRequestMetadataSet->getRowCount() == 1) {
        metadata = new MusicPlaybackRequestMetadata(musicPlaybackRequestMetadataSet);
    }

    delete(musicPlaybackRequestMetadataSet);


}

/**
 * Saves all of the data required for the metadata object
 * @param data
 */
MusicPlaybackRequestMetadata::MusicPlaybackRequestMetadata(DataSet *data) {
    // I have taken a different approach this time by simply passing the dataset into here
    // Might be nicer to do this everywhere

    // TODO: Throw errors when values are out of range
    id = data->getRow(0)->getColumn("id")->getData()->asInteger();
    pitch = data->getRow(0)->getColumn("pitch")->getData()->asFloat();
    volume = data->getRow(0)->getColumn("volume")->getData()->asInteger();
    loop = data->getRow(0)->getColumn("loop")->getData()->asBoolean();
    startInMilliseconds = data->getRow(0)->getColumn("startTime")->getData()->asInteger();
    endInMilliseconds = data->getRow(0)->getColumn("endTime")->getData()->asInteger();
    bool mute = data->getRow(0)->getColumn("muted")->getData()->asBoolean();

    // 1-10 are valid values, 5 is normal speed.
    if (pitch == 0) {
        pitch = 1;
    }

    if (volume > 100) {
        volume = 100;
    }

    if (volume <= 0) {
        volume = 100;
    }

    // Why anybody would want to do this though, I don't know...
    if (mute) {
        volume = 0;
    }

    // TODO: Throw error if end time is out of range
    if (endInMilliseconds < 0) {
        throw ResourceException("startInMilliseconds must be greater than 0");
    }

}