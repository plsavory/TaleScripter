#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include "Input/MouseHandler.hpp"

MouseHandler::MouseHandler(sf::RenderWindow *windowPointer) {
  window = windowPointer;
}

MouseHandler::~MouseHandler() {

}

void MouseHandler::update() {
  if (!enabled) {
    return;
  }

  // Store the current mouse mouse position
  mousePosition = sf::Mouse::getPosition(*window);
}

sf::Vector2i MouseHandler::getMousePosition() {
  if (!enabled) {
    return sf::Vector2i(0,0);
  }

  return mousePosition;
}

/**
 * [draw Draw a custom cursor, if that functionality is enabled.]
 */
void MouseHandler::draw() {

}

MouseEvent MouseHandler::addEvent(std::string name, MouseEventType eventType) {

}

MouseEvent MouseHandler::addEvent(std::string name, MouseEventType eventType, int areaX, int areaY, int areaWidth, int areaHeight)

// MouseEvent stuff

/**
 * [MouseEvent::MouseEvent Mouse event constructor where a bounding box is not required]
 * @param eventType [Mouse event type]
 */
MouseEvent::MouseEvent(MouseEventType eventType) {

  switch (eventType) {
    case MouseEventType::MouseInsideArea:
    case MouseEventType::GlobalLeftClick:
    case MouseEventType::GlobalMiddleClick:
    case MouseEventType::GlobalRightClick:
    throw "Global mouse event must provided with bounding box coordinates (Use other constructor overload.)";
    default:
    break;
  }

  MouseEvent(eventType, 0,0,0,0);

}

/**
 * [MouseEvent::MouseEvent MouseEvent constructor]
 * @param eventType  [Type of mouse event]
 * @param areaX      [Area X (If applicable)]
 * @param areaY      [Area Y (If applicable)]
 * @param areaWidth  [Area Width (If applicable)]
 * @param areaHeight [Area Height (If applicable)]
 */
MouseEvent::MouseEvent(MouseEventType eventType, int areaX, int areaY, int areaWidth, int areaHeight) {
  // Is the mouse event global or not
  bool global = false;

  switch (eventType) {
    case MouseEventType::MouseInsideArea:
    case MouseEventType::GlobalLeftClick:
    case MouseEventType::GlobalMiddleClick:
    case MouseEventType::GlobalRightClick:
    global = true;
    break;
    case MouseEventType::LeftClick:
    case MouseEventType::MiddleClick:
    case MouseEventType::RightClick:
    global = false;
    break;
  }

  // If the mouse event is global, we require the mouse bound coordinates of the event to continue
  if (global) {
    if (!(areaWidth > 0 && areaY > 0)) {
      throw "Global mouse event requires bounding area.";
    }
  }
};
