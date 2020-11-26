#ifndef CHARACTER_SPRITE_RENDERER_INCLUDED
#define CHARACTER_SPRITE_RENDERER_INCLUDED

struct CharacterSpriteDrawRequest {
public:
  CharacterSpriteDrawRequest(CharacterSprite *cSprite, CharacterState *characterState) {
    characterSprite = cSprite;
    this->characterState = characterState;
  }
  CharacterSprite *characterSprite;
  CharacterState *characterState;
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
  bool isDoingNothing() {
      for(int i = 0; i<activeSpriteCount; i++) {
          if (!spriteSlot[i]->isDoingNothing()) {
              return false;
          }
      }

      return true;
  }
private:
  ResourceManager *resourceManager;
  SpriteRenderer *spriteRenderer;
  NovelData *novel;
  DatabaseConnection *resource;
  CharacterSpriteSlot *spriteSlot[MAX_CHARACTER_SPRITE_SLOTS];
  int activeSpriteCount;
  void handleAutomaticSpritePositioning();
};

#endif
