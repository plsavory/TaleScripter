#ifndef NOVEL_TEXT_DISPLAY_INCLUDED
#define NOVEL_TEXT_DISPLAY_INCLUDED

class NovelTextDisplay {
public:
  NovelTextDisplay(TextRenderer *tRenderer);
  ~NovelTextDisplay();
  bool hasTextFinished();
  void update();
  void displayWholeStringImmediately();
  void setText(std::string newText);
private:
  std::string wordWrap(std::string textToWrap, float maxWidth);
  TextRenderer *textRenderer;
  FontManager *fontManager;
  std::string storyFont;
  std::string currentDisplayText;
  std::string fullDisplayText;
  int textCounter;
  sf::Clock *textCounterClock;
  int textCounterDelay;
  Text *myText;
  int maxTextWidth;
};

#endif
