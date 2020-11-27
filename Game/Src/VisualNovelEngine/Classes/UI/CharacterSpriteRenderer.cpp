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

    for (auto &slot : spriteSlot) {
        slot->update();
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

    int spriteCount = sprites.size(); // The number of characters we want to draw

    std::vector<sf::FloatRect> spriteSizes;

    for (unsigned int i = 0; i < MAX_CHARACTER_SPRITE_SLOTS; i++) {
        // TODO: Handle optional parameters

        if (i < spriteCount) { // We want to add a character here

            if (i+1 > spriteSlot.size()) {
                // Create a new sprite slot if we don't have enough to draw this character
                addCharacterSpriteSlot();
            }

            // We are going to use this sprite slot - so configure it.
            spriteSlot[i]->push(sprites[i]);
            continue;
        }

        if (i >= spriteSlot.size()) {
            break; // No remaining sprite slots exist - stop looping.
        }

        // Remove this sprite slot as it will not be used
        delete(spriteSlot[i]);
        spriteSlot.erase(spriteSlot.begin() + i);
        i--; // Erasing the sprite slot above will have pushed all the ones after it down by 1.
    }

    if (true) {
        handleAutomaticSpritePositioning();
    } else {
        // TODO: Handle positioning overrides here
    }

}

void CharacterSpriteRenderer::clear() {

}

void CharacterSpriteRenderer::handleAutomaticSpritePositioning() {

    if (spriteSlot.empty()) {
        return;
    }

    // Make sure that all of the sprites are positioned and scaled correctly
    std::vector<int> potentialSpriteOffsets;

    int fullDisplayArea = 1280 * 0.75; // TODO: Don't hardcode this, detect from current display mode
    int startPositionX = 1280 / 2;
    int startPositionY = 720;
    int spacingBetweenSprites = fullDisplayArea / spriteSlot.size();

    // Figure out some potential initial sprite offsets

    // Reduce the spacing between all of the sprite until they will all fit on screen.
    while (spriteSlot.size() * spacingBetweenSprites > fullDisplayArea) {

        spacingBetweenSprites--;

        // Just in case...
        if (spacingBetweenSprites == 0) {
            break;
        }
    }

    for (int i = 0; i < spriteSlot.size(); i++) {
        int thisSpriteOffset = i * (spacingBetweenSprites);
        potentialSpriteOffsets.push_back(thisSpriteOffset);
    }

    int renderingStartPosition = startPositionX - ((potentialSpriteOffsets[spriteSlot.size() - 1]) / 2);

    // Set the sprite positions and origins
    for (int i = 0; i < spriteSlot.size(); i++) {
        for (int j = 0; j <= 1; j++) {
            spriteSlot[i]->getSprite(j)->setPosition(renderingStartPosition + potentialSpriteOffsets[i],
                                                     startPositionY);
        }

    }
}
