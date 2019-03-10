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
