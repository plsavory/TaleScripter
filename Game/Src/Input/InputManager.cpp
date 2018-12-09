#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include "Input/InputManager.hpp"
#include <iostream>

InputManager::InputManager() {

  // Create classes which we rely on
  mouseHandler = new MouseHandler();
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
  mouseHandler->update();
  gamepadHandler->update();

}
