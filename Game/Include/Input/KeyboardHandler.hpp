#ifndef INPUT_KEYBOARD_HANDLER_INCLUDED
#define INPUT_KEYBOARD_HANDLER_INCLUDED

#include <iostream>
#include "Keymap.hpp"

struct KeyboardEvent {
public:
  /**
   * [KeyboardEvent Bind a key to this event]
   * @param eName [Name of the event]
   * @param eKey  [Keycode to bind]
   */
  KeyboardEvent(std::string eName, sf::Keyboard::Key eKey) {
    enabled = true;
    initialiseKeys();
    bindEvent(eName, eKey);
  };
  /**
   * [KeyboardEvent Bind a key to this event]
   * @param eName [Name of the event]
   * @param eKey  [Key to bind as string]
   */
  KeyboardEvent(std::string eName, std::string eKey) {
    sf::Keyboard::Key foundKey = Keymap::getKeycode(eName);

    if (foundKey != sf::Keyboard::Key::Unknown) {
      KeyboardEvent(eName, foundKey);
    }
  };

  /**
   * [bindEvent Binds an event to an instance of this struct]
   * @param  eName [Name of the event]
   * @param  eKey  [Keycode to bind]
   * @return       [True on success, False on failure]
   */
  bool bindEvent(std::string eName, sf::Keyboard::Key eKey) {

    if (eKey == sf::Keyboard::Key::Unknown) {
      return false;
    }

    // Make sure the same key is not already bound to this event, will stop game from wasting slots
    for (int i = 0; i<4; i++) {
      if (key[i] == eKey) {
        std::cout<<"Failure to bind key "<<eKey<<" to event "
          <<name<<" - event already has this key bound to it"<<std::endl;
        return false;
      }
    }

    for (int i = 0; i<4; i++) {
      if (key[i] == sf::Keyboard::Key::Unknown) {
        name = eName;
        key[i] = eKey;
        return true;
      }
    }

    // No empty slots found
    std::cout<<"Failure to bind key "<<eKey<<" to event "
      <<name<<" - event already has 4 keys bound to it"<<std::endl;

    return false;
  };
  bool enabled;
  bool isDown() {

    if (!enabled) {
      return false;
    }
    // Check all key codes bound to this event, return true as soon as we find one that is pressed.
    for (int i = 0; i<4; i++) {
      // Don't even bother checking unknown keys
      if (key[i] == sf::Keyboard::Unknown) {
        continue;
      }

      if (sf::Keyboard::isKeyPressed(key[i])) {
        return true;
      }
    }
    return false;

  }
  std::string getName() {
    return name;
  }
private:
  std::string name;
  sf::Keyboard::Key key[4];
  void initialiseKeys() {
    for(int i = 0; i < 4; i++) {
      key[i] = sf::Keyboard::Unknown;
    }
  };
};

class KeyboardHandler {
public:
  KeyboardHandler();
  ~KeyboardHandler();
  int bindEvent(std::string name, std::string key);
  int bindEvent(std::string name, sf::Keyboard::Key key);
  int getEvent(std::string name);
  bool isEventPressed(std::string name);
  bool isEventUp(std::string name);
private:
  bool enabled;
  KeyboardEvent *keyboardEvent[0xFF];
  Keymap *keymap;
};

#endif
