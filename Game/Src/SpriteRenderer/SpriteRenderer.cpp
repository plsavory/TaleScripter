#include <SFML/Graphics.hpp>
#include <iostream>
#include "Database/DatabaseConnection.hpp"
#include "Resource/TextureManager.hpp"
#include "SpriteRenderer/SpriteRenderer.hpp"

SpriteRenderer::SpriteRenderer(sf::RenderWindow *window, TextureManager *tManager) {
    displayWindow = window;
    renderMode = SpriteRenderMode::Standard;
    textureManager = tManager;

    renderMode = SpriteRenderMode::Prioritised;

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
        prioritiseSprites();
    }
}

void SpriteRenderer::draw() {
// TODO: add support for prioritised sprites rendering
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

void SpriteRenderer::prioritiseSprites() {

    // Prioritising the sprites is an expensive operation, so do this infrequently.
    // TODO: Find the most adequate frequency to balance response and performance

    if (!spritePriorityClock) {
        return;
    }

    if (spritePriorityClock->getElapsedTime().asMilliseconds() <= SPRITE_RENDERER_PRIORITISE_DELAY) {
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

Sprite *SpriteRenderer::getSprite(const std::string &name) {

    for (auto & currentSprite : sprites) {
        if (currentSprite) {
            if (currentSprite->name == name) {
                return currentSprite;
            }
        }
    }

    std::vector<std::string> errorMessage = {
            "Could not find sprites with name: ",
            name
    };
    throw ResourceException(Utils::implodeString(errorMessage));

}

/**
 * [SpriteRenderer::addSprite Add a sprite to the renderer]
 * @param  imageName [The name of the image (stored in TextureManager)]
 * @param  name      [Accessible name of the sprite]
 * @return           [Sprite object, null on failure]
 */
Sprite *SpriteRenderer::addSprite(const std::string &imageName, const std::string &name, int priority) {

    auto newSprite = new Sprite(textureManager, displayWindow, name, imageName, priority, sprites.size());
    sprites.push_back(newSprite);
    return newSprite;
}

Sprite *SpriteRenderer::addSprite(const std::string &name) {

    auto newSprite = new Sprite(textureManager, displayWindow, name, sprites.size());
    sprites.push_back(newSprite);
    return newSprite;

}

void SpriteRenderer::removeSprite(int id) {

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
