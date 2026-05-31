#ifndef UI_HUD_H
#define UI_HUD_H

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include "../core/GameState.h"
#include "../core/AssetManager.h"
#include "../core/FontRenderer.h"

class HUD {
public:
    void init(FontRenderer& fontRenderer);
    void update(const GameState& state);
    void render(sf::RenderWindow& window);

private:
    FontRenderer* fontRenderer_ = nullptr;
    int lastHealth_ = 3;
    char scoreBuf_[64] = {};
    char coinBuf_[64] = {};
    char distBuf_[64] = {};
    char speedBuf_[64] = {};
    char streakBuf_[64] = {};
};

#endif
