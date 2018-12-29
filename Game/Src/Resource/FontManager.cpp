#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"
#include <queue>
#include "Resource/FontManager.hpp"

FontManager::FontManager() {
  // Null-out all font slots
  for (int i = 0; i < MAX_FONTS; i++) {
    font[i] = NULL;
  }

  loadFontImmediately = true;
}

FontManager::~FontManager() {
  for (int i = 0; i < MAX_FONTS; i++) {
    if (font[i]) {
      delete font[i];
    }
  }
}

/**
 * [FontManager::addFont Creates a font]
 * @param name     [Name of the font]
 * @param filename [Filename]
 * @return [Font if successful, null otherwise]
 */
Font* FontManager::addFont(std::string name, std::string filename) {


  for (int i = 0; i < MAX_FONTS; i++) {
    if (!font[i]) {
      font[i] = new Font(name, filename);

      // Immediately load the font data if required
      if (loadFontImmediately) {
        loadFont(i);
      }

      return font[i];
    }
  }

  return NULL;

}

void FontManager::loadFont(std::string name) {

  int fontId = findFont(name);

  loadFont(fontId);

}

void FontManager::loadFont(int id) {

  // Handle "font not found" errors
  if (id < 0) {
    return;
  }

  fontLoadQueue.push(FontLoadRequest(id));

}

void FontManager::processQueue() {

  if (fontLoadQueue.size() == 0) {
    return;
  }

  font[fontLoadQueue.front().fontId]->loadFont();

  fontLoadQueue.pop();

}

bool FontManager::isQueueEmpty() {
  return (fontLoadQueue.size() == 0);
}

void FontManager::loadAllFromDatabase() {

}

Font* FontManager::getFont(std::string name) {
  int fontId = findFont(name);

  if (fontId < 0) {
    return NULL;
  }

  return font[fontId];
}

int FontManager::findFont(std::string name) {
  for (int i = 0; i < MAX_FONTS; i++) {
    if (font[i]) {
      if (font[i]->getName() == name) {
        return i;
      }
    }
  }

  return -1;
}
