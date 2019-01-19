#ifndef BACKGROUND_TRANSITION_RENDERER_INCLUDED
#define BACKGROUND_TRANSITION_RENDERER_INCLUDED

#include "BackgroundRenderer/BackgroundTransition.hpp"

class BackgroundTransitionRenderer {
public:
  BackgroundTransitionRenderer(sf::RenderWindow *windowPointer);
  ~BackgroundTransitionRenderer();
  void update();
  void draw();
  bool hasTransitionCompleted();
  void startTransition(int transitionType, sf::Color primaryColour, int delayBeforeStart, int delay, int animationLength);
  void setScreenSize(int width, int height);
  BackgroundTransition* getCurrentTransition();
  bool isInForeground();
private:
  sf::RenderWindow *window;
  BackgroundTransition *currentTransition;
  int screenWidth;
  int screenHeight;
};

#endif
