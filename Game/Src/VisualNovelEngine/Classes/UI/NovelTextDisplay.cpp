#include <iostream>
#include "SFML/Graphics.hpp"
#include "Database/DatabaseConnection.hpp"
#include "Resource/FontManager.hpp"
#include "TextRenderer/TextRenderer.hpp"
#include "VisualNovelEngine/Classes/UI/NovelTextDisplay.hpp"
#include <sstream>
#include <vector>
#include "Misc/Utils.hpp"

NovelTextDisplay::NovelTextDisplay(TextRenderer *tRenderer) {

  textRenderer = tRenderer;
  fontManager = tRenderer->getFontManager();

  textCounterDelay = 35;
  textCounterClock = new sf::Clock();

  currentDisplayText = "";
  fullDisplayText = "";

  storyFont = "story_font"; // TODO: Load from config

  myText = textRenderer->addText("novel_text_display_text", storyFont);
  myText->setPosition(10,400);

  maxTextWidth = 1280;
}

NovelTextDisplay::~NovelTextDisplay() {

}

void NovelTextDisplay::setText(std::string newText) {
  newText = wordWrap(newText, maxTextWidth);
  currentDisplayText = "";
  fullDisplayText = newText;
}

void NovelTextDisplay::update() {

  if (textCounterClock->getElapsedTime().asMilliseconds() < textCounterDelay) {
    return;
  }

  textCounterClock->restart();

  if (hasTextFinished()) {
    return;
  }

  // Append to currentDisplayText character by character
  char characterChar = fullDisplayText.at(currentDisplayText.length());
  std::string character;

  std::stringstream ss;
  ss << characterChar;
  ss >> character;

   // >> operator skips spaces and newlines, so screw me, right?
  switch (characterChar) {
    case 0x20:
    currentDisplayText.append(" ");
    break;
    case 0x0A:
    currentDisplayText.append("\n");
    break;
    case 0x09:
    currentDisplayText.append("\t");
    break;
    default:
    currentDisplayText.append(character);
    break;
  }

  myText->setString(currentDisplayText);

}

bool NovelTextDisplay::hasTextFinished() {
  return (currentDisplayText.length() == fullDisplayText.length());
}

void NovelTextDisplay::displayWholeStringImmediately() {
  currentDisplayText = fullDisplayText;
  myText->setString(currentDisplayText);
}

/**
 * [NovelTextDisplay::wordWrap Inserts newlines into a string until it fits within the specified width]
 */
std::string NovelTextDisplay::wordWrap(std::string textToWrap, float maxWidth) {

  std::string stringToProcess = fullDisplayText;

  sf::Text *text = new sf::Text();
  Font *font = fontManager->getFont("story_font");

  text->setFont(*font->getFont());
  text->setString(textToWrap);

  // Calculate how large the string is going to appear on the screen
  sf::FloatRect textSize = text->getGlobalBounds();

  // Split the string into an array of strings containing each word
  std::vector<std::string> splitString = Utils::explodeString(textToWrap, ' ');

  while (textSize.width > maxWidth) {

    sf::Text *firstWordFindText = new sf::Text();
    firstWordFindText->setFont(*font->getFont());
    std::string firstWordFindTextString;

    // Find the first word which will be drawn off-screen
    unsigned int count = 0;

    while (firstWordFindText->getGlobalBounds().width < maxWidth && count < splitString.size()) {
      firstWordFindTextString = Utils::implodeString(splitString, " ", (++count));
      firstWordFindText->setString(firstWordFindTextString);
    }

    // Insert a newline at the point where count is currently at (this will be the first word that would otherwise be drawn off-screen)
    splitString.insert(splitString.begin()+(count-1), "\n");
    text->setString(Utils::implodeString(splitString," ", 0));

    textSize = text->getGlobalBounds();

    delete firstWordFindText;
  }

  delete text;

  return Utils::implodeString(splitString," ", 0);
}
