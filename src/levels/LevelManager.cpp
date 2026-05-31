#include "../../include/levels/LevelManager.h"

LevelManager::LevelManager() = default;

void LevelManager::init(Constants::LevelType type, AssetManager& assets, GameState& state) {
    currentLevel_ = createLevel(type);
    currentType_ = type;
    currentLevel_->init(assets, state);
}

void LevelManager::update(float deltaTime, AssetManager& assets, GameState& state) {
    if (currentLevel_) {
        currentLevel_->update(deltaTime, assets, state);
    }
}

void LevelManager::render(sf::RenderWindow& window) {
    if (currentLevel_) {
        currentLevel_->render(window);
    }
}

void LevelManager::switchLevel(Constants::LevelType type, AssetManager& assets, GameState& state) {
    currentLevel_ = createLevel(type);
    currentType_ = type;
    currentLevel_->init(assets, state);
}

Constants::LevelType LevelManager::getCurrentLevelType() const {
    return currentType_;
}

std::unique_ptr<Level> LevelManager::createLevel(Constants::LevelType type) {
    switch (type) {
        case Constants::LevelType::ROOFTOP_DAY:
            return std::make_unique<RooftopDayLevel>();
        case Constants::LevelType::ROOFTOP_NIGHT:
            return std::make_unique<RooftopNightLevel>();
        default:
            return std::make_unique<RooftopDayLevel>();
    }
}
