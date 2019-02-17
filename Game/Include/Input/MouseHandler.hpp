#ifndef INPUT_MOUSE_HANDLER_INCLUDED
#define INPUT_MOUSE_HANDLER_INCLUDED

enum MouseEventType {LeftClick, MiddleClick, RightClick, MouseInsideArea, GlobalLeftClick, GlobalMiddleClick, GlobalRightClick};

class MouseEvent {
public:
  MouseEvent(MouseEventType eventType, int areaX, int areaY, int areaWidth, int areaHeight);
  MouseEvent(MouseEventType eventType);
  std::string name;
  int EventType;
  bool isEventPressed();
private:
  sf::Vector2i myBounds; // The area in which the mouse event becomes active (if neccessary)
};

class MouseHandler {
public:
  MouseHandler(sf::RenderWindow *windowPointer);
  ~MouseHandler();
  void update();
  void draw();
  MouseEvent addEvent(std::string name, MouseEventType eventType);
  MouseEvent addEvent(std::string name, MouseEventType eventType, int areaX, int areaY, int areaWidth, int areaHeight);
private:
  bool enabled;
  sf::RenderWindow *window;
  sf::Vector2i mousePosition;
  sf::Vector2i getMousePosition();
  private validateEventType(MoustEventType eventType ,bool requiresAreaCoordinates);
};

#endif
