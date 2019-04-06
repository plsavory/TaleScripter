#ifndef ERROR_SCREEN_INCLUDED
#define ERROR_SCREEN_INCLUDED

class ErrorScreen {
public:
    explicit ErrorScreen(sf::RenderWindow *window);
    ~ErrorScreen() = default;
    void start(GeneralException &exception);
    void start(const std::string& message);
    void update();
    void draw();
private:
    sf::Font *errorFont;
    sf::Text *errorText;
    sf::Text *headingText;
    sf::RenderWindow *mainWindow;
    void init(const std::string& errorMessage);
};

#endif