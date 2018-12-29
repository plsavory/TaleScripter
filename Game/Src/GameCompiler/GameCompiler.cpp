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

    // Create the Database
    resourceDb->createDatabase();
  }

}
