class Sprite {
public:
  Sprite(sf::RenderWindow *window);
  ~Sprite();
  sf::Sprite* getSfmlSprite();
  bool setImage(sf::Texture *image);
  void update();
  void draw();
  void setPosition(int x, int y);
private:
  sf::Sprite *mySprite;
  sf::Texture *myImage;
  int animationFrame;
  int animationSpeed;
  sf::RenderWindow *displayWindow;
};
