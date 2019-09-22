//
// Created by Pete on 2019-07-26.
//

#ifndef TALESCRIPTER_GAME_SAVE_MANAGER_H
#define TALESCRIPTER_GAME_SAVE_MANAGER_H

#include "GameData/GameState.h"

class GameSaveManager {
public:
    explicit GameSaveManager(GameState *gState);
    ~GameSaveManager();
    void save(int id);
    void load(int id);
    DataSet* getSaves();
    DataSetRow* getSave(int id);
    void storeSaves();
    void setScreenshot(sf::Texture *texture);
    sf::Texture* getThumbnail(int id);
    int getLoadedSaveId();
    void setSelectedSave(int saveId) {
        selectedSave = saveId;
    }
    DatabaseConnection *getSaveDb() {
        return saveDatabase;
    }
private:
    static void createDatabase();
    DatabaseConnection *saveDatabase;
    GameState *gameState;
    DataSet *gameSaves;
    sf::RenderWindow *window;
    sf::Texture *screenshot;
    std::vector<sf::Texture*> thumbnails;
    int selectedSave;
};

#endif //TALESCRIPTER_GAMESAVEMANAGER_H
