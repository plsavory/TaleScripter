// Used for testing features and code

#ifndef GAME_SCREEN_TEST_SCREEN_INCLUDED
#define GAME_SCREEN_TEST_SCREEN_INCLUDED

#include "Base/GameScreen.hpp"

class TestScreen: public GameScreen {
public:
  TestScreen(sf::RenderWindow *windowPointer, ResourceManager *rManager);
  ~TestScreen();
  void init(sf::RenderWindow *windowPointer);
  void update();
  void draw();
private:
  Sprite *sprite;
  SpriteRenderer *spriteRenderer;
  sf::Texture *testImage;
};

#endif
