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
    characterSprite[i] = nullptr;
  }

  // Load any character sprites related to this sprite from the database
  DataSet *dataSet = new DataSet();

  std::vector<std::string> spriteQuery = {
    "SELECT * FROM character_sprites WHERE character_id = ",
    std::to_string(id),
    ";"
  };

  novelDb->executeQuery(Utils::implodeString(spriteQuery, ""), dataSet);

  int spriteCount = dataSet->getRowCount();

  for (int i = 0; i < spriteCount; i++) {
    std::string name = dataSet->getRow(i)->getColumn("name")->getRawData();
    int textureId = std::stoi(dataSet->getRow(i)->getColumn("texture_id")->getRawData());
    int id = std::stoi(dataSet->getRow(i)->getColumn("id")->getRawData());
    characterSprite[i] = new CharacterSprite(name, id, textureId);
  }

  delete(dataSet);

}

Character::~Character() {

}

Character::Character(DataSetRow *data) {
  throw "This is currently unimplemented.";
}

CharacterSprite* Character::getSprite(int id) {
  return characterSprite[id];
}

CharacterSprite* Character::getSprite(std::string name) {

  for (int i = 0; i < MAX_CHARACTER_SPRITES; i++) {

    if (!characterSprite[i]) {
      continue;
    }

    if (characterSprite[i]->getName() == name) {
      return characterSprite[i];
    }
  }

  return nullptr;
}
