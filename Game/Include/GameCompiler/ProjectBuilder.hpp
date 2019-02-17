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
    ProjectBuilder(std::string fileName);
    ~ProjectBuilder();
};
