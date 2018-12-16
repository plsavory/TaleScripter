#include <SFML/Graphics.hpp>
#include "Resource/ResourceManager.hpp"

ResourceManager::ResourceManager() {

  terminateLoadingThread = false;

  // Create required objects
  textureManager = new TextureManager();

}

ResourceManager::~ResourceManager() {
  terminateLoadingThread = true;

  delete textureManager;
}

void ResourceManager::update() {
  // TODO: Make multithreading optional

  // Putting resource loading into a separate thread to prevent stuttering in-game as things are loaded.
  std::thread thread(&ResourceManager::processQueue, this);
  thread.join();
}

void ResourceManager::processQueue() {

  // Loop until the loading queue is empty
  while (!isQueueEmpty()) {
    textureManager->processQueue();
  }
}

/**
 * [ResourceManager::isQueueEmpty Returns whether all resource loading queues are empty or not]
 * @return [description]
 */
bool ResourceManager::isQueueEmpty() {
  // TODO: Monitor other resources too
  return textureManager->isQueueEmpty();
}

/**
 * [ResourceManager::loadTexture Loads a texture into memory]
 * @param  fname [Filename of the image]
 * @param  name  [Addressable name of the image]
 * @return       [ID of the image]
 */
int ResourceManager::loadTexture(std::string fname, std::string name) {
  return textureManager->loadTexture(fname, name);
}

Texture* ResourceManager::getTexture(std::string name) {
  return textureManager->getTexture(name);
}

Texture* ResourceManager::getTexture(int id) {
  return textureManager->getTexture(id);
}

TextureManager* ResourceManager::getTextureManager() {
  return textureManager;
}
