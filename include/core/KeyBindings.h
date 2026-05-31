#ifndef CORE_KEYBINDINGS_H
#define CORE_KEYBINDINGS_H

#include <SFML/Window/Keyboard.hpp>
#include <string>
#include <unordered_map>

struct KeyBindings {
    sf::Keyboard::Key jump = sf::Keyboard::Space;
    sf::Keyboard::Key jumpAlt = sf::Keyboard::Up;
    sf::Keyboard::Key slide = sf::Keyboard::Down;
    sf::Keyboard::Key slideAlt = sf::Keyboard::S;
    sf::Keyboard::Key dash = sf::Keyboard::LShift;
    sf::Keyboard::Key dashAlt = sf::Keyboard::RShift;
    sf::Keyboard::Key pause = sf::Keyboard::Escape;
    sf::Keyboard::Key pauseAlt = sf::Keyboard::P;
    sf::Keyboard::Key menuUp = sf::Keyboard::Up;
    sf::Keyboard::Key menuUpAlt = sf::Keyboard::W;
    sf::Keyboard::Key menuDown = sf::Keyboard::Down;
    sf::Keyboard::Key menuDownAlt = sf::Keyboard::S;
    sf::Keyboard::Key menuSelect = sf::Keyboard::Space;
    sf::Keyboard::Key menuSelectAlt = sf::Keyboard::Enter;
    sf::Keyboard::Key menuBack = sf::Keyboard::Escape;
};

#endif
