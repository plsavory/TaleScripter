class Sprite {
public:
  Sprite(TextureManager *sTextureManager,sf::RenderWindow *window, std::string sName, std::string sImageName, int sPriority);
  ~Sprite();
  sf::Sprite* getSfmlSprite();
  bool setImage(sf::Texture *image);
  void update();
  void draw();
  void setPosition(int x, int y);
  std::string name;
  int priority;
  bool textureSet;
private:
  sf::Sprite *mySprite;
  sf::Texture *myImage;
  int animationFrame;
  int animationSpeed;
  sf::RenderWindow *displayWindow;
  std::string imageName;
  int textureId;
  TextureManager *textureManager;
};
