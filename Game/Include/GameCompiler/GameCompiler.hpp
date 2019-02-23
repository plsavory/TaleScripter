class GameCompiler {
public:
  GameCompiler(GameCompilerOptions *gameCompilerOptions);
  ~GameCompiler();
  bool process();
private:
  void createResourceDatabase();
  void createNovelDatabase();
  GameCompilerOptions *compilerOptions;
};
