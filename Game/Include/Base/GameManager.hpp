// Add as many states as we need into here for future games

#ifndef BASE_GAME_MANAGER_INCLUDED
#define BASE_GAME_MANAGER_INCLUDED

// Include all of the game GameScreens
#include "VisualNovelEngine/Screens/NovelScreen.hpp"
#include "Base/ErrorScreen.hpp"
#include "VisualNovelEngine/Classes/Data/Novel.hpp"

// Don't give anything in this enum the same name as a class, it breaks the build process - I learned this the hard way.
enum GameState {
    Init, Title, Menu, GameField, Test, Novel, ExceptionCaught
};

class GameManager {
public:
    GameManager(Engine *enginePointer, const std::string& initialErrorMessage);

    ~GameManager();

    void init();

    void update();

    void draw();

    void updateWindowPointers(sf::RenderWindow *windowPointer);

    void changeScreen(GameState newState);

    void invokeErrorScreen(GeneralException &e);

    void invokeErrorScreen(const std::string& message);
private:

    GameState currentGameState;
    Engine *engine;
    InputManager *inputManager;
    NovelScreen *novelScreen;
    ErrorScreen *errorScreen;
    NovelData *novel;
    ResourceManager *resourceManager;
};

#endif
