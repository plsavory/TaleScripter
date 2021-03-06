#include "Base/Engine.hpp"
#include "Misc/Utils.hpp"
#include "VisualNovelEngine/Classes/Data/Novel.hpp"
#include "VisualNovelEngine/Classes/UI/CharacterSpriteSlot.hpp"
#include "Exceptions/MisuseException.hpp"

#define CHARACTER_FADE_SPEED 2
#define CHARACTER_MORPH_SPEED 2

CharacterSpriteSlot::CharacterSpriteSlot(SpriteRenderer *sRenderer, ResourceManager *rManager, int myId) {
    spriteRenderer = sRenderer;
    id = myId;
    previousCharacterId = 0;
    currentCharacterId = 0;
    setAlpha(0, 0);
    setAlpha(1, 0);
    updateState = UPDATE_STATE_NOTHING;
    fadingOut = true;

    // Create the sprites to hold in this class
    for (int i = 0; i < 2; i++) {
        std::vector<std::string> spriteIdentifier = {
                "character_sprite_slot_",
                std::to_string(id),
                "_",
                std::to_string(i)
        };

        sprite[i] = spriteRenderer->addSprite(Utils::implodeString(spriteIdentifier, ""));
        sprite[i] = spriteRenderer->addSprite(Utils::implodeString(spriteIdentifier, ""));
    }
}

CharacterSpriteSlot::~CharacterSpriteSlot() = default;

/**
 * Once implemented, this function will handle sprite transitions and smooth positioning changes
 */
void CharacterSpriteSlot::update() {

//    if (!sprite[0]->isLoaded() || !sprite[1]->isLoaded()) {
//        return;
//    }

    if (updateState == UPDATE_STATE_NOTHING) {
        return;
    }

    switch (updateState) {
        case UPDATE_STATE_NOTHING:
            return;
        case UPDATE_STATE_FADING:
            fadeUpdate();
            return;
        case UPDATE_STATE_MORPHING:
            morphUpdate();
            return;
        default:
            throw MisuseException("Unsupported update state");
    }
}

void CharacterSpriteSlot::morphUpdate() {

    setAlpha(0, alpha[0]-(255/(60 * CHARACTER_MORPH_SPEED))*2);
    setAlpha(1, alpha[1]+(255/(60 * CHARACTER_MORPH_SPEED))*2);

    if (alpha[1] >= 255) {
        sprite[0]->setVisible(false);
        auto *newSprite = sprite[1];
        sprite[1] = sprite[0];
        sprite[0] = newSprite;
        setAlpha(0, 255);
        setAlpha(1, 0);
        updateState = UPDATE_STATE_NOTHING;
    }



}

/**
 * Fades the currently-displaying character sprite out, and fades the new one in.
 * Used when a character replaces an existing one
 */
void CharacterSpriteSlot::fadeUpdate() {

    if (fadingOut) {
        setAlpha(0, alpha[0] - (255/(60 * CHARACTER_FADE_SPEED))*3);
    } else {
        setAlpha(0, alpha[0] + (255/(60 * CHARACTER_FADE_SPEED))*3);
    }

    if (!fadingOut && alpha[0] >= 255) {
        setAlpha(0,255);
        updateState = UPDATE_STATE_NOTHING;
    }

    // Switch the sprites around
    if (alpha[0] <= 0 && fadingOut) {
        auto *newSprite = sprite[1];
        sprite[1] = sprite[0];
        sprite[0] = newSprite;
        alpha[0] = 0;
        setAlpha(0, 0);
        fadingOut = false;
    }
}

void CharacterSpriteSlot::push(CharacterSpriteDrawRequest *drawRequest) {

    if (!drawRequest) {
        sprite[0]->setVisible(false);
        sprite[1]->setVisible(false);
        return;
    }

    if (previousCharacterId) {
        fadingOut = true;
        if (previousCharacterId == drawRequest->characterSprite->getCharacterId()) {
            startTransition(UPDATE_STATE_MORPHING, drawRequest);
        } else {
            startTransition(UPDATE_STATE_FADING, drawRequest);
        }
    } else {
        fadingOut = false;
        startTransition(UPDATE_STATE_FADING, drawRequest);
    }

    previousCharacterId = drawRequest->characterSprite->getCharacterId();
    // Todo: Handle priority and positioning
}

Sprite *CharacterSpriteSlot::getSprite(int id) {
    return sprite[id];
}

void CharacterSpriteSlot::startTransition(int transitionType, CharacterSpriteDrawRequest *drawRequest) {
    updateState = transitionType;

    int spriteToUse = fadingOut ? 1 : 0;

    switch (transitionType) {
        case UPDATE_STATE_FADING:
        case UPDATE_STATE_MORPHING:
            sprite[spriteToUse]->setTextureName(drawRequest->characterSprite->getTextureName(), true);
            setAlpha(0, (previousCharacterId == 0) ? 0 : 255);
            setAlpha(1, 0);
            spritesHaveFlipped = false;
            sprite[0]->setVisible(true);
            sprite[1]->setVisible(true);
            break;
        case UPDATE_STATE_NOTHING:
            sprite[0]->setTextureName(drawRequest->characterSprite->getTextureName(), true);
            sprite[0]->setVisible(true);
            return;
        default:
            throw MisuseException("Unsupported transition type");
    }
}
