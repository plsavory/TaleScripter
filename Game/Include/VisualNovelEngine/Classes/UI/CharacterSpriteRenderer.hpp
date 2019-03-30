#ifndef CHARACTER_SPRITE_RENDERER_INCLUDED
#define CHARACTER_SPRITE_RENDERER_INCLUDED

struct CharacterSpriteDrawRequest {
public:
  CharacterSpriteDrawRequest(CharacterSprite *cSprite) {
    characterSprite = cSprite;
  }
  int overridePositionX;
  int overridePositionY;
  float scaleX;
  float scaleY;
  CharacterSprite *characterSprite;
};

#include "VisualNovelEngine/Classes/UI/CharacterSpriteSlot.hpp"

#define MAX_CHARACTER_SPRITE_SLOTS 128

class CharacterSpriteRenderer {
public:
  CharacterSpriteRenderer(ResourceManager *rManager, SpriteRenderer *sRenderer);
  ~CharacterSpriteRenderer();
  void update();
  void draw();
  void initData(NovelData *novelData);
  void push(std::vector<CharacterSpriteDrawRequest*> sprites);
  void clear();
  bool hasProcessedPositioning() {
    return processedPositioning;
  };
private:
  ResourceManager *resourceManager;
  SpriteRenderer *spriteRenderer;
  NovelData *novel;
  DatabaseConnection *resource;
  CharacterSpriteSlot *spriteSlot[MAX_CHARACTER_SPRITE_SLOTS];
  bool processedPositioning;
  int activeSpriteCount;
};

#endif
