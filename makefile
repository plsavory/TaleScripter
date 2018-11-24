CC = g++
CFLAGS = -std=c++17 -g -Wall -O3

DIRECTORIES_TO_INCLUDE = -I Game/Include -I Libs/SFML/include -I Libs/Lua/Include -I Libs/SQLite/Include
FILES_TO_COMPILE = Game/Src/EntryPoint.cpp
GAME_FRAMEWORK_FILES = Game/Src/TileRenderer/Tile.cpp Game/Src/TileRenderer/Tilemap.cpp Game/Src/TileRenderer/TileRenderer.cpp Game/Src/Base/Game.cpp Game/Src/Base/GameManager.cpp Game/Src/Base/GameScreen.cpp Game/Src/Misc/Utils.cpp
LIBRARIES = -L Libs/SFML/lib -lsfml-graphics -lsfml-window -lsfml-system

all:
	$(CC) $(CFLAGS) $(DIRECTORIES_TO_INCLUDE) $(GAME_FRAMEWORK_FILES) $(FILES_TO_COMPILE) $(LIBRARIES) -o build\Test
