#include <SFML/Graphics.hpp>
#include <iostream>
#include "Database/DatabaseConnection.hpp"
#include "Exceptions/ResourceException.hpp"
#include "Resource/FontManager.hpp"
#include "TextRenderer/TextRenderer.hpp"

TextRenderer::TextRenderer(sf::RenderWindow *windowPointer, FontManager *fontManagerPointer) {
    window = windowPointer;
    fontManager = fontManagerPointer;
    retryLoadClock = new sf::Clock();
}

TextRenderer::~TextRenderer() {

    for (auto &currentText: text) {
        if (currentText) {
            delete (currentText);
        }
    }

}

/**
 * Adds a piece of text to the text renderer
 * @param name - Name of the new piece of text, can be used to retrieve or delete it later
 * @param font - The name of the font, must be a font already loaded in FontManager
 * @return - Pointer to the new text object, can be manually drawn from another object if the automatic draw flag is unset
 */
Text *TextRenderer::addText(const std::string &name, const std::string &font) {
    text.push_back(new Text(name, font, fontManager));
    return text.back();
}

/**
 * Removes a piece of text from the renderer
 * @param name
 */
void TextRenderer::removeText(const std::string &name) {
    // TODO: Test that this actually works...
    for (int i = 0; i < text.size(); i++) {
        if (text[i]->getName() == name) {
            delete (text[i]);
            text.erase(text.begin() + i);
        }
    }
}

void TextRenderer::update() {
    // Retry any font setting where the font wasn't loaded before
    if (retryLoadClock->getElapsedTime().asMilliseconds() < RETRY_LOADING_TIME) {
        return;
    }

    for (auto &currentText : text) {
        currentText->reAttemptLoad();
    }

    retryLoadClock->restart();
}

void TextRenderer::draw() {

    for (auto &currentText : text) {

        if (!currentText->isVisible() || !currentText->automaticDrawEnabled()) {
            continue;
        }

        window->draw(*currentText->getTextObject());
    }

}

FontManager *TextRenderer::getFontManager() {
    return fontManager;
}
