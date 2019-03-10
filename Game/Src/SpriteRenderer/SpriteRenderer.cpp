#include "SFML/Graphics.hpp"
#include <iostream>
#include "Database/DatabaseConnection.hpp"
#include "Resource/TextureManager.hpp"
#include "SpriteRenderer/SpriteRenderer.hpp"

SpriteRenderer::SpriteRenderer(sf::RenderWindow *window, TextureManager *tManager) {
  displayWindow = window;
  renderMode = SpriteRenderMode::Standard;
  textureManager = tManager;

  // Initialise all sprite pointers to null and priority matrix to -1 so that we can test whether a sprite exists or not
  for (int i = 0; i < MAX_SPRITE_COUNT; i++) {
    sprite[i] = NULL;
    priorityMatrix[i] = -1;
  }

  renderMode = SpriteRenderMode::Prioritised;

  updateClock = new sf::Clock();

  if (renderMode == SpriteRenderMode::Prioritised) {
    // TODO: Allow this to be switched on and off at will
    spritePriorityClock = new sf::Clock();
  } else {
    spritePriorityClock = NULL;
  }

}

SpriteRenderer::~SpriteRenderer() {

  // Delete all sprites if SpriteRenderer is destroyed
  for (int i = 0; i < MAX_SPRITE_COUNT; i++) {
    if (sprite[i]) {
      delete sprite[i];
    }
  }

  delete updateClock;
  delete spritePriorityClock;

}

void SpriteRenderer::update() {

  if (updateClock->getElapsedTime().asMilliseconds() >= SPRITE_RENDERER_UPDATE_DELAY) {
    updateClock->restart();

    // Make sprites refresh their images etc
    for (int i = 0; i<MAX_SPRITE_COUNT; i++) {
      if (sprite[i]) {
        sprite[i]->update();
      }
    }
  }

  if (renderMode == SpriteRenderMode::Prioritised) {
    prioritiseSprites();
  }
}

void SpriteRenderer::draw() {
// TODO: add support for prioritised sprite rendering
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
  for (int i = 0; i < MAX_SPRITE_COUNT; i++) {
    priorityMatrix[i] = -1;
  }

  // For each priority, loop over each sprite once and allocate it a spot within the priority matrix
  priorityMatrixIndex = 0;

  for (int iPriority = 0; iPriority < MAX_SPRITE_COUNT; iPriority++) {
    for (int iSprite = 0; iSprite < MAX_SPRITE_COUNT; iSprite++) {

      // TODO: Think of other checks which might be needed to prevent rendering
      if (!sprite[iSprite]) {
        continue;
      }

      if (!sprite[iSprite]->textureSet) {
        continue;
      }

      if (sprite[iSprite]->priority != iPriority) {
        continue;
      }

      // If all of the above checks have passed, prioritise the sprite into this slot
      priorityMatrix[priorityMatrixIndex] = iSprite;
      priorityMatrixIndex++;
    }
  }

  spritePriorityClock->restart();

}

void SpriteRenderer::renderSprites() {

  for (int i = 0; i<MAX_SPRITE_COUNT; i++) {
    if (sprite[i]) {
      sprite[i]->draw();
    }
  }
}

/**
 * [SpriteRenderer::renderPrioritisedSprites Render sprites based on their priority]
 */
void SpriteRenderer::renderPrioritisedSprites() {

  for (int i = priorityMatrixIndex; i>=0; i--) {

    if (priorityMatrix[i] == -1) {
      continue;
    }

    sprite[priorityMatrix[i]]->draw();
  }
}

Sprite* SpriteRenderer::getSprite(int id) {
  return sprite[id];
}

Sprite* SpriteRenderer::getSprite(std::string name) {

  for (int i = 0; i<MAX_SPRITE_COUNT; i++) {
    if (sprite[i]) {
      if (sprite[i]->name == name) {
        return sprite[i];
      }
    }
  }

  return NULL;

}

/**
 * [SpriteRenderer::addSprite Add a sprite to the renderer]
 * @param  imageName [The name of the image (stored in TextureManager)]
 * @param  name      [Accessible name of the sprite]
 * @return           [Sprite object, null on failure]
 */
Sprite* SpriteRenderer::addSprite(std::string imageName, std::string name, int priority) {

  for (int i = 0; i < MAX_SPRITE_COUNT; i++) {
    if (!sprite[i]) {
      sprite[i] = new Sprite(textureManager,displayWindow, name, imageName, priority, i);
      return sprite[i];
    }
  }

  return NULL;

}

void SpriteRenderer::removeSprite(int id) {

}

void SpriteRenderer::setCameraPosition(int x, int y) {

}
