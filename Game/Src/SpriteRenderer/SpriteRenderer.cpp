#include <SFML/Graphics.hpp>
#include <iostream>
#include "Database/DatabaseConnection.hpp"
#include "Resource/TextureManager.hpp"
#include "SpriteRenderer/SpriteRenderer.hpp"

SpriteRenderer::SpriteRenderer(sf::RenderWindow *window, TextureManager *tManager) {
    displayWindow = window;
    renderMode = SpriteRenderMode::Prioritised;
    textureManager = tManager;

    updateClock = new sf::Clock();

    if (renderMode == SpriteRenderMode::Prioritised) {
        // TODO: Allow this to be switched on and off at will
        spritePriorityClock = new sf::Clock();
    } else {
        spritePriorityClock = nullptr;
    }

}

SpriteRenderer::~SpriteRenderer() {

    // Delete all sprites if SpriteRenderer is destroyed
    for (auto &currentSprite : sprites) {
        if (currentSprite) {
            delete currentSprite;
        }
    }

    delete updateClock;
    delete spritePriorityClock;

}

void SpriteRenderer::update() {

    if (updateClock->getElapsedTime().asMilliseconds() >= SPRITE_RENDERER_UPDATE_DELAY) {
        updateClock->restart();

        // Make sprites refresh their images etc
        for (auto &currentSprite : sprites) {
            if (currentSprite) {
                currentSprite->update();
            }
        }
    }

    if (renderMode == SpriteRenderMode::Prioritised) {
        prioritiseSprites(false);
    }
}

void SpriteRenderer::draw() {
    switch (renderMode) {
        case SpriteRenderMode::Prioritised:
            renderPrioritisedSprites();
            break;
        case SpriteRenderMode::Standard:
            renderSprites();
            break;
        default:
            break;
    }

}

/**
 * Sort the sprites into the order in which they should be drawn to the screen
 */
void SpriteRenderer::prioritiseSprites(bool immediate) {

    // Prioritising the sprites is an expensive operation, so do this infrequently.
    // TODO: Find the most adequate frequency to balance response and performance

    if (!immediate && !spritePriorityClock) {
        return;
    }

    if (!immediate && spritePriorityClock->getElapsedTime().asMilliseconds() <= SPRITE_RENDERER_PRIORITISE_DELAY) {
        return;
    }

    // Empty the priority matrix
    priorityMatrix.clear();

    // For each priority, loop over each sprites once and allocate it a spot within the priority matrix
    for (int iPriority = SPRITE_RENDERER_MAX_PRIORITIES; iPriority >= 0; iPriority--) {
        for (auto & currentSprite : sprites) {

            if (!currentSprite) {
                continue;
            }

            if (!currentSprite->isLoaded()) {
                continue;
            }

            if (currentSprite->priority != iPriority) {
                continue;
            }

            // If all of the above checks have passed, prioritise the sprites into this slot
            priorityMatrix.push_back(currentSprite);
        }
    }

    spritePriorityClock->restart();

}

/**
 * Draw all of the sprites to the window
 */
void SpriteRenderer::renderSprites() {

    for (auto & currentSprite : sprites) {
        if (currentSprite) {
            currentSprite->draw();
        }
    }
}

/**
 * [SpriteRenderer::renderPrioritisedSprites Render sprites based on their priority]
 */
void SpriteRenderer::renderPrioritisedSprites() {

    for (auto &prioritisedSprite : priorityMatrix) {

        if (!prioritisedSprite) {
            continue;
        }

        prioritisedSprite->draw();
    }
}

/**
 * Will return true if a sprite exists with the given name
 * @param name
 * @return
 */
bool SpriteRenderer::spriteWithNameExists(const std::string &name) {
    return std::any_of(sprites.begin(), sprites.end(), [&name](Sprite *spr) {return spr->name == name;});
}

/**
 * Returns the sprite object with the given name
 * @param name - The name of the sprite
 * @return - The requested sprite object
 */
Sprite *SpriteRenderer::getSprite(const std::string &name) {

    for (auto & currentSprite : sprites) {
        if (currentSprite) {
            if (currentSprite->name == name) {
                return currentSprite;
            }
        }
    }

    std::vector<std::string> errorMessage = {
            "Could not find sprite with name: ",
            name
    };
    throw ResourceException(Utils::implodeString(errorMessage));

}

/**
 * Returns the index of the sprite with the given name
 * @param name - The name of the sprite
 * @return - The index of the requested sprite
 */
int SpriteRenderer::getSpriteId(const std::string &name) {
    for (int i = 0; i < sprites.size(); i++) {
        if (sprites[i]->name == name) {
            return i;
        }
    }

    std::vector<std::string> errorMessage = {
            "Could not find sprite index with name: ",
            name
    };
    throw ResourceException(Utils::implodeString(errorMessage));
}

/**
 * Add a sprite to the sprite renderer with a pre-loaded texture
 * @param textureName
 * @param name
 * @param priority - Render priority of the sprite, those with a lower number will be drawn last and will appear on top
 * @return
 */
Sprite *SpriteRenderer::addSprite(const std::string &textureName, const std::string &name, int priority) {

    if (spriteWithNameExists(name)) {
        throw ResourceException(Utils::implodeString({"A sprite with name '", name, " ' already exists."}));
    }

    auto newSprite = new Sprite(textureManager, displayWindow, name, textureName, priority, sprites.size());
    sprites.push_back(newSprite);
    return newSprite;
}

/**
 * Add a sprite to the sprite renderer without an image or priority
 * @param name
 * @return
 */
Sprite *SpriteRenderer::addSprite(const std::string &name) {

    if (spriteWithNameExists(name)) {
        throw ResourceException(Utils::implodeString({"A sprite with name '", name, " ' already exists."}));
    }

    auto newSprite = new Sprite(textureManager, displayWindow, name, sprites.size());
    sprites.push_back(newSprite);
    return newSprite;

}

/**
 * Remove a sprite with the given index from memory
 * @param id
 */
void SpriteRenderer::removeSprite(int id) {

    if (id >= sprites.size()) {
        throw ResourceException(Utils::implodeString({"Unable to remove sprite: sprite id '",
                                 std::to_string(id), "' is out of range (",
                                 std::to_string(sprites.size()),
                                                ")"}));
    }

    delete(sprites[id]);
    sprites.erase(sprites.begin() + id);

    // Re-prioritise sprites to prevent us from trying to render a deleted sprite
    if (renderMode == SpriteRenderMode::Prioritised) {
        prioritiseSprites(true);
    }
}

/**
 * Removes a sprite (Not its associated texture) from memory
 * @param name
 */
void SpriteRenderer::removeSprite(const std::string &name) {
    for (int i = 0; i < sprites.size(); i++) {
        if (sprites[i]->name == name) {
            delete(sprites[i]);
            sprites.erase(sprites.begin() + i);

            // Re-prioritise sprites to prevent us from trying to render a deleted sprite
            if (renderMode == SpriteRenderMode::Prioritised) {
                prioritiseSprites(true);
            }

            return; // There can be only one! (Should)
        }
    }

    throw ResourceException(Utils::implodeString({
         "Unable to remove sprite: could not find sprite with name: ",
         name}));
}

void SpriteRenderer::setCameraPosition(int x, int y) {

}

/**
 * [SpriteRenderer::getAvailableDrawArea Returns the available area to this sprite manager]
 * @return [description]
 */
sf::Vector2u SpriteRenderer::getRenderArea() {
    // TODO: Allow for other draw areas than just the main window, this could work for split screen operations.
    return displayWindow->getSize();
}
