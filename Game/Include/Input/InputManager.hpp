#ifndef INPUT_MANAGER_INCLUDED
#define INPUT_MANAGER_INCLUDED

#include "Input/MouseHandler.hpp"
#include "Input/KeyboardHandler.hpp"
#include "Input/GamepadHandler.hpp"

class InputManager {
public:
  InputManager();
  ~InputManager();
  void update();
  int bindKeyboardEvent(std::string name, std::string key, bool onlyTriggerOnPress);
  bool isEventPressed(int id);
  bool isEventPressed(std::string name);
  void setEnabled(bool isEnabled);
private:
  MouseHandler *mouseHandler;
  KeyboardHandler *keyboardHandler;
  GamepadHandler *gamepadHandler;
};

#endif
