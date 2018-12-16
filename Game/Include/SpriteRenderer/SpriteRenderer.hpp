#ifndef SPRITE_RENDERER_INCLUDED
#define SPRITE_RENDERER_INCLUDED

#define MAX_SPRITE_COUNT 50
#define SPRITE_RENDERER_UPDATE_DELAY 250

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
  Sprite* getSprite(std::string name);
  int addSprite(Sprite *sprite, int priority, std::string name);
  int addSprite(std::string imageName, std::string name, int priority);
  void removeSprite(int id);
  void setCameraPosition(int x, int y);
private:
  void prioritiseSprites();
  void renderSprites();
  void renderPrioritisedSprites();
  sf::RenderWindow *displayWindow;
  Sprite* sprite[MAX_SPRITE_COUNT];
  SpriteRenderMode renderMode;
  sf::Clock *updateClock;
  TextureManager *textureManager;
};

#endif
