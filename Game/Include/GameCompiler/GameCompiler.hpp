#ifndef GAME_COMPILER_INCLUDED
#define GAME_COMPILER_INCLUDED

class GameCompiler {
public:
  GameCompiler(GameCompilerOptions *gameCompilerOptions, FileHandler *fileHandler);
  ~GameCompiler();
  bool process();
private:
  void createResourceDatabase();
  void createNovelDatabase();
  GameCompilerOptions *compilerOptions;
  FileHandler *fHandler;
};

#endif
