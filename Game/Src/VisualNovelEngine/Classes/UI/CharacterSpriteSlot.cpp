#include "Base/Engine.hpp"
#include "Misc/Utils.hpp"
#include "VisualNovelEngine/Classes/Data/Novel.hpp"
#include "VisualNovelEngine/Classes/UI/CharacterSpriteSlot.hpp"

CharacterSpriteSlot::CharacterSpriteSlot(SpriteRenderer *sRenderer, ResourceManager *rManager, int myId) {
  spriteRenderer = sRenderer;
  id = myId;

  // Create the sprites to hold in this class
  for (int i = 0; i < 2; i++) {
    std::vector<std::string> spriteIdentifier = {
      "character_sprite_slot_",
      std::to_string(id),
      "_",
      std::to_string(i)
    };

    sprite[i] = spriteRenderer->addSprite(Utils::implodeString(spriteIdentifier, ""));
  }
}

CharacterSpriteSlot::~CharacterSpriteSlot() = default;

/**
 * Once implemented, this function will handle sprite transitions and smooth positioning changes
 */
void CharacterSpriteSlot::update() {

}

void CharacterSpriteSlot::push(CharacterSpriteDrawRequest *drawRequest) {

  if (!drawRequest) {
    sprite[0]->setVisible(false);
    return;
  }

  sprite[0]->setTextureName(drawRequest->characterSprite->getTextureName());
  sprite[0]->setVisible(true);
  // Todo: Handle priority and positioning
}

Sprite* CharacterSpriteSlot::getSprite() {
  // TODO: Handle transitioning sprites
  return sprite[0];
}