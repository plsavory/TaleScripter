// GameScreen.hpp - Contains functions and variables which will be common on each Game State/screen
// Designed to be extended by anything which handles a certain game state

#ifndef BASE_GAME_SCREEN_INCLUDED
#define BASE_GAME_SCREEN_INCLUDED

#include "SpriteRenderer/SpriteRenderer.hpp"

class GameScreen {
public:
  virtual void init(sf::RenderWindow *windowPointer);
  virtual void update();
  virtual void draw();
  void updateWindowPointer(sf::RenderWindow *windowPointer);
protected:
  sf::RenderWindow *window;
};

#endif
