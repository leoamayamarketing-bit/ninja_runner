#include "../../include/ui/GameOverScreen.h"
#include "../../include/core/AudioManager.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <cstdio>
#include <cmath>

static const sf::Color OVERLAY_BG(10, 8, 25, 220);
static const sf::Color PANEL_FILL(18, 15, 38, 200);
static const sf::Color PANEL_OUTLINE(44, 62, 80, 180);
static const sf::Color TITLE_COLOR(235, 230, 220);
static const sf::Color TEXT_WHITE(225, 225, 235);
static const sf::Color TEXT_GOLD(255, 215, 80);
static const sf::Color TEXT_DIM(160, 160, 175);
static const sf::Color ACCENT_RED(200, 45, 45);
static const sf::Color ACCENT_ORANGE(249, 199, 132);

static void drawRoundedPanel(sf::RenderWindow& window, float x, float y, float w, float h,
    const sf::Color& fill, const sf::Color& outline, float cr = 6.0f) {
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

void GameOverScreen::init(FontRenderer& fontRenderer, AudioManager* audio) {
    fontRenderer_ = &fontRenderer;
    audio_ = audio;
}

GameOverScreen::Action GameOverScreen::update(InputManager& input) {
    timer_ += 0.016f;

    int prevIndex = selectedIndex_;

    if (input.isMenuUpPressed()) {
        selectedIndex_ = (selectedIndex_ - 1 + 2) % 2;
    }
    if (input.isMenuDownPressed()) {
        selectedIndex_ = (selectedIndex_ + 1) % 2;
    }

    if (selectedIndex_ != prevIndex && audio_) {
        audio_->playNavigate();
    }

    if (input.isMenuSelectPressed()) {
        if (audio_) audio_->playSelect();
        if (selectedIndex_ == 0) return RESTART;
        else return MENU;
    }

    return NONE;
}

void GameOverScreen::render(sf::RenderWindow& window, const GameState& state) {
    // Background overlay
    sf::RectangleShape overlay(sf::Vector2f(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT));
    overlay.setFillColor(OVERLAY_BG);
    window.draw(overlay);

    if (!fontRenderer_) return;

    float pulse = 0.5f + 0.5f * std::sin(timer_ * 2.0f);

    // === Main result panel (centered) ===
    float panelW = 460, panelH = 360, panelX = (Constants::WINDOW_WIDTH - panelW) / 2, panelY = 110;
    drawRoundedPanel(window, panelX, panelY, panelW, panelH, PANEL_FILL, PANEL_OUTLINE, 10.0f);

    // === GAME OVER header with glow ===
    // Red glow behind text
    sf::RectangleShape glowLine(sf::Vector2f(200, 2));
    glowLine.setFillColor(sf::Color(200, 45, 45, static_cast<sf::Uint8>(60 * pulse)));
    glowLine.setPosition(panelX + (panelW - 200) / 2, panelY + 30);
    window.draw(glowLine);

    fontRenderer_->drawTextCentered(window, "GAME OVER", Constants::WINDOW_WIDTH / 2,
        panelY + 55, 48, ACCENT_RED);

    // Another glow line below
    glowLine.setPosition(panelX + (panelW - 200) / 2, panelY + 65);
    window.draw(glowLine);

    // === Decorative ninja star icon ===
    sf::ConvexShape star;
    star.setPointCount(8);
    float scx = Constants::WINDOW_WIDTH / 2, scy = panelY + 95;
    for (int i = 0; i < 8; i++) {
        float angle = i * 3.14159f / 4.0f;
        float r = (i % 2 == 0) ? 12.0f : 5.0f;
        star.setPoint(i, sf::Vector2f(scx + r * std::cos(angle), scy + r * std::sin(angle)));
    }
    star.setFillColor(sf::Color(200, 45, 45, 80));
    star.setOutlineColor(sf::Color(200, 45, 45, 120));
    star.setOutlineThickness(1.0f);
    window.draw(star);

    // === Stats panel inside main panel ===
    float statsY = panelY + 120;
    drawRoundedPanel(window, panelX + 30, statsY, panelW - 60, 130, sf::Color(15, 12, 32, 120), sf::Color(44, 62, 80, 80), 6.0f);

    // Score
    char buf[128];
    std::snprintf(buf, sizeof(buf), "SCORE");
    fontRenderer_->drawTextCentered(window, buf, Constants::WINDOW_WIDTH / 2,
        statsY + 12, 14, TEXT_DIM);
    std::snprintf(buf, sizeof(buf), "%.0f", state.score);
    fontRenderer_->drawTextCentered(window, buf, Constants::WINDOW_WIDTH / 2,
        statsY + 30, 28, TEXT_WHITE);

    // Coins
    std::snprintf(buf, sizeof(buf), "COINS COLLECTED");
    fontRenderer_->drawTextCentered(window, buf, Constants::WINDOW_WIDTH / 2 - 100,
        statsY + 65, 11, TEXT_DIM);
    std::snprintf(buf, sizeof(buf), "%d", state.coins);
    fontRenderer_->drawTextCentered(window, buf, Constants::WINDOW_WIDTH / 2 - 100,
        statsY + 82, 22, TEXT_GOLD);

    // Distance
    std::snprintf(buf, sizeof(buf), "DISTANCE");
    fontRenderer_->drawTextCentered(window, buf, Constants::WINDOW_WIDTH / 2 + 100,
        statsY + 65, 11, TEXT_DIM);
    std::snprintf(buf, sizeof(buf), "%.0f m", state.distance);
    fontRenderer_->drawTextCentered(window, buf, Constants::WINDOW_WIDTH / 2 + 100,
        statsY + 82, 22, TEXT_WHITE);

    // === Action buttons ===
    float btnY = panelY + panelH - 70;

    for (int i = 0; i < 2; i++) {
        bool selected = (i == selectedIndex_);
        float bx = panelX + 60 + i * 190;
        float bw = 160, bh = 36;

        // Button background
        sf::Color btnColor = selected ? sf::Color(200, 45, 45, 80) : sf::Color(44, 62, 80, 60);
        sf::Color btnOutline = selected ? sf::Color(200, 45, 45, 160) : sf::Color(44, 62, 80, 100);
        drawRoundedPanel(window, bx, btnY, bw, bh, btnColor, btnOutline, 6.0f);

        // Button text
        const char* label = (i == 0) ? "RESTART" : "MENU";
        sf::Color textColor = selected ? sf::Color(255, 220, 80) : TEXT_DIM;
        fontRenderer_->drawTextCentered(window, label, bx + bw / 2, btnY + 8, selected ? 20 : 16, textColor);
    }

    // === Bottom tip ===
    fontRenderer_->drawTextCentered(window, "Use ARROW KEYS  |  Press SPACE",
        Constants::WINDOW_WIDTH / 2, Constants::WINDOW_HEIGHT - 40, 13, sf::Color(100, 100, 120, 180));
}
