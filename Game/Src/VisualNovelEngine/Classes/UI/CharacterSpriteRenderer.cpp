#include "Base/Engine.hpp"
#include "Misc/Utils.hpp"
#include "VisualNovelEngine/Classes/Data/Novel.hpp"
#include "VisualNovelEngine/Classes/UI/CharacterSpriteRenderer.hpp"

CharacterSpriteRenderer::CharacterSpriteRenderer(ResourceManager *rManager, SpriteRenderer *sRenderer) {
  resourceManager = rManager;
  spriteRenderer = sRenderer;
  resource = resourceManager->getResourceDatabase();
}

CharacterSpriteRenderer::~CharacterSpriteRenderer() {

}

void CharacterSpriteRenderer::update() {

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

      std::string imageName = dataSet->getRow(0)->getColumn("name")->getData();

      std::vector<std::string> spriteIdentifier = {
        "spr",
        character->getFirstName(),
        character->getSurname(),
        sprite->getName()
      };

      Sprite *newSprite = spriteRenderer->addSprite(imageName, Utils::implodeString(spriteIdentifier,"_"), 5);

      sprite->setSpriteId(newSprite->getId());

      delete(dataSet);

      spriteCount++;
    }

    count++;
  }
}

void CharacterSpriteRenderer::push(CharacterSprite *characterSprite) {

}
