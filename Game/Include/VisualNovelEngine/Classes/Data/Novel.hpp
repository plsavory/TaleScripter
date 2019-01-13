#ifndef NOVEL_INCLUDED
#define NOVEL_INCLUDED

#define MAX_CHAPTERS 50
#define MAX_SCENES 50
#define MAX_SEGMENTS 50
#define MAX_LINES 1000
#define MAX_CHARACTERS 50

enum AdvanceState {
  ChapterEnd, SceneEnd, SceneSegmentEnd, NextLine
};

class Character {
public:
  Character(int cId, std::string cFirstName, std::string cSurname, std::string cBio, std::string cAge);
  int getId();
  std::string getFirstName();
private:
  int id;
  std::string firstName;
  std::string surname;
  std::string bio;
  std::string age;
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

class NovelSceneSegmentLine {
public:
  NovelSceneSegmentLine(DatabaseConnection *db, int sslId, int sslCharacterId, std::string sslText, std::string sslOverrideCharacterName);
  ~NovelSceneSegmentLine();
  std::string getText();
  int getCharacterId();
  std::string getOverrideCharacterName();
private:
  int id;
  int characterId;
  std::string text;
  std::string overrideCharacterName;
};

class NovelSceneSegment {
public:
  NovelSceneSegment(DatabaseConnection *db, int ssId, std::string ssBackgroundMusicName, std::string ssVisualEffectName);
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
  NovelScene(DatabaseConnection *db, int sId, std::string bgImage);
  ~NovelScene();
  NovelSceneSegment* getSceneSegment(int id);
  int getSegmentCount();
  int getId();
  std::string getBackgroundImageName();
private:
  int id;
  std::string backgroundImage;
  NovelSceneSegment *segment[MAX_SEGMENTS];
  int segmentCount;
};

class NovelChapter {
public:
  NovelChapter(DatabaseConnection *db, std::string chapterTitle, int chapterId);
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
