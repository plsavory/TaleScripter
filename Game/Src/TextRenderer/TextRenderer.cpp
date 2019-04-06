#include <SFML/Graphics.hpp>
#include <iostream>
#include "Database/DatabaseConnection.hpp"
#include "Exceptions/ResourceException.hpp"
#include "Resource/FontManager.hpp"
#include "TextRenderer/TextRenderer.hpp"

TextRenderer::TextRenderer(sf::RenderWindow *windowPointer, FontManager *fontManagerPointer) {
  window = windowPointer;
  fontManager = fontManagerPointer;

  for (auto & currentText : text) {
    currentText = nullptr;
  }

  retryLoadClock = new sf::Clock();

}

TextRenderer::~TextRenderer() {

  for (auto & currentText: text) {
    if (currentText) {
      delete(currentText);
    }
  }

}

Text* TextRenderer::addText(const std::string& name, const std::string& font) {

  for (auto & currentText : text) {
    if (!currentText) {
      currentText = new Text(name, font, fontManager);
      return currentText;
    }
  }

  // TODO: Store text objects in a vector so that this situation never happens.
  std::vector<std::string> errorMessage = {
          "Could not add text '", name, "' to the text renderer, the maximum number of text objects have already been added."
  };

  throw ResourceException(Utils::implodeString(errorMessage));
}

void TextRenderer::update() {
  // Retry any font setting where the font wasn't loaded before
  if (retryLoadClock->getElapsedTime().asMilliseconds() < RETRY_LOADING_TIME) {
    return;
  }

  for (auto & currentText : text) {
    if (currentText) {
      currentText->reAttemptLoad();
    }
  }

  retryLoadClock->restart();
}

void TextRenderer::draw() {

  for (auto & currentText : text) {
    if (currentText) {

      if (!currentText->isVisible()) {
        continue;
      }

      window->draw(*currentText->getTextObject());
    }
  }

}

FontManager* TextRenderer::getFontManager() {
  return fontManager;
}
