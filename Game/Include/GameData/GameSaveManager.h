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
private:
    static void createDatabase();
    DatabaseConnection *saveDatabase;
    GameState *gameState;
};

#endif //TALESCRIPTER_GAMESAVEMANAGER_H
