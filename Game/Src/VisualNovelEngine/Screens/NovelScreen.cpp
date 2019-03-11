#include "Base/Engine.hpp"
#include "Database/DatabaseConnection.hpp"
#include "VisualNovelEngine/Classes/Data/Novel.hpp"
#include "VisualNovelEngine/Screens/NovelScreen.hpp"
#include "Misc/ColourBuilder.hpp"
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
  characterSpriteRenderer = engine->getCharacterSpriteRenderer();
  sceneTransitioning = false;

  textDisplay = new NovelTextDisplay(textRenderer, spriteRenderer, resourceManager);

  // User input bindings for this screen (TODO: Also react to gamepad and mouse input)
  advanceEventId = inputManager->bindKeyboardEvent("novel_screen_text_advance","return", true);
}

NovelScreen::~NovelScreen() {

}

void NovelScreen::start() {
  nextScene();
}

void NovelScreen::update() {

  // If a transition is going on, do nothing
  if (!backgroundTransitionRenderer->hasTransitionCompleted()) {
    return;
  }

  // If we need to advance the scene after a transition, do that once the transition has finished
  if (sceneTransitioning && backgroundTransitionRenderer->hasTransitionCompleted()) {
    nextScene();
    return;
  }

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
    transitionToNextScene();
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
    Character* character = novel->getCharacter(characterId-1);

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

  // TODO: use the scene transition id and colour stored with the scene in the database
  NovelScene *nextScene = novel->advanceToNextScene();

  sf::Color *colour = ColourBuilder::get(novel->getCurrentScene()->getStartTransitionColourId());
  backgroundTransitionRenderer->startTransition(BackgroundTransition::FADE_IN, *colour, 2000, 2000, 1000);
  backgroundTransitionRenderer->getCurrentTransition()->setToForeground();
  delete(colour);

  // TODO: Disable the UI for a period of time

  // Set the new background image
  backgroundImageRenderer->setBackground(nextScene->getBackgroundImageName());

  nextSegment();

  sceneTransitioning = false;
  textDisplay->setVisible();
}

void NovelScreen::transitionToNextScene() {

  // TODO: use the scene transition id and colour stored with the scene in the database
  sf::Color *colour = ColourBuilder::get(novel->getCurrentScene()->getEndTransitionColourId());
  backgroundTransitionRenderer->startTransition(BackgroundTransition::FADE_OUT, *colour, 2000, 2000, 1000);
  backgroundTransitionRenderer->getCurrentTransition()->setToForeground();
  delete(colour);
  textDisplay->setInvisible();
  textDisplay->clear();
  sceneTransitioning = true;
}
