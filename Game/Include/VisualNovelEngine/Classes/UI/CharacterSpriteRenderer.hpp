#ifndef CHARACTER_SPRITE_RENDERER_INCLUDED
#define CHARACTER_SPRITE_RENDERER_INCLUDED

#include <vector>

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

  /**
   * Returns true if none of the character sprite slots are transitioning or moving
   * @return
   */
  bool isDoingNothing() {
      return !std::any_of(spriteSlot.begin(), spriteSlot.end(), [](CharacterSpriteSlot *slot) {return !slot->isDoingNothing();});
  }
private:
  ResourceManager *resourceManager;
  SpriteRenderer *spriteRenderer;
  NovelData *novel;
  DatabaseConnection *resource;
  std::vector<CharacterSpriteSlot*> spriteSlot;

  void handleAutomaticSpritePositioning();

  CharacterSpriteSlot* addCharacterSpriteSlot() {
      auto* newSlot = new CharacterSpriteSlot(spriteRenderer, resourceManager, spriteSlot.size());
      spriteSlot.push_back(newSlot);
      return newSlot;
  }
};

#endif
