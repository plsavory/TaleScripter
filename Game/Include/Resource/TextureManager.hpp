#ifndef RESOURCE_TEXTURE_MANAGER_INCLUDED
#define RESOURCE_TEXTURE_MANAGER_INCLUDED

#include <iostream>
#include <queue>
#include <SFML/Graphics.hpp>
#include "Exceptions/ResourceException.hpp"

#define MAX_TEXTURES 0xFF

struct Texture {
public:
  explicit Texture(const std::string& assignableName) {
    texture = new sf::Texture();
    loaded = false;

    assign(assignableName);
  };
  ~Texture() {
    delete texture;
  };
  void assign(const std::string& assignableName) {
    name = assignableName;
    loaded = false;
  }
  void loadFromFile(const std::string& fileName) {
      if (!texture->loadFromFile(fileName)) {
          std::vector<std::string> errorMessage = {
                  "Unable to load texture 'name' (File: '", fileName, "' is either missing, in the wrong format or corrupted)"
          };

          throw ResourceException(Utils::implodeString(errorMessage));
      };
  }
  std::string name;
  sf::Texture *texture;
  bool loaded;
};

struct TextureLoadRequest {
public:
  TextureLoadRequest(const std::string& fname, int textureId) {
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
  int loadTexture(const std::string& fname, const std::string& name);
  int findTexture(const std::string& name);
  Texture* getTexture(int id);
  Texture* getTexture(const std::string& name);
  bool isQueueEmpty();
  void processQueue();
  void loadAllFromDatabase(DatabaseConnection *resource);
private:
  std::queue<TextureLoadRequest> textureLoadQueue;
  Texture *texture[MAX_TEXTURES];
  void assignTexture(const std::string& fname, const std::string& name, int id);
  DatabaseConnection *resource;
};

#endif
