#ifndef INPUT_MANAGER_INCLUDED
#define INPUT_MANAGER_INCLUDED

#include "Input/MouseHandler.hpp"
#include "Input/KeyboardHandler.hpp"
#include "Input/GamepadHandler.hpp"

class InputManager {
public:
  InputManager(sf::RenderWindow *windowPointer);
  ~InputManager();
  void update();
  int bindKeyboardEvent(std::string name, std::string key, bool onlyTriggerOnPress);
  MouseEvent* bindMouseEvent(std::string name, MouseEventType mouseEventType);
  MouseEvent* bindMouseEvent(std::string name, MouseEventType mouseEventType, int areaX, int areaY, int areaWidth, int areaHeight);
  bool isEventPressed(int id);
  bool isEventPressed(std::string name);
  void setEnabled(bool isEnabled);
  MouseHandler* getMouseHandler() {
    return mouseHandler;
  }
private:
  MouseHandler *mouseHandler;
  KeyboardHandler *keyboardHandler;
  GamepadHandler *gamepadHandler;
};

#endif
