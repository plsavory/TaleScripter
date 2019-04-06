#ifndef RESOURCE_RESOURCE_MANAGER_INCLUDED
#define RESOURCE_RESOURCE_MANAGER_INCLUDED

#include <iostream>
#include <thread>
#include "Exceptions/ResourceException.hpp"
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

    Texture *getTexture(int id);

    Texture *getTexture(std::string name);

    TextureManager *getTextureManager();

    MusicManager *getMusicManager() {
        return musicManager;
    };

    FontManager *getFontManager() {
        return fontManager;
    }

    DatabaseConnection *getResourceDatabase() {
        return resourceDatabase;
    }

    void loadResourcesFromDatabase();

private:
    void processQueue();

    TextureManager *textureManager;
    MusicManager *musicManager;
    FontManager *fontManager;
    std::thread *resourceLoadThread;
    bool terminateLoadingThread;
    DatabaseConnection *resourceDatabase;
    BackgroundImageRenderer *backgroundImageRenderer;
    void openDatabase();

    void threadFunction();
};

#endif
