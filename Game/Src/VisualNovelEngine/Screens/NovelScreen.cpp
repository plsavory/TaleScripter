#include "Base/Engine.hpp"
#include "Database/DatabaseConnection.hpp"
#include "VisualNovelEngine/Classes/Data/Novel.hpp"
#include "VisualNovelEngine/Screens/NovelScreen.hpp"
#include <iostream>

// Objects used on this screen
#include "VisualNovelEngine/Classes/UI/NovelTextDisplay.hpp"

NovelScreen::NovelScreen(Engine *enginePointer, NovelData *novelPointer) {
  engine = enginePointer;
  window = engine->getWindow();
  spriteRenderer = engine->getSpriteRenderer();
  resourceManager = engine->getResourceManager();
  textRenderer = engine->getTextRenderer();
  inputManager = engine->getInputManager();
  novel = novelPointer;
  musicManager = resourceManager->getMusicManager();
  backgroundImageRenderer = engine->getBackgroundImageRenderer();
  backgroundTransitionRenderer = engine->getBackgroundTransitionRenderer();

  textDisplay = new NovelTextDisplay(textRenderer, spriteRenderer, resourceManager);

  // User input bindings for this screen (TODO: Also react to gamepad and mouse input)
  advanceEventId = inputManager->bindKeyboardEvent("novel_screen_text_advance","return", true);
}

NovelScreen::~NovelScreen() {

}

void NovelScreen::start() {
  nextScene();

  // Wait 5 seconds before displaying anything and fade in from black
  backgroundTransitionRenderer->startTransition(BackgroundTransition::FADE_IN, sf::Color(0,0,0,255), 2000, 2000, 1000);
  backgroundTransitionRenderer->getCurrentTransition()->setToForeground();
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

  // TODO: Handle transition animations between scenes when a transition has been set by the user
  switch (novel->getNextAction()) {
    case AdvanceState::NextLine:
    nextLine();
    break;
    case AdvanceState::SceneSegmentEnd:
    nextSegment();
    break;
    case AdvanceState::SceneEnd:
    nextScene();
    break;
    default:
    break;
  }
}

void NovelScreen::nextLine() {

  NovelSceneSegmentLine *nextLine = novel->getNextLine();

  // If the next line has a character or override name attached to it, use that as the character name
  int characterId = nextLine->getCharacterId();

  std::string characterName("");

  if (!nextLine->getOverrideCharacterName().empty()) {
    characterName = nextLine->getOverrideCharacterName();
  }

  if (characterId > 0 && characterName.empty()) {
    Character* character = novel->getCharacter(characterId);

    if (character) {
      characterName = character->getFirstName();
    }
  }

  textDisplay->setText(nextLine->getText(), characterName);

}

void NovelScreen::nextSegment() {

  NovelSceneSegment *nextSegment = novel->advanceToNextSegment();

  // Play the music file related to the scene segment
  musicManager->playAudioStream(nextSegment->getBackgroundMusicName());

  nextLine();

}

void NovelScreen::nextScene() {
  NovelScene *nextScene = novel->advanceToNextScene();

  // TODO: Disable the UI for a period of time

  // Set the new background image
  backgroundImageRenderer->setBackground(nextScene->getBackgroundImageName());

  nextSegment();
}
