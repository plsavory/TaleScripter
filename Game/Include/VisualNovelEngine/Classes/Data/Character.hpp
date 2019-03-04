#ifndef NOVEL_DATA_CHARACTER_INCLUDED
#define NOVEL_DATA_CHARACTER_INCLUDED

namespace NovelData {
  class Character {
  public:
    Character(int cId, std::string cFirstName, std::string cSurname, std::string cBio, std::string cAge, bool showOnCharacterMenu);
    Character(DataSetRow *data);
    ~Character();

    // Getters
    int getId() {
      return id;
    }
    std::string getFirstame() {
      return firstName;
    }
    std::string getsurname() {
      return surname;
    }
    std::string getBio() {
      return bio;
    }
    std::string getAge() {
      return age;
    }
    bool getShowOnCharacterMenu() {
      return showOnCharacterMenu;
    }
  private:
    int id;
    std::string firstName;
    std::string surname;
    std::string bio;
    std::string age;
    bool showOnCharacterMenu;
  };
};

#endif
