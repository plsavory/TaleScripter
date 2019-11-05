#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include <vector>
#include "Input/MouseHandler.hpp"
#include <iostream>
#include <Exceptions/InputHandlerException.hpp>
#include <Exceptions/MisuseException.hpp>

/**
 * @param windowPointer - Pointer to an SFML window
 */
MouseHandler::MouseHandler(sf::RenderWindow *windowPointer) {
    window = windowPointer;
    enabled = true;

    for (int i = 0; i < 16; i++) {
        wasClickedInLastFrame[i] = false;
    }
}

MouseHandler::~MouseHandler() {

    for (auto &event : events) {
        delete (event);
    }
}

void MouseHandler::update() {

    if (!enabled) {
        return;
    }

    // Store the current mouse mouse position
    mousePosition = sf::Mouse::getPosition(*window);

    unsigned int eventCount = events.size();

    for (unsigned int i = 0; i < eventCount; i++) {
        events[i]->setMousePosition(mousePosition);
        events[i]->update();
    }

    // Handle global mouse click stuff
    for (int i = 0; i < 16; i++) {
        if (wasClickedInLastFrame[i]) {
            wasClickedInLastFrame[i] = sf::Mouse::isButtonPressed((sf::Mouse::Button)i);
        }
    }

}

sf::Vector2i MouseHandler::getMousePosition() {
    if (!enabled) {
        return sf::Vector2i(0, 0);
    }

    return mousePosition;
}

/**
 * [draw Draw a custom cursor, if that functionality is enabled.]
 */
void MouseHandler::draw() {

}

/**
 * Returns true if the window is in focus and the given button has been pressed
 * @param button
 * @return
 */
bool MouseHandler::isButtonClicked(sf::Mouse::Button button) {
    bool clicked = window->hasFocus() && sf::Mouse::isButtonPressed(button);

    if (wasClickedInLastFrame[button]) {
        return false;
    }

    wasClickedInLastFrame[button] = clicked;
    return clicked;
}

/**
 * Returns true if the window is in focus and the given button has been pressed, regardless of whether it has been pressed recently
 * @param button
 * @return
 */
bool MouseHandler::isButtonDown(sf::Mouse::Button button) {
    bool clicked = window->hasFocus() && sf::Mouse::isButtonPressed(button);
    wasClickedInLastFrame[button] = clicked;
    return clicked;
}

MouseEvent *MouseHandler::addEvent(std::string name, MouseEventType eventType) {
    events.push_back(new MouseEvent(name, eventType));
    return events.back();
}

MouseEvent *MouseHandler::addEvent(std::string name, MouseEventType eventType, int areaX, int areaY, int areaWidth,
                                   int areaHeight) {
    events.push_back(new MouseEvent(name, eventType, areaX, areaY, areaWidth, areaHeight));
    return events.back();
}

// MouseEvent stuff

/**
 * [MouseEvent::MouseEvent Mouse event constructor where a bounding box is not required]
 * @param eventType [Mouse event type]
 */
MouseEvent::MouseEvent(std::string eventName, MouseEventType mouseEventType) {

    // TODO: Use configured display mode when this feature is supported
    enabled = true;
    eventType = mouseEventType;
    myBounds.left = 0;
    myBounds.top = 0;
    myBounds.width = 1280;
    myBounds.height = 720;
    hasFired = false;
    name = eventName;

}

/**
 * [MouseEvent::MouseEvent MouseEvent constructor]
 * @param eventType  [Type of mouse event]
 * @param areaX      [Area X (If applicable)]
 * @param areaY      [Area Y (If applicable)]
 * @param areaWidth  [Area Width (If applicable)]
 * @param areaHeight [Area Height (If applicable)]
 */
MouseEvent::MouseEvent(std::string eventName, MouseEventType mouseEventType, int areaX, int areaY, int areaWidth,
                       int areaHeight) {

    enabled = true;
    eventType = mouseEventType;
    myBounds.left = areaX;
    myBounds.top = areaY;
    myBounds.width = areaWidth;
    myBounds.height = areaHeight;
    hasFired = false;
    name = eventName;

};

/**
 * [MouseEvent::getEventStatue Returns whether the conditions of a mouse event have been met or not]
 * @return [description]
 */
bool MouseEvent::conditionsMet() {

    // TODO: Add support for the scroll wheel when we need it for the UI.

    if (!enabled) {
        return false;
    }

    if (hasFired) {
        return false;
    }


    bool mouseInsideBounds = (mousePosition.x >= myBounds.left && mousePosition.x <= (myBounds.left + myBounds.width) &&
                              (mousePosition.y >= myBounds.top && mousePosition.y <= (myBounds.top + myBounds.height)));

    sf::Mouse::Button mouseButton;

    switch (eventType) {
        case MouseEventType::MouseInsideArea:
            return mouseInsideBounds;
        case MouseEventType::LeftClick:
            mouseButton = sf::Mouse::Button::Left;
            break;
        case MouseEventType::MiddleClick:
            mouseButton = sf::Mouse::Button::Middle;
            break;
        case MouseEventType::RightClick:
            mouseButton = sf::Mouse::Button::Right;
            break;
        default:
            throw MisuseException("Unknown mouse event type");
    }

    if (sf::Mouse::isButtonPressed(mouseButton) && mouseInsideBounds) {
        hasFired = true;
        return true;
    }

    return false;
}

void MouseEvent::update() {

    sf::Mouse::Button mouseButton;

    switch (eventType) {
        case MouseEventType::LeftClick:
            mouseButton = sf::Mouse::Button::Left;
            break;
        case MouseEventType::MiddleClick:
            mouseButton = sf::Mouse::Button::Middle;
            break;
        case MouseEventType::RightClick:
            mouseButton = sf::Mouse::Button::Right;
            break;
        default:
            throw MisuseException("Unknown mouse event type");
    }

    if (hasFired) {
        hasFired = sf::Mouse::isButtonPressed(mouseButton);
    }
}

std::string MouseEvent::getName() {
    return name;
}