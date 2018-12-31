#include <iostream>
#include "SFML/Graphics.hpp"
#include "Database/DatabaseConnection.hpp"
#include "Resource/FontManager.hpp"
#include "TextRenderer/TextRenderer.hpp"
#include "VisualNovelEngine/Classes/UI/NovelTextDisplay.hpp"
#include <sstream>

NovelTextDisplay::NovelTextDisplay(TextRenderer *tRenderer) {

  textRenderer = tRenderer;

  textCounterDelay = 50;
  textCounterClock = new sf::Clock();

  currentDisplayText = "";
  fullDisplayText = "This is a test, hopefully things are beginning to work and appear on screen now.";

  storyFont = "story_font"; // TODO: Load from config

  myText = textRenderer->addText("novel_text_display_text", "story_font");
  myText->setPosition(10,400);
}

NovelTextDisplay::~NovelTextDisplay() {

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
