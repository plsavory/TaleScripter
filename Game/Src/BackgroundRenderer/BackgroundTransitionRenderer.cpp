#include "Base/Engine.hpp"
#include "BackgroundRenderer/BackgroundTransition.hpp"

BackgroundTransitionRenderer::BackgroundTransitionRenderer(sf::RenderWindow *windowPointer, BackgroundImageRenderer *bgImageRenderer) {
  window = windowPointer;
  backgroundImageRenderer = bgImageRenderer;
  currentTransition = NULL;

  // Set some defaults
  setScreenSize(1280,720);
}

BackgroundTransitionRenderer::~BackgroundTransitionRenderer() {

}

void BackgroundTransitionRenderer::startTransition(int transitionType, sf::Color primaryColour,int delayBeforeStart, int delayBeforeEnd, int animationLength) {

// If we're already doing a transition, delete it
  if (currentTransition) {
    delete(currentTransition);
    currentTransition = NULL;
  }

  currentTransition = new BackgroundTransition(window, transitionType, delayBeforeStart, screenWidth, screenHeight, delayBeforeEnd, animationLength, backgroundImageRenderer);

}

void BackgroundTransitionRenderer::update() {

  // Update the transition, deleting it when it has finished
  if (currentTransition) {
    if (currentTransition->update()) {
      delete(currentTransition);
      currentTransition = NULL;
    }
  }

}

void BackgroundTransitionRenderer::draw() {

  if (currentTransition) {
    currentTransition->draw();
  }

}

bool BackgroundTransitionRenderer::hasTransitionCompleted() {
  return !currentTransition;
}

void BackgroundTransitionRenderer::setScreenSize(int width, int height) {
  screenWidth = width;
  screenHeight = height;
}

BackgroundTransition* BackgroundTransitionRenderer::getCurrentTransition() {
  return currentTransition;
}

/**
 * [BackgroundTransitionRenderer::isInForeground Returns if the current transition should be drawn in the foreground or not]
 * @return [Boolean]
 */
bool BackgroundTransitionRenderer::isInForeground() {
  if (!currentTransition) {
    return false;
  }

  return currentTransition->isForegroundTransition();
}
