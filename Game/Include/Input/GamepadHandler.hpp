#ifndef INPUT_GAMEPAD_HANDLER_INCLUDED
#define INPUT_GAMEPAD_HANDLER_INCLUDED

class GamepadHandler {
public:
  GamepadHandler();
  ~GamepadHandler();
  void update();
private:
  bool enabled;
};

#endif
