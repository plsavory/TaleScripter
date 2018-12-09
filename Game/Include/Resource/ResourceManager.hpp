#ifndef RESOURCE_RESOURCE_MANAGER_INCLUDED
#define RESOURCE_RESOURCE_MANAGER_INCLUDED

#include <iostream>
#include <thread>
#include "Resource/TextureManager.hpp"

class ResourceManager {
public:
  ResourceManager();
  ~ResourceManager();
  void listenerThread();
  bool isQueueEmpty();
  void update();
  int loadTexture(std::string fname, std::string name);
  Texture* getTexture(int id);
  Texture* getTexture(std::string name);
private:
  void processQueue();
  TextureManager *textureManager;
  std::thread *resourceLoadThread;
  bool terminateLoadingThread;
};

#endif
