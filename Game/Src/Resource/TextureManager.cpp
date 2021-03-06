#include <iostream>
#include "Database/DatabaseConnection.hpp"
#include "Resource/TextureManager.hpp"

/**
 * [TextureManager::TextureManager Usual nullptr initialisation so that if statements will work]
 */
TextureManager::TextureManager() {

  for (int i = 0; i<MAX_TEXTURES; i++) {
    texture[i] = nullptr;
  }

}

/**
 * [TextureManager Delete all textures when self is deleted.]
 */
TextureManager::~TextureManager() {

  for (auto & currentTexture : texture) {
    if (currentTexture) {
      delete currentTexture;
    }
  }

}

/**
 * [TextureManager::loadTexture Creates a texture load request]
 * @param  fname     [Filename of the texture]
 * @param  name      [Name of the texture]
 * @return           [ID of the texture, -1 on failure]
 */
int TextureManager::loadTexture(const std::string& fname, const std::string& name) {

  // Find a texture with the same name if it exists
  int existingTextureId = findTexture(name);

  if (existingTextureId >= 0) {
    assignTexture(fname, name, existingTextureId);
    return existingTextureId;
  }

  // Find an empty texture slot
  for (auto i = 0; i<MAX_TEXTURES; i++) {
    if (!texture[i]) {
      assignTexture(fname, name, i);
      return i;
    }
  }

  // TODO: Store textures in a vector so that this never happens.
  std::vector<std::string> errorMessage = {
          "Error loading texture: Max number of textures (", std::to_string(MAX_TEXTURES),") are already loaded."
  };

  throw ResourceException(Utils::implodeString(errorMessage));

}

/**
 * [TextureManager::assignTexture Forces a load on the texture with ID]
 * @param fname [filename of the texture]
 * @param name  [accessible name]
 * @param id    [ID slot to use]
 */
void TextureManager::assignTexture(const std::string& fname, const std::string& name, int id) {

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
int TextureManager::findTexture(const std::string& name) {

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

  if (textureLoadQueue.empty()) {
    return;
  }

  // Load the texture into memory
  texture[textureLoadQueue.front().getId()]->loadFromFile(textureLoadQueue.front().getFilename());
  texture[textureLoadQueue.front().getId()]->loaded = true;

  // Remove the texture load request from memory
  textureLoadQueue.pop();

}

/**
 * [TextureManager::isQueueEmpty Check if the texture load queue is empty]
 * @return [True if queue is empty, otherwise false]
 */
bool TextureManager::isQueueEmpty() {
  return (textureLoadQueue.empty());
}

/**
 * [TextureManager::getTexture Gets the texture at the given texture array slot]
 * @param  id [The ID of the texture]
 * @return    [Texture object pointer]
 */
Texture* TextureManager::getTexture(int id) {
  return (texture[id]);
}

/**
 * [TextureManager::getTexture Returns a texture with the given name
 * Is slower than getting it directly through the ID so this should really
 * only be used once to get a pointer to it]
 * @param  name [Name of the texture]
 * @return      [Success: Texture object pointer
 *               Failure: nullptr]
 */
Texture* TextureManager::getTexture(const std::string& name) {
  int textureId = findTexture(name);

  if (textureId >= 0) {
    return texture[textureId];
  }

  return nullptr;
}

/**
 * [TextureManager::loadAllFromDatabase Reads all textures from the database and creates the neccesary class instances]
 */
void TextureManager::loadAllFromDatabase(DatabaseConnection *resource) {
  /*
  TODO: Memory manage this so that we don't just have all of them loaded.
  This is outside of the scope of this class. Once we have the memory management, we can
  create all of the class instances but don't set the images to load into memory immediately by default.
  For instance, for every chapter or scene, we can load or unload textures for character sprites as required if they appear in that scene/chapter or not.
  */

 auto *dataSet = new DataSet();
 resource->executeQuery("SELECT * FROM textures;", dataSet);

 int numberOfTextures = dataSet->getRowCount();

 for (int i = 0; i < numberOfTextures; i++) {
   std::string name = dataSet->getRow(i)->getColumn("name")->getRawData();
   std::string fileName = "resource/textures/";
   fileName.append(dataSet->getRow(i)->getColumn("filename")->getRawData());
   loadTexture(fileName, name);
 }
}
