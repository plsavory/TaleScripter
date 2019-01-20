#ifndef NOVEL_TEXT_DISPLAY_INCLUDED
#define NOVEL_TEXT_DISPLAY_INCLUDED

class NovelTextDisplay {
public:
  NovelTextDisplay(TextRenderer *tRenderer, SpriteRenderer *sRenderer, ResourceManager *rManager);
  ~NovelTextDisplay();
  bool hasTextFinished();
  void update();
  void displayWholeStringImmediately();
  void setText(std::string newText, std::string cName);
  void setVisible();
  void setInvisible();
  void clear();
private:
  std::string wordWrap(std::string textToWrap, float maxWidth);
  TextRenderer *textRenderer;
  FontManager *fontManager;
  SpriteRenderer *spriteRenderer;
  ResourceManager *resourceManager;
  std::string storyFont;
  std::string currentDisplayText;
  std::string fullDisplayText;
  std::string characterName;
  int textCounter;
  sf::Clock *textCounterClock;
  int textCounterDelay;
  Text *myText;
  Text *nameDisplayText;
  int maxTextWidth;
  Sprite *backgroundSprite;
  bool updateNameDisplayText;
};

#endif
