#ifndef UI_MENU_H
#define UI_MENU_H

#include <SFML/Graphics/RenderWindow.hpp>
#include <vector>
#include <string>
#include "../core/GameState.h"
#include "../core/InputManager.h"
#include "../core/FontRenderer.h"

class Menu {
public:
    enum MenuAction { NONE, START_GAME, NINJA_SELECT, QUIT };

    void init(FontRenderer& fontRenderer, class AudioManager* audio = nullptr);
    MenuAction update(InputManager& input);
    void render(sf::RenderWindow& window);
    void resetSelection() { selectedIndex_ = 0; }
    void setHighScore(float hs) { highScore_ = hs; }

private:
    std::vector<std::string> menuItems_ = {"Press START", "Select Ninja", "Quit"};
    int selectedIndex_ = 0;
    FontRenderer* fontRenderer_ = nullptr;
    class AudioManager* audio_ = nullptr;
    float highScore_ = 0.0f;
    float timer_ = 0.0f;
};

#endif
