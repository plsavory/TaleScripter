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
private:
  MouseHandler *mouseHandler;
  KeyboardHandler *keyboardHandler;
  GamepadHandler *gamepadHandler;
};

#endif
