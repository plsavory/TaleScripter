#include <iostream>
#include "Resource/TextureManager.hpp"

TextureManager::TextureManager() {

  for (int i = 0; i<MAX_TEXTURES; i++) {
    texture[i] = NULL;
  }

}

TextureManager::~TextureManager() {

  for (int i = 0; i<MAX_TEXTURES; i++) {
    if (texture[i]) {
      delete texture[i];
    }
  }

}

/**
 * [TextureManager::loadTexture Creates a texture load request]
 * @param  fname     [Filename of the texture]
 * @param  name      [Name of the texture]
 * @return           [ID of the texture, -1 on failure]
 */
int TextureManager::loadTexture(std::string fname, std::string name) {

  // Find a texture with the same name if it exists
  int existingTextureId = findTexture(name);

  if (existingTextureId >= 0) {
    assignTexture(fname, name, existingTextureId);
    return existingTextureId;
  }

  // Find an empty texture slot
  for (int i = 0; i<MAX_TEXTURES; i++) {
    if (!texture[i]) {
      assignTexture(fname, name, i);
      return i;
    }
  }

  std::cout<<"Error loading texture: Max number of textures ("<<MAX_TEXTURES<<") already loaded."<<std::endl;
  return -1;

}

/**
 * [TextureManager::assignTexture Forces a load on the texture with ID]
 * @param fname [filename of the texture]
 * @param name  [accessible name]
 * @param id    [ID slot to use]
 */
void TextureManager::assignTexture(std::string fname, std::string name, int id) {

  if (texture[id]) {
    // Texture already exists
    texture[id]->assign(name);
  } else {
    // Texture does not exist
    texture[id] = new Texture(name);
  }

  // Create a texture load request for this texture
  textureLoadQueue.push(TextureLoadRequest(fname, id));
}

/**
 * [TextureManager::findTexture Returns the texture ID of the texture with the given name]
 * @param  name [Name of the texture]
 * @return      [Texture ID if found, -1 if not found]
 */
int TextureManager::findTexture(std::string name) {

  for (int i = 0; i<MAX_TEXTURES; i++) {
    if (texture[i]) {
      if (texture[i]->name == name) {
        return i;
      }
    }
  }
  return -1;
}

/**
 * [TextureManager::processQueue Loads the texture at the front of the queue into memory]
 */
void TextureManager::processQueue() {

  if (textureLoadQueue.size() == 0) {
    return;
  }

  // Load the texture into memory
  texture[textureLoadQueue.front().getId()]->texture
    ->loadFromFile(textureLoadQueue.front().getFilename());

  texture[textureLoadQueue.front().getId()]->loaded = true;

  // Remove the texture load request from memory
  textureLoadQueue.pop();

}

bool TextureManager::isQueueEmpty() {
  return (textureLoadQueue.size() == 0);
}

Texture* TextureManager::getTexture(int id) {
  return (texture[id]);
}

Texture* TextureManager::getTexture(std::string name) {
  int textureId = findTexture(name);

  if (textureId >= 0) {
    return texture[textureId];
  }

  std::cout<<"Texture with name (" << name << ") not found."<<std::endl;
  return NULL;
}
