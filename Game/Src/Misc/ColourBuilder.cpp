#include <iostream>
#include <SFML/Graphics.hpp>
#include "Misc/ColourBuilder.hpp"

sf::Color* ColourBuilder::get(int id) {
  switch (id) {
    case 0:
    return new sf::Color(0,0,0,255);
    case 1:
    return new sf::Color(255,255,255,255);
    default:
    return new sf::Color(0,0,0,255);
  }
}
