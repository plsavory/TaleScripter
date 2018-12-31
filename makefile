CC = g++
CFLAGS = -std=c++17 -g -Wall -O3

DIRECTORIES_TO_INCLUDE = -I Game/Include -I Libs -I Libs/SFML/include -I Libs/Lua/Include -I Libs/SQLite/Include
FILES_TO_COMPILE = Game/Src/EntryPoint.cpp
GAME_FRAMEWORK_FILES = Game/Src/TileRenderer/Tile.cpp Game/Src/TileRenderer/Tilemap.cpp Game/Src/TileRenderer/TileRenderer.cpp Game/Src/GameScreens/TestScreen.cpp Game/Src/Base/Game.cpp Game/Src/Base/GameManager.cpp Game/Src/Base/GameScreen.cpp Game/Src/Misc/Utils.cpp Game/Src/SpriteRenderer/SpriteRenderer.cpp Game/Src/SpriteRenderer/Sprite.cpp
LIBRARIES = -L Libs/SFML/lib -L Libs/SQLite -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsqlite3

INPUT_MANAGER_FILES = Game/Src/Input/InputManager.cpp Game/Src/Input/KeyboardHandler.cpp Game/Src/Input/Keymap.cpp Game/Src/Input/GamepadHandler.cpp Game/Src/Input/MouseHandler.cpp
RESOURCE_MANAGER_FILES = Game/Src/Resource/ResourceManager.cpp Game/Src/Resource/TextureManager.cpp
DATABASE_FILES = Game/Src/Database/DatabaseConnection.cpp Game/Src/Database/DatabaseSchema.cpp
AUDIO_MANAGER_FILES = Game/Src/Resource/MusicManager.cpp
GAME_COMPILER_FILES = Game/Src/GameCompilerEntryPoint.cpp Game/Src/GameCompiler/GameCompiler.cpp Game/Src/Misc/Utils.cpp
FONT_MANAGER_FILES = Game/Src/Resource/FontManager.cpp
TEXT_RENDERER_FILES = Game/Src/TextRenderer/TextRenderer.cpp
VISUAL_NOVEL_ENGINE_FILES = Game/Src/VisualNovelEngine/Screens/NovelScreen.cpp Game/Src/VisualNovelEngine/Classes/UI/NovelTextDisplay.cpp

all:
	$(CC) $(CFLAGS) $(DIRECTORIES_TO_INCLUDE) $(GAME_FRAMEWORK_FILES) $(AUDIO_MANAGER_FILES) $(INPUT_MANAGER_FILES) $(RESOURCE_MANAGER_FILES) $(DATABASE_FILES) $(FONT_MANAGER_FILES) $(TEXT_RENDERER_FILES) $(FILES_TO_COMPILE) $(VISUAL_NOVEL_ENGINE_FILES) $(LIBRARIES) -o build\Test

compiler:
	$(CC) $(CFLAGS) $(DIRECTORIES_TO_INCLUDE) $(GAME_COMPILER_FILES) $(DATABASE_FILES) $(LIBRARIES) -o build\Test-compiler
