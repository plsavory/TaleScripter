#include <iostream>
#include <SFML/Graphics.hpp>
#include "Exceptions/ResourceException.hpp"
#include "Database/DatabaseConnection.hpp"
#include "Resource/TextureManager.hpp"
#include "SpriteRenderer/Sprite.hpp"

Sprite::Sprite(TextureManager *sTextureManager,sf::RenderWindow *window, const std::string& sName, const std::string& stextureName, int sPriority, int myId) {
  mySprite = new sf::Sprite();
  displayWindow = window;
  name = sName;
  textureName = stextureName;
  priority = sPriority;
  textureManager = sTextureManager;
  textureId = -1;
  textureSet = false;
  visible = true;
  id = myId;
}

Sprite::Sprite(TextureManager *sTextureManager, sf::RenderWindow *window, const std::string& sName, int myId) {
  mySprite = new sf::Sprite();
  textureManager = sTextureManager;
  displayWindow = window;
  name = sName;
  textureName = "";
  textureId = -1;
  textureSet = false;
  visible = false;
  id = myId;
  priority = 5;
}

Sprite::~Sprite() {
  delete mySprite;
}

bool Sprite::setImage(sf::Texture *image) {
  myImage = image;
  mySprite->setTexture(*myImage);
  sf::IntRect rect = sf::IntRect(0,0,myImage->getSize().x, myImage->getSize().y);
  mySprite->setTextureRect(rect);
  mySprite->setPosition(myPosition);
  mySprite->setColor(myColour);
  textureSet = true;
  return true;
}

void Sprite::setTextureName(std::string name, bool switchImmediately) {
  textureName = name;
  textureSet = false;
  textureId = -1;

  if (switchImmediately) {
      update();
  }
}

void Sprite::setPosition(int x, int y) {
    myPosition = sf::Vector2f((float)x,(float)y);
    mySprite->setPosition(myPosition);
}

void Sprite::update() {

  // If we've already loaded a texture, do nothing.
  // TODO: Allow updates for changed textures
  if (textureSet) {
    return;
  }

  if (textureName.empty()) {
    return;
  }

  if (textureId == -1) {
    // Texture has not been loaded, attempt to load it...
    int fetchedTextureId = textureManager->findTexture(textureName);

    if (fetchedTextureId == -1) {
        std::vector<std::string> errorMessage = {
                "Could not assign texture to sprite '", name, "': a texture with name '", textureName, "' could not be loaded"
        };

        throw ResourceException(Utils::implodeString(errorMessage));
    }

    textureId = fetchedTextureId;
  }

  // If we have a texture ID, and it has loaded, set our image to match it
  Texture *texture = textureManager->getTexture(textureId);

  if (!texture) {
    std::vector<std::string> errorMessage = {
        "Could not assign texture to sprite '", name, "': a texture with name '", textureName, "' could not be loaded"
    };

    throw ResourceException(Utils::implodeString(errorMessage));
  }

  if (texture->loaded) {
    setImage(texture->texture);
  }

  // If no image exists with the given name, keep trying to find one as it may be loaded in later

  // If the image exists, but hasn't been loaded yet, re-try again
}

void Sprite::draw() {

  if (!textureSet) {
    return;
  }

  if (!visible) {
    return;
  }

  displayWindow->draw(*mySprite);
}

sf::FloatRect Sprite::getSize() {
  if (textureSet) {
    return mySprite->getGlobalBounds();
  }

  return sf::FloatRect(0,0,0,0);
}

void Sprite::setOrigin(int x, int y) {
  mySprite->setOrigin((float)x, (float) y);
}
