#include "SFML/Graphics.hpp"
#include <iostream>
#include "SpriteRenderer/SpriteRenderer.hpp"

SpriteRenderer::SpriteRenderer(sf::RenderWindow *window) {
  displayWindow = window;

  // Initialise all sprite pointers to null so that we can test whether a sprite exists or not
  for (int i = 0; i < MAX_SPRITE_COUNT; i++) {
    sprite[i] = NULL;
  }
}

SpriteRenderer::~SpriteRenderer() {

  // Delete all sprites if SpriteRenderer is destroyed
  for (int i = 0; i < MAX_SPRITE_COUNT; i++) {
    if (sprite[i]) {
      delete sprite[i];
    }
  }

}

void SpriteRenderer::update() {

}

void SpriteRenderer::draw() {
  prioritiseSprites();
  renderSprites();
}

void SpriteRenderer::prioritiseSprites() {

}

void SpriteRenderer::renderSprites() {

}

void SpriteRenderer::getSprite(int id) {

}

void SpriteRenderer::getSprite(std::string name) {

}

int SpriteRenderer::addSprite(Sprite *sprite, int priority, std::string name) {

}

int SpriteRenderer::addSprite(std::string imagePath, std::string name) {

}

void SpriteRenderer::removeSprite(int id) {

}

void SpriteRenderer::setCameraPosition(int x, int y) {

}
