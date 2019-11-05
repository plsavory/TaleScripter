#ifndef NOVEL_SCREEN_INCLUDED
#define NOVEL_SCREEN_INCLUDED

#include "VisualNovelEngine/NovelScreenClasses.hpp"

class NovelScreen {
public:
  NovelScreen(Engine *enginePointer, NovelData *novelPointer, CommonUI *cUI);
  ~NovelScreen();
  void start();
  void start(DataSet *gameSave);
  void update();
  void draw();
private:
  Engine *engine;
  sf::RenderWindow *window;
  SpriteRenderer *spriteRenderer;
  ResourceManager *resourceManager;
  TextRenderer *textRenderer;
  NovelTextDisplay *textDisplay;
  InputManager *inputManager;
  MusicManager *musicManager;
  BackgroundImageRenderer *backgroundImageRenderer;
  BackgroundTransitionHandler *backgroundTransitionRenderer;
  CharacterSpriteRenderer *characterSpriteRenderer;
  MouseEvent* advanceMouseEvent;
  NovelData *novel;
  GameState *gameState;
  void advance();
  int advanceEventId;
  int novelScreenSaveKeyId;
  void nextLine();
  void nextSegment();
  void nextScene();
  void transitionToNextScene();
  bool sceneTransitioning; // Indicates that we need to advance the scene after an end transition
  CommonUI *commonUI;
  bool isFirstChange; // Is this the first change of scene or not upon game start?

  // Methods to set which part of the novel we are at - will perform the appropriate background image changes and whatever else.
  void setScene(NovelScene *scene, bool setSegment);
  void setSceneSegment(NovelSceneSegment *sceneSegment, bool setLine);
  void setSceneSegmentLine(NovelSceneSegmentLine *sceneSegmentLine);
};

#endif
