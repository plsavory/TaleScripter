cmake_minimum_required(VERSION 3.13)
project(TaleScripter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_PREFIX_PATH Libs)
set(EXECUTABLE_NAME "TaleScripter-Runner")
set(COMPILER_EXECUTABLE_NAME "TaleScripter-Compiler")

set(CMAKE_VERBOSE_MAKEFILE 1)

include_directories(Game/Include)
include_directories(Game/Include/BackgroundRenderer)
include_directories(Game/Include/Base)
include_directories(Game/Include/Database)
include_directories(Game/Include/GameCompiler)
include_directories(Game/Include/Input)
include_directories(Game/Include/Misc)
include_directories(Game/Include/Resource)
include_directories(Game/Include/SpriteRenderer)
include_directories(Game/Include/TextRenderer)
include_directories(Game/Include/TileRenderer)
include_directories(Game/Include/VisualNovelEngine)
include_directories(Game/Include/VisualNovelEngine/Classes)
include_directories(Game/Include/VisualNovelEngine/Classes/Data)
include_directories(Game/Include/VisualNovelEngine/Classes/UI)
include_directories(Game/Include/VisualNovelEngine/Screens)

include_directories(Libs/SFML/include)
include_directories(Libs/json)
include_directories(Libs/SQLite)

link_directories(Libs/SQLite)

add_executable(${EXECUTABLE_NAME}
        Game/Include/BackgroundRenderer/BackgroundImageRenderer.hpp
        Game/Include/BackgroundRenderer/BackgroundTransition.hpp
        Game/Include/BackgroundRenderer/BackgroundTransitionRenderer.hpp
        Game/Include/Base/Engine.hpp
        Game/Include/Base/Game.hpp
        Game/Include/Base/GameManager.hpp
        Game/Include/Base/GameScreen.hpp
        Game/Include/Base/Renderers.hpp
        Game/Include/Database/DatabaseConnection.hpp
        Game/Include/Database/DatabaseSchema.hpp
        Game/Include/Input/GamepadHandler.hpp
        Game/Include/Input/InputManager.hpp
        Game/Include/Input/KeyboardHandler.hpp
        Game/Include/Input/Keymap.hpp
        Game/Include/Input/MouseHandler.hpp
        Game/Include/Misc/ColourBuilder.hpp
        Game/Include/Misc/ParameterHandler.hpp
        Game/Include/Misc/ProjectInfo.hpp
        Game/Include/Misc/Utils.hpp
        Game/Include/Resource/FontManager.hpp
        Game/Include/Resource/MusicManager.hpp
        Game/Include/Resource/ResourceManager.hpp
        Game/Include/Resource/TextureManager.hpp
        Game/Include/SpriteRenderer/Sprite.hpp
        Game/Include/SpriteRenderer/SpriteRenderer.hpp
        Game/Include/TextRenderer/TextRenderer.hpp
        Game/Include/TileRenderer/Tile.hpp
        Game/Include/TileRenderer/Tilemap.hpp
        Game/Include/TileRenderer/TileRenderer.hpp
        Game/Include/VisualNovelEngine/Classes/Data/Character.hpp
        Game/Include/VisualNovelEngine/Classes/Data/CharacterSprite.hpp
        Game/Include/VisualNovelEngine/Classes/Data/Novel.hpp
        Game/Include/VisualNovelEngine/Classes/UI/CharacterSpriteRenderer.hpp
        Game/Include/VisualNovelEngine/Classes/UI/CharacterSpriteSlot.hpp
        Game/Include/VisualNovelEngine/Classes/UI/NovelTextDisplay.hpp
        Game/Include/VisualNovelEngine/Screens/NovelScreen.hpp
        Game/Include/VisualNovelEngine/NovelScreenClasses.hpp
        Game/Src/BackgroundRenderer/BackgroundImageRenderer.cpp
        Game/Src/BackgroundRenderer/BackgroundTransition.cpp
        Game/Src/BackgroundRenderer/BackgroundTransitionRenderer.cpp
        Game/Src/Base/Engine.cpp
        Game/Src/Base/Game.cpp
        Game/Src/Base/GameManager.cpp
        Game/Src/Base/GameScreen.cpp
        Game/Src/Database/DatabaseConnection.cpp
        Game/Src/Database/DatabaseSchema.cpp
        Game/Src/Input/GamepadHandler.cpp
        Game/Src/Input/InputManager.cpp
        Game/Src/Input/KeyboardHandler.cpp
        Game/Src/Input/Keymap.cpp
        Game/Src/Input/MouseHandler.cpp
        Game/Src/Misc/ColourBuilder.cpp
        Game/Src/Misc/ParameterHandler.cpp
        Game/Src/Misc/Utils.cpp
        Game/Src/Resource/FontManager.cpp
        Game/Src/Resource/MusicManager.cpp
        Game/Src/Resource/ResourceManager.cpp
        Game/Src/Resource/TextureManager.cpp
        Game/Src/SpriteRenderer/Sprite.cpp
        Game/Src/SpriteRenderer/SpriteRenderer.cpp
        Game/Src/TextRenderer/TextRenderer.cpp
        Game/Src/TileRenderer/Tile.cpp
        Game/Src/TileRenderer/Tilemap.cpp
        Game/Src/TileRenderer/TileRenderer.cpp
        Game/Src/VisualNovelEngine/Classes/Data/Character.cpp
        Game/Src/VisualNovelEngine/Classes/Data/CharacterSprite.cpp
        Game/Src/VisualNovelEngine/Classes/Data/Novel.cpp
        Game/Src/VisualNovelEngine/Classes/UI/CharacterSpriteRenderer.cpp
        Game/Src/VisualNovelEngine/Classes/UI/CharacterSpriteSlot.cpp
        Game/Src/VisualNovelEngine/Classes/UI/NovelTextDisplay.cpp
        Game/Src/VisualNovelEngine/Screens/NovelScreen.cpp
        Game/Src/EntryPoint.cpp)

add_executable(${COMPILER_EXECUTABLE_NAME}
        Game/Include/Misc/ProjectInfo.hpp
        Game/Include/Misc/Utils.hpp
        Game/Include/Database/DatabaseConnection.hpp
        Game/Include/Database/DatabaseSchema.hpp
        Game/Include/GameCompiler/ChapterBuilder.hpp
        Game/Include/GameCompiler/GameCompiler.hpp
        Game/Include/GameCompiler/GameCompilerChapterParser.hpp
        Game/Include/GameCompiler/ProjectBuilder.hpp
        Game/Include/GameCompiler/ResourceBuilder.hpp
        Game/Src/Database/DatabaseConnection.cpp
        Game/Src/Database/DatabaseSchema.cpp
        Game/Src/GameCompiler/ChapterBuilder.cpp
        Game/Src/GameCompiler/GameCompiler.cpp
        Game/Src/GameCompiler/ProjectBuilder.cpp
        Game/Src/GameCompiler/ResourceBuilder.cpp
        Game/Src/Misc/Utils.cpp
        Game/Src/GameCompilerEntryPoint.cpp)

find_package(SFML 2.5.1 REQUIRED audio graphics window system )

if (SFML_FOUND)
    set(SFML_LIBRARIES sfml-audio sfml-graphics sfml-window sfml-system sqlite3)
    target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})
    target_link_libraries(${COMPILER_EXECUTABLE_NAME} sqlite3)
else()
    message(SFML NOT FOUND)
endif()

