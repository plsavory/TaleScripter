#include <iostream>
#include "Misc/Utils.hpp"
#include "Database/DatabaseConnection.hpp"
#include "VisualNovelEngine/Classes/Data/Novel.hpp"
#include <sstream>

NovelData::NovelData() {

  chapterCount = 0;
  loadFromDatabase();
  start();
}

void NovelData::start() {

  currentChapter = 0;
  currentScene = -1;
  currentSceneSegment = -1;
  currentSceneSegmentLine = -1; // Game hasn't started yet, first line has id of 0

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

  for (int i = 0; i < MAX_CHAPTERS; i++) {
    chapter[i] = NULL;
  }

  novelDb = new DatabaseConnection("novel");

  // Load project information from Database
  projectInformation = new ProjectInformation(novelDb);

  DataSet *chapterData = new DataSet();

  novelDb->executeQuery("SELECT * FROM chapters", chapterData);

  for (int i = 0; i < chapterData->getRowCount()-1; i++) {

    if (!chapterData->getRow(i)->doesColumnExist("id")) {
      continue;
    }

    if (!chapterData->getRow(i)->doesColumnExist("title")) {
      continue;
    }

    chapter[i] = new NovelChapter(novelDb,
      chapterData->getRow(i)->getColumn("title")->getData(),
      std::stoi(chapterData->getRow(i)->getColumn("id")->getData()));

    chapterCount++;
  }

}

NovelData::~NovelData() {
  delete novelDb;

  for (int i = 0; i < MAX_CHAPTERS; i++) {
    if (chapter[i]) {
      delete chapter[i];
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

// Chapter-specific stuff
NovelChapter::NovelChapter(DatabaseConnection *db, std::string chapterTitle, int chapterId) {
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

  for (int i = 0; i < sceneData->getRowCount()-1; i++) {

    if (!sceneData->getRow(i)->doesColumnExist("id")) {
      continue;
    }

    if (!sceneData->getRow(i)->doesColumnExist("background_image_name")) {
      continue;
    }

    scene[i] = new NovelScene(db,
      std::stoi(sceneData->getRow(i)->getColumn("id")->getData()),
      sceneData->getRow(i)->getColumn("background_image_name")->getData());
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
NovelScene::NovelScene(DatabaseConnection *db, int sId, std::string bgImage) {
  id = sId;
  backgroundImage = bgImage;

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

  for (int i = 0; i < sceneSegmentData->getRowCount()-1; i++) {

    if (!sceneSegmentData->getRow(i)->doesColumnExist("id")) {
      continue;
    }

    if (!sceneSegmentData->getRow(i)->doesColumnExist("background_music_name")) {
      continue;
    }

    std::string visualEffectName = sceneSegmentData->getRow(i)->doesColumnExist("visual_effect_name") ? sceneSegmentData->getRow(i)->getColumn("visual_effect_name")->getData() : NULL;
    std::string backgroundMusicName = sceneSegmentData->getRow(i)->doesColumnExist("background_music_name") ? sceneSegmentData->getRow(i)->getColumn("background_music_name")->getData() : NULL;

    segment[i] = new NovelSceneSegment(db,
      std::stoi(sceneSegmentData->getRow(i)->getColumn("id")->getData()),
      backgroundMusicName,
      visualEffectName);

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

// Segment-specific stuff
NovelSceneSegment::NovelSceneSegment(DatabaseConnection *db, int ssId, std::string ssBackgroundMusicName, std::string ssVisualEffectName) {
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

  for (int i = 0; i < lineData->getRowCount()-1; i++) {

    if (!lineData->getRow(i)->doesColumnExist("id")) {
      continue;
    }

    if (!lineData->getRow(i)->doesColumnExist("text")) {
      continue;
    }

    std::string characterId = lineData->getRow(i)->doesColumnExist("character_id") ? lineData->getRow(i)->getColumn("character_id")->getData() : "0";

    line[i] = new NovelSceneSegmentLine(db,
      std::stoi(lineData->getRow(i)->getColumn("id")->getData()),
      std::stoi(characterId),
      lineData->getRow(i)->getColumn("text")->getData());

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

NovelSceneSegment::getLineCount() {
  return lineCount;
}

NovelSceneSegmentLine* NovelSceneSegment::getLine(int id) {
  return line[id];
}

std::string NovelSceneSegment::getBackgroundMusicName() {
  return backgroundMusicName;
}

// Line-specific stuff
NovelSceneSegmentLine::NovelSceneSegmentLine(DatabaseConnection *db, int sslId, int sslCharacterId, std::string sslText) {
  id = sslId;
  characterId = sslCharacterId;

  text = sslText;

  #ifdef DEBUG_NOVEL_DATA
    std::cout<<"Added line \""<<text<<"\""<<std::endl;
  #endif
}

NovelSceneSegmentLine::~NovelSceneSegmentLine() {

}

std::string NovelSceneSegmentLine::getText() {
  return text;
}

// ProjectInformation specific things
ProjectInformation::ProjectInformation(DatabaseConnection *db) {

  // Initial values
  gameTitle = "Unnamed GameFramework Project";
  authorName = "Unknown";
  versionNumber = "Unknown";

  // Gets information about the project and stores it
  DataSet *projectInformationDataSet = new DataSet();

  db->executeQuery("SELECT * FROM project_information LIMIT 1;", projectInformationDataSet);

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
