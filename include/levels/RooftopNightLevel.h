#ifndef LEVELS_ROOFTOPNIGHT_H
#define LEVELS_ROOFTOPNIGHT_H

#include "Level.h"
#include "RooftopDayLevel.h"
#include "../systems/ParallaxSystem.h"
#include "../systems/ParticleSystem.h"
#include <SFML/Graphics/Color.hpp>
#include <string>

struct NeonSign {
    float x, y;
    sf::Color color;
    float phase;
    std::string text;
};

class RooftopNightLevel : public Level {
public:
    void init(AssetManager& assets, GameState& state) override;
    void update(float deltaTime, AssetManager& assets, GameState& state) override;
    void render(sf::RenderWindow& window) override;
    void spawnObstacles(GameState& state, AssetManager& assets) override;
    void renderDecorative(sf::RenderWindow& window, AssetManager& assets);
    Constants::LevelType getType() const override { return Constants::LevelType::ROOFTOP_NIGHT; }

private:
    ParallaxSystem skyParallax_;
    std::vector<Building> buildings_;
    std::vector<NeonSign> neonSigns_;
    std::vector<DecorativeItem> decoratives_;
    float scrollOffset_ = 0.0f;
    float nextBuildingX_ = 0.0f;
    AssetManager* assets_ = nullptr;

    ParticleSystem dustParticles_;
    float dustTimer_ = 0.0f;
    float gameTime_ = 0.0f;

    void generateBuildings(int count);
    void generateNeonSign(int index, float gameTime);
    void drawNeonSign(const NeonSign& sign, float gameTime, sf::RenderWindow& window);
};

#endif
