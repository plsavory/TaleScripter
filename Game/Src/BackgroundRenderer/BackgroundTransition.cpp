/*
  BackgroundTransition.cpp - Represents a background transition animation
  TODO: Allow for custom animations programmed using a script
 */

#include <iostream>
#include "Base/Engine.hpp"
#include "BackgroundRenderer/BackgroundTransition.hpp"

const int BackgroundTransition::FADE_IN = 0;
const int BackgroundTransition::FADE_OUT = 1;

BackgroundTransition::BackgroundTransition(sf::RenderWindow *windowPointer, int transitionType, int delayBeforeStart,
                                           int screenWidth, int screenHeight, int delayAfterFinish, int animationLength,
                                           BackgroundImageRenderer *bgRenderer, BackgroundOverlay *bgOverlay) {
    window = windowPointer;
    width = screenWidth;
    height = screenHeight;
    delay = delayAfterFinish;
    delayClock = nullptr;
    type = transitionType;
    transitionCompleted = false;
    rectangleShape = nullptr;
    length = animationLength ? (animationLength / 1000) : 1;
    startDelay = delayBeforeStart;
    startDelayClock = nullptr;
    drawBeforeStartDelay = true;
    foregroundTransition = false;
    backgroundImageRenderer = bgRenderer;
    backgroundOverlay = bgOverlay;

    if (startDelay > 0) {
        startDelayClock = new sf::Clock();
    }

    primaryColour = sf::Color(0, 0, 0, 255); // Default: Black

    switch (type) {
        case FADE_IN:
            FadeInInit();
            break;
        case FADE_OUT:
            FadeOutInit();
            break;
        default:
            delete (this);
    }
}

BackgroundTransition::~BackgroundTransition() {
    if (delayClock) {
        delete (delayClock);
    }
}

/**
 * [setPrimaryColour Sets the primary colour of this transition]
 * @param colour [SFML Colour]
 */
void BackgroundTransition::setPrimaryColour(sf::Color colour) {
    primaryColour = colour;
    backgroundOverlay->setColour(sf::Color(primaryColour.r, primaryColour.g, primaryColour.b, alpha));
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

        delete (startDelayClock);
        startDelayClock = nullptr;
    }

    switch (type) {
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

    return true;
}

// Transition-specific init functions
void BackgroundTransition::FadeInInit() {
    alpha = 255;
    backgroundOverlay->setAlpha(alpha);
    backgroundOverlay->setColour(sf::Color(primaryColour.r, primaryColour.g, primaryColour.b, 0));
}

void BackgroundTransition::FadeOutInit() {
    alpha = 0;
    backgroundOverlay->setAlpha(alpha);
    backgroundOverlay->setColour(sf::Color(primaryColour.r, primaryColour.g, primaryColour.b, 0));
}

// Transition-specific update functions
void BackgroundTransition::FadeInUpdate() {

    if (transitionCompleted) {
        return;
    }

    if (alpha > 0) {
        alpha -= (255 / (60 * length)); // TODO: Use current frame rate
        backgroundOverlay->setAlpha(alpha);
        backgroundOverlay->setColour(sf::Color(primaryColour.r, primaryColour.g, primaryColour.b, 0));
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
        alpha += (255 / (60 * length)); // TODO: Use current frame rate

        // Prevent an overflow
        if (alpha > 255) {
            alpha = 255;
        }

        backgroundOverlay->setAlpha(alpha);
        return;
    }

    if (delay > 0) {
        delayClock = new sf::Clock();
    }

    alpha = 255;
    backgroundOverlay->setAlpha(alpha);
    transitionCompleted = true;

}
