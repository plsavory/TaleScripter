#ifndef BACKGROUND_TRANSITION_INCLUDED
#define BACKGROUND_TRANSITION_INCLUDED

class BackgroundTransition {
public:
    BackgroundTransition(sf::RenderWindow *windowPointer, int delayBeforeStart, int transitionType, int screenWidth,
                         int screenHeight, int delayAfterFinish, int animationLength,
                         BackgroundImageRenderer *bgRenderer, BackgroundOverlay *bgOverlay);

    ~BackgroundTransition();

    bool update();

    void draw();

    void setPrimaryColour(sf::Color colour);

    void setToForeground() {
        foregroundTransition = true;
    }

    void setToBackground() {
        foregroundTransition = false;
    }

    bool isForegroundTransition() {
        return foregroundTransition;
    }

    // Static types
    static const int FADE_IN;
    static const int FADE_OUT;
    static const int MORPH;
private:
    sf::RenderWindow *window;
    int width;
    int height;
    sf::Color primaryColour;
    sf::Color renderColour;
    int delay;
    int startDelay;
    int alpha;
    int type;
    double length;
    sf::Clock *delayClock;
    sf::Clock *startDelayClock;
    bool drawBeforeStartDelay;
    bool transitionCompleted;
    bool foregroundTransition;
    sf::RectangleShape *rectangleShape;
    BackgroundImageRenderer *backgroundImageRenderer;
    BackgroundOverlay *backgroundOverlay;

    // Transition-specific init functions
    void FadeInInit();

    void FadeOutInit();

    void FadeMorphInit();

    // Transition-specific update functions
    void FadeInUpdate();

    void FadeOutUpdate();

    void FadeMorphUpdate();

};

#endif
