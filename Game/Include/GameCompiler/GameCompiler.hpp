#ifndef GAME_COMPILER_INCLUDED
#define GAME_COMPILER_INCLUDED

class GameCompiler {
public:
    GameCompiler(GameCompilerOptions *gameCompilerOptions, JsonHandler *fileHandler);

    ~GameCompiler();

    bool process();

private:
    void createResourceDatabase();

    static void createNovelDatabase();

    GameCompilerOptions *compilerOptions;
    JsonHandler *fHandler;
};

#endif
