#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "Input/InputManager.hpp"
#include <iostream>

InputManager::InputManager(sf::RenderWindow *windowPointer) {

  // Create classes which we rely on
  mouseHandler = new MouseHandler(windowPointer);
  keyboardHandler = new KeyboardHandler();
  gamepadHandler = new GamepadHandler();

}

InputManager::~InputManager() {
  delete mouseHandler;
  delete keyboardHandler;
  delete gamepadHandler;
}

void InputManager::update() {

  // Update handler classes
  keyboardHandler->update();
  mouseHandler->update();
  gamepadHandler->update();

}

int InputManager::bindKeyboardEvent(std::string name, std::string key, bool onlyTriggerOnPress) {
  // TODO: Add a function to bind an event of all 3 types to the same thing when we have working mouse and gamepad input for convenience
  return keyboardHandler->bindEvent(name,key, onlyTriggerOnPress);
}


bool InputManager::isEventPressed(int id) {
  return keyboardHandler->isEventPressed(id);
}

bool InputManager::isEventPressed(std::string name) {
  return keyboardHandler->isEventPressed(name);
}

void InputManager::setEnabled(bool isEnabled) {
  keyboardHandler->setEnabled(isEnabled);
}
