#ifndef RESOURCE_RESOURCE_MANAGER_INCLUDED
#define RESOURCE_RESOURCE_MANAGER_INCLUDED

#include <iostream>
#include <thread>
#include "Resource/FontManager.hpp"
#include "Resource/MusicManager.hpp"
#include "Resource/TextureManager.hpp"

class ResourceManager {
public:
  ResourceManager(BackgroundImageRenderer *backgroundImageRendererPointer);
  ~ResourceManager();
  void listenerThread();
  bool isQueueEmpty();
  void update();
  int loadTexture(std::string fname, std::string name);
  Texture* getTexture(int id);
  Texture* getTexture(std::string name);
  TextureManager* getTextureManager();
  MusicManager* getMusicManager() {
    return musicManager;
  };
  FontManager* getFontManager() {
    return fontManager;
  }
  DatabaseConnection* getResourceDatabase() {
    return resourceDatabase;
  }
private:
  void processQueue();
  TextureManager *textureManager;
  MusicManager *musicManager;
  FontManager *fontManager;
  std::thread *resourceLoadThread;
  bool terminateLoadingThread;
  void loadResourcesFromDatabase();
  DatabaseConnection *resourceDatabase;
  BackgroundImageRenderer *backgroundImageRenderer;
  void threadFunction();
};

#endif
