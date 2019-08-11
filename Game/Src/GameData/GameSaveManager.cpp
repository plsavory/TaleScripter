#include "DatabaseConnection.hpp"
#include "DatabaseSchema.hpp"
#include "Data/Novel.hpp"
#include "GameData/GameSaveManager.h"
#include "MisuseException.hpp"

GameSaveManager::GameSaveManager(GameState *gState) {

    if (!Utils::fileExists("db/saves")) {
        createDatabase();
    }

    saveDatabase = new DatabaseConnection("saves");
    gameState = gState;

}

GameSaveManager::~GameSaveManager() {
    delete(saveDatabase);
}

/**
 * Creates the database to save game saves to
 */
void GameSaveManager::createDatabase() {

    // Create the database schema object
    DatabaseSchema *saveDb = new DatabaseSchema("saves");

    // Create the tables
    DatabaseTable *savesTable = saveDb->addTable("game_saves");
    savesTable->addPrimaryKey();
    savesTable->addColumn("saved_at", ColumnType::tTimestamp, true, "NOW()");
    savesTable->addColumn("text_line", ColumnType::tText, true, "");
    savesTable->addColumn("thumbnail_image_filename", ColumnType::tText, false, "");

    DatabaseTable *progressTable = saveDb->addTable("game_save_progress");
    progressTable->addPrimaryKey();
    progressTable->addColumn("segment_line_id", ColumnType::tInteger, true, "");
    progressTable->addColumn("game_save_id", ColumnType::tInteger, true, "");

    DatabaseTable *variablesTable = saveDb->addTable("game_save_variables");
    variablesTable->addPrimaryKey();
    variablesTable->addColumn("game_save_id", ColumnType::tInteger, true, "");
    variablesTable->addColumn("name", ColumnType::tText, true, "");
    variablesTable->addColumn("value", ColumnType::tInteger, true, "");

    // Write the database to disc
    saveDb->createDatabase();
    delete(saveDb);

}

/**
 * Saves a game
 * @param id - id of a game save (0 if new save)
 */
void GameSaveManager::save(int id) {

    // Check to see if a save with this id already exists in the database
    auto *dataSet = new DataSet();
    saveDatabase->execute("SELECT * FROM game_saves WHERE id = ?", dataSet, {std::to_string(id)}, {DatabaseConnection::TYPE_INT});

    std::string textLine = gameState->getSceneSegmentLine()->getText();

    if (dataSet->getRowCount() != 0) {
        // Overwriting a save - delete the existing one.
        saveDatabase->execute("DELETE FROM game_save_variables WHERE game_save_id = ?", nullptr, {std::to_string(id)}, {DatabaseConnection::TYPE_INT});
        saveDatabase->execute("DELETE FROM game_save_progress WHERE game_save_id = ?", nullptr, {std::to_string(id)}, {DatabaseConnection::TYPE_INT});
        saveDatabase->execute("DELETE FROM game_saves WHERE id = ?", nullptr, {std::to_string(id)}, {DatabaseConnection::TYPE_INT});
    }

    {
        std::vector<std::string> columns = {"id", "saved_at", "text_line"};
        std::vector<std::string> values = {std::to_string(id), "datetime('now')", textLine};
        std::vector<int> types = {DATA_TYPE_NUMBER, DATA_TYPE_DATE_TIME, DATA_TYPE_STRING};
        saveDatabase->insert("game_saves", columns, values, types);
    }

    {
        std::vector<std::string> columns = {"segment_line_id", "game_save_id"};
        std::vector<std::string> values = {std::to_string(gameState->getSceneSegmentLine()->getId()), std::to_string(id)};
        std::vector<int> types = {DATA_TYPE_NUMBER, DATA_TYPE_NUMBER};
        saveDatabase->insert("game_save_progress", columns, values, types);
    }

    delete(dataSet);
}

/**
 * Loads a game
 * @param id
 */
void GameSaveManager::load(int id) {
    if (id == 0) {
        throw MisuseException("Attempting to load a game save that does not exist.");
    }
}