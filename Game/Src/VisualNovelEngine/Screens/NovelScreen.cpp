#include "Base/Engine.hpp"
#include "Database/DatabaseConnection.hpp"
#include "VisualNovelEngine/Classes/Data/Novel.hpp"
#include "UI/CommonUI.h"
#include "VisualNovelEngine/Screens/NovelScreen.hpp"
#include "Misc/ColourBuilder.hpp"
#include <iostream>
#include "VisualNovelEngine/Classes/Data/VisualNovelEngineConstants.hpp"

// Objects used on this screen
#include "VisualNovelEngine/Classes/UI/NovelTextDisplay.hpp"

NovelScreen::NovelScreen(Engine *enginePointer, NovelData *novelPointer, CommonUI *cUI) {
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
    commonUI = cUI;

    textDisplay = new NovelTextDisplay(textRenderer, spriteRenderer, resourceManager, commonUI);

    // User input bindings for this screen (TODO: Also react to gamepad input)
    advanceEventId = inputManager->bindKeyboardEvent("novel_screen_text_advance", "return", true);
    advanceMouseEvent = inputManager->getMouseHandler()->addEvent("novel_screen_text_advance",
                                                                  MouseEventType::LeftClick);
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

    if (!characterSpriteRenderer->isDoingNothing()) {
        return;
    }

    textDisplay->update();

    // TODO: Allow for an auto mode which doesn't need user input
    if (inputManager->isEventPressed(advanceEventId) || advanceMouseEvent->conditionsMet()) {

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

    std::string characterName;

    if (!nextLine->getOverrideCharacterName().empty()) {
        characterName = nextLine->getOverrideCharacterName();
    }

    if (characterId > 0 && characterName.empty()) {
        Character *character = novel->getCharacter(characterId - 1);

        if (character) {
            characterName = character->getFirstName();
        }
    }

    // Handle character sprite drawing
    CharacterStateGroup *characterStateGroup = nextLine->getCharacterStateGroup();
    if (characterStateGroup) {
        std::vector<CharacterState *> states = characterStateGroup->getCharacterStates();

        std::vector<CharacterSpriteDrawRequest *> requests;

        for (unsigned int i = 0; i < states.size(); i++) {
            requests.push_back(new CharacterSpriteDrawRequest(states[i]->getCharacterSprite()));
        }

        characterSpriteRenderer->push(requests);

        // Delete the now-unneeded data
        for (unsigned int i = 0; i < requests.size(); i++) {
            delete (requests[i]);
        }

    }

    textDisplay->setText(nextLine->getText(), characterName);
}

void NovelScreen::nextSegment() {

    NovelSceneSegment *nextSegment = novel->advanceToNextSegment();

    // Play the music file related to the scene segment
    MusicPlaybackRequest *musicPlaybackRequest = nextSegment->getMusicPlaybackRequest();
    if (musicPlaybackRequest) {
        MusicPlaybackRequestMetadata *metadata = musicPlaybackRequest->getMetadata();

        if (metadata) {
            musicManager->playAudioStream(musicPlaybackRequest->getMusicName(), metadata);
        } else {
            musicManager->playAudioStream(musicPlaybackRequest->getMusicName());
        }
    }

    nextLine();

}

/**
 * Called when the novel starts running or at the end of a scene transition to start the next scene
 * Performs the start transition of a scene and processes the first scene segment of that scene
 */
void NovelScreen::nextScene() {

    // TODO: use the scene transition id and colour stored with the scene in the database
    NovelScene *nextScene = novel->advanceToNextScene();

    sf::Color *colour = ColourBuilder::get(novel->getCurrentScene()->getStartTransitionColourId());

    bool needsToFadeIn = true;
    bool instantBackgroundChange = false;

    // We have to do some extra handling here to stop the background from fading in when the previous scene did a morph or instant transition
    if (novel->getPreviousScene()) {
        int previousBackgroundTransition = novel->getPreviousScene()->getEndTransitionTypeId();
        needsToFadeIn = previousBackgroundTransition != BackgroundTransitionType::BACKGROUND_TRANSITION_TYPE_MORPH &&
                        previousBackgroundTransition != BackgroundTransitionType::BACKGROUND_TRANSITION_TYPE_INSTANT;

        if (previousBackgroundTransition == BackgroundTransitionType::BACKGROUND_TRANSITION_TYPE_INSTANT) {
            backgroundImageRenderer->setBackground(nextScene->getBackgroundImageName());
        }
    }

    if (needsToFadeIn) {
        backgroundTransitionRenderer->startTransition(BackgroundTransition::FADE_IN, *colour, 2000, 2000, 1000);
        backgroundTransitionRenderer->getCurrentTransition()->setToForeground();
        backgroundImageRenderer->setBackground(nextScene->getBackgroundImageName());
    }

    delete (colour);

    nextSegment();

    sceneTransitioning = false;
    textDisplay->setVisible();
}

/**
 * Starts a transition to the next scene - Performs a morph or fade-out transition if it needs to.
 */
void NovelScreen::transitionToNextScene() {

    sf::Color *colour = ColourBuilder::get(novel->getCurrentScene()->getEndTransitionColourId());

    // Calculate that the next scene will be
    // This is easy now, but in the future we won't always be transitioning between scenes in a linear fashion
    // TODO: Move this code into a "changeScene" function which can be called by this one with the arguments to get the next scene
    NovelScene *nextScene = novel->getCurrentChapter()->getScene(novel->getCurrentSceneIndex() + 1);

    // Figure out which type of start transition we need to use
    switch (novel->getCurrentScene()->getEndTransitionTypeId()) {
        case BackgroundTransitionType::BACKGROUND_TRANSITION_TYPE_MORPH:
            backgroundImageRenderer->setUpcomingBackground(nextScene->getBackgroundImageName());
            backgroundTransitionRenderer->startTransition(BackgroundTransition::MORPH, *colour, 2000, 2000, 1000);
            break;
        case BackgroundTransitionType::BACKGROUND_TRANSITION_TYPE_FADE:
            backgroundTransitionRenderer->startTransition(BackgroundTransition::FADE_OUT, *colour, 2000, 2000, 1000);
            backgroundTransitionRenderer->getCurrentTransition()->setToForeground();
            break;
        case BackgroundTransitionType::BACKGROUND_TRANSITION_TYPE_INSTANT:
            // Do nothing - Background image will be changed when the next scene starts
            break;
        default:
            std::vector<std::string> error = {
                    "Unknown background transition type: ",
                    std::to_string(novel->getCurrentScene()->getEndTransitionTypeId())
            };
            throw GeneralException(Utils::implodeString(error));
    }

    delete (colour);
    textDisplay->setInvisible();
    textDisplay->clear();
    sceneTransitioning = true;
}
