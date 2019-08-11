//
// Created by Pete on 2019-08-03.
//

#include <iostream>
#include "DatabaseConnection.hpp"
#include "Data/Novel.hpp"
#include "GameData/GameState.h"

/**
 * Initialise any values
 */
GameState::GameState() {
    reset();
}

/**
 * Delete any objects that we created
 */
GameState::~GameState() {

}

/**
 * Resets the game's state
 */
void GameState::reset() {
    sceneSegmentLine = nullptr;
}

/**
 * Loads the current state from the game save database
 * @param gameSaveId
 */
void GameState::setState(int gameSaveId) {

}

/**
 * Sets the scene segment line id, we can use this to figure out which scene to jump to
 * @param id
 * @param character - The current character being drawn on the novel text display
 */
void GameState::setNovelTextLine(NovelSceneSegmentLine *line) {
    sceneSegmentLine = line;
}

void GameState::setNovelTextDisplayCharacter(int character) {
    novelTextDisplayCharacter = character;
}

NovelSceneSegmentLine* GameState::getSceneSegmentLine() {
    return sceneSegmentLine;
}