#ifndef LEVELS_ROOFTOPDAY_H
#define LEVELS_ROOFTOPDAY_H

#include "Level.h"
#include "../systems/ParallaxSystem.h"
#include "../systems/ParticleSystem.h"

struct Building {
    float x;
    float width;
    float height;
    sf::Color bodyColor;
    sf::Color accentColor;
    sf::Color windowColor;
    int windowCount;
    int windowRows;
};

struct DecorativeItem {
    float x, y;
    std::string textureName;
    float scale = 0.3f;
};

class RooftopDayLevel : public Level {
public:
    void init(AssetManager& assets, GameState& state) override;
    void update(float deltaTime, AssetManager& assets, GameState& state) override;
    void render(sf::RenderWindow& window) override;
    void spawnObstacles(GameState& state, AssetManager& assets) override;
    void renderDecorative(sf::RenderWindow& window, AssetManager& assets);
    Constants::LevelType getType() const override { return Constants::LevelType::ROOFTOP_DAY; }

private:
    ParallaxSystem skyParallax_;
    ParallaxSystem buildingParallax_;
    std::vector<Building> buildings_;
    std::vector<DecorativeItem> decoratives_;
    float scrollOffset_ = 0.0f;
    float nextBuildingX_ = 0.0f;
    float buildingTimer_ = 0.0f;
    AssetManager* assets_ = nullptr;

    sf::Color skyColor_{100, 170, 220};
    sf::Color buildingColor_{70, 75, 85};

    ParticleSystem dustParticles_;
    float dustTimer_ = 0.0f;

    void generateBuildings(int count);
    void scrollBuildings(float speed, float deltaTime, GameState& state);
    void drawBuilding(sf::RenderWindow& window, const Building& b, float screenX);
};

#endif
