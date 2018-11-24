class Tilemap {
public:
  Tilemap();
  Tilemap(std::string fileName);
  ~Tilemap();
private:
  unsigned short width;
  unsigned short height;
  unsigned char addressingMode;
  unsigned short renderWidth;
  unsigned short renderHeight;
  Tile *tiles[0xFFFF];
};
