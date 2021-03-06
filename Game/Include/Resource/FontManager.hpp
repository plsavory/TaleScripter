#ifndef FONT_MANAGER_INCLUDED
#define FONT_MANAGER_INCLUDED

#include <queue>

enum FontState {fsUnloaded, fsLoaded, fsError};

// TODO: Allow whether a font should be loaded on use or on instantiation as a config option

#define MAX_FONTS 50

struct FontLoadRequest {
public:
  FontLoadRequest(int id) {
    fontId = id;
  }
  int fontId;
};

struct Font {
public:
  Font(std::string fontName, std::string filename) {

    // Set default values on our variables
    myFont = nullptr;
    name = fontName;
    fontFilename = filename;
    state = FontState::fsUnloaded;
  }
  ~Font() {
      delete myFont;
  }
  std::string getName() {
    return name;
  }
  std::string getFilename() {
    return fontFilename;
  }
  void loadFont() {

    // Create font object in memory if it doesn't exist
    if (!myFont) {
      myFont = new sf::Font();
    }

    // Attempt to load the font file
    if (!myFont->loadFromFile(fontFilename)) {
        std::vector<std::string> errorMessage = {
                "Unable to load font ",name," (File: ",fontFilename," is missing, in the wrong format or corrupted)"
        };
        throw ResourceException(Utils::implodeString(errorMessage));
    }

    state = FontState::fsLoaded;

  }
  sf::Font* getFont() {
    return myFont;
  }
private:
  std::string name;
  std::string fontFilename;
  sf::Font *myFont;
  FontState state;
};

class FontManager {
public:
  FontManager();
  ~FontManager();
  Font* addFont(std::string name, std::string filename);
  void loadAllFromDatabase(DatabaseConnection *database);
  void processQueue();
  void loadFont(const std::string& name);
  void loadFont(int id);
  Font* getFont(const std::string& name);
  bool isQueueEmpty();
private:
  Font *font[MAX_FONTS];
  std::queue<FontLoadRequest> fontLoadQueue;
  int findFont(std::string name);
  bool loadFontImmediately; // TODO: Load from config
};

#endif
