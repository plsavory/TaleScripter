// Add as many states as we need into here for future games

#ifndef BASE_GAME_MANAGER_INCLUDED
#define BASE_GAME_MANAGER_INCLUDED

// Include all of the game GameScreens
#include "ScreenState.h"
#include "VisualNovelEngine/Screens/NovelScreen.hpp"
#include "Menus/TitleScreen.h"
#include "Base/ErrorScreen.hpp"
#include "VisualNovelEngine/Classes/Data/Novel.hpp"

class GameManager {
public:
    GameManager(Engine *enginePointer, const std::string& initialErrorMessage);

    ~GameManager();

    void init();

    void update();

    void draw();

    void updateWindowPointers(sf::RenderWindow *windowPointer);

    void handleScreenChanges();

    void invokeErrorScreen(GeneralException &e);

    void invokeErrorScreen(const std::string& message);
private:

    Engine *engine;
    InputManager *inputManager;
    NovelScreen *novelScreen;
    ErrorScreen *errorScreen;
    NovelData *novel;
    ResourceManager *resourceManager;
    ScreenState *screenState;

    // Menus
    CommonUI *commonUI;
    TitleScreen *titleScreen;

    sf::Clock *gameTime;
};

#endif
