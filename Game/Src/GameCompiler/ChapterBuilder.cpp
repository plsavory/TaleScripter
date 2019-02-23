/*
  This class handles the parsing of a chapter .json file, and will create the appropriate records in the SQLite database for it
  This class will also handle sanity checks for errors which could make it into the game's database.
 */

#include "nlohmann/json.hpp"
#include "Database/DatabaseConnection.hpp"

#include "GameCompiler/ChapterBuilder.hpp"

ChapterBuilder::ChapterBuilder(std::string fileName) {

}

ChapterBuilder::~ChapterBuilder() {

}
