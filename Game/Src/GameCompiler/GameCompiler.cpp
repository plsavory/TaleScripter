#include <iostream>
#include "Misc/Utils.hpp"
#include "Database/DatabaseSchema.hpp"
#include "Database/DatabaseConnection.hpp"
#include "Exceptions/ProjectBuilderException.hpp"
#include "Misc/JsonHandler.hpp"
#include "ProjectBuilderHelper.h"
#include "GameCompiler/ProjectBuilder.hpp"
#include "GameCompiler/GameCompiler.hpp"

GameCompiler::GameCompiler(GameCompilerOptions *gameCompilerOptions, JsonHandler *fileHandler) {

    compilerOptions = gameCompilerOptions;
    fHandler = fileHandler;

    // If we have no project file path, set it as the default
    if (compilerOptions->getProjectFilePath().empty()) {

        if (!Utils::fileExists("projects/Default/project.json")) {
            throw ProjectBuilderException(
                    "No project has been specified, and there is no project located at 'projects/Default/project.json'");
        }

        compilerOptions->setProjectFilePath("projects/default/project.json");

    }

    // Create the database structure required for the game
    createResourceDatabase();
    createNovelDatabase();
}

GameCompiler::~GameCompiler() = default;

bool GameCompiler::process() {

    // Open the appropriate databases
    // Open up the newly-created Novel database
    DatabaseConnection *novel = new DatabaseConnection("novel");
    DatabaseConnection *resource = new DatabaseConnection("resource");

    if (!novel->isUsable()) {
        throw ProjectBuilderException("GameCompiler: Unable to continue - could not open Novel database.");
    }

    if (!resource->isUsable()) {
        throw ProjectBuilderException("GameCompiler: Unable to continue - could not open Resource database.");
    }

    // Create an instance of ProjectBuilder to read the main project.json file
    ProjectBuilder *projectBuilder = new ProjectBuilder(compilerOptions->getProjectFilePath(), novel, resource,
                                                        fHandler);
    projectBuilder->process();
    return false;
}

void GameCompiler::createResourceDatabase() {

    DatabaseSchema *resourceDb = new DatabaseSchema("resource");

    // Create music table
    DatabaseTable *musicTable = resourceDb->addTable("music");
    musicTable->addPrimaryKey();
    musicTable->addColumn("name", ColumnType::tText, false, "");
    musicTable->addColumn("filename", ColumnType::tText, false, "");
    musicTable->addColumn("enabled", ColumnType::tBoolean, false, "");

    // Create font and text transform tables
    DatabaseTable *fontTable = resourceDb->addTable("fonts");
    fontTable->addPrimaryKey();
    fontTable->addColumn("name", ColumnType::tText, false, "");
    fontTable->addColumn("filename", ColumnType::tText, false, "");
    fontTable->addColumn("enabled", ColumnType::tBoolean, false, "");

    DatabaseTable *textStyleTable = resourceDb->addTable("text_styles");
    textStyleTable->addPrimaryKey();
    textStyleTable->addColumn("name", ColumnType::tText, false, "");
    textStyleTable->addColumn("font_id", ColumnType::tInteger, false, "");
    textStyleTable->addColumn("size", ColumnType::tInteger, false, "");
    textStyleTable->addColumn("text_colour_id", ColumnType::tInteger, false, "");
    textStyleTable->addColumn("rotation", ColumnType::tInteger, false, "");
    textStyleTable->addColumn("enabled", ColumnType::tBoolean, false, "");
    textStyleTable->addColumn("bold", ColumnType::tBoolean, false, "");
    textStyleTable->addColumn("italic", ColumnType::tBoolean, false, "");

    // Create background texture table
    DatabaseTable *backgroundTextureTable = resourceDb->addTable("background_textures");
    backgroundTextureTable->addPrimaryKey();
    backgroundTextureTable->addColumn("name", ColumnType::tText, false, "");
    backgroundTextureTable->addColumn("filename", ColumnType::tText, false, "");

    // Create backgrounds table
    DatabaseTable *backgroundsTable = resourceDb->addTable("backgrounds");
    backgroundsTable->addPrimaryKey();
    backgroundsTable->addColumn("name", ColumnType::tText, false, "");
    backgroundsTable->addColumn("enabled", ColumnType::tBoolean, false, "");

    // Create background image table
    DatabaseTable *backgroundLayersTable = resourceDb->addTable("background_layers");
    backgroundLayersTable->addPrimaryKey();
    backgroundLayersTable->addColumn("background_id", ColumnType::tInteger, true, "");
    backgroundLayersTable->addColumn("background_texture_id", ColumnType::tInteger, true, "");
    backgroundLayersTable->addColumn("enabled", ColumnType::tBoolean, false, "");
    backgroundLayersTable->addColumn("max_width", ColumnType::tInteger, false, "");
    backgroundLayersTable->addColumn("max_height", ColumnType::tInteger, false, "");
    backgroundLayersTable->addColumn("offset_left", ColumnType::tInteger, false, "");
    backgroundLayersTable->addColumn("offset_top", ColumnType::tInteger, false, "");
    backgroundLayersTable->addColumn("horizontal_scroll_speed", ColumnType::tInteger, false, "");
    backgroundLayersTable->addColumn("vertical_scroll_speed", ColumnType::tInteger, false, "");
    backgroundLayersTable->addColumn("depth", ColumnType::tInteger, false, "");
    backgroundLayersTable->addColumn("is_tiled", ColumnType::tInteger, false, "");

    // Create textures table
    DatabaseTable *texturesTable = resourceDb->addTable("textures");
    texturesTable->addPrimaryKey();
    texturesTable->addColumn("name", ColumnType::tText, false, "");
    texturesTable->addColumn("filename", ColumnType::tText, false, "");
    texturesTable->addColumn("enabled", ColumnType::tBoolean, false, "");

    // TODO: Pull in the Lua library to allow the script system to work
    DatabaseTable *scriptsTable = resourceDb->addTable("scripts");
    scriptsTable->addPrimaryKey();
    scriptsTable->addColumn("name", ColumnType::tText, false, "");
    scriptsTable->addColumn("filename", ColumnType::tText, false, "");
    scriptsTable->addColumn("enabled", ColumnType::tBoolean, false, "");

    // Create the Database
    resourceDb->createDatabase();

}

void GameCompiler::createNovelDatabase() {

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
    gameInformationTable->addColumn("text_display_type", ColumnType::tInteger, false, "");

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
    chaptersTable->addColumn("description", ColumnType::tText, false, "");
    chaptersTable->addColumn("hidden", ColumnType::tBoolean, false, "");
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
    scenesTable->addColumn("background_colour_id", ColumnType::tInteger, false, "");
    scenesTable->addColumn("start_transition_colour_id", ColumnType::tInteger, false, "");
    scenesTable->addColumn("end_transition_colour_id", ColumnType::tInteger, false, "");
    scenesTable->addColumn("start_transition_type_id", ColumnType::tInteger, false, "");
    scenesTable->addColumn("end_transition_type_id", ColumnType::tInteger, false, "");

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
    sceneSegmentsTable->addColumn("music_playback_request_id", ColumnType::tInteger, false, "");
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
    segmentLinesTable->addColumn("override_character_name", ColumnType::tText, false, "");
    segmentLinesTable->addColumn("text", ColumnType::tText, true, "");
    segmentLinesTable->addColumn("character_state_group_id", ColumnType::tInteger, false, "");

    /*
      When a row in this table is linked to a segment line, the given action will happen with the given argument
     */
    DatabaseTable *segmentLineActionsTable = novelDb->addTable("segment_line_actions");
    segmentLineActionsTable->addPrimaryKey();
    segmentLineActionsTable->addColumn("segment_line_id", ColumnType::tInteger, true, "");
    segmentLineActionsTable->addColumn("action_type_id", ColumnType::tInteger, true, "");
    segmentLineActionsTable->addColumn("argument0", ColumnType::tText, false, "");
    segmentLineActionsTable->addColumn("argument1", ColumnType::tText, false, "");
    segmentLineActionsTable->addColumn("argument2", ColumnType::tText, false, "");
    segmentLineActionsTable->addColumn("argument3", ColumnType::tText, false, "");

    /*
      Contains name and signatures of action types
     */
    DatabaseTable *actionTypesTable = novelDb->addTable("action_types");
    actionTypesTable->addPrimaryKey();
    actionTypesTable->addColumn("name", ColumnType::tText, true, "");
    actionTypesTable->addColumn("description", ColumnType::tText, false, "");
    actionTypesTable->addColumn("number_of_arguments", ColumnType::tText, true, "");
    actionTypesTable->addColumn("argument0_description", ColumnType::tText, false, "");
    actionTypesTable->addColumn("argument1_description", ColumnType::tText, false, "");
    actionTypesTable->addColumn("argument2_description", ColumnType::tText, false, "");
    actionTypesTable->addColumn("argument3_description", ColumnType::tText, false, "");

    /*
      The text_breaks table will be used to signify when the screen should clear
      at the end of the segment line with the id of segment_line_id in this table

      This will be used when the engine is running in Higurashi/Umineko-like mode
      where text will cover the entire screen rather than just the dialog box
      at the bottom.
     */
    DatabaseTable *textBreaksTable = novelDb->addTable("text_breaks");
    textBreaksTable->addPrimaryKey();
    textBreaksTable->addColumn("segment_line_id", ColumnType::tInteger, false, "");

    DatabaseTable *charactersTable = novelDb->addTable("characters");
    charactersTable->addPrimaryKey();
    charactersTable->addColumn("first_name", ColumnType::tText, true, "");
    charactersTable->addColumn("surname", ColumnType::tText, false, "");
    charactersTable->addColumn("bio", ColumnType::tText, false, "");
    charactersTable->addColumn("age", ColumnType::tText, false, "");
    charactersTable->addColumn("showOnCharacterMenu", ColumnType::tBoolean, false, "TRUE");

    DatabaseTable *characterSpritesTable = novelDb->addTable("character_sprites");
    characterSpritesTable->addPrimaryKey();
    characterSpritesTable->addColumn("character_id", ColumnType::tInteger, true, "");
    characterSpritesTable->addColumn("name", ColumnType::tText, false, "");
    characterSpritesTable->addColumn("texture_id", ColumnType::tInteger, true, "");

    // Create Character States table - This refers to the appearance of one character at any point within a group
    DatabaseTable *characterStatesTable = novelDb->addTable("character_states");
    characterStatesTable->addPrimaryKey();
    characterStatesTable->addColumn("character_sprite_id", ColumnType::tInteger, true, "");
    characterStatesTable->addColumn("character_state_group_id", ColumnType::tInteger, true, "");

    /*
      Create Character State Groups table. This table links a segment_line to many character_states.
      Having this table allows us to prevent massive duplication of data within the database
      related to character sprite states.
     */
    DatabaseTable *characterStateGroupsTable = novelDb->addTable("character_state_groups");
    characterStateGroupsTable->addPrimaryKey();

    // Create music play request table
    DatabaseTable *musicPlaybackRequestTable = novelDb->addTable("music_playback_requests");
    musicPlaybackRequestTable->addPrimaryKey();
    musicPlaybackRequestTable->addColumn("music_name", ColumnType::tText, false, "");
    musicPlaybackRequestTable->addColumn("music_playback_request_metadata_id", ColumnType::tInteger, false, "");

    // Create audio effect table
    DatabaseTable *musicPlaybackRequestMetadataTable = novelDb->addTable("music_playback_request_metadata");
    musicPlaybackRequestMetadataTable->addPrimaryKey();
    musicPlaybackRequestMetadataTable->addColumn("pitch", ColumnType::tDouble, false, "");
    musicPlaybackRequestMetadataTable->addColumn("speed", ColumnType::tDouble, false,
                                                 ""); // Speed 5 should be normal, music plays backwards if lower
    musicPlaybackRequestMetadataTable->addColumn("loop", ColumnType::tBoolean, false, "");
    musicPlaybackRequestMetadataTable->addColumn("volume", ColumnType::tInteger, false, "");
    musicPlaybackRequestMetadataTable->addColumn("startTime", ColumnType::tInteger, false, "");
    musicPlaybackRequestMetadataTable->addColumn("endTime", ColumnType::tInteger, false, "");
    musicPlaybackRequestMetadataTable->addColumn("muted", ColumnType::tBoolean, false, "");

    // Create UI themes-related database tables
    DatabaseTable *uiThemesTable = novelDb->addTable("ui_themes");
    uiThemesTable->addPrimaryKey();
    uiThemesTable->addColumn("name", ColumnType::tText, true, "");

    // Stores an element name and its theme id, used to link a group of image ids to a theme for a certain element type
    DatabaseTable *uiThemeElementsTable = novelDb->addTable("ui_theme_elements");
    uiThemeElementsTable->addPrimaryKey();
    uiThemeElementsTable->addColumn("ui_theme_id", ColumnType::tInteger, true, "");
    uiThemeElementsTable->addColumn("name", ColumnType::tText, true, "");

    DatabaseTable *uiThemeElementTexturesTable = novelDb->addTable("ui_theme_element_textures");
    uiThemeElementTexturesTable->addPrimaryKey();
    uiThemeElementTexturesTable->addColumn("ui_theme_element_id", ColumnType::tInteger, true, "");
    uiThemeElementTexturesTable->addColumn("texture_id", ColumnType::tInteger, true, "");

    // Screen-specific tables
    DatabaseTable *novelScreenAttributeGroupsTable = novelDb->addTable("novel_screen_attribute_groups");
    novelScreenAttributeGroupsTable->addPrimaryKey();
    novelScreenAttributeGroupsTable->addColumn("name", ColumnType::tText, true, "");
    novelScreenAttributeGroupsTable->addColumn("ui_theme_id", ColumnType::tInteger, true, "");

    DatabaseTable *novelScreenAttributesTable = novelDb->addTable("novel_screen_attributes");
    novelScreenAttributesTable->addPrimaryKey();
    novelScreenAttributesTable->addColumn("novel_screen_attribute_group_id", ColumnType::tInteger, true, "");
    novelScreenAttributesTable->addColumn("name", ColumnType::tInteger, true, "");
    novelScreenAttributesTable->addColumn("value", ColumnType::tText, true, "");

    DatabaseTable *titleScreensTable = novelDb->addTable("title_screens");
    titleScreensTable->addPrimaryKey();
    titleScreensTable->addColumn("background_id", ColumnType::tInteger, true, "");
    titleScreensTable->addColumn("background_music_id", ColumnType::tInteger, true, "");
    titleScreensTable->addColumn("menu_id", ColumnType::tInteger, true, "");

    DatabaseTable *menusTable = novelDb->addTable("menus");
    menusTable->addPrimaryKey();
    menusTable->addColumn("heading_text", ColumnType::tText, true, "");
    menusTable->addColumn("orientation", ColumnType::tText, true, "");

    DatabaseTable *menuOptionsTable = novelDb->addTable("menu_options");
    menuOptionsTable->addPrimaryKey();
    menuOptionsTable->addColumn("x_position", ColumnType::tInteger, true, "");
    menuOptionsTable->addColumn("y_position", ColumnType::tInteger, true, "");
    menuOptionsTable->addColumn("text", ColumnType::tText, false, "");
    menuOptionsTable->addColumn("texture_id", ColumnType::tInteger, false, "");
    menuOptionsTable->addColumn("action", ColumnType::tText, false, "");
    menuOptionsTable->addColumn("menu_id", ColumnType::tInteger, false, "");

    novelDb->createDatabase();

}
