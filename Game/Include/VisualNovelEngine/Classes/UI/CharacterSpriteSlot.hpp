#ifndef CHARACTER_SPRITE_SLOT_INCLUDED
#define CHARACTER_SPRITE_SLOT_INCLUDED

class CharacterSpriteSlot {
public:
  CharacterSpriteSlot(SpriteRenderer *sRenderer, ResourceManager *rManager, int myId);
  ~CharacterSpriteSlot();
  void update();
  void push(CharacterSpriteDrawRequest *drawRequest);
private:
  Sprite *sprite[2]; // We need 2 of them for fade effects.
  SpriteRenderer *spriteRenderer;
  int id;
};

#endif
