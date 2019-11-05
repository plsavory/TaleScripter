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

#include "VisualNovelEngine/Classes/Data/DataModels/MusicPlaybackRequestMetadata.hpp"
class MusicPlaybackRequest {
public:
    MusicPlaybackRequest(DatabaseConnection *db, int myId, std::string myMusicName, int musicPlaybackRequestMetadataId);
    MusicPlaybackRequestMetadata* getMetadata() {
        return metadata;
    }

    std::string getMusicName() {
        return musicName;
    }
private:
    MusicPlaybackRequestMetadata *metadata;
    int id;
    std::string musicName;
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
  int getId() {
      return id;
  }
private:
  int id;
  int characterId;
  std::string text;
  std::string overrideCharacterName;
  CharacterStateGroup *characterStateGroup;
};

class NovelSceneSegment {
public:
  NovelSceneSegment(DatabaseConnection *db, int ssId, std::string ssVisualEffectName, Character *character[], int musicPlaybackRequestId);
  ~NovelSceneSegment();
  int getLineCount();
  int getId() {
      return id;
  }
  NovelSceneSegmentLine* getLine(int id);
  NovelSceneSegmentLine* getLineUsingDatabaseId(int id);
  int getNovelSceneSegmentLineOffsetUsingDatabaseId(int id);

  std::vector<NovelSceneSegmentLine*> getSceneSegmentLines() {
      return line;
  }

  std::string getBackgroundMusicName();
  MusicPlaybackRequest* getMusicPlaybackRequest() {
      return musicPlaybackRequest;
  }
private:
  int id;
  std::string backgroundMusicName;
  std::string visualEffectName;
  std::vector<NovelSceneSegmentLine*> line;
  int lineCount;
  MusicPlaybackRequest *musicPlaybackRequest;
};

class NovelScene {
public:
  NovelScene(DatabaseConnection *db, DataSetRow *data, Character *character[]);
  ~NovelScene();
  NovelSceneSegment* getSceneSegment(int id);
  NovelSceneSegment* getSceneSegmentUsingDatabaseId(int id);
  int getSceneSegmentOffsetUsingDatabaseId(int id);
  int getSegmentCount();
  int getId();
  std::string getBackgroundImageName();
  int getBackgroundColourId();
  int getStartTransitionColourId();
  int getEndTransitionColourId();
  int getEndTransitionTypeId() {
      return endTransitionTypeId;
  }
  std::vector<NovelSceneSegment*> getSceneSegments() {
      return segment;
  }
private:
  int id;
  std::string backgroundImage;
  std::vector<NovelSceneSegment*> segment;
  int segmentCount;
  int backgroundColourId;
  int startTransitionColourId;
  int endTransitionColourId;
  int startTransitionTypeId;
  int endTransitionTypeId;
};

class NovelChapter {
public:
  NovelChapter(DatabaseConnection *db, std::string chapterTitle, int chapterId, Character *character[]);
  ~NovelChapter();
  std::string getTitle();
  int getId();
  void start();
  NovelScene* getScene(int id);
  NovelScene* getSceneUsingDatabaseId(int id);
  int getSceneOffsetUsingDatabaseId(int id);
  std::vector<NovelScene*> getScenes() {
      return scene;
  }
  int getSceneCount();
private:
  DatabaseConnection *novelDb;
  int id;
  std::string title;
  std::vector<NovelScene*> scene;
  int sceneCount;
};

class NovelData {
public:
  NovelData(DatabaseConnection *saveDb);
  ~NovelData();
  void start();
  void start(int cChapter, int cScene, int cSceneSegment, int cSceneSegmentLine);
  void start(DataSet *novelProgressInformation);
  AdvanceState getNextAction();
  NovelSceneSegment* getCurrentSceneSegment();
  NovelSceneSegmentLine* getNextLine();
  NovelSceneSegment* advanceToNextSegment();
  NovelScene* advanceToNextScene();
  NovelScene* getCurrentScene();
  NovelChapter* getCurrentChapter();
  NovelChapter* getChapter(int offset) {

      if (!chapter[offset]) {
          throw GeneralException(Utils::implodeString({"No chapter with id ", std::to_string(offset)}, " exists"));
      }

      // TODO: Load the chapter's data into memory if it exists but is not loaded

      return chapter[offset];
  }
  ProjectInformation* getProjectInformation();
  Character* getCharacter(int id);
  NovelScene* getPreviousScene() {
      return previousScene;
  };
  DatabaseConnection* getNovelDatabase() {
      return novelDb;
  }
  /**
   * Returns the scene segment line with the given id in the database
   * @param id
   * @return
   */
  NovelSceneSegmentLine* getSceneSegmentLine(int id);
  DatabaseConnection *gameSaveDb;

  /**
   * Gets the current scene index within the chapter
   * @return the scene index
   */
  int getCurrentSceneIndex() {
      return sceneOffset;
  };
private:
  void loadFromDatabase();
  DatabaseConnection *novelDb;
  NovelChapter *chapter[MAX_CHAPTERS];
  Character *character[MAX_CHARACTERS];
  int chapterCount;
  int chapterOffset;
  int sceneOffset;
  int sceneSegmentOffset;
  int sceneSegmentLineOffset;
  ProjectInformation *projectInformation;
  NovelScene *previousScene;
};

#endif
