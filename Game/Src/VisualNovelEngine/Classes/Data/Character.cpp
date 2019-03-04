#include <iostream>
#include "Database/DatabaseConnection.hpp"
#include "VisualNovelEngine/Classes/Data/Character.hpp"

using namespace NovelData;

Character::Character(int cId, std::string cFirstName, std::string cSurname, std::string cBio, std::string cAge, bool cShowOnCharacterMenu) {
  id = cId;
  firstName = cFirstName;
  surname = cSurname;
  bio = cBio;
  age = cAge;
  showOnCharacterMenu = cShowOnCharacterMenu;
}

Character::Character(DataSetRow *data) {
  throw "This is currently unimplemented.";
}

Character::~Character() {

}
