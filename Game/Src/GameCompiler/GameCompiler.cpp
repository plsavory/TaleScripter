#include <iostream>
#include "Misc/Utils.hpp"
#include "Database/DatabaseSchema.hpp"
#include "GameCompiler/GameCompiler.hpp"

GameCompiler::GameCompiler() {
  // Create the database structure required for the game
  createResourceDatabase();
}

GameCompiler::~GameCompiler() {

}

void GameCompiler::createResourceDatabase() {

  if (!Utils::fileExists("db\\resource.db")) {
    std::cout<<"Creatng resource.db..."<<std::endl;

    DatabaseSchema *resourceDb = new DatabaseSchema("resource");

    // Create music table
    DatabaseTable *musicTable = resourceDb->addTable("music");
    musicTable->addPrimaryKey();
    musicTable->addColumn("name", ColumnType::tText, false, "");
    musicTable->addColumn("filename", ColumnType::tText, false, "");
    musicTable->addColumn("enabled", ColumnType::tBoolean, false, "");
    musicTable->addColumn("audio_effect_id", ColumnType::tInteger, false, "");

    // Create audio effect table
    DatabaseTable *audioEffectTable = resourceDb->addTable("audio_effects");
    audioEffectTable->addPrimaryKey();
    audioEffectTable->addColumn("name", ColumnType::tText, false, "");
    audioEffectTable->addColumn("enabled", ColumnType::tBoolean, false, "");
    audioEffectTable->addColumn("pitch", ColumnType::tInteger, false, "");
    audioEffectTable->addColumn("speed", ColumnType::tInteger, false, ""); // Speed 5 should be normal, music plays backwards if lower

    // Create font and text transform tables
    DatabaseTable *fontTable = resourceDb->addTable("fonts");

    fontTable->addPrimaryKey();
    fontTable->addColumn("name", ColumnType::tText, false, "");
    fontTable->addColumn("filename", ColumnType::tText, false, "");
    fontTable->addColumn("enabled", ColumnType::tBoolean, false, "");

    DatabaseTable *textStyleTable = resourceDb->addTable("text_styles");
    textStyleTable->addColumn("name", ColumnType::tText, false, "");
    textStyleTable->addColumn("font_id", ColumnType::tInteger, false, "");
    textStyleTable->addColumn("size", ColumnType::tInteger, false, "");
    textStyleTable->addColumn("text_colour_id", ColumnType::tInteger, false, "");
    textStyleTable->addColumn("rotation", ColumnType::tInteger, false, "");
    textStyleTable->addColumn("enabled", ColumnType::tBoolean, false, "");
    textStyleTable->addColumn("bold", ColumnType::tBoolean, false, "");
    textStyleTable->addColumn("italic", ColumnType::tBoolean, false, "");

    // Create background image table
    DatabaseTable *backgroundImageTable = resourceDb->addTable("background_images");

    backgroundImageTable->addPrimaryKey();
    backgroundImageTable->addColumn("name", ColumnType::tText, false, "");
    backgroundImageTable->addColumn("filename", ColumnType::tText, false, "");
    backgroundImageTable->addColumn("enabled", ColumnType::tBoolean, false, "");

    // Create the Database
    resourceDb->createDatabase();

  }

  if (!Utils::fileExists("db\\novel")) {

    // Create the novel database which will be used to store the actual visual novel
    DatabaseSchema *novelDb = new DatabaseSchema("novel");

    DatabaseTable *gameInformationTable = novelDb->addTable("game_information");
    gameInformationTable->addColumn("game_title", ColumnType::tText, false, "");
    gameInformationTable->addColumn("author_name", ColumnType::tText, false, "");
    gameInformationTable->addColumn("version_number", ColumnType::tDouble, false, "");

    DatabaseTable *chaptersTable = novelDb->addTable("chapters");
    chaptersTable->addPrimaryKey();
    chaptersTable->addColumn("title", ColumnType::tText, false, "");
    chaptersTable->addColumn("accessible_name", ColumnType::tText, false, "");
    chaptersTable->addColumn("hidden_from_chapter_menu", ColumnType::tBoolean, false, "");
    chaptersTable->addColumn("requirement_id", ColumnType::tInteger, false, "");

    DatabaseTable *scenesTable = novelDb->addTable("scenes");
    scenesTable->addPrimaryKey();
    scenesTable->addColumn("chapter_id", ColumnType::tInteger, false, "");
    scenesTable->addColumn("background_image_name", ColumnType::tText, false, "");

    DatabaseTable *sceneSegmentsTable = novelDb->addTable("scene_segments");
    sceneSegmentsTable->addPrimaryKey();
    sceneSegmentsTable->addColumn("scene_id", ColumnType::tInteger, false, "");
    sceneSegmentsTable->addColumn("background_music_name", ColumnType::tText, false, "");
    sceneSegmentsTable->addColumn("visual_effect_name", ColumnType::tText, false, "");

    DatabaseTable *segmentLinesTable = novelDb->addTable("segment_lines");
    segmentLinesTable->addPrimaryKey();
    segmentLinesTable->addColumn("scene_segment_id", ColumnType::tInteger, true, "");
    segmentLinesTable->addColumn("character_id", ColumnType::tInteger, false, "");
    segmentLinesTable->addColumn("text", ColumnType::tText, true, "");

    novelDb->createDatabase();
  }

}
