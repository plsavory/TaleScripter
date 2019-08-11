#include "UI/Groups/Menu.h"
#include "GameData/GameSaveManager.h"

class DataMenuGameSave {
public:
    DataMenuGameSave(sf::RenderWindow *renderWindow, ResourceManager *rManager, InputManager *inputManager,
                     sf::Vector2f pos, int saveId, int saveMode, GameSaveManager *gsManager);

    ~DataMenuGameSave();

    bool update();

    void draw();

    void setText(std::string newText);

private:
    int id;
    sf::RenderWindow *window;
    ResourceManager *resourceManager;
    sf::RectangleShape *bg;
    sf::Vector2f position;
    sf::Text *text;
    InputManager *inputManager;
    GameSaveManager *gameSaveManager;
    int saveMode;
};

class DataMenu {
public:
    DataMenu(sf::RenderWindow *renderWindow, ResourceManager *rManager, InputManager *inputManager, int menuMode,
             GameSaveManager *gsManager);

    ~DataMenu();

    void update(sf::Clock *gameTime);

    void draw();

    bool needsToBeClosed();

    static const int MODE_SAVE_ONLY = 0;
    static const int MODE_LOAD_ONLY = 1;
    static const int MODE_DEFAULT = 3;

    bool hasSelectedASave();

    int getSelectedSave();

    int getMode();

    void resetSelectedSave();

    void close();

    void setOffset(int newOffset);

    void reset() {
        selectedSave = 0;
    }

    void getSaves();

private:
    sf::RenderWindow *window;
    ResourceManager *resourceManager;
    sf::RectangleShape *bg;
    sf::Text *heading;
    sf::Text *pageText;
    int mode;
    std::vector<DataMenuGameSave *> saves;
    int offset;
    Menu *navigationMenu;
    bool needsClosing;
    int navigationButtonCount;
    int selectedSave;
    InputManager *inputManager;
    GameSaveManager *gameSaveManager;

    int populateSaves();
};