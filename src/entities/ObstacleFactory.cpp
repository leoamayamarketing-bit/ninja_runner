#include "../../include/entities/ObstacleFactory.h"
#include "../../include/components/TransformComponent.h"
#include "../../include/components/SpriteComponent.h"
#include "../../include/components/AnimationComponent.h"
#include "../../include/components/CollisionComponent.h"
#include <cstdlib>

static uint32_t obstacleIdCounter_ = 100;

std::unique_ptr<Entity> ObstacleFactory::createChimney(AssetManager& assets, float x) {
    auto entity = std::make_unique<Entity>(obstacleIdCounter_++);
    entity->addComponent<TransformComponent>(x, Constants::GROUND_Y);
    auto& sprite = entity->addComponent<SpriteComponent>("chimney");
    if (assets.hasTexture("chimney")) {
        sprite.setTexture(assets.getTexture("chimney"));
    }
    sprite.zOrder = 5;
    sprite.origin = sf::Vector2f(15.0f, 50.0f);
    sprite.applyOrigin();
    auto& coll = entity->addComponent<CollisionComponent>(
        sf::FloatRect(0, 0, 30, 50), "obstacle");
    coll.localOffset = sf::Vector2f(-15.0f, -50.0f);
    coll.isStatic = true;
    return entity;
}

std::unique_ptr<Entity> ObstacleFactory::createACUnit(AssetManager& assets, float x) {
    auto entity = std::make_unique<Entity>(obstacleIdCounter_++);
    entity->addComponent<TransformComponent>(x, Constants::GROUND_Y);
    auto& sprite = entity->addComponent<SpriteComponent>("ac_unit");
    if (assets.hasTexture("ac_unit")) {
        sprite.setTexture(assets.getTexture("ac_unit"));
    }
    sprite.zOrder = 5;
    sprite.origin = sf::Vector2f(20.0f, 25.0f);
    sprite.applyOrigin();
    auto& coll = entity->addComponent<CollisionComponent>(
        sf::FloatRect(0, 0, 40, 25), "obstacle");
    coll.localOffset = sf::Vector2f(-20.0f, -25.0f);
    coll.isStatic = true;
    return entity;
}

std::unique_ptr<Entity> ObstacleFactory::createSatelliteDish(AssetManager& assets, float x) {
    auto entity = std::make_unique<Entity>(obstacleIdCounter_++);
    entity->addComponent<TransformComponent>(x, Constants::GROUND_Y);
    auto& sprite = entity->addComponent<SpriteComponent>("satellite");
    if (assets.hasTexture("satellite")) {
        sprite.setTexture(assets.getTexture("satellite"));
    }
    sprite.zOrder = 5;
    sprite.origin = sf::Vector2f(12.0f, 35.0f);
    sprite.applyOrigin();
    auto& coll = entity->addComponent<CollisionComponent>(
        sf::FloatRect(0, 0, 24, 35), "obstacle");
    coll.localOffset = sf::Vector2f(-12.0f, -35.0f);
    coll.isStatic = true;
    return entity;
}

std::unique_ptr<Entity> ObstacleFactory::createClothesline(AssetManager& assets, float x) {
    auto entity = std::make_unique<Entity>(obstacleIdCounter_++);
    entity->addComponent<TransformComponent>(x, Constants::GROUND_Y - 100);
    auto& sprite = entity->addComponent<SpriteComponent>("clothesline");
    if (assets.hasTexture("clothesline")) {
        sprite.setTexture(assets.getTexture("clothesline"));
    }
    sprite.zOrder = 3;
    sprite.origin = sf::Vector2f(30.0f, 5.0f);
    sprite.applyOrigin();
    // Slide-able obstacle (low profile - must slide under it)
    auto& coll = entity->addComponent<CollisionComponent>(
        sf::FloatRect(0, 0, 60, 10), "obstacle");
    coll.localOffset = sf::Vector2f(-30.0f, -5.0f);
    coll.isStatic = true;
    return entity;
}

std::unique_ptr<Entity> ObstacleFactory::createSign(AssetManager& assets, float x) {
    auto entity = std::make_unique<Entity>(obstacleIdCounter_++);
    entity->addComponent<TransformComponent>(x, Constants::GROUND_Y - 110);
    auto& sprite = entity->addComponent<SpriteComponent>("sign");
    if (assets.hasTexture("sign")) {
        sprite.setTexture(assets.getTexture("sign"));
    }
    sprite.zOrder = 3;
    sprite.origin = sf::Vector2f(15.0f, 30.0f);
    sprite.applyOrigin();
    auto& coll = entity->addComponent<CollisionComponent>(
        sf::FloatRect(0, 0, 30, 30), "obstacle");
    coll.localOffset = sf::Vector2f(-15.0f, -30.0f);
    coll.isStatic = true;
    return entity;
}

std::unique_ptr<Entity> ObstacleFactory::createPigeon(AssetManager& assets, float x) {
    auto entity = std::make_unique<Entity>(obstacleIdCounter_++);
    float y = Constants::GROUND_Y - 60 - (std::rand() % 50);
    entity->addComponent<TransformComponent>(x, y);
    auto& sprite = entity->addComponent<SpriteComponent>("pigeon_0");
    if (assets.hasTexture("pigeon_0")) {
        sprite.setTexture(assets.getTexture("pigeon_0"));
    }
    sprite.zOrder = 4;
    sprite.origin = sf::Vector2f(15.0f, 10.0f);
    sprite.applyOrigin();

    // 2-frame wing flap animation
    auto& anim = entity->addComponent<AnimationComponent>();
    anim.useSeparateTextures = true;
    std::vector<AnimationFrame> flyFrames;
    flyFrames.emplace_back(sf::IntRect(0, 0, 30, 20), 0.15f, "pigeon_0");
    flyFrames.emplace_back(sf::IntRect(0, 0, 30, 20), 0.15f, "pigeon_1");
    anim.addAnimation("fly", flyFrames);
    anim.play("fly");

    auto& coll = entity->addComponent<CollisionComponent>(
        sf::FloatRect(0, 0, 30, 20), "obstacle");
    coll.localOffset = sf::Vector2f(-15.0f, -10.0f);
    coll.isStatic = true;
    return entity;
}

std::unique_ptr<Entity> ObstacleFactory::createCoin(AssetManager& assets, float x, float y) {
    auto entity = std::make_unique<Entity>(obstacleIdCounter_++);
    entity->addComponent<TransformComponent>(x, y);
    auto& sprite = entity->addComponent<SpriteComponent>("shuriken");
    if (assets.hasTexture("shuriken")) {
        sprite.setTexture(assets.getTexture("shuriken"));
    }
    sprite.zOrder = 6;
    sprite.origin = sf::Vector2f(12.0f, 12.0f);
    sprite.applyOrigin();
    auto& coll = entity->addComponent<CollisionComponent>(
        sf::FloatRect(0, 0, 24, 24), "coin");
    coll.localOffset = sf::Vector2f(-12.0f, -12.0f);
    coll.isTrigger = true;
    coll.isStatic = true;
    return entity;
}

std::unique_ptr<Entity> ObstacleFactory::createPowerUp(AssetManager& assets, Constants::PowerUpType type, float x, float y) {
    auto entity = std::make_unique<Entity>(obstacleIdCounter_++);
    entity->addComponent<TransformComponent>(x, y);
    auto& sprite = entity->addComponent<SpriteComponent>("shuriken");
    if (assets.hasTexture("shuriken")) {
        sprite.setTexture(assets.getTexture("shuriken"));
    }
    sprite.zOrder = 6;
    sprite.setColor(sf::Color(255, 255, 100));
    sprite.origin = sf::Vector2f(12.0f, 12.0f);
    sprite.applyOrigin();
    auto& coll = entity->addComponent<CollisionComponent>(
        sf::FloatRect(0, 0, 24, 24), "powerup");
    coll.localOffset = sf::Vector2f(-12.0f, -12.0f);
    coll.isTrigger = true;
    coll.isStatic = true;
    return entity;
}
