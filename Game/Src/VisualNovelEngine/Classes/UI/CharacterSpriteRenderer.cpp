#include "Base/Engine.hpp"
#include "Misc/Utils.hpp"
#include "VisualNovelEngine/Classes/Data/Novel.hpp"
#include "VisualNovelEngine/Classes/UI/CharacterSpriteRenderer.hpp"

CharacterSpriteRenderer::CharacterSpriteRenderer(ResourceManager *rManager, SpriteRenderer *sRenderer) {
    resourceManager = rManager;
    spriteRenderer = sRenderer;
    resource = resourceManager->getResourceDatabase();
    processedPositioning = true;
    activeSpriteCount = 0;

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

    if (activeSpriteCount == 0) {
        return;
    }

    if (processedPositioning) {
        return;
    }

    // Make sure that all of the sprites are positioned and scaled correctly
    std::vector<int> potentialSpriteOffsets;

    int fullDisplayArea = 1280 * 0.75; // TODO: Don't hardcode this, detect from current display mode
    int startPositionX = 1280 / 2;
    int startPositionY = 720;
    int spacingBetweenSprites = fullDisplayArea / activeSpriteCount;

    // Figure out some potential initial sprite offsets

    // Reduce the spacing between all of the sprite until they will all fit on screen.
    while (activeSpriteCount * spacingBetweenSprites > fullDisplayArea) {

        spacingBetweenSprites--;

        // Just in case...
        if (spacingBetweenSprites == 0) {
            break;
        }
    }

    for (int i = 0; i < activeSpriteCount; i++) {

        // Do nothing if all of the sprites have not loaded yet.
        for (int j = 0; j <= 0; j++) {
            if (!spriteSlot[i]->getSprite(j)->isLoaded()) {
                return;
            }
        }

        int thisSpriteOffset = i * (spacingBetweenSprites);
        potentialSpriteOffsets.push_back(thisSpriteOffset);

        // TODO: Handle positioning overrides here
    }

    int renderingStartPosition = startPositionX - ((potentialSpriteOffsets[activeSpriteCount - 1]) / 2);

    // Set the sprite positions and origins
    for (int i = 0; i < activeSpriteCount; i++) {
        for (int j = 0; j <= 1; j++) {
            spriteSlot[i]->getSprite(j)->setPosition(renderingStartPosition + potentialSpriteOffsets[i],
                                                     startPositionY);

            spriteSlot[i]->getSprite(j)->setOrigin(
                    spriteSlot[i]->getSprite(j)->getSize().width / 2,
                    spriteSlot[i]->getSprite(j)->getSize().height
            );
        }

    }
    processedPositioning = true;
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

            resource->executeQuery(Utils::implodeString(textureNameQuery), dataSet);

            if (dataSet->getRowCount() == 0) {
                std::vector<std::string> error = {
                        "Could not find texture with id: ",
                        std::to_string(sprite->getTextureId())
                };

                throw ResourceException(Utils::implodeString(error));
            }

            sprite->setTextureName(dataSet->getRow(0)->getColumn("name")->getRawData());

            delete (dataSet);

            spriteCount++;
        }

        count++;
    }

}

/**
 * [CharacterSpriteRenderer::push Takes a vector of sprite draw requests and adds them all to the sprite renderer]
 * @param sprites [description]
 */
void CharacterSpriteRenderer::push(std::vector<CharacterSpriteDrawRequest *> sprites) {

    int spriteCount = sprites.size();

    std::vector<sf::FloatRect> spriteSizes;

    for (int i = 0; i < MAX_CHARACTER_SPRITE_SLOTS; i++) {
        // TODO: Handle optional parameters
        if (i < spriteCount) {
            spriteSlot[i]->push(sprites[i]);
            if (sprites[i] != nullptr && processedPositioning) {
                processedPositioning = false;
            }
            continue;
        }

        spriteSlot[i]->push(nullptr);
    }

    activeSpriteCount = spriteCount;

}

void CharacterSpriteRenderer::clear() {

}
