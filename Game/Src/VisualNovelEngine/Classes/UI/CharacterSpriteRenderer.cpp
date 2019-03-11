#include "Base/Engine.hpp"
#include "Misc/Utils.hpp"
#include "VisualNovelEngine/Classes/Data/Novel.hpp"
#include "VisualNovelEngine/Classes/UI/CharacterSpriteRenderer.hpp"

CharacterSpriteRenderer::CharacterSpriteRenderer(ResourceManager *rManager, SpriteRenderer *sRenderer) {
  resourceManager = rManager;
  spriteRenderer = sRenderer;
  resource = resourceManager->getResourceDatabase();

  // Create our sprite slots
  for (int i = 0; i < MAX_CHARACTER_SPRITE_SLOTS; i++) {
    spriteSlot[i] = new CharacterSpriteSlot(spriteRenderer, resourceManager, i);
  }
}

CharacterSpriteRenderer::~CharacterSpriteRenderer() {

}

void CharacterSpriteRenderer::update() {
  for (int i = 0; i < MAX_CHARACTER_SPRITE_SLOTS; i++) {
    spriteSlot[i]->update();
  }
}

void CharacterSpriteRenderer::draw() {

}

void CharacterSpriteRenderer::initData(NovelData *novelData) {
  novel = novelData;

  // Link all of the Character Sprites up with their textures
  int count = 0;

  while (novel->getCharacter(count)) {
    Character *character = novel->getCharacter(count);

    int spriteCount = 0;

    while (character->getSprite(spriteCount)) {
      CharacterSprite *sprite = character->getSprite(spriteCount);

      std::vector<std::string> textureNameQuery = {
        "SELECT name FROM textures WHERE id = ",
        std::to_string(sprite->getTextureId()),
        ";"
      };

      DataSet *dataSet = new DataSet();

      resource->executeQuery(Utils::implodeString(textureNameQuery, ""), dataSet);

      if (dataSet->getRowCount() == 0) {
        std::vector<std::string> error = {
          "Could not find texture with id: ",
          std::to_string(sprite->getTextureId())
        };

        throw Utils::implodeString(error, "");
      }

      sprite->setTextureName(dataSet->getRow(0)->getColumn("name")->getData());

      delete(dataSet);

      spriteCount++;
    }

    count++;
  }
}

/**
 * [CharacterSpriteRenderer::push Takes a vector of sprite draw requests and adds them all to the sprite renderer]
 * @param sprites [description]
 */
void CharacterSpriteRenderer::push(std::vector<CharacterSpriteDrawRequest*> sprites) {

  for (unsigned int i = 0; i < sprites.size(); i++) {
    // TODO: Handle optional parameters
    spriteSlot[i]->push(sprites[i]);
  }

}

void CharacterSpriteRenderer::clear() {

}
