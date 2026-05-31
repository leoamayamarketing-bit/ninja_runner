#include "../../include/levels/RooftopDayLevel.h"
#include "../../include/entities/ObstacleFactory.h"
#include "../../include/components/TransformComponent.h"
#include "../../include/components/PhysicsComponent.h"
#include "../../include/components/SpriteComponent.h"
#include "../../include/components/CollisionComponent.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <algorithm>
#include <cmath>
#include <cstdlib>

// Flat design palette
static const sf::Color BUILDING_COLORS[] = {
    sf::Color(74, 144, 217),     // Corporate blue
    sf::Color(155, 142, 196),    // Soft violet
    sf::Color(135, 206, 235),    // Pastel sky blue
    sf::Color(232, 232, 232),    // Light gray
    sf::Color(249, 199, 132),    // Pastel orange
    sf::Color(244, 167, 167),    // Soft coral
    sf::Color(200, 220, 240),    // Ice blue
    sf::Color(220, 210, 230),    // Lavender
};
static const int NUM_BUILDING_COLORS = sizeof(BUILDING_COLORS) / sizeof(BUILDING_COLORS[0]);

static const sf::Color ACCENT_COLORS[] = {
    sf::Color(60, 120, 200),     // Darker blue
    sf::Color(130, 115, 175),    // Darker violet
    sf::Color(110, 180, 210),    // Darker sky blue
    sf::Color(200, 200, 200),    // Darker gray
    sf::Color(220, 170, 100),    // Darker orange
    sf::Color(210, 135, 135),    // Darker coral
    sf::Color(170, 195, 220),    // Darker ice
    sf::Color(190, 175, 205),    // Darker lavender
};

static const sf::Color WINDOW_COLORS[] = {
    sf::Color(255, 220, 150, 220),  // Warm amber
    sf::Color(180, 215, 255, 220),  // Cool blue
    sf::Color(255, 200, 200, 220),  // Pink
    sf::Color(200, 240, 200, 220),  // Mint
};

static const sf::Color OUTLINE_COLOR(44, 62, 80);       // Dark blue for outlines
static const sf::Color SHADOW_COLOR(44, 62, 80, 30);    // Soft shadow

void RooftopDayLevel::init(AssetManager& assets, GameState& state) {
    spawnInterval_ = 2.2f;
    spawnTimer_ = 0.0f;
    minSpawnInterval_ = 1.2f;

    scrollOffset_ = 0.0f;
    nextBuildingX_ = 0.0f;
    buildingTimer_ = 0.0f;

    // Generate initial buildings
    buildings_.clear();
    generateBuildings(10);

    // Parallax layers - city skyline background
    if (assets.hasTexture("city_bg")) {
        skyParallax_.addLayer(assets.getTexture("city_bg"), 0.1f, 0);
    }
    if (assets.hasTexture("city_mid")) {
        skyParallax_.addLayer(assets.getTexture("city_mid"), 0.3f, 50);
    }

    // Configure dust particles for rooftop
    dustParticles_.setGravity(-20.0f); // dust floats up
    dustTimer_ = 0.0f;

    spawnObstacles(state, assets);
}

void RooftopDayLevel::generateBuildings(int count) {
    for (int i = 0; i < count; i++) {
        Building b;
        b.x = nextBuildingX_;
        b.width = 120.0f + (std::rand() % 200);
        b.height = 200.0f + (std::rand() % 250);
        int ci = std::rand() % NUM_BUILDING_COLORS;
        b.bodyColor = BUILDING_COLORS[ci];
        b.accentColor = ACCENT_COLORS[ci];
        b.windowColor = WINDOW_COLORS[std::rand() % 4];
        b.windowCount = 3 + (std::rand() % 6);
        b.windowRows = 3 + (std::rand() % 3);
        buildings_.push_back(b);
        nextBuildingX_ += b.width;
    }
}

void RooftopDayLevel::update(float deltaTime, AssetManager& assets, GameState& state) {
    float speed = state.currentSpeed;
    skyParallax_.setScrollDirection(1.0f);
    skyParallax_.update(deltaTime, speed);

    scrollOffset_ += speed * deltaTime;
    scrollBuildings(speed, deltaTime, state);

    // Spawn obstacles
    spawnTimer_ += deltaTime;
    if (spawnTimer_ >= spawnInterval_) {
        spawnTimer_ = 0.0f;
        spawnObstacles(state, assets);
        spawnInterval_ = std::max(minSpawnInterval_, spawnInterval_ - (state.hardMode ? 0.08f : 0.04f));
    }

    // Dust particles from running
    dustTimer_ += deltaTime;
    if (dustTimer_ > 0.1f && state.player) {
        dustTimer_ = 0.0f;
        auto* phys = state.player->getComponent<PhysicsComponent>();
        auto* trans = state.player->getComponent<TransformComponent>();
        if (phys && trans && phys->isGrounded) {
            dustParticles_.emit(
                sf::Vector2f(trans->position.x - 10, Constants::GROUND_Y),
                1, sf::Color(180, 170, 160, 80), 15.0f, 0.8f, 3.0f);
        }
    }
    dustParticles_.updateParticles(deltaTime);

    cleanupOffscreen(state);
}

void RooftopDayLevel::scrollBuildings(float speed, float deltaTime, GameState& state) {
    float scrollAmount = speed * deltaTime;

    for (auto& b : buildings_) {
        b.x -= scrollAmount;
    }

    // Remove off-screen buildings and add new ones
    while (!buildings_.empty() && buildings_.front().x + buildings_.front().width < -200) {
        buildings_.erase(buildings_.begin());
    }

    // Add new buildings ahead
    while (nextBuildingX_ - scrollOffset_ < Constants::WINDOW_WIDTH + 500) {
        float nextX = buildings_.empty() ? nextBuildingX_ - scrollOffset_
                     : buildings_.back().x + buildings_.back().width;
        Building b;
        b.x = nextX;
        b.width = 120.0f + (std::rand() % 200);
        b.height = 200.0f + (std::rand() % 250);
        int ci = std::rand() % NUM_BUILDING_COLORS;
        b.bodyColor = BUILDING_COLORS[ci];
        b.accentColor = ACCENT_COLORS[ci];
        b.windowColor = WINDOW_COLORS[std::rand() % 4];
        b.windowCount = 3 + (std::rand() % 6);
        b.windowRows = 3 + (std::rand() % 3);
        buildings_.push_back(b);
    }
}

void RooftopDayLevel::render(sf::RenderWindow& window) {
    // Sky gradient (flat design — clean color bands)
    sf::RectangleShape skyBand(sf::Vector2f(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT));
    skyBand.setFillColor(sf::Color(200, 225, 245));
    window.draw(skyBand);

    // Subtle gradient bands
    sf::RectangleShape stripe(sf::Vector2f(Constants::WINDOW_WIDTH, 40));
    for (int i = 0; i < Constants::WINDOW_HEIGHT; i += 40) {
        float t = static_cast<float>(i) / Constants::WINDOW_HEIGHT;
        sf::Uint8 r = static_cast<sf::Uint8>(200 + 40 * t);
        sf::Uint8 g = static_cast<sf::Uint8>(225 + 10 * t);
        sf::Uint8 b = static_cast<sf::Uint8>(245 - 25 * t);
        stripe.setFillColor(sf::Color(r, g, b, 80));
        stripe.setPosition(0, static_cast<float>(i));
        window.draw(stripe);
    }

    // Draw far background buildings (parallax)
    skyParallax_.render(window);

    // Draw mid-ground buildings with flat design
    for (auto& b : buildings_) {
        drawBuilding(window, b, b.x);
    }

    // Draw ground (rooftop surface)
    sf::RectangleShape groundShadow(sf::Vector2f(Constants::WINDOW_WIDTH, 6));
    groundShadow.setFillColor(SHADOW_COLOR);
    groundShadow.setPosition(4, Constants::GROUND_Y + 2);
    window.draw(groundShadow);

    sf::RectangleShape groundLine(sf::Vector2f(Constants::WINDOW_WIDTH, 3));
    groundLine.setFillColor(OUTLINE_COLOR);
    groundLine.setPosition(0, Constants::GROUND_Y);
    window.draw(groundLine);

    sf::RectangleShape groundFill(sf::Vector2f(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT - Constants::GROUND_Y));
    groundFill.setFillColor(sf::Color(232, 225, 218));
    window.draw(groundFill);

    // Dust particles
    dustParticles_.render(window);
}

void RooftopDayLevel::drawBuilding(sf::RenderWindow& window, const Building& b, float screenX) {
    float roofY = Constants::GROUND_Y - b.height;

    // Shadow (offset to bottom-right)
    sf::RectangleShape shadow(sf::Vector2f(b.width, b.height));
    shadow.setFillColor(SHADOW_COLOR);
    shadow.setPosition(screenX + 5, roofY + 5);
    window.draw(shadow);

    // Main building body
    sf::RectangleShape body(sf::Vector2f(b.width, b.height));
    body.setFillColor(b.bodyColor);
    body.setPosition(screenX, roofY);
    body.setOutlineColor(OUTLINE_COLOR);
    body.setOutlineThickness(2.0f);
    window.draw(body);

    // Rooftop parapet / crown
    sf::RectangleShape parapet(sf::Vector2f(b.width, 12));
    parapet.setFillColor(b.accentColor);
    parapet.setPosition(screenX, roofY - 12);
    parapet.setOutlineColor(OUTLINE_COLOR);
    parapet.setOutlineThickness(2.0f);
    window.draw(parapet);

    // Decorative stripe below parapet
    sf::RectangleShape stripe(sf::Vector2f(b.width - 16, 3));
    stripe.setFillColor(sf::Color::White);
    stripe.setPosition(screenX + 8, roofY + 6);
    window.draw(stripe);

    // Windows as clean rectangular blocks
    float winAreaW = b.width - 30;
    float winAreaH = b.height - 40;
    float winSpacingX = winAreaW / (b.windowCount + 1);
    float winSpacingY = winAreaH / (b.windowRows + 1);
    float winW = std::min(14.0f, winSpacingX * 0.5f);
    float winH = std::min(18.0f, winSpacingY * 0.5f);

    for (int row = 0; row < b.windowRows; row++) {
        for (int col = 0; col < b.windowCount; col++) {
            float wx = screenX + 15 + (col + 1) * winSpacingX - winW / 2;
            float wy = roofY + 20 + (row + 1) * winSpacingY - winH / 2;

            // Window shadow
            sf::RectangleShape winShadow(sf::Vector2f(winW, winH));
            winShadow.setFillColor(SHADOW_COLOR);
            winShadow.setPosition(wx + 2, wy + 2);
            window.draw(winShadow);

            // Window body
            bool lit = (std::rand() % 3) > 0;
            sf::Color winColor = lit ? b.windowColor : sf::Color(44, 62, 80, 100);
            sf::RectangleShape win(sf::Vector2f(winW, winH));
            win.setFillColor(winColor);
            win.setPosition(wx, wy);
            win.setOutlineColor(OUTLINE_COLOR);
            win.setOutlineThickness(1.0f);
            window.draw(win);

            // Window cross (if large enough)
            if (winW >= 10 && winH >= 12) {
                sf::RectangleShape crossH(sf::Vector2f(winW, 1));
                crossH.setFillColor(OUTLINE_COLOR);
                crossH.setPosition(wx, wy + winH / 2);
                window.draw(crossH);
                sf::RectangleShape crossV(sf::Vector2f(1, winH));
                crossV.setFillColor(OUTLINE_COLOR);
                crossV.setPosition(wx + winW / 2, wy);
                window.draw(crossV);
            }
        }
    }

    // Corner dots (simulate rounded corners)
    float cd = 3.0f;
    sf::CircleShape corner(cd);

    // Top-left
    corner.setFillColor(OUTLINE_COLOR);
    corner.setPosition(screenX - cd, roofY - cd);
    corner.setOutlineColor(OUTLINE_COLOR);
    corner.setOutlineThickness(2.0f);
    window.draw(corner);

    // Top-right
    corner.setPosition(screenX + b.width - cd, roofY - cd);
    window.draw(corner);

    // Bottom-left
    corner.setPosition(screenX - cd, Constants::GROUND_Y - cd);
    window.draw(corner);

    // Bottom-right
    corner.setPosition(screenX + b.width - cd, Constants::GROUND_Y - cd);
    window.draw(corner);
}

void RooftopDayLevel::spawnObstacles(GameState& state, AssetManager& assets) {
    float startX = Constants::WINDOW_WIDTH + 100.0f + static_cast<float>(std::rand() % 200);
    int roll = std::rand() % 100;

    if (roll < 25) {
        auto chimney = ObstacleFactory::createChimney(assets, startX);
        state.entities.push_back(std::move(chimney));
    } else if (roll < 40) {
        auto acUnit = ObstacleFactory::createACUnit(assets, startX);
        state.entities.push_back(std::move(acUnit));
    } else if (roll < 50) {
        auto dish = ObstacleFactory::createSatelliteDish(assets, startX);
        state.entities.push_back(std::move(dish));
    } else if (roll < 60) {
        auto clothesline = ObstacleFactory::createClothesline(assets, startX);
        state.entities.push_back(std::move(clothesline));
    } else if (roll < 70) {
        auto sign = ObstacleFactory::createSign(assets, startX);
        state.entities.push_back(std::move(sign));
    } else if (roll < 80) {
        auto pigeon = ObstacleFactory::createPigeon(assets, startX);
        state.entities.push_back(std::move(pigeon));
    } else if (roll < 90) {
        // Coin cluster
        for (int i = 0; i < 3; i++) {
            auto coin = ObstacleFactory::createCoin(assets, startX + i * 40,
                Constants::GROUND_Y - 60 - (std::rand() % 40));
            state.entities.push_back(std::move(coin));
        }
    } else {
        // Powerup
        auto pu = ObstacleFactory::createPowerUp(assets,
            static_cast<Constants::PowerUpType>(std::rand() % 3),
            startX + 250, Constants::GROUND_Y - 80);
        state.entities.push_back(std::move(pu));
    }
}
