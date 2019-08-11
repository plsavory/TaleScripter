//
// Created by Pete on 2019-08-03.
//

#ifndef TALESCRIPTER_GAMESTATE_H
#define TALESCRIPTER_GAMESTATE_H

class GameState {
public:
    GameState();

    ~GameState();

    void reset();

    void setState(int gameSaveId);

    void setNovelTextDisplayCharacter(int character);

    void setNovelTextLine(NovelSceneSegmentLine *line);

    NovelSceneSegmentLine* getSceneSegmentLine();
private:
    NovelSceneSegmentLine *sceneSegmentLine;
    int novelTextDisplayCharacter;
};

#endif //TALESCRIPTER_GAMESTATE_H
