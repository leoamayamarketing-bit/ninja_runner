#include "../../include/core/InputManager.h"

InputManager::InputManager() = default;

void InputManager::update() {
    previousKeys_ = currentKeys_;
}

bool InputManager::isKeyPressed(sf::Keyboard::Key key) const {
    auto it = currentKeys_.find(key);
    return it != currentKeys_.end() && it->second;
}

bool InputManager::isKeyJustPressed(sf::Keyboard::Key key) const {
    auto cur = currentKeys_.find(key);
    auto prev = previousKeys_.find(key);
    bool curPressed = (cur != currentKeys_.end() && cur->second);
    bool prevPressed = (prev != previousKeys_.end() && prev->second);
    return curPressed && !prevPressed;
}

bool InputManager::isKeyJustReleased(sf::Keyboard::Key key) const {
    auto cur = currentKeys_.find(key);
    auto prev = previousKeys_.find(key);
    bool curPressed = (cur != currentKeys_.end() && cur->second);
    bool prevPressed = (prev != previousKeys_.end() && prev->second);
    return !curPressed && prevPressed;
}

void InputManager::onKeyPressed(sf::Keyboard::Key key) {
    currentKeys_[key] = true;
}

void InputManager::onKeyReleased(sf::Keyboard::Key key) {
    currentKeys_[key] = false;
}

bool InputManager::matchesAny(const sf::Keyboard::Key* a, const sf::Keyboard::Key* b,
                               bool justPressed) const {
    bool result = false;
    if (a) {
        result = justPressed ? isKeyJustPressed(*a) : isKeyPressed(*a);
    }
    if (b && !result) {
        result = justPressed ? isKeyJustPressed(*b) : isKeyPressed(*b);
    }
    return result;
}

bool InputManager::isJumpPressed() const {
    return matchesAny(&bindings_->jump, &bindings_->jumpAlt, false) ||
           isKeyJustPressed(bindings_->jump) ||
           isKeyJustPressed(bindings_->jumpAlt);
}

bool InputManager::isSlidePressed() const {
    return isKeyPressed(bindings_->slide) || isKeyPressed(bindings_->slideAlt);
}

bool InputManager::isDashPressed() const {
    return isKeyJustPressed(bindings_->dash) || isKeyJustPressed(bindings_->dashAlt);
}

bool InputManager::isPausePressed() const {
    return isKeyJustPressed(bindings_->pause) || isKeyJustPressed(bindings_->pauseAlt);
}

bool InputManager::isMenuUpPressed() const {
    return isKeyJustPressed(bindings_->menuUp) || isKeyJustPressed(bindings_->menuUpAlt);
}

bool InputManager::isMenuDownPressed() const {
    return isKeyJustPressed(bindings_->menuDown) || isKeyJustPressed(bindings_->menuDownAlt);
}

bool InputManager::isMenuSelectPressed() const {
    return isKeyJustPressed(bindings_->menuSelect) || isKeyJustPressed(bindings_->menuSelectAlt);
}

bool InputManager::isMenuBackPressed() const {
    return isKeyJustPressed(bindings_->menuBack);
}
