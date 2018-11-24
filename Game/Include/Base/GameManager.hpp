// Add as many states as we need into here for future games
enum GameState {
  Init, Title, Menu, GameField, TestHarness
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
};
