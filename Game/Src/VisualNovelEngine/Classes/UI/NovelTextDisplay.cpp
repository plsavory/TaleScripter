#include <iostream>
#include "SFML/Graphics.hpp"
#include "Database/DatabaseConnection.hpp"
#include "Resource/FontManager.hpp"
#include "TextRenderer/TextRenderer.hpp"
#include "VisualNovelEngine/Classes/UI/NovelTextDisplay.hpp"
#include <sstream>

NovelTextDisplay::NovelTextDisplay(TextRenderer *tRenderer) {

  textRenderer = tRenderer;

  textCounterDelay = 35;
  textCounterClock = new sf::Clock();

  currentDisplayText = "";
  fullDisplayText = "";

  storyFont = "story_font"; // TODO: Load from config

  myText = textRenderer->addText("novel_text_display_text", storyFont);
  myText->setPosition(10,400);
}

NovelTextDisplay::~NovelTextDisplay() {

}

void NovelTextDisplay::setText(std::string newText) {
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

  if (characterChar == 32) {
    currentDisplayText.append(" "); // >> operator skips spaces, so screw me, right?
    // TODO: Find out if there's any other characters that might need special handling here
  } else {
    currentDisplayText.append(character);
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
