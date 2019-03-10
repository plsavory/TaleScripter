#ifndef NOVEL_DATA_CHARACTER_SPRITE_INCLUDED
#define NOVEL_DATA_CHARACTER_SPRITE_INCLUDED

class CharacterSprite {
public:
  CharacterSprite(std::string cName, int tId, int characterId);
  ~CharacterSprite();
private:
  int id;
  int textureId;
  std::string name;
};

#endif
