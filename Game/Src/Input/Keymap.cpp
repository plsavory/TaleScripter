#include <iostream>
#include <SFML/Window.hpp>
#include "Input/Keymap.hpp"

/**
 * [Keymap::getKeycode Returns the SF Keycode of the key with the given name]
 * @param  name [name (as a string) of the key]
 * @return      [SFML Keycode]
 */
sf::Keyboard::Key Keymap::getKeycode(std::string name) {

  Keycode *keycodes[0xFF];

  for (int i = 0;i<0xFF;i++) {
    if (keycodes[i]) {
      keycodes[i] = NULL;
    }
  }

  // Gamepad-like keys
  keycodes[sf::Keyboard::Left] = new Keycode("left", sf::Keyboard::Left);
  keycodes[sf::Keyboard::Right] = new Keycode("right", sf::Keyboard::Right);
  keycodes[sf::Keyboard::Up] = new Keycode("up", sf::Keyboard::Up);
  keycodes[sf::Keyboard::Down] = new Keycode("down", sf::Keyboard::Down);
  keycodes[sf::Keyboard::Space] = new Keycode("space", sf::Keyboard::Space);
  keycodes[sf::Keyboard::Return] = new Keycode("return", sf::Keyboard::Return);
  keycodes[sf::Keyboard::Escape] = new Keycode("escape", sf::Keyboard::Escape);

  // Function keys
  keycodes[sf::Keyboard::F1] = new Keycode("F1", sf::Keyboard::F1);
  keycodes[sf::Keyboard::F2] = new Keycode("F2", sf::Keyboard::F2);
  keycodes[sf::Keyboard::F3] = new Keycode("F3", sf::Keyboard::F3);
  keycodes[sf::Keyboard::F4] = new Keycode("F4", sf::Keyboard::F4);
  keycodes[sf::Keyboard::F5] = new Keycode("F5", sf::Keyboard::F5);
  keycodes[sf::Keyboard::F6] = new Keycode("F6", sf::Keyboard::F6);
  keycodes[sf::Keyboard::F7] = new Keycode("F7", sf::Keyboard::F7);
  keycodes[sf::Keyboard::F8] = new Keycode("F8", sf::Keyboard::F8);
  keycodes[sf::Keyboard::F9] = new Keycode("F9", sf::Keyboard::F9);
  keycodes[sf::Keyboard::F10] = new Keycode("F10", sf::Keyboard::F10);
  keycodes[sf::Keyboard::F11] = new Keycode("F11", sf::Keyboard::F11);
  keycodes[sf::Keyboard::F12] = new Keycode("F12", sf::Keyboard::F12);
  keycodes[sf::Keyboard::F13] = new Keycode("F13", sf::Keyboard::F13);
  keycodes[sf::Keyboard::F14] = new Keycode("F14", sf::Keyboard::F14);
  keycodes[sf::Keyboard::F15] = new Keycode("F15", sf::Keyboard::F15);

  // Numpad
  keycodes[sf::Keyboard::Numpad0] = new Keycode("numpad0", sf::Keyboard::Numpad0);
  keycodes[sf::Keyboard::Numpad1] = new Keycode("numpad1", sf::Keyboard::Numpad1);
  keycodes[sf::Keyboard::Numpad2] = new Keycode("numpad2", sf::Keyboard::Numpad2);
  keycodes[sf::Keyboard::Numpad3] = new Keycode("numpad3", sf::Keyboard::Numpad3);
  keycodes[sf::Keyboard::Numpad4] = new Keycode("numpad4", sf::Keyboard::Numpad4);
  keycodes[sf::Keyboard::Numpad5] = new Keycode("numpad5", sf::Keyboard::Numpad5);
  keycodes[sf::Keyboard::Numpad6] = new Keycode("numpad6", sf::Keyboard::Numpad6);
  keycodes[sf::Keyboard::Numpad7] = new Keycode("numpad7", sf::Keyboard::Numpad7);
  keycodes[sf::Keyboard::Numpad8] = new Keycode("numpad8", sf::Keyboard::Numpad8);
  keycodes[sf::Keyboard::Numpad9] = new Keycode("numpad9", sf::Keyboard::Numpad9);

  // Numbers
  keycodes[sf::Keyboard::Num0] = new Keycode("num0", sf::Keyboard::Num0);
  keycodes[sf::Keyboard::Num1] = new Keycode("num1", sf::Keyboard::Num1);
  keycodes[sf::Keyboard::Num2] = new Keycode("num2", sf::Keyboard::Num2);
  keycodes[sf::Keyboard::Num3] = new Keycode("num3", sf::Keyboard::Num3);
  keycodes[sf::Keyboard::Num4] = new Keycode("num4", sf::Keyboard::Num4);
  keycodes[sf::Keyboard::Num5] = new Keycode("num5", sf::Keyboard::Num5);
  keycodes[sf::Keyboard::Num6] = new Keycode("num6", sf::Keyboard::Num6);
  keycodes[sf::Keyboard::Num7] = new Keycode("num7", sf::Keyboard::Num7);
  keycodes[sf::Keyboard::Num8] = new Keycode("num8", sf::Keyboard::Num8);
  keycodes[sf::Keyboard::Num9] = new Keycode("num9", sf::Keyboard::Num9);

  // Letters
  keycodes[sf::Keyboard::A] = new Keycode("A", sf::Keyboard::A);
  keycodes[sf::Keyboard::B] = new Keycode("B", sf::Keyboard::B);
  keycodes[sf::Keyboard::C] = new Keycode("C", sf::Keyboard::C);
  keycodes[sf::Keyboard::D] = new Keycode("D", sf::Keyboard::D);
  keycodes[sf::Keyboard::E] = new Keycode("E", sf::Keyboard::E);
  keycodes[sf::Keyboard::F] = new Keycode("F", sf::Keyboard::F);
  keycodes[sf::Keyboard::G] = new Keycode("G", sf::Keyboard::G);
  keycodes[sf::Keyboard::H] = new Keycode("H", sf::Keyboard::H);
  keycodes[sf::Keyboard::I] = new Keycode("I", sf::Keyboard::I);
  keycodes[sf::Keyboard::J] = new Keycode("J", sf::Keyboard::J);
  keycodes[sf::Keyboard::K] = new Keycode("K", sf::Keyboard::K);
  keycodes[sf::Keyboard::L] = new Keycode("L", sf::Keyboard::L);
  keycodes[sf::Keyboard::M] = new Keycode("M", sf::Keyboard::M);
  keycodes[sf::Keyboard::N] = new Keycode("N", sf::Keyboard::N);
  keycodes[sf::Keyboard::O] = new Keycode("O", sf::Keyboard::O);
  keycodes[sf::Keyboard::P] = new Keycode("P", sf::Keyboard::P);
  keycodes[sf::Keyboard::Q] = new Keycode("Q", sf::Keyboard::Q);
  keycodes[sf::Keyboard::R] = new Keycode("R", sf::Keyboard::R);
  keycodes[sf::Keyboard::S] = new Keycode("S", sf::Keyboard::S);
  keycodes[sf::Keyboard::T] = new Keycode("T", sf::Keyboard::T);
  keycodes[sf::Keyboard::U] = new Keycode("U", sf::Keyboard::U);
  keycodes[sf::Keyboard::V] = new Keycode("V", sf::Keyboard::V);
  keycodes[sf::Keyboard::W] = new Keycode("W", sf::Keyboard::W);
  keycodes[sf::Keyboard::X] = new Keycode("X", sf::Keyboard::X);
  keycodes[sf::Keyboard::Y] = new Keycode("Y", sf::Keyboard::Y);
  keycodes[sf::Keyboard::Z] = new Keycode("Z", sf::Keyboard::Z);

  sf::Keyboard::Key returnValue = sf::Keyboard::Key::Unknown;

  for (int i = 0; i<0xFF;i++) {
    if (keycodes[i]) {
      if (keycodes[i]->name == name) {
        returnValue = (*keycodes[i]).code;
        break;
      }
    }
  }

  for (int i = 0;i<0xFF;i++) {
    if (keycodes[i]) {
      delete keycodes[i];
    }
  }

  return returnValue;

}
