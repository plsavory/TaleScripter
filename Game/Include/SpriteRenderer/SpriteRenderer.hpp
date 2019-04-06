#ifndef SPRITE_RENDERER_INCLUDED
#define SPRITE_RENDERER_INCLUDED

#define MAX_SPRITE_COUNT 512
#define SPRITE_RENDERER_UPDATE_DELAY 250
#define SPRITE_RENDERER_PRIORITISE_DELAY 500
#define SPRITE_RENDERER_MAX_PRIORITIES 5

#include "SpriteRenderer/Sprite.hpp"

enum SpriteRenderMode {
  Standard, Prioritised
};

class SpriteRenderer {
public:
  SpriteRenderer(sf::RenderWindow *window, TextureManager *tManager);
  ~SpriteRenderer();
  void update();
  void draw();
  Sprite* getSprite(int id);
  Sprite* getSprite(const std::string& name);
  Sprite* addSprite(const std::string& imageName, const std::string& name, int priority);
  Sprite* addSprite(const std::string& name);
  void removeSprite(int id);
  void setCameraPosition(int x, int y);
  sf::Vector2u getRenderArea();
private:
  void prioritiseSprites();
  void renderSprites();
  void renderPrioritisedSprites();
  sf::RenderWindow *displayWindow;
  Sprite* sprite[MAX_SPRITE_COUNT];
  int priorityMatrix[MAX_SPRITE_COUNT]; // TODO: Experiment with a linked list rather than a priority matrix
  int priorityMatrixIndex;
  SpriteRenderMode renderMode;
  sf::Clock *updateClock;
  TextureManager *textureManager;
  sf::Clock *spritePriorityClock;
};

#endif
