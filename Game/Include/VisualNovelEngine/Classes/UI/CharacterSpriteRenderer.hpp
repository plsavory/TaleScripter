#ifndef CHARACTER_SPRITE_RENDERER_INCLUDED
#define CHARACTER_SPRITE_RENDERER_INCLUDED

class CharacterSpriteRenderer {
public:
  CharacterSpriteRenderer(ResourceManager *rManager, SpriteRenderer *sRenderer);
  ~CharacterSpriteRenderer();
  void update();
  void draw();
  void initData(NovelData *novelData);
  void push(CharacterSprite *characterSprite);
private:
  ResourceManager *resourceManager;
  SpriteRenderer *spriteRenderer;
  NovelData *novel;
  DatabaseConnection *resource;
};

#endif
