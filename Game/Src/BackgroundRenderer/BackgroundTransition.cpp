/*
  BackgroundTransition.cpp - Represents a background transition animation
  TODO: Allow for custom animations programmed using a script
 */

#include <iostream>
#include "Base/Engine.hpp"
#include "BackgroundRenderer/BackgroundTransition.hpp"

const int BackgroundTransition::FADE_IN = 0;
const int BackgroundTransition::FADE_OUT = 1;

BackgroundTransition::BackgroundTransition(sf::RenderWindow *windowPointer, int transitionType, int delayBeforeStart, int screenWidth, int screenHeight, int delayAfterFinish, int animationLength, BackgroundImageRenderer *bgRenderer) {
  window = windowPointer;
  width = screenWidth;
  height = screenHeight;
  delay = delayAfterFinish;
  delayClock = NULL;
  type = transitionType;
  transitionCompleted = false;
  rectangleShape = NULL;
  length = animationLength ? (animationLength/1000) : 1;
  startDelay = delayBeforeStart;
  startDelayClock = NULL;
  drawBeforeStartDelay = true;
  foregroundTransition = false;
  backgroundImageRenderer = bgRenderer;

  if (startDelay > 0) {
    startDelayClock = new sf::Clock();
  }

  primaryColour = sf::Color(0,0,0,255); // Default: Black

  switch (type) {
    case FADE_IN:
    FadeInInit();
    break;
    case FADE_OUT:
    FadeOutInit();
    break;
    default:
    delete(this);
  }
}

BackgroundTransition::~BackgroundTransition() {
  if (delayClock) {
    delete(delayClock);
  }

  if (rectangleShape) {
    delete(rectangleShape);
  }
}

/**
 * [BackgroundTransition::update Performs the transition/animation]
 * @return [True if finished]
 */
bool BackgroundTransition::update() {

  // Wait until the start delay has elapsed if we need to
  if (startDelayClock) {

    if (startDelayClock->getElapsedTime().asMilliseconds() < startDelay) {
      return false;
    }

    delete(startDelayClock);
    startDelayClock = NULL;
  }

  switch(type) {
    case FADE_IN:
    FadeInUpdate();
    break;
    case FADE_OUT:
    FadeOutUpdate();
    break;
    default:
    break;
  }

  if (!transitionCompleted) {
    return false;
  }

  if (delayClock) {
    if (delayClock->getElapsedTime().asMilliseconds() < delay) {
      return false;
    }
  }

  // TODO: Temporarily disable background rendering

  return true;
}

void BackgroundTransition::draw() {

  // Don't draw if we aren't meant to when a startDelayClock exists;
  if (!drawBeforeStartDelay && startDelayClock) {
    return;
  }

  switch(type) {
    case FADE_IN:
    FadeInDraw();
    break;
    case FADE_OUT:
    FadeOutDraw();
    break;
    default:
    break;
  }
}

// Transition-specific init functions
void BackgroundTransition::FadeInInit() {
  alpha = 255;
  rectangleShape = new sf::RectangleShape(sf::Vector2f(width,height));
  renderColour = sf::Color(primaryColour.r, primaryColour.g, primaryColour.b, alpha);
  rectangleShape->setFillColor(renderColour);
}

void BackgroundTransition::FadeOutInit() {
  alpha = 0;
  rectangleShape = new sf::RectangleShape(sf::Vector2f(width,height));
  renderColour = sf::Color(primaryColour.r, primaryColour.g, primaryColour.b, alpha);
  rectangleShape->setFillColor(renderColour);
}

// Transition-specific update functions
void BackgroundTransition::FadeInUpdate() {

  if (transitionCompleted) {
    return;
  }

  if (alpha > 0) {
    alpha-=(255/(60*length)); // TODO: Use current frame rate
    renderColour = sf::Color(primaryColour.r, primaryColour.g, primaryColour.b, alpha);
    rectangleShape->setFillColor(renderColour);
    return;
  }

  if (delay > 0) {
    delayClock = new sf::Clock();
  }

  alpha = 0;
  transitionCompleted = true;

}

void BackgroundTransition::FadeOutUpdate() {

  if (transitionCompleted) {
    return;
  }

  if (alpha < 255) {
    alpha+=(255/(60*length)); // TODO: Use current frame rate

    // Prevent an overflow
    if (alpha > 255) {
      alpha = 255;
    }

    renderColour = sf::Color(primaryColour.r, primaryColour.g, primaryColour.b, alpha);
    rectangleShape->setFillColor(renderColour);
    return;
  }

  if (delay > 0) {
    delayClock = new sf::Clock();
  }

  alpha = 255;
  renderColour = sf::Color(primaryColour.r, primaryColour.g, primaryColour.b, alpha);
  rectangleShape->setFillColor(renderColour);
  // Set the background colour to match the transition
  backgroundImageRenderer->setBackgroundColour(new sf::Color(0,0,0,255)); // TODO: Use from transition
  // TODO: Make sure that background rendering gets re-enabled and the colour gets reset after a couple of frames
  transitionCompleted = true;

}

// Transition-specific draw functions
void BackgroundTransition::FadeInDraw() {
  if (rectangleShape) {
    window->draw(*rectangleShape);
  }
}

void BackgroundTransition::FadeOutDraw() {
  if (rectangleShape) {
    window->draw(*rectangleShape);
  }
}
