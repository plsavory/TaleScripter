//
// Created by Pete on 2019-06-09.
//

#ifndef TALESCRIPTER_BUTTON_H
#define TALESCRIPTER_BUTTON_H

class Button {
public:
    Button(sf::RenderWindow *renderWindow, ResourceManager *rManager);

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
private:
    sf::RenderWindow *window;
    ResourceManager *resourceManager;
    sf::Vector2f position;
    std::string name;
    sf::Text *text;
    sf::RectangleShape *selectedRectangle;
};

#endif //TALESCRIPTER_BUTTON_H
