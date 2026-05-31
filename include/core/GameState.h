#ifndef CORE_GAMESTATE_H
#define CORE_GAMESTATE_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include "Constants.h"
#include "../ecs/Entity.h"

struct GameState {
    Constants::GameStateType currentState = Constants::GameStateType::MENU;
    Constants::LevelType currentLevel = Constants::LevelType::ROOFTOP_DAY;
    Constants::NinjaType selectedNinja = Constants::NinjaType::SHADOW;

    int lives = Constants::INITIAL_LIVES;
    float score = 0.0f;
    float distance = 0.0f;
    float highScore = 0.0f;
    int coins = 0;

    float currentSpeed = Constants::BASE_SPEED;
    float speedTimer = 0.0f;

    bool shieldActive = false;
    bool magnetActive = false;
    bool doublePointsActive = false;
    bool speedBoostActive = false;
    float shieldTimer = 0.0f;
    float magnetTimer = 0.0f;
    float doublePointsTimer = 0.0f;
    float speedBoostTimer = 0.0f;

    std::vector<std::unique_ptr<Entity>> entities;
    Entity* player = nullptr;

    bool ninjaShadowUnlocked = true;
    bool ninjaBlazeUnlocked = false;
    bool ninjaStormUnlocked = false;

    float levelProgress = 0.0f;
    float levelLength = 5000.0f;

    float screenShakeIntensity = 0.0f;
    float screenShakeTimer = 0.0f;

    float gameTime = 0.0f;
    int enemiesDestroyed = 0;
    int powerUpsCollected = 0;
    int totalCoinsCollected = 0;
    bool hardMode = false;

    void reset() {
        lives = Constants::INITIAL_LIVES;
        score = 0.0f;
        distance = 0.0f;
        coins = 0;
        currentSpeed = Constants::BASE_SPEED;
        speedTimer = 0.0f;
        shieldActive = false;
        magnetActive = false;
        doublePointsActive = false;
        speedBoostActive = false;
        shieldTimer = 0.0f;
        magnetTimer = 0.0f;
        doublePointsTimer = 0.0f;
        speedBoostTimer = 0.0f;
        gameTime = 0.0f;
        screenShakeIntensity = 0.0f;
        screenShakeTimer = 0.0f;
        levelProgress = 0.0f;
        enemiesDestroyed = 0;
        powerUpsCollected = 0;
        totalCoinsCollected = 0;
        hardMode = false;
        entities.clear();
        player = nullptr;
    }
};

#endif
