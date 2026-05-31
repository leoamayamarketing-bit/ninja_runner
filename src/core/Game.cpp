#include "../../include/core/Game.h"
#include "../../include/components/TransformComponent.h"
#include "../../include/components/PhysicsComponent.h"
#include "../../include/components/CollisionComponent.h"
#include "../../include/components/PlayerComponent.h"
#include "../../include/components/HealthComponent.h"
#include "../../include/components/AnimationComponent.h"
#include "../../include/components/SpriteComponent.h"
#include "../../include/entities/PlayerFactory.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <algorithm>

Game::Game()
    : window_(sf::VideoMode(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT),
              Constants::GAME_TITLE,
              sf::Style::Close | sf::Style::Titlebar)    , menu_()
      , hud_()
      , gameOverScreen_()
{
    window_.setFramerateLimit(Constants::FPS_LIMIT);
    window_.setVerticalSyncEnabled(false);
}

Game::~Game() = default;

void Game::run() {
    init();

    while (window_.isOpen()) {
        float deltaTime = clock_.restart().asSeconds();
        if (deltaTime > 0.05f) deltaTime = 0.05f; // Cap delta time

        handleEvents();
        update(deltaTime);
        render();
    }
}

void Game::init() {
    // Initialize audio
    audioManager_.init();

    // Generate all procedural textures
    assetManager_.generateAllAssets();

    // Initialize font renderer (procedural bitmap font)
    fontRenderer_.generate();

    // Initialize UI
    menu_.init(fontRenderer_, &audioManager_);
    hud_.init(fontRenderer_);
    gameOverScreen_.init(fontRenderer_, &audioManager_);
    menu_.setHighScore(gameState_.highScore);

    // Set up keybindings
    keyBindings_.jump = sf::Keyboard::Space;
    keyBindings_.jumpAlt = sf::Keyboard::Up;
    keyBindings_.slide = sf::Keyboard::Down;
    keyBindings_.slideAlt = sf::Keyboard::LShift;
    keyBindings_.dash = sf::Keyboard::X;
    keyBindings_.dashAlt = sf::Keyboard::Z;
    keyBindings_.pause = sf::Keyboard::Escape;
    keyBindings_.pauseAlt = sf::Keyboard::P;
    keyBindings_.menuUp = sf::Keyboard::Up;
    keyBindings_.menuUpAlt = sf::Keyboard::W;
    keyBindings_.menuDown = sf::Keyboard::Down;
    keyBindings_.menuDownAlt = sf::Keyboard::S;
    keyBindings_.menuSelect = sf::Keyboard::Enter;
    keyBindings_.menuSelectAlt = sf::Keyboard::Space;
    keyBindings_.menuBack = sf::Keyboard::Escape;

    inputManager_.setBindings(&keyBindings_);

    // Initialize systems
    initSystems();
    initCollisionRules();

    // Initialize level manager
    levelManager_.init(Constants::LevelType::ROOFTOP_DAY, assetManager_, gameState_);
}

void Game::initSystems() {
    systems_.push_back(std::make_unique<PhysicsSystem>());
    physicsSystem_ = static_cast<PhysicsSystem*>(systems_.back().get());

    systems_.push_back(std::make_unique<CollisionSystem>());
    collisionSystem_ = static_cast<CollisionSystem*>(systems_.back().get());

    systems_.push_back(std::make_unique<AnimationSystem>(&assetManager_));
    animationSystem_ = static_cast<AnimationSystem*>(systems_.back().get());

    systems_.push_back(std::make_unique<PlayerControlSystem>(inputManager_, gameState_, &audioManager_));
    playerControlSystem_ = static_cast<PlayerControlSystem*>(systems_.back().get());

    systems_.push_back(std::make_unique<RenderSystem>(window_));
    renderSystem_ = static_cast<RenderSystem*>(systems_.back().get());
}

void Game::initCollisionRules() {
    // Player vs obstacle
    collisionSystem_->onCollision("player", "obstacle",
        [this](Entity* player, Entity* obstacle) {
            if (!player->isActive() || !obstacle->isActive()) return;

            auto* health = player->getComponent<HealthComponent>();
            if (!health) return;

            // Check if player has shield
            if (gameState_.shieldActive) {
                gameState_.shieldActive = false;
                gameState_.shieldTimer = 0.0f;
                obstacle->setActive(false);
                audioManager_.playPowerUp();
                return;
            }

            // HealthComponent handles invulnerability
            if (health->isInvulnerable) return;

            health->takeDamage(1);
            gameState_.screenShakeIntensity = 8.0f;
            gameState_.screenShakeTimer = 0.3f;
            audioManager_.playDamage();
            obstacle->setActive(false);

            if (health->currentHealth <= 0) {
                gameOver();
            }
        });

    // Player vs coin
    collisionSystem_->onCollision("player", "coin",
        [this](Entity* player, Entity* coin) {
            if (!coin->isActive()) return;
            coin->setActive(false);
            float points = gameState_.doublePointsActive
                ? Constants::COIN_SCORE * 2 : Constants::COIN_SCORE;
            gameState_.score += points;
            gameState_.coins++;
            gameState_.totalCoinsCollected++;
            audioManager_.playCoin();
        });

    // Player vs powerup
    collisionSystem_->onCollision("player", "powerup",
        [this](Entity* player, Entity* powerup) {
            if (!powerup->isActive()) return;
            powerup->setActive(false);

            // Random power-up effect
            int type = std::rand() % 5;
            switch (static_cast<Constants::PowerUpType>(type)) {
                case Constants::PowerUpType::SHIELD:
                    gameState_.shieldActive = true;
                    gameState_.shieldTimer = Constants::POWERUP_DURATION;
                    break;
                case Constants::PowerUpType::MAGNET:
                    gameState_.magnetActive = true;
                    gameState_.magnetTimer = Constants::POWERUP_DURATION;
                    break;
                case Constants::PowerUpType::DOUBLE_POINTS:
                    gameState_.doublePointsActive = true;
                    gameState_.doublePointsTimer = Constants::POWERUP_DURATION;
                    break;
                case Constants::PowerUpType::SPEED_BOOST:
                    gameState_.speedBoostActive = true;
                    gameState_.speedBoostTimer = Constants::POWERUP_DURATION;
                    break;
                case Constants::PowerUpType::EXTRA_LIFE:
                    if (gameState_.lives < Constants::MAX_LIVES) {
                        gameState_.lives++;
                    }
                    break;
            }
            gameState_.powerUpsCollected++;
            audioManager_.playPowerUp();
        });
}

void Game::handleEvents() {
    sf::Event event;
    while (window_.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window_.close();
        } else if (event.type == sf::Event::KeyPressed) {
            inputManager_.onKeyPressed(event.key.code);

            if (gameState_.currentState == Constants::GameStateType::PLAYING) {
                if (inputManager_.isPausePressed()) {
                    gameState_.currentState = Constants::GameStateType::PAUSED;
                }
            } else if (gameState_.currentState == Constants::GameStateType::PAUSED) {
                if (inputManager_.isPausePressed()) {
                    gameState_.currentState = Constants::GameStateType::PLAYING;
                }
            }
        } else if (event.type == sf::Event::KeyReleased) {
            inputManager_.onKeyReleased(event.key.code);
        }
    }
}

void Game::update(float deltaTime) {
    switch (gameState_.currentState) {
        case Constants::GameStateType::MENU: {
            Menu::MenuAction action = menu_.update(inputManager_);
            switch (action) {
                case Menu::START_GAME:
                    startGame();
                    break;
                case Menu::NINJA_SELECT:
                    // Cycle through ninja types
                    switch (gameState_.selectedNinja) {
                        case Constants::NinjaType::SHADOW:
                            gameState_.selectedNinja = Constants::NinjaType::BLAZE;
                            break;
                        case Constants::NinjaType::BLAZE:
                            gameState_.selectedNinja = Constants::NinjaType::STORM;
                            break;
                        case Constants::NinjaType::STORM:
                            gameState_.selectedNinja = Constants::NinjaType::SHADOW;
                            break;
                    }
                    break;
                case Menu::QUIT:
                    window_.close();
                    break;
                default:
                    break;
            }
            break;
        }

        case Constants::GameStateType::PLAYING: {
            gameState_.gameTime += deltaTime;

            // Update speed
            updateSpeed(deltaTime);

            // Update level
            levelManager_.update(deltaTime, assetManager_, gameState_);

            // Get active entities
            std::vector<Entity*> activeEntities;
            for (auto& entity : gameState_.entities) {
                if (entity && entity->isActive()) {
                    activeEntities.push_back(entity.get());
                }
            }
            if (gameState_.player && gameState_.player->isActive()) {
                activeEntities.push_back(gameState_.player);
            }

    // Update health invulnerability timers
    if (gameState_.player) {
        auto* health = gameState_.player->getComponent<HealthComponent>();
        if (health) health->update(deltaTime);
    }

    // Update systems
    for (auto& system : systems_) {
        system->update(deltaTime, activeEntities);
    }

            // Update particle system (game dust effects)
            particleSystem_.updateParticles(deltaTime);

            // Update power-ups
            updatePowerUps(deltaTime);

            // Update screen shake
            updateScreenShake(deltaTime);

            // Update distance
            gameState_.distance += gameState_.currentSpeed * deltaTime * 0.01f;
            gameState_.score += gameState_.currentSpeed * deltaTime * Constants::DISTANCE_SCORE_MULT * 0.01f;

            // Update HUD
            hud_.update(gameState_);

            // Check game over
            checkGameOver();

            // Magnet effect: attract coins to player
            if (gameState_.magnetActive && gameState_.player) {
                auto* playerTrans = gameState_.player->getComponent<TransformComponent>();
                if (playerTrans) {
                    for (auto& entity : gameState_.entities) {
                        if (!entity || !entity->isActive()) continue;
                        auto* coll = entity->getComponent<CollisionComponent>();
                        auto* trans = entity->getComponent<TransformComponent>();
                        if (coll && trans && coll->tag == "coin") {
                            float dx = playerTrans->position.x - trans->position.x;
                            float dy = playerTrans->position.y - trans->position.y;
                            float dist = std::sqrt(dx * dx + dy * dy);
                            if (dist < Constants::MAGNET_RADIUS && dist > 0.0f) {
                                float force = 200.0f / dist;
                                trans->position.x += dx / dist * force;
                                trans->position.y += dy / dist * force;
                            }
                        }
                    }
                }
            }

            break;
        }

        case Constants::GameStateType::PAUSED: {
            // Just wait for unpause
            break;
        }

        case Constants::GameStateType::GAME_OVER: {
            GameOverScreen::Action action = gameOverScreen_.update(inputManager_);
            switch (action) {
                case GameOverScreen::RESTART:
                    restartGame();
                    break;
                case GameOverScreen::MENU:
                    gameState_.currentState = Constants::GameStateType::MENU;
                    gameState_.reset();
                    levelManager_.init(Constants::LevelType::ROOFTOP_DAY, assetManager_, gameState_);
                    menu_.resetSelection();
                    break;
                default:
                    break;
            }
            break;
        }

        default:
            break;
    }

    // Cleanup inactive entities periodically
    auto it = std::remove_if(gameState_.entities.begin(), gameState_.entities.end(),
        [](const std::unique_ptr<Entity>& e) {
            return e && !e->isActive();
        });
    gameState_.entities.erase(it, gameState_.entities.end());

    // Update input state AFTER all input checks this frame
    inputManager_.update();
}

void Game::render() {
    window_.clear(sf::Color(10, 10, 30));

    switch (gameState_.currentState) {
        case Constants::GameStateType::MENU:
            menu_.render(window_);
            break;

        case Constants::GameStateType::PLAYING: {
            // Apply screen shake
            sf::View view = window_.getDefaultView();
            applyScreenShake(view);
            window_.setView(view);

            // Render level background
            levelManager_.render(window_);

            // Render particles
            particleSystem_.render(window_);

            // Render entities
            std::vector<Entity*> activeEntities;
            for (auto& entity : gameState_.entities) {
                if (entity && entity->isActive()) {
                    activeEntities.push_back(entity.get());
                }
            }
            if (gameState_.player && gameState_.player->isActive()) {
                activeEntities.push_back(gameState_.player);
            }
            renderSystem_->update(0, activeEntities);

            // Power-up halo effect (draw in world coords BEFORE view reset)
            if (gameState_.shieldActive || gameState_.magnetActive ||
                gameState_.doublePointsActive || gameState_.speedBoostActive) {
                drawPowerUpHalo();
            }

            // Reset view for UI
            window_.setView(window_.getDefaultView());

            // Render HUD
            hud_.render(window_);
            break;
        }

        case Constants::GameStateType::PAUSED: {
            // Render a dimmed version of the game
            sf::View view = window_.getDefaultView();
            window_.setView(view);
            levelManager_.render(window_);
            std::vector<Entity*> activeEntities;
            for (auto& entity : gameState_.entities) {
                if (entity && entity->isActive()) {
                    activeEntities.push_back(entity.get());
                }
            }
            if (gameState_.player && gameState_.player->isActive()) {
                activeEntities.push_back(gameState_.player);
            }
            renderSystem_->update(0, activeEntities);
            hud_.render(window_);

            // Pause overlay
            sf::RectangleShape overlay(sf::Vector2f(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT));
            overlay.setFillColor(sf::Color(0, 0, 0, 150));
            window_.draw(overlay);

            fontRenderer_.drawTextCentered(window_, "PAUSED", 640, 360, 56, sf::Color::White);
            break;
        }

        case Constants::GameStateType::GAME_OVER:
            levelManager_.render(window_);
            gameOverScreen_.render(window_, gameState_);
            break;

        default:
            break;
    }

    window_.display();
}

void Game::startGame() {
    gameState_.reset();
    gameState_.currentState = Constants::GameStateType::PLAYING;
    menu_.setHighScore(gameState_.highScore);

    // Initialize level
    levelManager_.init(Constants::LevelType::ROOFTOP_DAY, assetManager_, gameState_);

    // Create player
    spawnInitialEntities();

    audioManager_.playJump();
}

void Game::gameOver() {
    gameState_.currentState = Constants::GameStateType::GAME_OVER;

    // Update high score
    if (gameState_.score > gameState_.highScore) {
        gameState_.highScore = gameState_.score;
        menu_.setHighScore(gameState_.highScore);
    }

    audioManager_.playGameOver();
}

void Game::restartGame() {
    gameState_.reset();
    gameState_.currentState = Constants::GameStateType::PLAYING;

    levelManager_.init(Constants::LevelType::ROOFTOP_DAY, assetManager_, gameState_);
    spawnInitialEntities();
}

void Game::spawnInitialEntities() {
    auto player = PlayerFactory::createPlayer(gameState_.selectedNinja, assetManager_);
    gameState_.player = player.get();

    // Set initial position with a small offset above ground (for landing animation)
    auto* trans = player->getComponent<TransformComponent>();
    if (trans) {
        trans->position.y = Constants::GROUND_Y;
    }

    gameState_.entities.push_back(std::move(player));
}

void Game::updateSpeed(float deltaTime) {
    gameState_.speedTimer += deltaTime;

    if (gameState_.speedTimer >= Constants::SPEED_INTERVAL) {
        gameState_.speedTimer = 0.0f;
        float boost = gameState_.speedBoostActive
            ? Constants::SPEED_INCREMENT * 2.0f : Constants::SPEED_INCREMENT;
        gameState_.currentSpeed = std::min(
            gameState_.currentSpeed + boost,
            Constants::MAX_SPEED
        );
    }

    // Speed boost power-up
    if (gameState_.speedBoostActive) {
        gameState_.currentSpeed = std::min(
            gameState_.currentSpeed + Constants::SPEED_INCREMENT * deltaTime * 2.0f,
            Constants::MAX_SPEED
        );
    }
}

void Game::updatePowerUps(float deltaTime) {
    if (gameState_.shieldActive) {
        gameState_.shieldTimer -= deltaTime;
        if (gameState_.shieldTimer <= 0.0f) {
            gameState_.shieldActive = false;
        }
    }
    if (gameState_.magnetActive) {
        gameState_.magnetTimer -= deltaTime;
        if (gameState_.magnetTimer <= 0.0f) {
            gameState_.magnetActive = false;
        }
    }
    if (gameState_.doublePointsActive) {
        gameState_.doublePointsTimer -= deltaTime;
        if (gameState_.doublePointsTimer <= 0.0f) {
            gameState_.doublePointsActive = false;
        }
    }
    if (gameState_.speedBoostActive) {
        gameState_.speedBoostTimer -= deltaTime;
        if (gameState_.speedBoostTimer <= 0.0f) {
            gameState_.speedBoostActive = false;
        }
    }
}

void Game::updateScreenShake(float deltaTime) {
    if (gameState_.screenShakeTimer > 0.0f) {
        gameState_.screenShakeTimer -= deltaTime;
        if (gameState_.screenShakeTimer <= 0.0f) {
            gameState_.screenShakeIntensity = 0.0f;
        }
    }
}

void Game::applyScreenShake(sf::View& view) {
    if (gameState_.screenShakeIntensity > 0.0f) {
        float offsetX = (std::rand() % 200 - 100) / 100.0f * gameState_.screenShakeIntensity;
        float offsetY = (std::rand() % 200 - 100) / 100.0f * gameState_.screenShakeIntensity;
        view.move(offsetX, offsetY);
    }
}

void Game::drawPowerUpHalo() {
    if (!gameState_.player || !gameState_.player->isActive()) return;
    auto* trans = gameState_.player->getComponent<TransformComponent>();
    if (!trans) return;

    float pulse = 0.7f + 0.3f * std::sin(gameState_.gameTime * 5.0f);
    float radius = 50.0f + 10.0f * (1.0f - pulse);

    sf::Color haloColor;
    if (gameState_.shieldActive)
        haloColor = sf::Color(50, 200, 255, static_cast<sf::Uint8>(80 * pulse));
    else if (gameState_.magnetActive)
        haloColor = sf::Color(255, 200, 50, static_cast<sf::Uint8>(80 * pulse));
    else if (gameState_.doublePointsActive)
        haloColor = sf::Color(255, 255, 100, static_cast<sf::Uint8>(80 * pulse));
    else if (gameState_.speedBoostActive)
        haloColor = sf::Color(255, 80, 80, static_cast<sf::Uint8>(80 * pulse));
    else
        return;

    // Outer glow ring
    sf::CircleShape outerHalo(radius);
    outerHalo.setFillColor(sf::Color::Transparent);
    outerHalo.setOutlineThickness(4.0f * pulse);
    outerHalo.setOutlineColor(haloColor);
    outerHalo.setPosition(trans->position.x - radius, trans->position.y - radius);
    window_.draw(outerHalo);

    // Inner glow ring
    sf::CircleShape innerHalo(radius * 0.6f);
    innerHalo.setFillColor(sf::Color::Transparent);
    innerHalo.setOutlineThickness(2.0f * pulse);
    sf::Color innerColor = haloColor;
    innerColor.a = static_cast<sf::Uint8>(120 * pulse);
    innerHalo.setOutlineColor(innerColor);
    innerHalo.setPosition(trans->position.x - radius * 0.6f, trans->position.y - radius * 0.6f);
    window_.draw(innerHalo);

    // Inner fill (subtle glow)
    sf::CircleShape fill(radius * 0.8f);
    sf::Color fillColor = haloColor;
    fillColor.a = static_cast<sf::Uint8>(20 * pulse);
    fill.setFillColor(fillColor);
    fill.setPosition(trans->position.x - radius * 0.8f, trans->position.y - radius * 0.8f);
    window_.draw(fill);
}

void Game::checkGameOver() {
    if (!gameState_.player || !gameState_.player->isActive()) {
        gameOver();
        return;
    }

    auto* health = gameState_.player->getComponent<HealthComponent>();
    if (health && health->currentHealth <= 0) {
        gameOver();
    }

    // Check if player fell off screen
    auto* trans = gameState_.player->getComponent<TransformComponent>();
    if (trans && trans->position.y > Constants::WINDOW_HEIGHT + 100) {
        health->takeDamage(health->currentHealth);
        gameOver();
    }
}
