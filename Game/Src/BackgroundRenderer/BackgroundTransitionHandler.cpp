#include "Base/Engine.hpp"
#include "BackgroundRenderer/BackgroundTransition.hpp"

BackgroundTransitionHandler::BackgroundTransitionHandler(sf::RenderWindow *windowPointer,
                                                         BackgroundImageRenderer *bgImageRenderer,
                                                         BackgroundOverlay *bgOverlay) {
    window = windowPointer;
    backgroundImageRenderer = bgImageRenderer;
    currentTransition = nullptr;
    backgroundOverlay = bgOverlay;

    // Set some defaults
    setScreenSize(1280, 720);
}

BackgroundTransitionHandler::~BackgroundTransitionHandler() {
    delete(currentTransition);
}

void BackgroundTransitionHandler::startTransition(int transitionType, sf::Color primaryColour, int delayBeforeStart,
                                                  int delayBeforeEnd, int animationLength) {

// If we're already doing a transition, delete it
    if (currentTransition) {
        delete (currentTransition);
        currentTransition = nullptr;
    }

    currentTransition = new BackgroundTransition(window, transitionType, delayBeforeStart, screenWidth, screenHeight,
                                                 delayBeforeEnd, animationLength, backgroundImageRenderer,
                                                 backgroundOverlay);
    currentTransition->setPrimaryColour(primaryColour);

}

void BackgroundTransitionHandler::update() {

    // Update the transition, deleting it when it has finished
    if (currentTransition) {
        if (currentTransition->update()) {
            delete (currentTransition);
            currentTransition = nullptr;
        }
    }

}

bool BackgroundTransitionHandler::hasTransitionCompleted() {
    return !currentTransition;
}

void BackgroundTransitionHandler::setScreenSize(int width, int height) {
    screenWidth = width;
    screenHeight = height;
}

BackgroundTransition *BackgroundTransitionHandler::getCurrentTransition() {
    return currentTransition;
}

