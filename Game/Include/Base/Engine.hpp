#ifndef ENGINE_INCLUDED
#define ENGINE_INCLUDED

#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Database/DatabaseConnection.hpp"
#include "BackgroundRenderer/BackgroundImageRenderer.hpp"
#include "Resource/ResourceManager.hpp"
#include "SpriteRenderer/SpriteRenderer.hpp"
#include "TextRenderer/TextRenderer.hpp"
#include "Input/InputManager.hpp"

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
  sf::RenderWindow* getWindow() {
    return window;
  }
private:
  ResourceManager *resourceManager;
  SpriteRenderer *spriteRenderer;
  TextRenderer *textRenderer;
  InputManager *inputManager;
  BackgroundImageRenderer *backgroundImageRenderer;
  sf::RenderWindow *window;
};

#endif
