#include "../../include/ui/Menu.h"
#include "../../include/core/AudioManager.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cstdio>
#include <algorithm>
#include <cmath>

void Menu::init(FontRenderer& fontRenderer, AudioManager* audio) {
    fontRenderer_ = &fontRenderer;
    audio_ = audio;
}

Menu::MenuAction Menu::update(InputManager& input) {
    timer_ += 0.016f;

    int prevIndex = selectedIndex_;

    if (input.isMenuDownPressed()) {
        selectedIndex_ = (selectedIndex_ + 1) % menuItems_.size();
    }
    if (input.isMenuUpPressed()) {
        selectedIndex_ = (selectedIndex_ - 1 + menuItems_.size()) % menuItems_.size();
    }

    if (selectedIndex_ != prevIndex && audio_) {
        audio_->playNavigate();
    }

    if (input.isMenuSelectPressed()) {
        if (audio_) audio_->playSelect();
        if (selectedIndex_ == 0) return START_GAME;
        else if (selectedIndex_ == 1) return NINJA_SELECT;
        else if (selectedIndex_ == 2) return QUIT;
    }

    return NONE;
}

void Menu::render(sf::RenderWindow& window) {
    // Night sky gradient (flat design)
    sf::RectangleShape bg(sf::Vector2f(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT));
    bg.setFillColor(sf::Color(15, 12, 32));
    window.draw(bg);

    // Subtle gradient bands
    sf::RectangleShape stripe(sf::Vector2f(Constants::WINDOW_WIDTH, 50));
    for (int i = 0; i < Constants::WINDOW_HEIGHT; i += 50) {
        float t = static_cast<float>(i) / Constants::WINDOW_HEIGHT;
        sf::Uint8 r = static_cast<sf::Uint8>(15 + 12 * t);
        sf::Uint8 g = static_cast<sf::Uint8>(12 + 15 * t);
        sf::Uint8 b = static_cast<sf::Uint8>(32 + 10 * t);
        stripe.setFillColor(sf::Color(r, g, b, 80));
        stripe.setPosition(0, static_cast<float>(i));
        window.draw(stripe);
    }

    // Stars
    sf::CircleShape star(1.5f);
    for (int i = 0; i < 30; i++) {
        float sx = (i * 137 + 50) % Constants::WINDOW_WIDTH;
        float sy = (i * 89 + 20) % 180;
        int bright = 150 + (i * 20) % 105;
        star.setFillColor(sf::Color(bright, bright, bright + 30));
        star.setPosition(sx, sy);
        window.draw(star);
    }

    // Moon
    sf::CircleShape moon(28);
    moon.setFillColor(sf::Color(225, 220, 205));
    moon.setPosition(1050, 40);
    window.draw(moon);
    sf::CircleShape moonShadow(24);
    moonShadow.setFillColor(sf::Color(15, 12, 32));
    moonShadow.setPosition(1060, 32);
    window.draw(moonShadow);

    // City skyline silhouettes (flat design buildings)
    struct MenuBuilding { float x, w, h; sf::Color c; };
    MenuBuilding mb[] = {
        {50, 180, 380, sf::Color(25, 22, 45)},
        {280, 160, 420, sf::Color(30, 26, 50)},
        {480, 200, 460, sf::Color(22, 20, 42)},
        {720, 140, 350, sf::Color(28, 24, 48)},
        {900, 170, 400, sf::Color(32, 28, 52)},
        {1100, 150, 430, sf::Color(24, 22, 44)},
    };

    for (auto& b : mb) {
        float roofY = Constants::WINDOW_HEIGHT - b.h;
        // Main building body
        sf::RectangleShape body(sf::Vector2f(b.w, b.h));
        body.setFillColor(b.c);
        body.setOutlineColor(sf::Color(40, 35, 60));
        body.setOutlineThickness(1.5f);
        body.setPosition(b.x, roofY);
        window.draw(body);

        // Windows (warm dots for flat design)
        for (float wx = b.x + 15; wx < b.x + b.w - 15; wx += 22) {
            for (float wy = roofY + 15; wy < roofY + b.h - 10; wy += 22) {
                int chance = (static_cast<int>(wx) * 7 + static_cast<int>(wy) * 13) % 4;
                if (chance > 0) {
                    sf::Color winColor(
                        static_cast<sf::Uint8>(255),
                        static_cast<sf::Uint8>(190 + (chance * 15)),
                        static_cast<sf::Uint8>(80 + (chance * 20)),
                        static_cast<sf::Uint8>(120 + (chance * 30)));
                    sf::RectangleShape win(sf::Vector2f(6, 8));
                    win.setFillColor(winColor);
                    win.setPosition(wx, wy);
                    window.draw(win);
                }
            }
        }

        // Parapet / rooftop accent
        sf::RectangleShape parapet(sf::Vector2f(b.w, 6));
        parapet.setFillColor(sf::Color(35, 30, 55));
        parapet.setPosition(b.x, roofY - 6);
        window.draw(parapet);
    }

    // Ninja silhouette (flat design — geometric shapes)
    float ninjaX = 150.0f;
    float ninjaY = Constants::WINDOW_HEIGHT - 70.0f;
    sf::Color ninjaColor(45, 42, 60);

    // Body
    sf::RectangleShape ninjaBody(sf::Vector2f(18, 28));
    ninjaBody.setFillColor(ninjaColor);
    ninjaBody.setPosition(ninjaX, ninjaY - 50);
    window.draw(ninjaBody);

    // Head
    sf::CircleShape head(10);
    head.setFillColor(sf::Color(55, 52, 70));
    head.setPosition(ninjaX - 1, ninjaY - 72);
    window.draw(head);

    // Headband
    sf::RectangleShape band(sf::Vector2f(20, 3));
    band.setFillColor(sf::Color(120, 25, 25));
    band.setPosition(ninjaX - 1, ninjaY - 67);
    window.draw(band);

    // Scarf trail
    sf::RectangleShape scarf(sf::Vector2f(14, 3));
    scarf.setFillColor(sf::Color(100, 20, 20, 180));
    scarf.setPosition(ninjaX - 16, ninjaY - 58);
    window.draw(scarf);

    // Legs
    sf::RectangleShape legL(sf::Vector2f(7, 16));
    legL.setFillColor(ninjaColor);
    legL.setPosition(ninjaX, ninjaY - 22);
    window.draw(legL);
    sf::RectangleShape legR(sf::Vector2f(7, 16));
    legR.setFillColor(ninjaColor);
    legR.setPosition(ninjaX + 11, ninjaY - 22);
    window.draw(legR);

    // Sword on back
    sf::RectangleShape sword(sf::Vector2f(2, 22));
    sword.setFillColor(sf::Color(140, 140, 160));
    sword.setPosition(ninjaX + 16, ninjaY - 52);
    window.draw(sword);
    sf::CircleShape guard(2);
    guard.setFillColor(sf::Color(160, 120, 60));
    guard.setPosition(ninjaX + 15, ninjaY - 32);
    window.draw(guard);

    // Pulsing glow behind the ninja
    float pulse = 0.5f + 0.5f * std::sin(timer_ * 2.0f);
    sf::CircleShape glow(30);
    glow.setFillColor(sf::Color(180, 30, 30, static_cast<sf::Uint8>(20 * pulse)));
    glow.setPosition(ninjaX - 21, ninjaY - 75);
    window.draw(glow);
    sf::CircleShape glow2(22);
    glow2.setFillColor(sf::Color(200, 50, 50, static_cast<sf::Uint8>(15 * pulse)));
    glow2.setPosition(ninjaX - 13, ninjaY - 67);
    window.draw(glow2);

    // Title area with flat design feel
    if (fontRenderer_) {
        // Title glow
        fontRenderer_->drawTextCentered(window, "NINJA RUNNER", 643, 103, 56, sf::Color(180, 30, 30, 60));

        // Title
        fontRenderer_->drawTextCentered(window, "NINJA RUNNER", 640, 100, 56, sf::Color(235, 230, 220));
        fontRenderer_->drawTextCentered(window, "Shadow Dash", 640, 160, 28, sf::Color(200, 45, 45));

        // Decorative line below title
        sf::RectangleShape titleLine(sf::Vector2f(100, 2));
        titleLine.setFillColor(sf::Color(200, 45, 45, 150));
        titleLine.setPosition(590, 185);
        window.draw(titleLine);

        // High score
        char hsBuf[64];
        std::snprintf(hsBuf, sizeof(hsBuf), "Best: %.0f", highScore_);
        fontRenderer_->drawTextCentered(window, hsBuf, 640, 215, 18, sf::Color(160, 155, 170));

        // Menu items with flat design styling
        for (size_t i = 0; i < menuItems_.size(); i++) {
            int baseY = 330 + static_cast<int>(i) * 80;
            bool selected = (i == static_cast<size_t>(selectedIndex_));

            if (selected) {
                // Selection background pill
                sf::RectangleShape selBg(sf::Vector2f(220, 40));
                selBg.setFillColor(sf::Color(200, 45, 45, 40));
                selBg.setOutlineColor(sf::Color(200, 45, 45, 120));
                selBg.setOutlineThickness(1.5f);
                selBg.setPosition(530, baseY - 5);
                window.draw(selBg);
            }

            sf::Color textColor = selected ? sf::Color(255, 220, 80) : sf::Color(180, 175, 190);
            int charSize = selected ? 34 : 26;
            fontRenderer_->drawTextCentered(window, menuItems_[i], 640, baseY, charSize, textColor);

            if (selected) {
                // Selection arrow
                fontRenderer_->drawText(window, ">", 520, baseY, charSize, sf::Color(200, 45, 45));
            }
        }

        // Bottom instructions
        fontRenderer_->drawTextCentered(window, "Use ARROW KEYS to navigate", 640, 610, 14, sf::Color(120, 115, 135));
        fontRenderer_->drawTextCentered(window, "Press SPACE to select", 640, 630, 14, sf::Color(120, 115, 135));
    }
}
