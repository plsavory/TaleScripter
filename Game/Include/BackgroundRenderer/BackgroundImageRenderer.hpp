#ifndef BACKGROUND_IMAGE_RENDERER_INCLUDED
#define BACKGROUND_IMAGE_RENDERER_INCLUDED

#define MAX_BACKGROUNDS 50

#include <queue>

enum BackgroundStatus {bgLoaded, bgUnloaded, bgError};
enum TransitionType {none, fate, whiteout, blink};

class BackgroundTransition {
  BackgroundTransition(TransitionType tType) {
    transitionType = tType;
  }
private:
  TransitionType transitionType;
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

    std::cout<<"Background added:" << name << " file: "<<bFilename<<std::endl;
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
private:
  std::string name;
  std::string fileName;
  int id;
  sf::Texture *myTexture;
  sf::Sprite *mySprite;
  BackgroundStatus myStatus;
  sf::RenderWindow *window;
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
private:
  sf::RenderWindow *window;
  Background* addBackground(std::string name, std::string filename);
  Background *background[MAX_BACKGROUNDS];
  std::queue<BackgroundLoadRequest> backgroundLoadQueue;
  Background *currentBackground;
  Background *upcomingBackground; // Used for transitions
  void loadBackground(int id);
};

#endif
