#ifndef BACKGROUND_IMAGE_RENDERER_INCLUDED
#define BACKGROUND_IMAGE_RENDERER_INCLUDED

#define MAX_BACKGROUNDS 50

#include <queue>

enum BackgroundStatus {bgLoaded, bgUnloaded, bgError};

class BackgroundImageAttributes {
public:
  BackgroundImageAttributes(float mWidth, float mHeight, float oLeft, float oTop) {
    maxHeight = mHeight;
    maxWidth = mWidth;
    offsetLeft = oLeft;
    offsetTop = oTop;
  }
  float getMaxHeight() {
    return maxHeight;
  }
  float getMaxWidth() {
    return maxWidth;
  }
  float getOffsetLeft() {
    return offsetLeft;
  }
  float getOffsetTop() {
    return offsetTop;
  }
private:
  float maxHeight;
  float maxWidth;
  float offsetLeft;
  float offsetTop;
};

class Background {
public:
  Background(std::string bName, std::string bFilename, sf::RenderWindow *windowPointer) {
    name = bName;
    fileName = bFilename;
    myStatus = BackgroundStatus::bgUnloaded;
    myTexture = NULL;
    mySprite = new sf::Sprite();
    mySprite->setPosition(sf::Vector2f(0.f,0.f));
    window = windowPointer;
  }
  ~Background() {
    if (myTexture) {
      delete myTexture;
    }

    if (mySprite) {
      delete mySprite;
    }
  }
  void load() {
    if (!myTexture) {
      myTexture = new sf::Texture();
    }

    myTexture->loadFromFile(fileName);
    mySprite->setTexture(*myTexture, true);

    if (attributes) {
      // Apply the attributes onto the sprite object
      mySprite->setScale(attributes->getMaxWidth()/myTexture->getSize().x,attributes->getMaxHeight()/myTexture->getSize().y);
    }
    myStatus = BackgroundStatus::bgLoaded;
  }
  BackgroundStatus getStatus() {
    return myStatus;
  }
  void draw() {
    if (myTexture) {
      if (myStatus == BackgroundStatus::bgLoaded) {
        window->draw(*mySprite);
      }
    }
  }
  std::string getName() {
    return name;
  }
  void setAttributes(BackgroundImageAttributes *a) {
    if (a) {
      attributes = a;
    }
  }
private:
  std::string name;
  std::string fileName;
  int id;
  sf::Texture *myTexture;
  sf::Sprite *mySprite;
  BackgroundStatus myStatus;
  sf::RenderWindow *window;
  BackgroundImageAttributes *attributes = NULL;
};

class BackgroundLoadRequest {
  public:
    BackgroundLoadRequest(int backgroundId) {
      id = backgroundId;
    }
    ~BackgroundLoadRequest() {

    }
    int getId() {
      return id;
    }
  private:
    int id;
};

class BackgroundImageRenderer {
public:
  BackgroundImageRenderer(sf::RenderWindow *windowPointer);
  ~BackgroundImageRenderer();
  void addAllFromDatabase(DatabaseConnection *db);
  void processQueue();
  bool isQueueEmpty();
  void update();
  void draw();
  void setBackground(std::string name);
  int findBackground(std::string name);
  void setBackgroundColour(sf::Color *colour);
  sf::Color* getBackgroundColour();
private:
  sf::RenderWindow *window;
  Background* addBackground(std::string name, std::string filename);
  Background *background[MAX_BACKGROUNDS];
  std::queue<BackgroundLoadRequest> backgroundLoadQueue;
  Background *currentBackground;
  Background *upcomingBackground; // Used for transitions
  void loadBackground(int id);
  sf::Color *backgroundColour;
};

#endif
