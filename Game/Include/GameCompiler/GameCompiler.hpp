class GameCompiler {
public:
  GameCompiler(GameCompilerOptions *gameCompilerOptions);
  ~GameCompiler();
private:
  void createResourceDatabase();
  void createNovelDatabase();
  GameCompilerOptions *compilerOptions;
};
