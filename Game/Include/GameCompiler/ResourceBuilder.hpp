#ifndef RESOURCE_BUILDER_INCLUDED
#define RESOURCE_BUILDER_INCLUDED

class ResourceBuilder {
public:
  ResourceBuilder(DatabaseConnection *resourceDb, const std::string &projectDirectory, JsonHandler *fileHandler);
  ~ResourceBuilder();
  void process();
private:
  DatabaseConnection *resource;
  std::string resourceDirectory;
  std::string assetDirectory;
  JsonHandler *fHandler;
  void processBackgroundTextures();
  void processBackgrounds();
  void processTextures();
  void processSprites();
  void processFonts();
  void processMusic();
  void processBackgroundLayer(int backgroundId, json layerJson);
};

#endif
