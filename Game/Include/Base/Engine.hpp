#ifndef ENGINE_INCLUDED
#define ENGINE_INCLUDED

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "Database/DatabaseConnection.hpp"
#include "BackgroundRenderer/BackgroundOverlay.hpp"
#include "BackgroundRenderer/BackgroundImageRenderer.hpp"
#include "BackgroundRenderer/BackgroundTransitionHandler.hpp"
#include "Resource/ResourceManager.hpp"
#include "SpriteRenderer/SpriteRenderer.hpp"
#include "TextRenderer/TextRenderer.hpp"
#include "Input/InputManager.hpp"
#include "VisualNovelEngine/Classes/Data/Novel.hpp"
#include "VisualNovelEngine/Classes/UI/CharacterSpriteRenderer.hpp"

class Engine {
public:
  Engine(sf::RenderWindow *windowPointer);
  ~Engine();
  // To avoid cluttering up Engine.cpp with small functions, just implement the getters here
  ResourceManager* getResourceManager() {
    return resourceManager;
  }
  SpriteRenderer* getSpriteRenderer() {
    return spriteRenderer;
  }
  TextRenderer* getTextRenderer() {
    return textRenderer;
  }
  InputManager* getInputManager() {
    return inputManager;
  }
  BackgroundImageRenderer* getBackgroundImageRenderer() {
    return backgroundImageRenderer;
  }
  BackgroundTransitionHandler* getBackgroundTransitionRenderer() {
    return backgroundTransitionRenderer;
  }
  CharacterSpriteRenderer* getCharacterSpriteRenderer() {
    return characterSpriteRenderer;
  }
  sf::RenderWindow* getWindow() {
    return window;
  }
  BackgroundOverlay* getBackgroundOverlay() {
      return backgroundOverlay;
  }
  std::string getErrorMessage() {
      return errorMessage;
  }
private:
  ResourceManager *resourceManager;
  SpriteRenderer *spriteRenderer;
  TextRenderer *textRenderer;
  InputManager *inputManager;
  BackgroundOverlay *backgroundOverlay;
  BackgroundImageRenderer *backgroundImageRenderer;
  BackgroundTransitionHandler *backgroundTransitionRenderer;
  CharacterSpriteRenderer *characterSpriteRenderer;
  sf::RenderWindow *window;
  std::string errorMessage;
};

#endif
