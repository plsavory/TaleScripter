class ChapterBuilder {
public:
  ChapterBuilder(std::string fileName, DatabaseConnection *novelDb);
  ~ChapterBuilder();
  void process();
private:
  DatabaseConnection *novel;
  std::string chapterFileName;
  void processScene(json sceneJson, int chapterId);
  void processSceneSegment(json sceneSegmentJson, int sceneId);
  void processLine(json lineJson, int sceneSegmentId);
};
