#ifndef INPUT_MOUSE_HANDLER_INCLUDED
#define INPUT_MOUSE_HANDLER_INCLUDED

class MouseHandler {
public:
  MouseHandler();
  ~MouseHandler();
  void update();
private:
  bool enabled;
};

#endif
