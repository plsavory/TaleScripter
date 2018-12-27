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

all:
	$(CC) $(CFLAGS) $(DIRECTORIES_TO_INCLUDE) $(GAME_FRAMEWORK_FILES) $(AUDIO_MANAGER_FILES) $(INPUT_MANAGER_FILES) $(RESOURCE_MANAGER_FILES) $(DATABASE_FILES) $(FILES_TO_COMPILE) $(LIBRARIES) -o build\Test
