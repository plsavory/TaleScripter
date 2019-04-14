#ifndef TEXT_RENDERER_INCLUDED
#define TEXT_RENDERER_INCLUDED

#define MAX_TEXT_OBJECTS 50
#define RETRY_LOADING_TIME 250

struct Text {
public:
    Text(const std::string &name, const std::string &fontName, FontManager *fManager) {
        fontManager = fManager;
        textObj = new sf::Text();
        fontSet = false;
        loadAttempted = false;
        textObj->setFillColor(sf::Color::White); // TODO: Un-hardcode this
        setFont(fontName);
        enabled = true;
        automaticDraw = true;
    };

    ~Text() {
        delete textObj;
    };

    std::string getName() {
        return name;
    }

    void setFont(const std::string &fName) {
        fontName = fName;
        setFont();
    }

    void setFont() {
        Font *fontFindAttempt = fontManager->getFont(fontName);

        if (!fontFindAttempt) {
            // A font needs to be added to the FontManager before we attempt to use it.
            std::vector<std::string> errorMessage = {
                    "No font with the name '", fontName, "' has been loaded"
            };
            throw ResourceException(Utils::implodeString(errorMessage));
        }

        font = fontFindAttempt;

        if (!font->getFont()) {
            loadAttempted = true; // Font hasn't been loaded into memory yet, set this so that it will be re-tried
            return;
        }

        textObj->setFont(*font->getFont());

        fontSet = true;
    }

    void setOutline(sf::Color color, float thickness) {
        textObj->setOutlineColor(color);
        textObj->setOutlineThickness(thickness);
    }

    bool hasFontLoaded() {
        return font != nullptr;
    }

    sf::Text *getTextObject() {
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
        textObj->setPosition(sf::Vector2f((float) x, (float) y));
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

    void setVisible(bool toEnable) {
        enabled = toEnable;
    }

    bool isVisible() {
        return enabled;
    }

    bool automaticDrawEnabled() {
        return automaticDraw;
    }

    void disableAutomaticDrawing() {
        automaticDraw = false;
    }

    void enableAutomaticDrawing() {
        automaticDraw = true;
    }

private:
    std::string name;
    std::string fontName;
    sf::Text *textObj;
    Font *font;
    FontManager *fontManager;
    bool fontSet;
    bool loadAttempted;
    bool enabled;
    bool automaticDraw; // If set to false, this text will be skipped when the text renderer is drawing
};

class TextRenderer {
public:
    TextRenderer(sf::RenderWindow *windowPointer, FontManager *fontManagerPointer);

    ~TextRenderer();

    void update();

    void draw();

    Text *addText(const std::string &name, const std::string &font);

    void removeText(const std::string &name);

    FontManager *getFontManager();

private:
    sf::RenderWindow *window;
    FontManager *fontManager;
    std::vector<Text *> text;
    sf::Clock *retryLoadClock;
};

#endif
