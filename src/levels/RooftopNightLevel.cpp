#include "../../include/levels/RooftopNightLevel.h"
#include "../../include/entities/ObstacleFactory.h"
#include "../../include/components/TransformComponent.h"
#include "../../include/components/PhysicsComponent.h"
#include "../../include/components/SpriteComponent.h"
#include "../../include/components/CollisionComponent.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <algorithm>
#include <cmath>
#include <cstdlib>

// Flat design night palette
static const sf::Color NIGHT_BUILDING_COLORS[] = {
    sf::Color(35, 32, 55),     // Deep violet
    sf::Color(28, 30, 52),     // Dark navy
    sf::Color(40, 35, 50),     // Plum
    sf::Color(25, 28, 48),     // Midnight blue
    sf::Color(38, 32, 45),     // Dark mauve
    sf::Color(30, 35, 50),     // Slate navy
    sf::Color(42, 38, 55),     // Dark lavender
    sf::Color(32, 30, 48),     // Indigo
};
static const int NUM_NIGHT_COLORS = sizeof(NIGHT_BUILDING_COLORS) / sizeof(NIGHT_BUILDING_COLORS[0]);

static const sf::Color NIGHT_ACCENT_COLORS[] = {
    sf::Color(55, 50, 75),     // Lighter violet
    sf::Color(48, 48, 72),     // Lighter navy
    sf::Color(60, 52, 70),     // Lighter plum
    sf::Color(45, 45, 68),     // Lighter midnight
    sf::Color(58, 48, 65),     // Lighter mauve
    sf::Color(50, 52, 70),     // Lighter slate
    sf::Color(62, 55, 75),     // Lighter lavender
    sf::Color(52, 48, 68),     // Lighter indigo
};

static const sf::Color NIGHT_WINDOW_COLORS[] = {
    sf::Color(255, 200, 120, 200),  // Warm amber
    sf::Color(255, 180, 100, 200),  // Orange
    sf::Color(200, 220, 255, 180),  // Cool blue
    sf::Color(180, 200, 255, 180),  // Ice blue
};

static const sf::Color NIGHT_OUTLINE(20, 18, 35);       // Near-black for outlines
static const sf::Color NIGHT_SHADOW(0, 0, 0, 50);       // Dark shadow

static const char* NEON_TEXTS[] = {"OPEN", "BAR", "SUSHI", "RAMEN", "HOTEL", "24H", "LOVE", "DREAM"};
static const sf::Color NEON_COLORS[] = {
    sf::Color(255, 50, 100),    // Hot pink
    sf::Color(50, 200, 255),     // Cyan
    sf::Color(50, 255, 120),     // Neon green
    sf::Color(255, 200, 50),     // Amber
    sf::Color(200, 50, 255),     // Purple
    sf::Color(255, 100, 50),     // Orange
    sf::Color(255, 50, 200),     // Magenta
    sf::Color(100, 255, 200),   // Mint
};

void RooftopNightLevel::init(AssetManager& assets, GameState& state) {
    spawnInterval_ = 2.0f;
    spawnTimer_ = 0.0f;
    minSpawnInterval_ = 1.0f;

    scrollOffset_ = 0.0f;
    nextBuildingX_ = 0.0f;
    gameTime_ = 0.0f;
    assets_ = &assets;

    buildings_.clear();
    neonSigns_.clear();
    decoratives_.clear();
    generateBuildings(10);

    if (assets.hasTexture("city_bg")) {
        skyParallax_.addLayer(assets.getTexture("city_bg"), 0.1f, 0);
    }

    dustParticles_.setGravity(-15.0f);
    dustTimer_ = 0.0f;
    spawnObstacles(state, assets);
}

void RooftopNightLevel::generateBuildings(int count) {
    for (int i = 0; i < count; i++) {
        Building b;
        b.x = nextBuildingX_;
        b.width = 120.0f + (std::rand() % 200);
        b.height = 200.0f + (std::rand() % 250);
        int ci = std::rand() % NUM_NIGHT_COLORS;
        b.bodyColor = NIGHT_BUILDING_COLORS[ci];
        b.accentColor = NIGHT_ACCENT_COLORS[ci];
        b.windowColor = NIGHT_WINDOW_COLORS[std::rand() % 4];
        b.windowCount = 3 + (std::rand() % 6);
        b.windowRows = 3 + (std::rand() % 3);
        buildings_.push_back(b);
        nextBuildingX_ += b.width;

        // Generate decorative items for this building
        int decoRoll = std::rand() % 100;
        if (decoRoll < 20 && assets_ && assets_->hasTexture("flags")) {
            float roofY = Constants::GROUND_Y - b.height;
            DecorativeItem d;
            d.x = b.x + b.width * 0.2f + (std::rand() % static_cast<int>(b.width * 0.4f));
            d.y = roofY - 16.0f;
            d.textureName = "flags";
            d.scale = 0.25f;
            decoratives_.push_back(d);
        }
        if (decoRoll < 30 && assets_ && assets_->hasTexture("ladder")) {
            float roofY = Constants::GROUND_Y - b.height;
            DecorativeItem d;
            d.x = b.x + (std::rand() % 2 == 0 ? -6.0f : b.width - 4.0f);
            d.y = roofY + 10.0f;
            d.textureName = "ladder";
            d.scale = 0.3f;
            decoratives_.push_back(d);
        }
        if (decoRoll < 12 && assets_ && assets_->hasTexture("accessory")) {
            float roofY = Constants::GROUND_Y - b.height;
            DecorativeItem d;
            d.x = b.x + b.width * 0.5f + (std::rand() % static_cast<int>(b.width * 0.3f));
            d.y = roofY - 8.0f;
            d.textureName = "accessory";
            d.scale = 0.2f;
            decoratives_.push_back(d);
        }
    }
}

void RooftopNightLevel::update(float deltaTime, AssetManager& assets, GameState& state) {
    gameTime_ += deltaTime;
    float speed = state.currentSpeed;
    skyParallax_.setScrollDirection(1.0f);
    skyParallax_.update(deltaTime, speed);

    scrollOffset_ += speed * deltaTime;
    float scrollAmount = speed * deltaTime;

    // Scroll buildings
    for (auto& b : buildings_) b.x -= scrollAmount;
    for (auto& n : neonSigns_) n.x -= scrollAmount;
    for (auto& d : decoratives_) d.x -= scrollAmount;

    // Remove off-screen
    while (!buildings_.empty() && buildings_.front().x + buildings_.front().width < -200) {
        buildings_.erase(buildings_.begin());
    }
    while (!neonSigns_.empty() && neonSigns_.front().x < -200) {
        neonSigns_.erase(neonSigns_.begin());
    }
    while (!decoratives_.empty() && decoratives_.front().x < -200) {
        decoratives_.erase(decoratives_.begin());
    }

    // Add new buildings ahead
    while (nextBuildingX_ - scrollOffset_ < Constants::WINDOW_WIDTH + 500) {
        float nextX = buildings_.empty() ? nextBuildingX_ - scrollOffset_
                     : buildings_.back().x + buildings_.back().width;
        Building b;
        b.x = nextX;
        b.width = 120.0f + (std::rand() % 200);
        b.height = 200.0f + (std::rand() % 250);
        int ci = std::rand() % NUM_NIGHT_COLORS;
        b.bodyColor = NIGHT_BUILDING_COLORS[ci];
        b.accentColor = NIGHT_ACCENT_COLORS[ci];
        b.windowColor = NIGHT_WINDOW_COLORS[std::rand() % 4];
        b.windowCount = 3 + (std::rand() % 6);
        b.windowRows = 3 + (std::rand() % 3);
        buildings_.push_back(b);

        // Generate decorative items for this new building
        int decoRoll = std::rand() % 100;
        if (decoRoll < 20 && assets_ && assets_->hasTexture("flags")) {
            float roofY = Constants::GROUND_Y - b.height;
            DecorativeItem d;
            d.x = b.x + b.width * 0.2f + (std::rand() % static_cast<int>(b.width * 0.4f));
            d.y = roofY - 16.0f;
            d.textureName = "flags";
            d.scale = 0.25f;
            decoratives_.push_back(d);
        }
        if (decoRoll < 30 && assets_ && assets_->hasTexture("ladder")) {
            float roofY = Constants::GROUND_Y - b.height;
            DecorativeItem d;
            d.x = b.x + (std::rand() % 2 == 0 ? -6.0f : b.width - 4.0f);
            d.y = roofY + 10.0f;
            d.textureName = "ladder";
            d.scale = 0.3f;
            decoratives_.push_back(d);
        }
        if (decoRoll < 12 && assets_ && assets_->hasTexture("accessory")) {
            float roofY = Constants::GROUND_Y - b.height;
            DecorativeItem d;
            d.x = b.x + b.width * 0.5f + (std::rand() % static_cast<int>(b.width * 0.3f));
            d.y = roofY - 8.0f;
            d.textureName = "accessory";
            d.scale = 0.2f;
            decoratives_.push_back(d);
        }

        // Neon sign on ~30% of new buildings
        if (std::rand() % 100 < 30) {
            generateNeonSign(static_cast<int>(neonSigns_.size()), gameTime_);
            neonSigns_.back().x = b.x + b.width * 0.3f + (std::rand() % static_cast<int>(b.width * 0.4f));
        }
    }

    spawnTimer_ += deltaTime;
    if (spawnTimer_ >= spawnInterval_) {
        spawnTimer_ = 0.0f;
        spawnObstacles(state, assets);
        spawnInterval_ = std::max(minSpawnInterval_, spawnInterval_ - 0.05f);
    }

    dustTimer_ += deltaTime;
    if (dustTimer_ > 0.12f && state.player) {
        dustTimer_ = 0.0f;
        auto* phys = state.player->getComponent<PhysicsComponent>();
        auto* trans = state.player->getComponent<TransformComponent>();
        if (phys && trans && phys->isGrounded) {
            dustParticles_.emit(
                sf::Vector2f(trans->position.x - 10, Constants::GROUND_Y),
                1, sf::Color(60, 65, 90, 60), 12.0f, 0.6f, 2.5f);
        }
    }
    dustParticles_.updateParticles(deltaTime);
    cleanupOffscreen(state);
}

void RooftopNightLevel::render(sf::RenderWindow& window) {
    // Night sky gradient (flat design bands)
    sf::RectangleShape skyBand(sf::Vector2f(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT));
    skyBand.setFillColor(sf::Color(12, 10, 30));
    window.draw(skyBand);

    sf::RectangleShape stripe(sf::Vector2f(Constants::WINDOW_WIDTH, 40));
    for (int i = 0; i < Constants::WINDOW_HEIGHT; i += 40) {
        float t = static_cast<float>(i) / Constants::WINDOW_HEIGHT;
        sf::Uint8 r = static_cast<sf::Uint8>(12 + 15 * t);
        sf::Uint8 g = static_cast<sf::Uint8>(10 + 18 * t);
        sf::Uint8 b = static_cast<sf::Uint8>(30 + 15 * t);
        stripe.setFillColor(sf::Color(r, g, b, 60));
        stripe.setPosition(0, static_cast<float>(i));
        window.draw(stripe);
    }

    // Stars (clean circles, no noise)
    sf::CircleShape star(1.5f);
    for (int i = 0; i < 40; i++) {
        float sx = (i * 137 + 50) % Constants::WINDOW_WIDTH;
        float sy = (i * 89 + 20) % 220;
        int bright = 150 + (i * 20) % 105;
        star.setFillColor(sf::Color(bright, bright, bright + 20));
        star.setPosition(sx, sy);
        window.draw(star);
    }

    // Moon (crescent — flat design)
    sf::CircleShape moonBody(32);
    moonBody.setFillColor(sf::Color(230, 225, 210));
    moonBody.setPosition(1050, 50);
    window.draw(moonBody);

    sf::CircleShape moonShadow(28);
    moonShadow.setFillColor(sf::Color(12, 10, 30));
    moonShadow.setPosition(1062, 42);
    window.draw(moonShadow);

    // Moon glow
    sf::CircleShape moonGlow(40);
    moonGlow.setFillColor(sf::Color(230, 225, 210, 30));
    moonGlow.setPosition(1042, 42);
    window.draw(moonGlow);

    // Parallax far background
    skyParallax_.render(window);

    // Draw buildings with flat design
    for (auto& b : buildings_) {
        float roofY = Constants::GROUND_Y - b.height;

        // Shadow
        sf::RectangleShape shadow(sf::Vector2f(b.width, b.height));
        shadow.setFillColor(NIGHT_SHADOW);
        shadow.setPosition(b.x + 5, roofY + 5);
        window.draw(shadow);

        // Main body
        sf::RectangleShape body(sf::Vector2f(b.width, b.height));
        body.setFillColor(b.bodyColor);
        body.setPosition(b.x, roofY);
        body.setOutlineColor(NIGHT_OUTLINE);
        body.setOutlineThickness(2.0f);
        window.draw(body);

        // Parapet
        sf::RectangleShape parapet(sf::Vector2f(b.width, 12));
        parapet.setFillColor(b.accentColor);
        parapet.setPosition(b.x, roofY - 12);
        parapet.setOutlineColor(NIGHT_OUTLINE);
        parapet.setOutlineThickness(2.0f);
        window.draw(parapet);

        // Decorative stripe below parapet
        sf::RectangleShape stripe(sf::Vector2f(b.width - 16, 3));
        stripe.setFillColor(sf::Color(60, 55, 80));
        stripe.setPosition(b.x + 8, roofY + 6);
        window.draw(stripe);

        // Windows
        float winAreaW = b.width - 30;
        float winAreaH = b.height - 40;
        float winSpacingX = winAreaW / (b.windowCount + 1);
        float winSpacingY = winAreaH / (b.windowRows + 1);
        float winW = std::min(14.0f, winSpacingX * 0.5f);
        float winH = std::min(18.0f, winSpacingY * 0.5f);

        for (int row = 0; row < b.windowRows; row++) {
            for (int col = 0; col < b.windowCount; col++) {
                float wx = b.x + 15 + (col + 1) * winSpacingX - winW / 2;
                float wy = roofY + 20 + (row + 1) * winSpacingY - winH / 2;

                // Shadow
                sf::RectangleShape winShadow(sf::Vector2f(winW, winH));
                winShadow.setFillColor(NIGHT_SHADOW);
                winShadow.setPosition(wx + 2, wy + 2);
                window.draw(winShadow);

                // Window (most lit at night)
                bool lit = (std::rand() % 4) > 0;
                sf::Color winColor = lit ? b.windowColor : sf::Color(20, 18, 35, 100);
                sf::RectangleShape win(sf::Vector2f(winW, winH));
                win.setFillColor(winColor);
                win.setPosition(wx, wy);
                win.setOutlineColor(NIGHT_OUTLINE);
                win.setOutlineThickness(1.0f);
                window.draw(win);

                // Window cross
                if (winW >= 10 && winH >= 12) {
                    sf::RectangleShape crossH(sf::Vector2f(winW, 1));
                    crossH.setFillColor(NIGHT_OUTLINE);
                    crossH.setPosition(wx, wy + winH / 2);
                    window.draw(crossH);
                    sf::RectangleShape crossV(sf::Vector2f(1, winH));
                    crossV.setFillColor(NIGHT_OUTLINE);
                    crossV.setPosition(wx + winW / 2, wy);
                    window.draw(crossV);
                }
            }
        }

        // Corner dots (simulate rounded corners)
        sf::CircleShape corner(3.0f);
        corner.setFillColor(NIGHT_OUTLINE);
        corner.setPosition(b.x - 3, roofY - 3);
        window.draw(corner);
        corner.setPosition(b.x + b.width - 3, roofY - 3);
        window.draw(corner);
        corner.setPosition(b.x - 3, Constants::GROUND_Y - 3);
        window.draw(corner);
        corner.setPosition(b.x + b.width - 3, Constants::GROUND_Y - 3);
        window.draw(corner);
    }

    // Neon signs
    for (auto& sign : neonSigns_) {
        drawNeonSign(sign, gameTime_, window);
    }

    // Ground
    sf::RectangleShape groundShadow(sf::Vector2f(Constants::WINDOW_WIDTH, 6));
    groundShadow.setFillColor(NIGHT_SHADOW);
    groundShadow.setPosition(4, Constants::GROUND_Y + 2);
    window.draw(groundShadow);

    sf::RectangleShape groundLine(sf::Vector2f(Constants::WINDOW_WIDTH, 3));
    groundLine.setFillColor(NIGHT_OUTLINE);
    groundLine.setPosition(0, Constants::GROUND_Y);
    window.draw(groundLine);

    sf::RectangleShape groundFill(sf::Vector2f(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT - Constants::GROUND_Y));
    groundFill.setFillColor(sf::Color(20, 18, 32));
    groundFill.setPosition(0, Constants::GROUND_Y);
    window.draw(groundFill);

    dustParticles_.render(window);

    // Decorative items
    if (assets_) renderDecorative(window, *assets_);
}

void RooftopNightLevel::renderDecorative(sf::RenderWindow& window, AssetManager& assets) {
    for (auto& d : decoratives_) {
        if (!assets.hasTexture(d.textureName)) continue;
        sf::Sprite decoSprite(assets.getTexture(d.textureName));
        decoSprite.setScale(d.scale, d.scale);
        sf::Vector2u sz = decoSprite.getTexture()->getSize();
        decoSprite.setOrigin(sz.x / 2.0f, static_cast<float>(sz.y));
        decoSprite.setPosition(d.x, d.y);
        window.draw(decoSprite);
    }
}

void RooftopNightLevel::spawnObstacles(GameState& state, AssetManager& assets) {
    float startX = Constants::WINDOW_WIDTH + 100.0f + static_cast<float>(std::rand() % 200);
    int roll = std::rand() % 100;

    if (roll < 25) {
        auto chimney = ObstacleFactory::createChimney(assets, startX);
        state.entities.push_back(std::move(chimney));
    } else if (roll < 40) {
        auto acUnit = ObstacleFactory::createACUnit(assets, startX);
        state.entities.push_back(std::move(acUnit));
    } else if (roll < 55) {
        auto dish = ObstacleFactory::createSatelliteDish(assets, startX);
        state.entities.push_back(std::move(dish));
    } else if (roll < 65) {
        auto sign = ObstacleFactory::createSign(assets, startX);
        state.entities.push_back(std::move(sign));
    } else if (roll < 80) {
        auto pigeon = ObstacleFactory::createPigeon(assets, startX);
        state.entities.push_back(std::move(pigeon));
    } else if (roll < 92) {
        for (int i = 0; i < 3; i++) {
            auto coin = ObstacleFactory::createCoin(assets, startX + i * 40,
                Constants::GROUND_Y - 60 - (std::rand() % 40));
            state.entities.push_back(std::move(coin));
        }
    } else {
        auto pu = ObstacleFactory::createPowerUp(assets,
            static_cast<Constants::PowerUpType>(std::rand() % 3),
            startX + 250, Constants::GROUND_Y - 80);
        state.entities.push_back(std::move(pu));
    }
}

void RooftopNightLevel::generateNeonSign(int index, float gameTime) {
    NeonSign sign;
    sign.x = 0;
    sign.y = Constants::GROUND_Y - 80 - (std::rand() % 40);
    sign.color = NEON_COLORS[index % 8];
    sign.phase = (index * 1.7f) + gameTime;
    sign.text = NEON_TEXTS[index % 8];
    neonSigns_.push_back(sign);
}

void RooftopNightLevel::drawNeonSign(const NeonSign& sign, float gameTime, sf::RenderWindow& window) {
    float pulse = 0.6f + 0.4f * std::sin(gameTime * 3.0f + sign.phase);
    int alpha = static_cast<int>(180.0f * pulse);
    sf::Color glowColor = sign.color;
    glowColor.a = static_cast<sf::Uint8>(alpha);

    float signW = static_cast<float>(sign.text.length() * 10 + 12);
    float signH = 22.0f;

    // Outer glow (3 layers)
    for (int g = 3; g >= 1; g--) {
        sf::Color glow = glowColor;
        glow.a = static_cast<sf::Uint8>(alpha / (g * 1.5f));
        sf::RectangleShape glowRect(sf::Vector2f(signW + g * 10, signH + g * 8));
        glowRect.setFillColor(glow);
        glowRect.setPosition(sign.x - g * 5, sign.y - g * 4);
        window.draw(glowRect);
    }

    // Sign background
    sf::RectangleShape signBg(sf::Vector2f(signW, signH));
    signBg.setFillColor(sf::Color(12, 10, 20, static_cast<sf::Uint8>(180 * pulse)));
    signBg.setOutlineThickness(1.5f);
    signBg.setOutlineColor(glowColor);
    signBg.setPosition(sign.x, sign.y);
    window.draw(signBg);

    // Neon text bars
    float cx = sign.x + 6.0f;
    float cy = sign.y + 4.0f;
    for (size_t ci = 0; ci < sign.text.length(); ci++) {
        float charW = 8.0f;
        float charH = 14.0f;

        sf::RectangleShape charBg(sf::Vector2f(charW, charH));
        charBg.setFillColor(glowColor);
        charBg.setPosition(cx, cy);
        window.draw(charBg);

        sf::Color coreColor = sign.color;
        coreColor.a = static_cast<sf::Uint8>(220 * pulse);
        sf::RectangleShape core(sf::Vector2f(charW - 2, charH - 2));
        core.setFillColor(coreColor);
        core.setPosition(cx + 1, cy + 1);
        window.draw(core);

        cx += charW + 1;
    }

    // Random flicker
    if (std::rand() % 50 == 0) {
        sf::RectangleShape flicker(sf::Vector2f(signW, 1));
        flicker.setFillColor(sf::Color::White);
        flicker.setPosition(sign.x, sign.y + signH / 2);
        window.draw(flicker);
    }
}
