// TODO: Move this - I have no clue what I was thinking last weekend.
class GameCompilerOptions {
public:
  GameCompilerOptions() {
    // Set some defaults
    projectFilePath = std::string("");
  }
  void setProjectFilePath(std::string path) {
    projectFilePath = path;
  }
  std::string getProjectFilePath() {
    return projectFilePath;
  }
private:
  std::string projectFilePath;
};

class ProjectBuilder {
  public:
    ProjectBuilder(std::string fileName, DatabaseConnection *novelDb, DatabaseConnection *resourceDb, FileHandler *fileHandler);
    ~ProjectBuilder();
    void process();
  private:
    std::string projectFileName;
    std::string projectPath;
    DatabaseConnection *novel;
    DatabaseConnection *resource;
    void processCharacters();
    FileHandler *fHandler;
};
