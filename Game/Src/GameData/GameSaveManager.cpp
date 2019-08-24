#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "DatabaseConnection.hpp"
#include "DatabaseSchema.hpp"
#include "Data/Novel.hpp"
#include "GameData/GameSaveManager.h"
#include "MisuseException.hpp"
#include "ResourceException.hpp"

GameSaveManager::GameSaveManager(GameState *gState) {

    if (!Utils::fileExists("db/saves")) {
        createDatabase();
    }

    saveDatabase = new DatabaseConnection("saves");
    gameState = gState;
    gameSaves = nullptr;
    storeSaves();

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

    if (id != 0) {
        saveDatabase->execute("SELECT * FROM game_saves WHERE id = ?", dataSet, {std::to_string(id)}, {DatabaseConnection::TYPE_INT});
    }

    std::string textLine = gameState->getSceneSegmentLine()->getText();

    if (dataSet->getRowCount() != 0) {
        // Overwriting a save - delete the existing one.
        saveDatabase->execute("DELETE FROM game_save_variables WHERE game_save_id = ?", nullptr, {std::to_string(id)}, {DatabaseConnection::TYPE_INT});
        saveDatabase->execute("DELETE FROM game_save_progress WHERE game_save_id = ?", nullptr, {std::to_string(id)}, {DatabaseConnection::TYPE_INT});
        saveDatabase->execute("DELETE FROM game_saves WHERE id = ?", nullptr, {std::to_string(id)}, {DatabaseConnection::TYPE_INT});
    }

    int newRowId;

    {
        std::vector<std::string> columns;
        std::vector<std::string> values;
        std::vector<int> types;

        if (id == 0) {
            columns = {"saved_at", "text_line"};
            values = {"datetime('now')", textLine};
            types = {DATA_TYPE_DATE_TIME, DATA_TYPE_STRING};
        } else {
            columns = {"id", "saved_at", "text_line"};
            values = {std::to_string(id), "datetime('now')", textLine};
            types = {DATA_TYPE_NUMBER, DATA_TYPE_DATE_TIME, DATA_TYPE_STRING};
        }

        newRowId = saveDatabase->insert("game_saves", columns, values, types);
    }

    {
        std::vector<std::string> columns = {"segment_line_id", "game_save_id"};
        std::vector<std::string> values = {std::to_string(gameState->getSceneSegmentLine()->getId()), std::to_string(newRowId)};
        std::vector<int> types = {DATA_TYPE_NUMBER, DATA_TYPE_NUMBER};
        saveDatabase->insert("game_save_progress", columns, values, types);
    }

    // Save the screenshot
    bool success = screenshot->copyToImage().saveToFile(Utils::implodeString({"save_thumbnail_", std::to_string(newRowId), ".png"}));

    if (!success) {
        throw GeneralException("Unable to save screenshot for game save.");
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

/**
 * Returns all of the game saves currently existing
 * @return DataSet containing game saves
 */
DataSet* GameSaveManager::getSaves() {
    return gameSaves;
}

/**
 * Returns a specific save, joined with its data.
 * @param id
 * @return
 */
DataSetRow* GameSaveManager::getSave(int id) {

    if (!gameSaves) {
        return nullptr;
    }

    --id;

    if (gameSaves->getRowCount() < id) {
        throw DataSetException(Utils::implodeString({"No save file with id ", std::to_string(id), " was found."}));
    }

    return gameSaves->getRow(id);
}

void GameSaveManager::storeSaves() {

    // Clear the existing textures
    for (int i = 0; i < thumbnails.size(); i++) {
        delete(thumbnails[i]);
        thumbnails.erase(thumbnails.begin() + i);
    }

    if (gameSaves) {
        delete(gameSaves);
    }

    gameSaves = new DataSet();

    saveDatabase->execute("SELECT *, strftime('%d/%m/%Y %H:%M', saved_at) AS formatted_saved_at, gs.id as save_id FROM game_saves gs INNER JOIN game_save_progress gsp ON gsp.game_save_id = gs.id;", gameSaves, {}, {});

    for (auto &save : gameSaves->getRows()) {
        auto *texture = new sf::Texture();
        if (!texture->loadFromFile(Utils::implodeString({"save_thumbnail_", save->getColumn("save_id")->getData()->asString(), ".png"}))) {
            throw ResourceException(Utils::implodeString({"Unable to load save thumbnail image for save id ", save->getColumn("save_id")->getData()->asString()}));
        }
        thumbnails.push_back(texture);
    }
}

void GameSaveManager::setScreenshot(sf::Texture *texture) {
    screenshot = texture;
}

sf::Texture* GameSaveManager::getThumbnail(int id) {
    --id;
    return thumbnails[id];
}