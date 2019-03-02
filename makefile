CC = g++
CFLAGS = -std=c++17 -g -Wall -O2
GITVER = $(shell git rev-parse --abbrev-ref HEAD)
REVISION = $(shell git rev-parse HEAD)

DIRECTORIES_TO_INCLUDE = -I Game/Include -I Libs -I Libs/SFML/include -I Libs/Lua/Include -I Libs/SQLite/Include
FILES_TO_COMPILE = Game/Src/EntryPoint.cpp Game/Src/Misc/ParameterHandler.cpp Game/Src/Base/Engine.cpp
GAME_FRAMEWORK_FILES = Game/Src/TileRenderer/Tile.cpp Game/Src/TileRenderer/Tilemap.cpp Game/Src/TileRenderer/TileRenderer.cpp Game/Src/GameScreens/TestScreen.cpp Game/Src/Base/Game.cpp Game/Src/Base/GameManager.cpp Game/Src/Base/GameScreen.cpp Game/Src/Misc/Utils.cpp Game/Src/SpriteRenderer/SpriteRenderer.cpp Game/Src/SpriteRenderer/Sprite.cpp
LIBRARIES = -L Libs/SFML/lib -L Libs/SQLite -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsqlite3

DEFAULT_DEFINES = -DMULTITHREADED_RENDERING

INPUT_MANAGER_FILES = Game/Src/Input/InputManager.cpp Game/Src/Input/KeyboardHandler.cpp Game/Src/Input/Keymap.cpp Game/Src/Input/GamepadHandler.cpp Game/Src/Input/MouseHandler.cpp
RESOURCE_MANAGER_FILES = Game/Src/Resource/ResourceManager.cpp Game/Src/Resource/TextureManager.cpp
DATABASE_FILES = Game/Src/Database/DatabaseConnection.cpp Game/Src/Database/DatabaseSchema.cpp
AUDIO_MANAGER_FILES = Game/Src/Resource/MusicManager.cpp
FONT_MANAGER_FILES = Game/Src/Resource/FontManager.cpp
TEXT_RENDERER_FILES = Game/Src/TextRenderer/TextRenderer.cpp
VISUAL_NOVEL_ENGINE_FILES = Game/Src/VisualNovelEngine/Screens/NovelScreen.cpp Game/Src/VisualNovelEngine/Classes/UI/NovelTextDisplay.cpp Game/Src/VisualNovelEngine/Classes/Data/Novel.cpp
BACKGROUND_RENDERER_FILES = Game/Src/BackgroundRenderer/BackgroundImageRenderer.cpp Game/Src/BackgroundRenderer/BackgroundTransitionRenderer.cpp Game/Src/BackgroundRenderer/BackgroundTransition.cpp Game/Src/Misc/ColourBuilder.cpp

GAME_COMPILER_FILES = Game/Src/GameCompilerEntryPoint.cpp Game/Src/GameCompiler/GameCompiler.cpp Game/Src/GameCompiler/ProjectBuilder.cpp Game/Src/GameCompiler/ChapterBuilder.cpp Game/Src/GameCompiler/ResourceBuilder.cpp Game/Src/Misc/Utils.cpp

BUILD_STRING_WITHOUT_DEFINES = $(DIRECTORIES_TO_INCLUDE) -DCURRENT_BRANCH=$(GITVER) -DCURRENT_COMMIT=$(REVISION) $(GAME_FRAMEWORK_FILES) $(AUDIO_MANAGER_FILES) $(INPUT_MANAGER_FILES) $(RESOURCE_MANAGER_FILES) $(DATABASE_FILES) $(FONT_MANAGER_FILES) $(TEXT_RENDERER_FILES) $(FILES_TO_COMPILE) $(BACKGROUND_RENDERER_FILES) $(VISUAL_NOVEL_ENGINE_FILES) $(LIBRARIES) -o build\TaleScripter-Runner

all:
	$(CC) $(CFLAGS) -DMULTITHREADED_RENDERING -DDISPLAY_LICENCE $(BUILD_STRING_WITHOUT_DEFINES)

single-threaded-rendering:
	$(CC) $(CFLAGS) $(BUILD_STRING_WITHOUT_DEFINES)

compiler:
	$(CC) $(CFLAGS) $(DIRECTORIES_TO_INCLUDE) $(GAME_COMPILER_FILES) $(DATABASE_FILES) $(LIBRARIES) -o build\TaleScripter-compiler
