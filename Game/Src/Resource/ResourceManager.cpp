#include <SFML/Graphics.hpp>
#include "Database/DatabaseConnection.hpp"
#include "BackgroundRenderer/BackgroundImageRenderer.hpp"
#include "Resource/ResourceManager.hpp"
#include "Misc/Utils.hpp"

#define MULTITHREADED_RESOURCE_LOADING_DISABLED

ResourceManager::ResourceManager(BackgroundImageRenderer *backgroundImageRendererPointer) {

  terminateLoadingThread = false;

  // Create required objects
  textureManager = new TextureManager();
  musicManager = new MusicManager();
  fontManager = new FontManager();
  backgroundImageRenderer = backgroundImageRendererPointer;

  loadResourcesFromDatabase();

  #ifdef MULTITHREADED_RESOURCE_LOADING
  // Putting resource loading into a separate thread to prevent stuttering in-game as things are loaded.
  resourceLoadThread = new std::thread(&ResourceManager::threadFunction, this);
  resourceLoadThread->detach();
  #endif

}

ResourceManager::~ResourceManager() {
  terminateLoadingThread = true;

  delete textureManager;
}

void ResourceManager::update() {
  #ifndef MULTITHREADED_RESOURCE_LOADING
  processQueue();
  #endif
}

void ResourceManager::threadFunction() {

  // Keep running the thread until we're told to terminate it
  while (!terminateLoadingThread) {
    processQueue();

    // We only need to run once every few frames (15 times per second in this case)
    std::this_thread::sleep_for(std::chrono::milliseconds(1000/15));
  }
}

void ResourceManager::processQueue() {

    // Loop until the loading queue is empty
    while (!isQueueEmpty()) {
      textureManager->processQueue();
      musicManager->processQueue();
      fontManager->processQueue();
      backgroundImageRenderer->processQueue();
    }
}

/**
 * [ResourceManager::isQueueEmpty Returns whether all resource loading queues are empty or not]
 * @return [description]
 */
bool ResourceManager::isQueueEmpty() {
  return (textureManager->isQueueEmpty() && musicManager->isQueueEmpty() && fontManager->isQueueEmpty() && backgroundImageRenderer->isQueueEmpty());
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

/**
 * [loadResourcesFromDatabase Attempts to load all resource files which are linked in the database]
 */
void ResourceManager::loadResourcesFromDatabase() {
  // Attempt to connect to the Resource database if it exists
  if (!Utils::fileExists("db\\resource")) {
    return;
  }

  // Connect to the resource database
  resourceDatabase = new DatabaseConnection("resource");

  // Load all resources from the database
  musicManager->loadAllFromDatabase(resourceDatabase);
  fontManager->loadAllFromDatabase(resourceDatabase);
  backgroundImageRenderer->addAllFromDatabase(resourceDatabase);
}
