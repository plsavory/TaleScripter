//
// Created by Pete on 2019-06-09.
//

#ifndef TALESCRIPTER_BUTTON_H
#define TALESCRIPTER_BUTTON_H

class Button {
public:
    Button(sf::RenderWindow *renderWindow, ResourceManager *rManager, MouseHandler *mHandler, bool isCompact);

    ~Button();

    void update(sf::Clock *gameTime);

    void drawText();

    void drawBack(bool selected);

    void setName(std::string newName);

    std::string getName();

    void setText(std::string newText);

    void setPosition(sf::Vector2f newPosition);

    sf::Vector2f getPosition();

    sf::FloatRect getSize();

    bool isClicked();

    bool handleMouseInput();

    void setSize();

    void setSize(sf::Vector2f size);
private:
    sf::RenderWindow *window;
    ResourceManager *resourceManager;
    sf::Vector2f position;
    std::string name;
    bool compact;
    sf::Text *text;
    sf::RectangleShape *selectedRectangle;
    MouseHandler *mouseHandler;
};

#endif //TALESCRIPTER_BUTTON_H
