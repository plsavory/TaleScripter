#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"
#include "Input/InputManager.hpp"
#include "Database/DatabaseConnection.hpp"
#include "Resource/ResourceManager.hpp"
#include "Base/Renderers.hpp"
#include "VisualNovelEngine/Classes/Data/Novel.hpp"
#include "VisualNovelEngine/Screens/NovelScreen.hpp"
#include <iostream>

// Objects used on this screen
#include "VisualNovelEngine/Classes/UI/NovelTextDisplay.hpp"

NovelScreen::NovelScreen(sf::RenderWindow *windowPointer,ResourceManager *rManager, SpriteRenderer *sRenderer, TextRenderer *tRenderer, InputManager *iManager, NovelData *novelPointer) {
  window = windowPointer;
  spriteRenderer = sRenderer;
  resourceManager = rManager;
  textRenderer = tRenderer;
  inputManager = iManager;
  novel = novelPointer;

  textDisplay = new NovelTextDisplay(tRenderer);

  // User input bindings for this screen (TODO: Also react to gamepad and mouse input)
  advanceEventId = inputManager->bindKeyboardEvent("novel_screen_text_advance","return", true);
}

NovelScreen::~NovelScreen() {

}

void NovelScreen::start() {
  nextLine();
}

void NovelScreen::update() {
  textDisplay->update();

  // TODO: Allow for an auto mode which doesn't need user input
  if (inputManager->isEventPressed(advanceEventId)) {

    // If text display has not drawn all characters yet, force it to draw them all
    if (!textDisplay->hasTextFinished()) {
      textDisplay->displayWholeStringImmediately();
      return;
    }

    // Otherwise, load the next part of the novel
    advance();
  }
}

void NovelScreen::draw() {

}

/**
 * [NovelScreen::advance Advance the story]
 */
void NovelScreen::advance() {
  switch (novel->getNextAction()) {
    case NextLine:
    nextLine();
    break;
    default:
    break;
  }
}

void NovelScreen::nextLine() {

  NovelSceneSegmentLine *nextLine = novel->getNextLine();
  textDisplay->setText(nextLine->getText());
  
}
