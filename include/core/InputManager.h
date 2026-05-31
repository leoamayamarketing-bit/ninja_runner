#ifndef CORE_INPUTMANAGER_H
#define CORE_INPUTMANAGER_H

#include <SFML/Window/Keyboard.hpp>
#include <unordered_map>
#include "KeyBindings.h"

class InputManager {
public:
    InputManager();
    void update();
    bool isKeyPressed(sf::Keyboard::Key key) const;
    bool isKeyJustPressed(sf::Keyboard::Key key) const;
    bool isKeyJustReleased(sf::Keyboard::Key key) const;
    void onKeyPressed(sf::Keyboard::Key key);
    void onKeyReleased(sf::Keyboard::Key key);
    void setBindings(const KeyBindings* bindings) { bindings_ = bindings; }

    bool isJumpPressed() const;
    bool isSlidePressed() const;
    bool isDashPressed() const;
    bool isPausePressed() const;

    bool isMenuUpPressed() const;
    bool isMenuDownPressed() const;
    bool isMenuSelectPressed() const;
    bool isMenuBackPressed() const;

private:
    bool matchesAny(const sf::Keyboard::Key* a,
                    const sf::Keyboard::Key* b,
                    bool justPressed) const;
    std::unordered_map<sf::Keyboard::Key, bool> currentKeys_;
    std::unordered_map<sf::Keyboard::Key, bool> previousKeys_;
    const KeyBindings* bindings_ = nullptr;
};

#endif
