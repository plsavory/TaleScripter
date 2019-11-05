#include "Base/Engine.hpp"
#include "Database/DatabaseConnection.hpp"
#include "VisualNovelEngine/Classes/Data/Novel.hpp"
#include "ScreenState.h"
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
    gameState = engine->getGameState();

    textDisplay = new NovelTextDisplay(textRenderer, spriteRenderer, resourceManager, commonUI, gameState);

    // User input bindings for this screen (TODO: Also react to gamepad input)
    advanceEventId = inputManager->bindKeyboardEvent("novel_screen_text_advance", "return", true);
    advanceMouseEvent = inputManager->getMouseHandler()->addEvent("novel_screen_text_advance",
                                                                  MouseEventType::LeftClick);
    novelScreenSaveKeyId = inputManager->bindKeyboardEvent("novel_screen_save", "S", true);

    isFirstChange = true;

}

NovelScreen::~NovelScreen() = default;

void NovelScreen::start() {
    isFirstChange = true;
    novel->start();
    nextScene();
}

/**
 * Load a game and set the novel's progress to it
 * @param gameSave
 */
void NovelScreen::start(DataSet *gameSave) {
    isFirstChange = true;

    // TODO: Set any variables stored within the game save relating to decisions and scripts when we are supporting these.

    // TODO: Get the position in the track that the music was currently at when the game was saved

    // Get the information required to set our position in the novel
    auto *novelProgressInformation = new DataSet();

    std::string queryString = Utils::implodeString({
                   "SELECT",
                   "sl.id AS segment_line_id,",
                   "ss.id AS scene_segment_id,",
                   "s.id AS scene_id,",
                   "c.id AS chapter_id",
                   "FROM segment_lines sl",
                   "INNER JOIN scene_segments ss ON ss.id = sl.scene_segment_id",
                   "INNER JOIN scenes s ON s.id = ss.scene_id",
                   "INNER JOIN chapters c ON c.id = s.chapter_id",
                   "WHERE sl.id = ?"
           }, " ");

    novel->getNovelDatabase()->execute(queryString, novelProgressInformation, {gameSave->getRow(0)->getColumn("segment_line_id")->getData()->asString()}, {DatabaseConnection::TYPE_TEXT});

    if (!novelProgressInformation->getRowCount()) {
        throw GeneralException(Utils::implodeString({"No segment line with id ", gameSave->getRow(0)->getColumn("segment_line_id")->getData()->asString(), " could be found."}));
    }

    novel->start(novelProgressInformation);

    // Fetch the actual objects to pass into the NovelScreen functions, as we will need to do some processing at each step of restoring the player's progress.
    // TODO: Make these functions attempt to load the data related to a scene if it isn't currently loaded (We are currently not memory handling/purging anything)
    NovelChapter *chapter = novel->getCurrentChapter(); // TODO: Support multiple chapters.
    NovelScene *scene = chapter->getSceneUsingDatabaseId(novelProgressInformation->getRow(0)->getColumn("scene_id")->getData()->asInteger());
    NovelSceneSegment *sceneSegment = scene->getSceneSegmentUsingDatabaseId(novelProgressInformation->getRow(0)->getColumn("scene_segment_id")->getData()->asInteger());
    NovelSceneSegmentLine *sceneSegmentLine = sceneSegment->getLineUsingDatabaseId(novelProgressInformation->getRow(0)->getColumn("segment_line_id")->getData()->asInteger());

    setScene(scene, false);
    setSceneSegment(sceneSegment, false);
    setSceneSegmentLine(sceneSegmentLine);

    sceneTransitioning = false;
    textDisplay->setVisible();
    isFirstChange = false;

    delete(novelProgressInformation);
    delete(gameSave);

    // Set the background to instantly match the current scene
    backgroundImageRenderer->setBackground(novel->getCurrentScene()->getBackgroundImageName());
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

    // Listen for the save key being pressed and display the save menu when needed.
    if (inputManager->isEventPressed(novelScreenSaveKeyId)) {
        commonUI->showSaveMenu();
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

    setSceneSegmentLine(nextLine);
}

void NovelScreen::nextSegment() {

    NovelSceneSegment *nextSegment = novel->advanceToNextSegment();
    setSceneSegment(nextSegment, true);

}

/**
 * Called when the novel starts running or at the end of a scene transition to start the next scene
 * Performs the start transition of a scene and processes the first scene segment of that scene
 */
void NovelScreen::nextScene() {

    // TODO: use the scene transition id and colour stored with the scene in the database
    NovelScene *nextScene = novel->advanceToNextScene();
    setScene(nextScene, true);
}

/**
 * Sets the screen to match what the given scene represents.
 * @param scene
 * @param setSegment - Whether to set the scene segment to that at the start of the scene or not.
 */
void NovelScreen::setScene(NovelScene *scene, bool setSegment) {

    sf::Color *colour = ColourBuilder::get(novel->getCurrentScene()->getStartTransitionColourId());

    bool needsToFadeIn = true;
    bool instantBackgroundChange = false;

    // We have to do some extra handling here to stop the background from fading in when the previous scene did a morph or instant transition
    if (novel->getPreviousScene()) {
        int previousBackgroundTransition = novel->getPreviousScene()->getEndTransitionTypeId();
        needsToFadeIn = previousBackgroundTransition != BackgroundTransitionType::BACKGROUND_TRANSITION_TYPE_MORPH &&
                        previousBackgroundTransition != BackgroundTransitionType::BACKGROUND_TRANSITION_TYPE_INSTANT;

        if (previousBackgroundTransition == BackgroundTransitionType::BACKGROUND_TRANSITION_TYPE_INSTANT) {
            backgroundImageRenderer->setBackground(scene->getBackgroundImageName());
        }
    }

    if (needsToFadeIn) {
        backgroundTransitionRenderer->startTransition(BackgroundTransition::FADE_IN, *colour, 2000, 2000, 1000);
        backgroundTransitionRenderer->getCurrentTransition()->setToForeground();
        backgroundImageRenderer->setBackground(scene->getBackgroundImageName());
    }

    delete (colour);

    if (setSegment) {
        nextSegment();

        sceneTransitioning = false;
        textDisplay->setVisible();
        isFirstChange = false;
    }
}

void NovelScreen::setSceneSegment(NovelSceneSegment *sceneSegment, bool setLine) {

    // Play the music file related to the scene segment
    MusicPlaybackRequest *musicPlaybackRequest = sceneSegment->getMusicPlaybackRequest();
    if (musicPlaybackRequest) {
        MusicPlaybackRequestMetadata *metadata = musicPlaybackRequest->getMetadata();

        if (metadata) {
            musicManager->playAudioStream(musicPlaybackRequest->getMusicName(), metadata);
        } else {
            musicManager->playAudioStream(musicPlaybackRequest->getMusicName());
        }
    }

    if (setLine) {
        nextLine();
    }

}

void NovelScreen::setSceneSegmentLine(NovelSceneSegmentLine *sceneSegmentLine) {
    // If the next line has a character or override name attached to it, use that as the character name
    int characterId = sceneSegmentLine->getCharacterId();

    std::string characterName;

    if (!sceneSegmentLine->getOverrideCharacterName().empty()) {
        characterName = sceneSegmentLine->getOverrideCharacterName();
    }

    if (characterId > 0 && characterName.empty()) {
        Character *character = novel->getCharacter(characterId - 1);

        if (character) {
            characterName = character->getFirstName();
        }
    }

    // Handle character sprite drawing
    CharacterStateGroup *characterStateGroup = sceneSegmentLine->getCharacterStateGroup();

    if (!characterStateGroup && isFirstChange) {
        // If this is the first line change since the game has started, this means a game has just been loaded (or started)
        // In this instance, we need to find the previous character state group which was displaying to restore the state of the on-screen characters.
        auto *dataSet = new DataSet();

        novel->getNovelDatabase()->execute(Utils::implodeString({
                                                                        "SELECT sl.id AS segment_line_id",
                                                                        "FROM segment_lines sl",
                                                                        "INNER JOIN scene_segments ss ON ss.id = sl.scene_segment_id",
                                                                        "INNER JOIN scenes s ON s.id = ss.scene_id",
                                                                        "INNER JOIN chapters c ON c.id = s.chapter_id",
                                                                        "INNER JOIN character_state_groups csg ON csg.id = sl.character_state_group_id",
                                                                        "WHERE sl.id < ?",
                                                                        "AND c.id = ?",
                                                                        "ORDER BY sl.id DESC",
                                                                        "LIMIT 1;"
                                                                }, " "),dataSet, {std::to_string(sceneSegmentLine->getId()), std::to_string(novel->getCurrentChapter()->getId())},
                                           {DatabaseConnection::TYPE_INT, DatabaseConnection::TYPE_INT});

        if (dataSet->getRowCount()) {
            // If we've found something, use this to populate the character sprite renderer
            characterStateGroup = novel->getSceneSegmentLine(dataSet->getRow(0)->getColumn("segment_line_id")->getData()->asInteger())->getCharacterStateGroup();
        }

        delete(dataSet);
    }

    if (characterStateGroup) {
        std::vector<CharacterState *> states = characterStateGroup->getCharacterStates();

        std::vector<CharacterSpriteDrawRequest *> requests;

        for (auto & state : states) {
            requests.push_back(new CharacterSpriteDrawRequest(state->getCharacterSprite()));
        }

        characterSpriteRenderer->push(requests);

        // Delete the now-unneeded data from RAM
        for (auto & request : requests) {
            delete request;
        }

    }

    textDisplay->setText(sceneSegmentLine->getText(), characterName);

    gameState->setNovelTextLine(sceneSegmentLine);
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
