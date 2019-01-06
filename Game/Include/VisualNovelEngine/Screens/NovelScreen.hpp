#ifndef NOVEL_SCREEN_INCLUDED
#define NOVEL_SCREEN_INCLUDED

#include "VisualNovelEngine/NovelScreenClasses.hpp"

class NovelScreen {
public:
  NovelScreen(sf::RenderWindow *windowPointer,ResourceManager *rManager, SpriteRenderer *sRenderer, TextRenderer *tRenderer, InputManager *iManager, NovelData *novelPointer, BackgroundImageRenderer *backgroundImageRendererPointer);
  ~NovelScreen();
  void start();
  void update();
  void draw();
private:
  sf::RenderWindow *window;
  SpriteRenderer *spriteRenderer;
  ResourceManager *resourceManager;
  TextRenderer *textRenderer;
  NovelTextDisplay *textDisplay;
  InputManager *inputManager;
  MusicManager *musicManager;
  BackgroundImageRenderer *backgroundImageRenderer;
  NovelData *novel;
  void advance();
  int advanceEventId;
  void nextLine();
  void nextSegment();
  void nextScene();
};

#endif
