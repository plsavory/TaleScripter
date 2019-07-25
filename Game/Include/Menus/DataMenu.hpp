#include "UI/Groups/Menu.h"

class DataMenuGameSave {
public:
    DataMenuGameSave(sf::RenderWindow *renderWindow, ResourceManager *rManager, sf::Vector2f pos);
    ~DataMenuGameSave();
    void update();
    void draw();
    void setText(std::string newText);
private:
    sf::RenderWindow *window;
    ResourceManager *resourceManager;
    sf::RectangleShape *bg;
    sf::Vector2f position;
    sf::Text *text;
};

class DataMenu {
public:
    DataMenu(sf::RenderWindow *renderWindow, ResourceManager *rManager, InputManager *inputManager);
    ~DataMenu();
    void update(sf::Clock *gameTime);
    void draw();
    bool needsToBeClosed();
    static const int MODE_SAVE_ONLY = 0;
    static const int MODE_LOAD_ONLY = 1;
    static const int MODE_DEFAULT = 3;
private:
    sf::RenderWindow *window;
    ResourceManager *resourceManager;
    sf::RectangleShape *bg;
    sf::Text *heading;
    int mode;
    std::vector<DataMenuGameSave*> saves;
    int offset;
    Menu *navigationMenu;
    bool needsClosing;
    int navigationButtonCount;
};