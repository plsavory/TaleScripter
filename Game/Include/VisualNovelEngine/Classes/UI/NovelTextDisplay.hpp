#ifndef NOVEL_TEXT_DISPLAY_INCLUDED
#define NOVEL_TEXT_DISPLAY_INCLUDED

class NovelTextDisplay {
public:
  NovelTextDisplay(TextRenderer *tRenderer);
  ~NovelTextDisplay();
  bool hasTextFinished();
  void update();
  void displayWholeStringImmediately();
private:
  TextRenderer *textRenderer;
  std::string storyFont;
  std::string currentDisplayText;
  std::string fullDisplayText;
  int textCounter;
  sf::Clock *textCounterClock;
  int textCounterDelay;
  Text *myText;
};

#endif
