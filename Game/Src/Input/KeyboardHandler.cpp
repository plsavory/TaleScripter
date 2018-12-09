#include <nlohmann/json.hpp>
#include <SFML/Window.hpp>
#include "Input/KeyboardHandler.hpp"
#include <iostream>

KeyboardHandler::KeyboardHandler() {

  // Make sure all keyboard event objects are null
  for (int i = 0; i<0xFF;i++) {
    keyboardEvent[i] = NULL;
  }

  enabled = true;
}

KeyboardHandler::~KeyboardHandler() {
  for (int i = 0; i<0xFF; i++) {
    if (keyboardEvent[i]) {
      delete keyboardEvent[i];
    }
  }
}

/**
 * [KeyboardHandler::bindEvent Binds a keyboard key to an event
 * Events can be bound while KeyboardHandler is disabled, but they will never be fired.]
 * @param name [Name of the event]
 * @param key  [Key (as a string) to bind]
 */
int KeyboardHandler::bindEvent(std::string name, std::string key) {

  sf::Keyboard::Key keycode = Keymap::getKeycode(key);

  return bindEvent(name, keycode);

}

/**
 * [KeyboardHandler::bindEvent Binds a keyboard key to an event
 * Events can be bound while KeyboardHandler is disabled, but they will never be fired.]
 * @param name [Name of the event]
 * @param key  [Key (SFML keycode) to bind]
 * @return      [ID of the bound event, or -1 on failure]
 */
int KeyboardHandler::bindEvent(std::string name, sf::Keyboard::Key key) {

  // Find if a keyboard event with this name already exists
  int eventId = getEvent(name);

  if (eventId >= 0) {
    // Event exists if we get into here
    if (keyboardEvent[eventId]->bindEvent(name,key)) {
      return eventId;
    } else {
      return -1;
    }

    // TODO: Create some sort of error reporting in the game engine to handle failure
  }

  // Find an empty slot in the events, once an empty one is found, create one there.
  for (int i = 0; i<0xFF; i++) {
    if (!keyboardEvent[i]) {
      keyboardEvent[i] = new KeyboardEvent(name, key);
      return i;
    }
  }

  return -1;

}

/**
 * [KeyboardHandler::getEvent Returns the event ID of the event with the given name]
 * @param  name [The name of the event]
 * @return      [Event ID - Returns -1 if none found.]
 */
int KeyboardHandler::getEvent(std::string name) {

  for (int i = 0; i<0xFF; i++) {
    if (keyboardEvent[i]) {
      if (keyboardEvent[i]->getName() == name) {
        return i;
      }
    }
  }
  return -1;

}

/**
 * [KeyboardHandler::isEventPressed Returns whether a key bound to the given event is pressed]
 * @param  name [The name of the event]
 * @return      [True if key is held down, otherwise false]
 */
bool KeyboardHandler::isEventPressed(std::string name) {

  // Check for KeyboardHandler being disabled
  if (!enabled) {
    return false;
  }

  int eventId = getEvent(name);

  if (eventId >= 0) {
    return keyboardEvent[eventId]->isDown();
  }

  // No event found with this id
  return false;

}

bool KeyboardHandler::isEventUp(std::string name) {

  // Check for KeyboardHandler being disabled
  if (!enabled) {
    return false;
  }

  int eventId = getEvent(name);

  if (eventId >= 0) {
    return !keyboardEvent[eventId]->isDown();
  }

  // No event found with this id
  return false;

}
