// Used for testing features and code

#ifndef GAME_SCREEN_TEST_SCREEN_INCLUDED
#define GAME_SCREEN_TEST_SCREEN_INCLUDED

#include "Base/GameScreen.hpp"

class TestScreen: public GameScreen {
public:
  TestScreen(Engine *enginePointer);
  ~TestScreen();
  void init(sf::RenderWindow *windowPointer);
  void update();
  void draw();
private:
  Sprite *sprite;
  SpriteRenderer *spriteRenderer;
  ResourceManager *resourceManager;
  TextRenderer *textRenderer;
  sf::Texture *testImage;
  Engine *engine;
};

#endif
