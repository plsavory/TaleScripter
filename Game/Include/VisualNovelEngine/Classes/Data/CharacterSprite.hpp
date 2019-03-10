#ifndef NOVEL_DATA_CHARACTER_SPRITE_INCLUDED
#define NOVEL_DATA_CHARACTER_SPRITE_INCLUDED

class CharacterSprite {
public:
  CharacterSprite(std::string cName, int tId, int characterId);
  ~CharacterSprite();
  void setSprite(int id);
  std::string getName();
  int getTextureId();
  void setSpriteId(int id);
private:
  int id;
  int textureId;
  std::string name;
  int spriteId;
};

#endif
