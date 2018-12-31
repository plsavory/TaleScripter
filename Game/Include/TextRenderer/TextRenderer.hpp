#ifndef TEXT_RENDERER_INCLUDED
#define TEXT_RENDERER_INCLUDED

#define MAX_TEXT_OBJECTS 50
#define RETRY_LOADING_TIME 250

struct Text {
public:
  Text(std::string name, std::string fontName, FontManager *fManager) {
    fontManager = fManager;
    textObj = new sf::Text();
    fontSet = false;
    loadAttempted = false;
    textObj->setFillColor(sf::Color::White); // TODO: Un-hardcode this
    setFont(fontName);
  };
  ~Text() {
    delete textObj;
  };
  std::string getName() {
    return name;
  }
  void setFont(std::string fName) {
    fontName = fName;

    setFont();
  }
  void setFont() {
    Font *fontFindAttempt = fontManager->getFont(fontName);

    if (!fontFindAttempt) {
      return;
    }

    font = fontFindAttempt;

    if (!font->getFont()) {
      loadAttempted = true; // Font hasn't been loaded into memory yet, set this so that it will be re-tried
      return;
    }

    textObj->setFont(*font->getFont());

    fontSet = true;
  }
  bool hasFontLoaded() {
    return !(font == NULL);
  }
  sf::Text* getTextObject() {
    return textObj;
  }
  /**
   * [setString Sets the string on the text]
   * @param text [The text, prefix the string with an L to get special characters (such as Arabic) to display]
   */
  void setString(std::string text) {
    textObj->setString(text);
  }
  void setPosition(int x, int y) {
    textObj->setPosition(sf::Vector2f((float) x,(float) y));
  }
  void reAttemptLoad() {
    if (!loadAttempted) {
      return; // We've not tried to load yet
    }

    if (fontSet) {
      return; // No error has occured
    }

    setFont(); // Attempt to load a font
  }
private:
  std::string name;
  std::string fontName;
  sf::Text *textObj;
  Font *font;
  FontManager *fontManager;
  bool fontSet;
  bool loadAttempted;
};

class TextRenderer {
public:
  TextRenderer(sf::RenderWindow *windowPointer, FontManager *fontManagerPointer);
  ~TextRenderer();
  void update();
  void draw();
  Text* addText(std::string name, std::string font);
private:
  sf::RenderWindow *window;
  FontManager *fontManager;
  Text *text[MAX_TEXT_OBJECTS];
  sf::Clock *retryLoadClock;
};

#endif
