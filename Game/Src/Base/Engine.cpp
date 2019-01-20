/*
  Engine.cpp - This class will create and store core engine objects
  It will also contain core methods which will be used to manipulate certain
  engine core functions.
 */

#include "Base/Engine.hpp"

/**
 * [Engine::Engine Create all classes which we need]
 */
Engine::Engine(sf::RenderWindow *windowPointer) {

  window = windowPointer;

  inputManager = new InputManager();
  backgroundImageRenderer = new BackgroundImageRenderer(window);
  resourceManager = new ResourceManager(backgroundImageRenderer);
  spriteRenderer = new SpriteRenderer(window,resourceManager->getTextureManager());
  textRenderer = new TextRenderer(window, resourceManager->getFontManager());
  backgroundTransitionRenderer = new BackgroundTransitionRenderer(window, backgroundImageRenderer);
}

/**
 * [Engine Delete everything when this class is deleted]
 */
Engine::~Engine() {
  delete(resourceManager);
  delete(spriteRenderer);
  delete(textRenderer);
  delete(inputManager);
  delete(backgroundImageRenderer);
  delete(backgroundTransitionRenderer);
}
