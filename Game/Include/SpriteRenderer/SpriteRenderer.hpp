#define MAX_SPRITE_COUNT 50

#include "SpriteRenderer/Sprite.hpp"

class SpriteRenderer {
public:
  SpriteRenderer(sf::RenderWindow *window);
  ~SpriteRenderer();
  void update();
  void draw();
  void getSprite(int id);
  void getSprite(std::string name);
  int addSprite(Sprite *sprite, int priority, std::string name);
  int addSprite(std::string imagePath, std::string name);
  void removeSprite(int id);
  void setCameraPosition(int x, int y);
private:
  void prioritiseSprites();
  void renderSprites();
  sf::RenderWindow *displayWindow;
  Sprite* sprite[MAX_SPRITE_COUNT];
};
