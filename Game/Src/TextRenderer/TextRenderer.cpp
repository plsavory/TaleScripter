#include "SFML/Graphics.hpp"
#include <iostream>
#include "Database/DatabaseConnection.hpp"
#include "Resource/FontManager.hpp"
#include "TextRenderer/TextRenderer.hpp"

TextRenderer::TextRenderer(sf::RenderWindow *windowPointer, FontManager *fontManagerPointer) {
  window = windowPointer;
  fontManager = fontManagerPointer;

  for (int i = 0; i < MAX_TEXT_OBJECTS; i++) {
    text[i] = NULL;
  }

  retryLoadClock = new sf::Clock();

}

TextRenderer::~TextRenderer() {

  for (int i = 0; i < MAX_TEXT_OBJECTS; i++) {
    if (text[i]) {
      delete(text[i]);
    }
  }

}

Text* TextRenderer::addText(std::string name, std::string font) {

  for (int i = 0; i < MAX_TEXT_OBJECTS; i++) {
    if (!text[i]) {
      text[i] = new Text(name, font, fontManager);
      return text[i];
    }
  }

  return NULL;
}

void TextRenderer::update() {
  // Retry any font setting where the font wasn't loaded before
  if (retryLoadClock->getElapsedTime().asMilliseconds() < RETRY_LOADING_TIME) {
    return;
  }

  for (int i = 0; i < MAX_TEXT_OBJECTS; i++) {
    if (text[i]) {
      text[i]->reAttemptLoad();
    }
  }

  retryLoadClock->restart();
}

void TextRenderer::draw() {

  for (int i = 0; i < MAX_TEXT_OBJECTS; i++) {
    if (text[i]) {
      window->draw(*text[i]->getTextObject());
    }
  }

}

FontManager* TextRenderer::getFontManager() {
  return fontManager;
}
