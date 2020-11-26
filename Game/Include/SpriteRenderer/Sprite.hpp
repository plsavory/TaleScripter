class Sprite {
public:
  Sprite(TextureManager *sTextureManager,sf::RenderWindow *window, const std::string& sName, const std::string& sImageName, int sPriority, int myId);
  Sprite(TextureManager *sTextureManager, sf::RenderWindow *window, const std::string& sName, int myId);
  ~Sprite();
  sf::Sprite* getSfmlSprite() {
      return mySprite;
  };
  bool setImage(sf::Texture *image);
  void update();
  void draw();
  void setPosition(int x, int y);
  void setOrigin(float x, float y);
  void setColour(sf::Color colour) {
      myColour = colour;
      mySprite->setColor(colour);
  };

  void setScaleFactor(sf::Vector2f scale) {
      myScale = scale;
      mySprite->setScale(scale);
  }

  void updateAttributes();
  std::string name;
  int priority;
  bool isVisible() {
    return visible;
  };
  void setVisible(bool shouldBeVisible) {
    visible = shouldBeVisible;
  }
  int getId() {
    return id;
  }
  bool isLoaded() {
    return textureSet;
  }
  void setTextureName(std::string name, bool switchImmediately);
  sf::FloatRect getSize();
private:
  sf::Sprite *mySprite;
  sf::Texture *myImage;
  int animationFrame;
  int animationSpeed;
  sf::RenderWindow *displayWindow;
  std::string textureName;
  int textureId;
  TextureManager *textureManager;
  bool visible;
  int id;
  bool textureSet;
  sf::Vector2f myPosition = sf::Vector2f(0,0);
  sf::Vector2f myScale = sf::Vector2f(1,1);
  sf::Color myColour = sf::Color(255,255,255,255);
  sf::Vector2f origin;
};
