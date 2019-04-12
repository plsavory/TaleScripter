#ifndef BACKGROUND_TRANSITION_RENDERER_INCLUDED
#define BACKGROUND_TRANSITION_RENDERER_INCLUDED

#include "BackgroundRenderer/BackgroundTransition.hpp"

class BackgroundTransitionHandler {
public:
    BackgroundTransitionHandler(sf::RenderWindow *windowPointer, BackgroundImageRenderer *bgImageRenderer,
                                BackgroundOverlay *bgOverlay);

    ~BackgroundTransitionHandler();

    void update();

    bool hasTransitionCompleted();

    void
    startTransition(int transitionType, sf::Color primaryColour, int delayBeforeStart, int delay, int animationLength);

    void setScreenSize(int width, int height);

    BackgroundTransition *getCurrentTransition();

private:
    sf::RenderWindow *window;
    BackgroundTransition *currentTransition;
    BackgroundImageRenderer *backgroundImageRenderer;
    BackgroundOverlay *backgroundOverlay;
    int screenWidth;
    int screenHeight;
};

#endif
