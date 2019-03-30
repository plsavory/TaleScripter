class Sprite {
public:
  Sprite(TextureManager *sTextureManager,sf::RenderWindow *window, std::string sName, std::string sImageName, int sPriority, int myId);
  Sprite(TextureManager *sTextureManager, sf::RenderWindow *window, std::string sName, int myId);
  ~Sprite();
  sf::Sprite* getSfmlSprite();
  bool setImage(sf::Texture *image);
  void update();
  void draw();
  void setPosition(int x, int y);
  void setOrigin(int x, int y);
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
  void setTextureName(std::string name);
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
};
