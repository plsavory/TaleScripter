#ifndef INPUT_MOUSE_HANDLER_INCLUDED
#define INPUT_MOUSE_HANDLER_INCLUDED

enum MouseEventType {
    LeftClick, MiddleClick, RightClick, MouseInsideArea
};

class MouseEvent {
public:
    MouseEvent(std::string name, MouseEventType mouseEventType, int areaX, int areaY, int areaWidth, int areaHeight);

    MouseEvent(std::string name, MouseEventType mouseEventType);

    bool conditionsMet();

    void update();

    void setMousePosition(sf::Vector2i mPos) {
        mousePosition = mPos;
    }

    std::string getName();

private:
    sf::IntRect myBounds; // The area in which the mouse event becomes active (if neccessary)
    bool enabled;
    MouseEventType eventType;
    sf::Vector2i mousePosition;
    bool hasFired;
    std::string name;
};

class MouseHandler {
public:
    MouseHandler(sf::RenderWindow *windowPointer);

    ~MouseHandler();

    void update();

    void draw();

    MouseEvent *addEvent(std::string eventName, MouseEventType eventType);

    MouseEvent *
    addEvent(std::string eventName, MouseEventType eventType, int areaX, int areaY, int areaWidth, int areaHeight);

    sf::Vector2i getMousePosition();

    bool isButtonClicked(sf::Mouse::Button button);
private:
    bool enabled;
    sf::RenderWindow *window;

    sf::Vector2i mousePosition;

    std::vector<MouseEvent *> events;

    bool wasClickedInLastFrame[16];
};

#endif
