// Keymap class - Gives every SFML keycode a readable name so that it is easy to set these in a config file
struct Keycode {
public:
  Keycode(std::string kName, sf::Keyboard::Key kCode) {
    name = kName;
    code = kCode;
  };
  std::string name;
  sf::Keyboard::Key code;
};

class Keymap {
public:
  static sf::Keyboard::Key getKeycode(std::string name);
};
