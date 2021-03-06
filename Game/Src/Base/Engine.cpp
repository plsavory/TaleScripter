/*
  Engine.cpp - This class will create and store core engine objects
  It will also contain core methods which will be used to manipulate certain
  engine core functions.
 */

#include <iostream>
#include "Database/DatabaseConnection.hpp"
#include "Base/Engine.hpp"

/**
 * [Engine::Engine Create all classes which we need]
 */
Engine::Engine(sf::RenderWindow *windowPointer) {

    window = windowPointer;
    errorMessage = "";

    try {

        inputManager = new InputManager(window);
        backgroundOverlay = new BackgroundOverlay(window);
        backgroundImageRenderer = new BackgroundImageRenderer(window);
        resourceManager = new ResourceManager(backgroundImageRenderer);
        spriteRenderer = new SpriteRenderer(window, resourceManager->getTextureManager());
        textRenderer = new TextRenderer(window, resourceManager->getFontManager());
        backgroundTransitionRenderer = new BackgroundTransitionHandler(window, backgroundImageRenderer, backgroundOverlay);
        characterSpriteRenderer = new CharacterSpriteRenderer(resourceManager, spriteRenderer);

    } catch (GeneralException &e) {
        errorMessage = e.what();
    }
}

/**
 * [Engine Delete everything when this class is deleted]
 */
Engine::~Engine() {
    delete (resourceManager);
    delete (spriteRenderer);
    delete (textRenderer);
    delete (inputManager);
    delete (backgroundImageRenderer);
    delete (backgroundTransitionRenderer);
    delete (characterSpriteRenderer);
}
