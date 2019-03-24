#ifndef NOVEL_INCLUDED
#define NOVEL_INCLUDED

#define MAX_CHAPTERS 50
#define MAX_SCENES 50
#define MAX_SEGMENTS 50
#define MAX_LINES 1000
#define MAX_CHARACTERS 50

// Include headers for other classes which we need
#include "VisualNovelEngine/Classes/Data/Character.hpp"

enum AdvanceState {
  ChapterEnd, SceneEnd, SceneSegmentEnd, NextLine
};

class ProjectInformation {
public:
  ProjectInformation(DatabaseConnection *db);
  std::string getGameTitle() {
    return gameTitle;
  }
  std::string getAuthorName() {
    return authorName;
  }
  std::string getVersionNumber() {
    return versionNumber;
  }
private:
  std::string gameTitle;
  std::string authorName;
  std::string versionNumber;
};

class CharacterState {
public:
  CharacterState(int myId, DatabaseConnection *db, Character *character[]);
  ~CharacterState();
  CharacterSprite* getCharacterSprite() {
    return characterSprite;
  }
private:
  int id;
  CharacterSprite *characterSprite;
};

class CharacterStateGroup {
public:
  CharacterStateGroup(int myId, DatabaseConnection *db, Character *character[]);
  ~CharacterStateGroup();
  int getId() {
    return id;
  }
  std::vector<CharacterState*> getCharacterStates();
private:
  int id;
  std::vector<CharacterState*> characterState;
};

class NovelSceneSegmentLine {
public:
  NovelSceneSegmentLine(DatabaseConnection *db, int sslId, int sslCharacterId, std::string sslText, int sslCharacterStateGroupId, std::string sslOverrideCharacterName, Character *character[]);
  ~NovelSceneSegmentLine();
  std::string getText();
  int getCharacterId();
  std::string getOverrideCharacterName();
  CharacterStateGroup* getCharacterStateGroup();
private:
  int id;
  int characterId;
  std::string text;
  std::string overrideCharacterName;
  CharacterStateGroup *characterStateGroup;
};

class NovelSceneSegment {
public:
  NovelSceneSegment(DatabaseConnection *db, int ssId, std::string ssBackgroundMusicName, std::string ssVisualEffectName, Character *character[]);
  ~NovelSceneSegment();
  int getLineCount();
  NovelSceneSegmentLine* getLine(int id);
  std::string getBackgroundMusicName();
private:
  int id;
  std::string backgroundMusicName;
  std::string visualEffectName;
  NovelSceneSegmentLine *line[MAX_LINES];
  int lineCount;
};

class NovelScene {
public:
  NovelScene(DatabaseConnection *db, int sId, std::string bgImage, int bgColourId, int strColourId, int etrColourId, Character *character[]);
  ~NovelScene();
  NovelSceneSegment* getSceneSegment(int id);
  int getSegmentCount();
  int getId();
  std::string getBackgroundImageName();
  int getBackgroundColourId();
  int getTransitionColourId();
  int getStartTransitionColourId();
  int getEndTransitionColourId();
private:
  int id;
  std::string backgroundImage;
  NovelSceneSegment *segment[MAX_SEGMENTS];
  int segmentCount;
  int backgroundColourId;
  int startTransitionColourId;
  int endTransitionColourId;
};

class NovelChapter {
public:
  NovelChapter(DatabaseConnection *db, std::string chapterTitle, int chapterId, Character *character[]);
  ~NovelChapter();
  std::string getTitle();
  int getId();
  void start();
  NovelScene* getScene(int id);
  int getSceneCount();
private:
  DatabaseConnection *novelDb;
  int id;
  std::string title;
  NovelScene *scene[MAX_SCENES];
  int sceneCount;
};

class NovelData {
public:
  NovelData();
  ~NovelData();
  void start();
  void start(int cChapter, int cScene, int cSceneSegment, int cSceneSegmentLine);
  AdvanceState getNextAction();
  NovelSceneSegment* getCurrentSceneSegment();
  NovelSceneSegmentLine* getNextLine();
  NovelSceneSegment* advanceToNextSegment();
  NovelScene* advanceToNextScene();
  NovelScene* getCurrentScene();
  NovelChapter* getCurrentChapter();
  ProjectInformation* getProjectInformation();
  Character* getCharacter(int id);
private:
  void loadFromDatabase();
  DatabaseConnection *novelDb;
  NovelChapter *chapter[MAX_CHAPTERS];
  Character *character[MAX_CHARACTERS];
  int chapterCount;
  int currentChapter;
  int currentScene;
  int currentSceneSegment;
  int currentSceneSegmentLine;
  ProjectInformation *projectInformation;
};

#endif
