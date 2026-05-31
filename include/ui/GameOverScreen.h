#ifndef UI_GAMEOVERSCREEN_H
#define UI_GAMEOVERSCREEN_H

#include <SFML/Graphics/RenderWindow.hpp>
#include "../core/GameState.h"
#include "../core/InputManager.h"
#include "../core/FontRenderer.h"

class GameOverScreen {
public:
    enum Action { NONE, RESTART, MENU };

    void init(FontRenderer& fontRenderer, class AudioManager* audio = nullptr);
    Action update(InputManager& input);
    void render(sf::RenderWindow& window, const GameState& state);

private:
    FontRenderer* fontRenderer_ = nullptr;
    class AudioManager* audio_ = nullptr;
    int selectedIndex_ = 0;
    float timer_ = 0.0f;
};

#endif
