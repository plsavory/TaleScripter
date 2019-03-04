#ifndef RESOURCE_TEXTURE_MANAGER_INCLUDED
#define RESOURCE_TEXTURE_MANAGER_INCLUDED

#include <iostream>
#include <queue>
#include <SFML/Graphics.hpp>

#define MAX_TEXTURES 0xFF

struct Texture {
public:
  Texture(std::string assignableName) {
    texture = new sf::Texture();

    assign(assignableName);
  };
  ~Texture() {
    delete texture;
  };
  void assign(std::string assignableName) {
    name = assignableName;
    loaded = false;
  }
  std::string name;
  sf::Texture *texture;
  bool loaded;
};

struct TextureLoadRequest {
public:
  TextureLoadRequest(std::string fname, int textureId) {
    filename = fname;
    id = textureId;
  }
  int getId() {
    return id;
  }
  std::string getFilename() {
    return filename;
  }
private:
  std::string filename;
  std::string name;
  int id;
};

class TextureManager {
public:
  TextureManager();
  ~TextureManager();
  int loadTexture(std::string fname, std::string name);
  int findTexture(std::string name);
  Texture* getTexture(int id);
  Texture* getTexture(std::string name);
  bool isQueueEmpty();
  void processQueue();
  void loadAllFromDatabase(DatabaseConnection *resource);
private:
  std::queue<TextureLoadRequest> textureLoadQueue;
  Texture *texture[MAX_TEXTURES];
  void assignTexture(std::string fname, std::string name, int id);
  DatabaseConnection *resource;
};

#endif
