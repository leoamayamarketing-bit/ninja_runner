#ifndef CORE_GAME_H
#define CORE_GAME_H

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>
#include <vector>

#include "GameState.h"
#include "InputManager.h"
#include "AssetManager.h"
#include "AudioManager.h"
#include "FontRenderer.h"
#include "KeyBindings.h"
#include "Constants.h"
#include "../ecs/Entity.h"
#include "../systems/RenderSystem.h"
#include "../systems/PhysicsSystem.h"
#include "../systems/CollisionSystem.h"
#include "../systems/AnimationSystem.h"
#include "../systems/PlayerControlSystem.h"
#include "../systems/ParticleSystem.h"
#include "../levels/LevelManager.h"
#include "../ui/HUD.h"
#include "../ui/Menu.h"
#include "../ui/GameOverScreen.h"

class Game {
public:
    Game();
    ~Game();
    void run();

private:
    sf::RenderWindow window_;
    sf::Clock clock_;
    FontRenderer fontRenderer_;

    GameState gameState_;
    InputManager inputManager_;
    AssetManager assetManager_;
    AudioManager audioManager_;
    KeyBindings keyBindings_;

    std::vector<std::unique_ptr<System>> systems_;
    RenderSystem* renderSystem_ = nullptr;
    PhysicsSystem* physicsSystem_ = nullptr;
    CollisionSystem* collisionSystem_ = nullptr;
    AnimationSystem* animationSystem_ = nullptr;
    PlayerControlSystem* playerControlSystem_ = nullptr;
    ParticleSystem particleSystem_;

    LevelManager levelManager_;
    Menu menu_;
    HUD hud_;
    GameOverScreen gameOverScreen_;

    float dustTrailTimer_ = 0.0f;

    void init();
    void initSystems();
    void initCollisionRules();
    void handleEvents();
    void update(float deltaTime);
    void render();

    void startGame();
    void gameOver();
    void restartGame();

    void spawnInitialEntities();
    void updateSpeed(float deltaTime);
    void updatePowerUps(float deltaTime);
    void updateScreenShake(float deltaTime);
    void applyScreenShake(sf::View& view);
    void drawPowerUpHalo();
    void checkGameOver();
};

#endif
