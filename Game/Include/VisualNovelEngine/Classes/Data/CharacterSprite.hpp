#ifndef NOVEL_DATA_CHARACTER_SPRITE_INCLUDED
#define NOVEL_DATA_CHARACTER_SPRITE_INCLUDED

class CharacterSprite {
public:
  CharacterSprite(std::string cName, int tId, int pCharacterId);
  ~CharacterSprite();
  void setSprite(int id);
  std::string getName();
  int getTextureId();
  void setSpriteId(int id);
  void setTextureName(std::string name) {
    textureName = name;
  }
  std::string getTextureName() {
    return textureName;
  }
  int getCharacterId() {
      return characterId;
  }
private:
  int characterId;
  int textureId;
  std::string name;
  std::string textureName;
  int spriteId;
};

#endif
