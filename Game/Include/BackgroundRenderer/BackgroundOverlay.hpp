/* The BackgroundOverlay class draws a solid colour over backgrounds and sprites,
     Its position in the rendering pipeline will be able to be manually set, so that it appears either behind or above sprites

     It is intended to be replacement for the BackgroundTransitionHandler class, and can be used to display either solid colours or
     images over the background layer.

     I have decided to separate this out from the background renderer itself as it will only ever be used for transitions or visual effects
     and should never be a replacement for an actual background.
*/

#ifndef BACKGROUND_OVERLAY_INCLUDED
#define BACKGROUND_OVERLAY_INCLUDED

class BackgroundOverlay {
public:
    BackgroundOverlay(sf::RenderWindow *targetWindow);
    ~BackgroundOverlay();
    void update();
    void draw();
    void setAlpha(int alpha);
    int getAlpha();
    void setColour(sf::Color newColour);
    sf::Color getColour();
    bool isInForeground();
private:
    sf::RenderWindow *window;
    sf::RectangleShape *overlay;
    sf::Color *colour;
    bool foreground;
};

#endif