#ifndef BASE_GAME_INCLUDED

#define BASE_GAME_INCLUDED

class Game {
public:
  Game();
  ~Game();
  void run();
private:
  void init();
  void update(int gameTime);
  void draw();
  float frameTime; // Used to keep track of timing so that Update gets called once per frame, can also be used to measure FPS.
  sf::RenderWindow *window;
  Engine *engine;
  std::string gameTitle = "TaleScripter - Unnamed Project";
  ResourceManager *resourceManager;
  GameManager *gameManager;
  SpriteRenderer *spriteRenderer;
  TextRenderer *textRenderer;
  InputManager *inputManager;
  BackgroundImageRenderer *backgroundImageRenderer;
  BackgroundTransitionRenderer *backgroundTransitionRenderer;
  #ifdef MULTITHREADED_RENDERING
  std::thread *renderingThread;
  void renderingThreadFunction();
  #endif
};

#endif
