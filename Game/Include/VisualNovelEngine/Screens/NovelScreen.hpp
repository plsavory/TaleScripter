#ifndef NOVEL_SCREEN_INCLUDED
#define NOVEL_SCREEN_INCLUDED

#include "VisualNovelEngine/NovelScreenClasses.hpp"

class NovelScreen {
public:
  NovelScreen(Engine *enginePointer, NovelData *novelPointer);
  ~NovelScreen();
  void start();
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
  BackgroundTransitionRenderer *backgroundTransitionRenderer;
  CharacterSpriteRenderer *characterSpriteRenderer;
  NovelData *novel;
  void advance();
  int advanceEventId;
  void nextLine();
  void nextSegment();
  void nextScene();
  void transitionToNextScene();
  bool sceneTransitioning; // Indicates that we need to advance the scene after an end transition
};

#endif
