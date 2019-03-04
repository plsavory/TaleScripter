#include <iostream>
#include "SFML/Graphics.hpp"
#include "Database/DatabaseConnection.hpp"
#include "Resource/TextureManager.hpp"
#include "SpriteRenderer/Sprite.hpp"

Sprite::Sprite(TextureManager *sTextureManager,sf::RenderWindow *window, std::string sName, std::string sImageName, int sPriority) {
  mySprite = new sf::Sprite();
  displayWindow = window;
  name = sName;
  imageName = sImageName;
  priority = sPriority;
  textureManager = sTextureManager;
  textureId = -1;
  textureSet = false;
  visible = true;
}

Sprite::~Sprite() {
  delete mySprite;
}

bool Sprite::setImage(sf::Texture *image) {
  myImage = image;
  mySprite->setTexture(*myImage);
  textureSet = true;
  return true;
}

void Sprite::setPosition(int x, int y) {
  mySprite->setPosition(sf::Vector2f((float)x,(float)y));
}

void Sprite::update() {

  // If we've already loaded a texture, do nothing.
  // TODO: Allow updates for changed textures
  if (textureSet) {
    return;
  }

  if (imageName == "") {
    return;
  }

  if (textureId == -1) {
    // Texture has not been loaded, attempt to load it...
    int fetchedTextureId = textureManager->findTexture(imageName);

    if (fetchedTextureId == -1) {
      return;
    }

    textureId = fetchedTextureId;
  }

  // If we have a texture ID, and it has loaded, set our image to match it
  Texture *texture = textureManager->getTexture(textureId);

  if (!texture) {
    return;
  }

  if (texture->loaded) {
    setImage(texture->texture);
  }

  // If no image exists with the given name, keep trying to find one as it may be loaded in later

  // If the image exists, but hasn't been loaded yet, re-try again
}

void Sprite::draw() {

  if (!visible) {
    return;
  }

  displayWindow->draw(*mySprite);
}
