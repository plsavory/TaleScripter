#include <iostream>
#include "Misc/Utils.hpp"
#include "Database/DatabaseConnection.hpp"
#include "VisualNovelEngine/Classes/Data/Novel.hpp"
#include <sstream>

NovelData::NovelData() {

  for (int i = 0; i<MAX_CHARACTERS; i++) {
      character[i] = NULL;
  }

  chapterCount = 0;
  loadFromDatabase();
  start();
}

/**
 * [NovelData::start Start the novel from the start of the game]
 */
void NovelData::start() {
  start(0,-1,-1,-1);
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

  if (currentSceneSegmentLine == getCurrentSceneSegment()->getLineCount()-1) {

    if (currentSceneSegment == getCurrentScene()->getSegmentCount()-1) {

      if (currentScene == getCurrentChapter()->getSceneCount()-1) {
        return AdvanceState::ChapterEnd;
      }

      return AdvanceState::SceneEnd;
    }

    return AdvanceState::SceneSegmentEnd;
  }

  return AdvanceState::NextLine;
}

NovelSceneSegment* NovelData::getCurrentSceneSegment() {
  return chapter[currentChapter]->getScene(currentScene)->getSceneSegment(currentSceneSegment);
}

NovelScene* NovelData::getCurrentScene() {
    return chapter[currentChapter]->getScene(currentScene);
}

NovelChapter* NovelData::getCurrentChapter() {
  return chapter[currentChapter];
}

void NovelData::loadFromDatabase() {

  if (!Utils::fileExists("db/novel")) {
    std::cout<<"Error: Unable to find novel database file"<<std::endl;
    exit(0);
  }

  // Empty the character array
  for (int i = 0; i<MAX_CHARACTERS; i++) {
    if (character[i]) {
      delete(character[i]);
      character[i] = NULL;
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
       id = std::stoi(characterData->getRow(i)->getColumn("id")->getData());
      }

      if (characterData->getRow(i)->doesColumnExist("first_name")) {
        firstName = characterData->getRow(i)->getColumn("first_name")->getData();
      }

      if (characterData->getRow(i)->doesColumnExist("surname")) {
        surname = characterData->getRow(i)->getColumn("surname")->getData();
      }

      if (characterData->getRow(i)->doesColumnExist("bio")) {
        bio = characterData->getRow(i)->getColumn("bio")->getData();
      }

      if (characterData->getRow(i)->doesColumnExist("age")) {
        age = characterData->getRow(i)->getColumn("age")->getData();
      }

      if (characterData->getRow(i)->doesColumnExist("showOnCharacterMenu")) {
        std::string comparison = characterData->getRow(i)->getColumn("showOnCharacterMenu")->getData();
        showOnCharacterMenu = (comparison == "TRUE" || comparison == "true");
      }

      character[i] = new Character(id,firstName,surname,bio,age, showOnCharacterMenu, novelDb);
    }
  }

  for (int i = 0; i < MAX_CHAPTERS; i++) {
    chapter[i] = NULL;
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
      chapterData->getRow(i)->getColumn("title")->getData(),
      std::stoi(chapterData->getRow(i)->getColumn("id")->getData()),
      character);

    chapterCount++;
  }

  // Cleanup
  delete(characterData);
  delete(chapterData);
}

NovelData::~NovelData() {
  delete novelDb;

  for (int i = 0; i < MAX_CHAPTERS; i++) {
    if (chapter[i]) {
      delete(chapter[i]);
    }
  }
}

NovelSceneSegmentLine* NovelData::getNextLine() {
  return getCurrentSceneSegment()->getLine(++currentSceneSegmentLine);
}

NovelSceneSegment* NovelData::advanceToNextSegment() {
  currentSceneSegmentLine = -1; // Reset which line we're on

  if (getCurrentScene()->getSegmentCount() == 0) {
    // TODO: Allow scenes with no segments as a background image transition between multiple places
    std::cout<<"Error: scene "<<getCurrentScene()->getId()<<" has no scene segments"<<std::endl;
  }
  return getCurrentScene()->getSceneSegment(++currentSceneSegment);
}

NovelScene* NovelData::advanceToNextScene() {
  currentSceneSegment = -1;
  currentScene++;
  return getCurrentScene();
}

ProjectInformation* NovelData::getProjectInformation() {
  return projectInformation;
}

Character* NovelData::getCharacter(int id) {
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
    scene[i] = NULL;
  }

  // Get all of the scenes within the chapter
  DataSet *sceneData = new DataSet();

  std::ostringstream ss;

  ss << "SELECT * FROM scenes WHERE chapter_id = " << id << ";";

  db->executeQuery(ss.str(), sceneData);

  for (int i = 0; i < sceneData->getRowCount(); i++) {

    if (!sceneData->getRow(i)->doesColumnExist("id")) {
      continue;
    }

    std::string backgroundImageName("");

    if (sceneData->getRow(i)->doesColumnExist("background_image_name")) {
      backgroundImageName = sceneData->getRow(i)->getColumn("background_image_name")->getData();
    }

    int backgroundColourId = 0;
    int startTransitionColourId = 0;
    int endTransitionColourId = 0;

    if (sceneData->getRow(i)->doesColumnExist("background_colour_id")) {
      backgroundColourId = std::stoi(sceneData->getRow(i)->getColumn("background_colour_id")->getData());
    }

    if (sceneData->getRow(i)->doesColumnExist("start_transition_colour_id")) {
      startTransitionColourId = std::stoi(sceneData->getRow(i)->getColumn("start_transition_colour_id")->getData());
    }

    if (sceneData->getRow(i)->doesColumnExist("end_transition_colour_id")) {
      endTransitionColourId = std::stoi(sceneData->getRow(i)->getColumn("end_transition_colour_id")->getData());
    }

    scene[i] = new NovelScene(db,
      std::stoi(sceneData->getRow(i)->getColumn("id")->getData()),
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

NovelScene* NovelChapter::getScene(int id) {
  return scene[id];
}

int NovelChapter::getSceneCount() {
  return sceneCount;
}

// Scene-specific stuff
NovelScene::NovelScene(DatabaseConnection *db, int sId, std::string bgImage, int bgColourId, int strColourId, int etrColourId, Character *character[]) {
  id = sId;
  backgroundImage = bgImage;
  backgroundColourId = bgColourId;
  startTransitionColourId = strColourId;
  endTransitionColourId = etrColourId;

  segmentCount = 0;

  for (int i = 0; i < MAX_SEGMENTS; i++) {
    segment[i] = NULL;
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

    std::string visualEffectName = sceneSegmentData->getRow(i)->doesColumnExist("visual_effect_name") ? sceneSegmentData->getRow(i)->getColumn("visual_effect_name")->getData() : "";
    std::string backgroundMusicName = sceneSegmentData->getRow(i)->doesColumnExist("background_music_name") ? sceneSegmentData->getRow(i)->getColumn("background_music_name")->getData() : "";

    segment[i] = new NovelSceneSegment(db,
      std::stoi(sceneSegmentData->getRow(i)->getColumn("id")->getData()),
      backgroundMusicName,
      visualEffectName,
      character);

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

NovelSceneSegment* NovelScene::getSceneSegment(int id) {
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
NovelSceneSegment::NovelSceneSegment(DatabaseConnection *db, int ssId, std::string ssBackgroundMusicName, std::string ssVisualEffectName, Character *character[]) {
  id = ssId;
  backgroundMusicName = ssBackgroundMusicName;
  visualEffectName = ssVisualEffectName;
  lineCount = 0;

  for (int i = 0; i < MAX_LINES; i++) {
    line[i] = NULL;
  }

  #ifdef DEBUG_NOVEL_DATA
    std::cout<<"Added scene segment "<<id<<std::endl;
  #endif

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
      characterStateGroupId = std::stoi(lineData->getRow(i)->getColumn("character_state_group_id")->getData());
    }

    std::string characterId = lineData->getRow(i)->doesColumnExist("character_id") ? lineData->getRow(i)->getColumn("character_id")->getData() : "0";
    std::string overrideCharacterName = "";

    if (lineData->getRow(i)->doesColumnExist("override_character_name")) {
      overrideCharacterName = lineData->getRow(i)->getColumn("override_character_name")->getData();
    }

    line[i] = new NovelSceneSegmentLine(db,
      std::stoi(lineData->getRow(i)->getColumn("id")->getData()),
      std::stoi(characterId),
      lineData->getRow(i)->getColumn("text")->getData(),
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

NovelSceneSegmentLine* NovelSceneSegment::getLine(int id) {
  return line[id];
}

std::string NovelSceneSegment::getBackgroundMusicName() {
  return backgroundMusicName;
}

// Line-specific stuff
NovelSceneSegmentLine::NovelSceneSegmentLine(DatabaseConnection *db, int sslId, int sslCharacterId, std::string sslText, int sslCharacterStateGroupId, std::string sslOverrideCharacterName, Character *character[]) {
  id = sslId;
  characterId = sslCharacterId;
  overrideCharacterName = sslOverrideCharacterName;

  text = sslText;
  characterStateGroup = NULL;

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
    characterStateGroup = new CharacterStateGroup(std::stoi(dataSet->getRow(0)->getColumn("id")->getData()), db, character);
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

CharacterStateGroup* NovelSceneSegmentLine::getCharacterStateGroup() {
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
    gameTitle = projectInformationDataSet->getRow(0)->getColumn("game_title")->getData();
  }

  if (projectInformationDataSet->getRow(0)->doesColumnExist("author_name")) {
    authorName = projectInformationDataSet->getRow(0)->getColumn("author_name")->getData();
  }

  if (projectInformationDataSet->getRow(0)->doesColumnExist("version_number")) {
    versionNumber = projectInformationDataSet->getRow(0)->getColumn("version_number")->getData();
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
      characterState.push_back(new CharacterState(std::stoi(dataSet->getRow(i)->getColumn("id")->getData()),db, character));
    }

  }
  
  delete(dataSet);
}

CharacterStateGroup::~CharacterStateGroup() {

}

std::vector<CharacterState*> CharacterStateGroup::getCharacterStates() {
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

  std::string characterSpriteId = dataSet->getRow(0)->getColumn("character_sprite_id")->getData();

  if (dataSet->getRowCount() > 0) {

    std::vector<std::string> query = {
      "SELECT * FROM character_sprites WHERE id = ",
      characterSpriteId,
      ";"
    };

    DataSet *dataSet = new DataSet();

    db->executeQuery(Utils::implodeString(query), dataSet);

    if (dataSet->getRowCount() == 0) {
      std::vector<std::string> error = {
        "Unable to find a character sprite with id ",
        characterSpriteId
      };

      throw Utils::implodeString(error);
    }

    std::string characterId = dataSet->getRow(0)->getColumn("character_id")->getData();
    std::string characterSpriteName = dataSet->getRow(0)->getColumn("name")->getData();

    characterSprite = character[std::stoi(characterId)-1]->getSprite(characterSpriteName);

    if (!characterSprite) {
      std::vector<std::string> error = {
        "Could not find sprite for character ",
        characterId,
        " with name: ",
        characterSpriteName
      };

      throw Utils::implodeString(error);
    }

    delete(dataSet);

  } else {
    std::vector<std::string> error = {
      "No character state with id ",
      std::to_string(id),
      " could be found."
    };

    throw Utils::implodeString(error);
  }

  delete(dataSet);
}

CharacterState::~CharacterState() {

}
