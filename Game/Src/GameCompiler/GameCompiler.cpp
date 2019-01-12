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

    // TODO: Pull in the Lua library to allow the script system to work
    DatabaseTable *scriptsTable = resourceDb->addTable("scripts");

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

    /*
      game_information should contain useful information about the game which the engine
      needs to run the game appropriately. This could be default video mode, version number
      of the game, which text display mode to use etc.

      TODO: Figure out more use cases for this table and add them as required
     */
    DatabaseTable *gameInformationTable = novelDb->addTable("game_information");
    gameInformationTable->addColumn("game_title", ColumnType::tText, false, "");
    gameInformationTable->addColumn("author_name", ColumnType::tText, false, "");
    gameInformationTable->addColumn("version_number", ColumnType::tDouble, false, "");
    gameInformationTable->addColumn("text_display_type", ColumnType:tInteger, false, "");

    /*
      A chapter can appear on the main menu once the player has read the previous one.
      TODO: In future add a toggle as to whether the player should be kicked out to a menu
      after a chapter has ended, or whether the next chapter should start immediately.

      Some chapters can be hidden, in which case will be started by a script or a custom
      menu designed by the user. An example how a chapter setup like this could be
      used is the TIPs from Higurashi.

      Hidden chapters could also be used for end-game extra content or special features.
     */
    DatabaseTable *chaptersTable = novelDb->addTable("chapters");
    chaptersTable->addPrimaryKey();
    chaptersTable->addColumn("title", ColumnType::tText, false, "");
    chaptersTable->addColumn("accessible_name", ColumnType::tText, false, "");
    chaptersTable->addColumn("hidden_from_chapter_menu", ColumnType::tBoolean, false, "");
    chaptersTable->addColumn("requirement_id", ColumnType::tInteger, false, "");

    /*
      The scenes table is a way to group scene_segments together.
      A "scene" is a part of the game all contained within one location, so each
      scene can only have one background image, unless a user-witten lua script
      which is called during a scene changes the background image.
     */
    DatabaseTable *scenesTable = novelDb->addTable("scenes");
    scenesTable->addPrimaryKey();
    scenesTable->addColumn("chapter_id", ColumnType::tInteger, false, "");
    scenesTable->addColumn("background_image_name", ColumnType::tText, false, "");

    /*
      The scene_segments table links a scene to actual text content represented
      in the segment_lines table. A scene segment allows the background music to
      be changed within a scene, and also will be the core of the decision-making
      feature when this is added in future.

      When a player makes a decision, the decision will have a scene_segment id
      linked to it, when the player makes this decision, the game will jump to
      this scene segment.
     */
    DatabaseTable *sceneSegmentsTable = novelDb->addTable("scene_segments");
    sceneSegmentsTable->addPrimaryKey();
    sceneSegmentsTable->addColumn("scene_id", ColumnType::tInteger, false, "");
    sceneSegmentsTable->addColumn("background_music_name", ColumnType::tText, false, "");
    sceneSegmentsTable->addColumn("visual_effect_name", ColumnType::tText, false, "");

    /*
      The segment_lines table contains the actual novel's text.
      Each entry in this represents a piece of text which will be drawn to the screen
      These will be displayed sequentially until we hit the end of a scene segment.

      TODO: Add support for multiple languages using the language_id column to allow for
      one visual novel distribution to support many languages.
     */
    DatabaseTable *segmentLinesTable = novelDb->addTable("segment_lines");
    segmentLinesTable->addPrimaryKey();
    segmentLinesTable->addColumn("scene_segment_id", ColumnType::tInteger, true, "");
    segmentLinesTable->addColumn("language_id", ColumnType::tInteger, true, "");
    segmentLinesTable->addColumn("character_id", ColumnType::tInteger, false, "");
    segmentLinesTable->addColumn("text", ColumnType::tText, true, "");

    /*
      The text_breaks table will be used to signify when the screen should clear
      at the end of the segment line with the id of segment_line_id in this table

      This will be used when the engine is running in Higurashi/Umineko-like mode
      where text will cover the entire screen rather than just the dialog box
      at the bottom.
     */
    DatabaseTable *textBreaksTable = novelDb->addTable("text_breaks");
    textBreaksTable->addPrimaryKey();
    textBreaksTable->addColumn("", ColumnType::tInteger, false, "");

    novelDb->createDatabase();
  }

}
