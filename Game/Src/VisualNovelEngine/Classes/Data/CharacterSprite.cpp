#include <iostream>
#include "Misc/Utils.hpp"
#include "VisualNovelEngine/Classes/Data/CharacterSprite.hpp"

CharacterSprite::CharacterSprite(std::string cName, int tId, int characterId) {
  name = cName;
  id = characterId;
  textureId = tId;
}

CharacterSprite::~CharacterSprite() {

}

void CharacterSprite::setSprite(int id) {
  spriteId = id;
}

std::string CharacterSprite::getName() {
  return name;
}

int CharacterSprite::getTextureId() {
  return textureId;
}

void CharacterSprite::setSpriteId(int id) {
  spriteId = id;
}
