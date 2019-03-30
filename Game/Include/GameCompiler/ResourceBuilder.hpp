#ifndef RESOURCE_BUILDER_INCLUDED
#define RESOURCE_BUILDER_INCLUDED

class ResourceBuilder {
public:
  ResourceBuilder(DatabaseConnection *resourceDb, std::string projectDirectory);
  ~ResourceBuilder();
  void process();
private:
  DatabaseConnection *resource;
  std::string resourceDirectory;
  std::string assetDirectory;
  void processBackgroundImages();
  void processTextures();
  void processSprites();
  void processFonts();
  void processMusic();
};

#endif
