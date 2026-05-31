#include "../../include/ui/HUD.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <cmath>

static const sf::Color PANEL_BG(15, 12, 32, 160);
static const sf::Color PANEL_OUTLINE(44, 62, 80, 120);
static const sf::Color TEXT_WHITE(235, 235, 240);
static const sf::Color TEXT_GOLD(255, 215, 80);
static const sf::Color TEXT_DIM(160, 160, 175);

void HUD::init(FontRenderer& fontRenderer) {
    fontRenderer_ = &fontRenderer;

    std::memset(scoreBuf_, 0, sizeof(scoreBuf_));
    std::memset(coinBuf_, 0, sizeof(coinBuf_));
    std::memset(distBuf_, 0, sizeof(distBuf_));
    std::memset(speedBuf_, 0, sizeof(speedBuf_));
    std::memset(streakBuf_, 0, sizeof(streakBuf_));
}

void HUD::update(const GameState& state) {
    std::snprintf(scoreBuf_, sizeof(scoreBuf_), "SCORE: %.0f", state.score);
    std::snprintf(coinBuf_, sizeof(coinBuf_), "%d", state.coins);
    std::snprintf(distBuf_, sizeof(distBuf_), "DIST: %.0f m", state.distance);
    std::snprintf(speedBuf_, sizeof(speedBuf_), "SPD: %.0f", state.currentSpeed);

    if (state.highScore > 0) {
        std::snprintf(streakBuf_, sizeof(streakBuf_), "BEST: %.0f", state.highScore);
    } else {
        streakBuf_[0] = '\0';
    }

    lastHealth_ = state.lives;
}

static void drawRoundedPanel(sf::RenderWindow& window, float x, float y, float w, float h,
    const sf::Color& fill, const sf::Color& outline, float cr = 4.0f) {
    sf::RectangleShape rect(sf::Vector2f(w - cr * 2, h - cr * 2));
    rect.setFillColor(fill);
    rect.setPosition(x + cr, y + cr);
    window.draw(rect);
    for (float cx : {x + cr, x + w - cr})
        for (float cy : {y + cr, y + h - cr}) {
            sf::CircleShape c(cr);
            c.setFillColor(fill);
            c.setPosition(cx - cr, cy - cr);
            window.draw(c);
        }
    sf::RectangleShape outlineRect(sf::Vector2f(w, h));
    outlineRect.setFillColor(sf::Color::Transparent);
    outlineRect.setOutlineColor(outline);
    outlineRect.setOutlineThickness(1.5f);
    outlineRect.setPosition(x, y);
    window.draw(outlineRect);
}

void HUD::render(sf::RenderWindow& window) {
    if (!fontRenderer_) return;

    // === Top-left panel: Score + Distance ===
    drawRoundedPanel(window, 10, 10, 200, 72, PANEL_BG, PANEL_OUTLINE);
    fontRenderer_->drawText(window, scoreBuf_, 20, 14, 22, TEXT_WHITE);
    fontRenderer_->drawText(window, distBuf_, 20, 40, 14, TEXT_DIM);
    if (streakBuf_[0] != '\0') {
        fontRenderer_->drawText(window, streakBuf_, 20, 58, 12, sf::Color(249, 199, 132, 200));
    }

    // === Top-right panel: Coin counter ===
    drawRoundedPanel(window, Constants::WINDOW_WIDTH - 110, 10, 100, 40, PANEL_BG, PANEL_OUTLINE);

    // Coin icon (small shuriken circle)
    sf::CircleShape coinIcon(7);
    coinIcon.setFillColor(TEXT_GOLD);
    coinIcon.setOutlineColor(sf::Color(44, 62, 80));
    coinIcon.setOutlineThickness(1.0f);
    coinIcon.setPosition(Constants::WINDOW_WIDTH - 100, 16);
    window.draw(coinIcon);
    // Star inside coin (tiny)
    sf::ConvexShape star;
    star.setPointCount(4);
    float scx = Constants::WINDOW_WIDTH - 93, scy = 23;
    for (int i = 0; i < 4; i++) {
        float ang = i * 3.14159f / 2.0f - 3.14159f / 4.0f;
        float r = (i % 2 == 0) ? 4.0f : 1.5f;
        star.setPoint(i, sf::Vector2f(scx + r * std::cos(ang), scy + r * std::sin(ang)));
    }
    star.setFillColor(sf::Color::White);
    star.setOutlineColor(sf::Color(44, 62, 80));
    star.setOutlineThickness(0.5f);
    window.draw(star);

    fontRenderer_->drawText(window, coinBuf_, Constants::WINDOW_WIDTH - 82, 16, 18, TEXT_GOLD);

    // === Speed indicator (bottom) ===
    drawRoundedPanel(window, 10, Constants::WINDOW_HEIGHT - 40, 130, 30, PANEL_BG, PANEL_OUTLINE);
    fontRenderer_->drawText(window, speedBuf_, 20, Constants::WINDOW_HEIGHT - 34, 14, TEXT_DIM);

    // === Health bar (bottom-right) ===
    drawRoundedPanel(window, Constants::WINDOW_WIDTH - 170, Constants::WINDOW_HEIGHT - 40, 160, 30, PANEL_BG, PANEL_OUTLINE);

    // Health hearts
    for (int i = 0; i < lastHealth_; i++) {
        float hx = Constants::WINDOW_WIDTH - 158 + i * 28;
        float hy = Constants::WINDOW_HEIGHT - 32;

        // Heart shadow
        sf::CircleShape heartShadow(7);
        heartShadow.setFillColor(sf::Color(0, 0, 0, 40));
        heartShadow.setPosition(hx + 1, hy + 1);
        window.draw(heartShadow);

        // Heart body (two circles + triangle)
        sf::Color heartColor;
        if (lastHealth_ <= 1) heartColor = sf::Color(244, 100, 100);
        else if (lastHealth_ == 2) heartColor = sf::Color(249, 199, 132);
        else heartColor = sf::Color(80, 220, 160);

        sf::CircleShape h1(5);
        h1.setFillColor(heartColor);
        h1.setOutlineColor(sf::Color(44, 62, 80));
        h1.setOutlineThickness(1.0f);
        h1.setPosition(hx, hy);
        window.draw(h1);
        sf::CircleShape h2(5);
        h2.setFillColor(heartColor);
        h2.setOutlineColor(sf::Color(44, 62, 80));
        h2.setOutlineThickness(1.0f);
        h2.setPosition(hx + 7, hy);
        window.draw(h2);

        sf::ConvexShape heartTri;
        heartTri.setPointCount(3);
        heartTri.setPoint(0, sf::Vector2f(hx - 1, hy + 3));
        heartTri.setPoint(1, sf::Vector2f(hx + 13, hy + 3));
        heartTri.setPoint(2, sf::Vector2f(hx + 6, hy + 11));
        heartTri.setFillColor(heartColor);
        heartTri.setOutlineColor(sf::Color(44, 62, 80));
        heartTri.setOutlineThickness(1.0f);
        window.draw(heartTri);
    }

    // Empty hearts for lost lives
    for (int i = lastHealth_; i < 3; i++) {
        float hx = Constants::WINDOW_WIDTH - 158 + i * 28;
        float hy = Constants::WINDOW_HEIGHT - 32;
        sf::CircleShape h1(5);
        h1.setFillColor(sf::Color(44, 62, 80, 60));
        h1.setOutlineColor(sf::Color(44, 62, 80, 40));
        h1.setOutlineThickness(1.0f);
        h1.setPosition(hx, hy);
        window.draw(h1);
        sf::CircleShape h2(5);
        h2.setFillColor(sf::Color(44, 62, 80, 60));
        h2.setOutlineColor(sf::Color(44, 62, 80, 40));
        h2.setOutlineThickness(1.0f);
        h2.setPosition(hx + 7, hy);
        window.draw(h2);
    }
}
