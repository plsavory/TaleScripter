#include "SFML/Graphics.hpp"
#include <iostream>
#include "Resource/TextureManager.hpp"
#include "SpriteRenderer/SpriteRenderer.hpp"

SpriteRenderer::SpriteRenderer(sf::RenderWindow *window, TextureManager *tManager) {
  displayWindow = window;
  renderMode = SpriteRenderMode::Standard;
  textureManager = tManager;

  // Initialise all sprite pointers to null so that we can test whether a sprite exists or not
  for (int i = 0; i < MAX_SPRITE_COUNT; i++) {
    sprite[i] = NULL;
  }

  renderMode = SpriteRenderMode::Standard;

  updateClock = new sf::Clock();
}

SpriteRenderer::~SpriteRenderer() {

  // Delete all sprites if SpriteRenderer is destroyed
  for (int i = 0; i < MAX_SPRITE_COUNT; i++) {
    if (sprite[i]) {
      delete sprite[i];
    }
  }

  delete updateClock;

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
}

void SpriteRenderer::draw() {
// TODO: add support for prioritised sprite rendering
  switch (renderMode) {
    case SpriteRenderMode::Standard:
    renderSprites();
    break;
    default:
    break;
  }

}

void SpriteRenderer::prioritiseSprites() {

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

int SpriteRenderer::addSprite(Sprite *sprite, int priority, std::string name) {

}

/**
 * [SpriteRenderer::addSprite Add a sprite to the renderer]
 * @param  imageName [The name of the image (stored in TextureManager)]
 * @param  name      [Accessible name of the sprite]
 * @return           [ID of the sprite, -1 on failure]
 */
int SpriteRenderer::addSprite(std::string imageName, std::string name, int priority) {

  for (int i = 0; i < MAX_SPRITE_COUNT; i++) {
    if (!sprite[i]) {
      sprite[i] = new Sprite(textureManager,displayWindow, name, imageName, priority);
      return i;
    }
  }

  return -1;

}

void SpriteRenderer::removeSprite(int id) {

}

void SpriteRenderer::setCameraPosition(int x, int y) {

}
