// Add as many states as we need into here for future games

#ifndef BASE_GAME_MANAGER_INCLUDED
#define BASE_GAME_MANAGER_INCLUDED

// Include all of the game GameScreens
#include "GameScreens/TestScreen.hpp"

// Don't give anything in this enum the same name as a class, it breaks the build process - I learned this the hard way.
enum GameState {
  Init, Title, Menu, GameField, Test
};

class GameManager {
public:
  GameManager(sf::RenderWindow *window);
  ~GameManager();
  void init();
  void update();
  void draw();
  void updateWindowPointers(sf::RenderWindow *windowPointer);
private:
  GameState currentGameState;
  TestScreen *testScreen;
};

#endif
