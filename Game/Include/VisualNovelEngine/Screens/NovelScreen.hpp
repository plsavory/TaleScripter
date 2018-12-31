#ifndef NOVEL_SCREEN_INCLUDED
#define NOVEL_SCREEN_INCLUDED

#include "VisualNovelEngine/NovelScreenClasses.hpp"

class NovelScreen {
public:
  NovelScreen(sf::RenderWindow *windowPointer,ResourceManager *rManager, SpriteRenderer *sRenderer, TextRenderer *tRenderer, InputManager *iManager);
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
  void advance();
  int advanceEventId;
};

#endif
