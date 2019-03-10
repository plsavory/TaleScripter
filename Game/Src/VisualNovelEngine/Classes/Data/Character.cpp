#include <iostream>
#include "Database/DatabaseConnection.hpp"
#include "Misc/Utils.hpp"
#include "Base/Engine.hpp"
#include "VisualNovelEngine/Classes/Data/Character.hpp"

Character::Character(int cId, std::string cFirstName, std::string cSurname, std::string cBio, std::string cAge, bool cShowOnCharacterMenu, DatabaseConnection *novelDb) {
  id = cId;
  firstName = cFirstName;
  surname = cSurname;
  bio = cBio;
  age = cAge;
  showOnCharacterMenu = cShowOnCharacterMenu;

  for (int i = 0; i < MAX_CHARACTER_SPRITES; i++) {
    characterSprite[i] = NULL;
  }

  // Load any character sprites related to this sprite from the database
  DataSet *dataSet = new DataSet();

  int spriteCount = dataSet->getRowCount();

  for (int i = 0; i < spriteCount; i++) {
    std::string name = dataSet->getRow(i)->getColumn("name")->getData();
    int textureId = std::stoi(dataSet->getRow(i)->getColumn("texture_id")->getData());
    int id = std::stoi(dataSet->getRow(i)->getColumn("id")->getData());
    characterSprite[i] = new CharacterSprite(name, id, textureId);
  }

}

Character::~Character() {

}

Character::Character(DataSetRow *data) {
  throw "This is currently unimplemented.";
}
