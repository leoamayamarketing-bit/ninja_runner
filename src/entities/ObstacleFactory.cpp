#include "../../include/entities/ObstacleFactory.h"
#include "../../include/components/TransformComponent.h"
#include "../../include/components/SpriteComponent.h"
#include "../../include/components/AnimationComponent.h"
#include "../../include/components/CollisionComponent.h"
#include <cstdlib>
#include <cstdio>

static uint32_t obstacleIdCounter_ = 100;

// Pick a random obstacle PNG texture (0, 1, or 2) and set up scaling
static void applyObstacleSprite(AssetManager& assets, SpriteComponent& sprite, const std::string& baseKey) {
    // Try to use a PNG obstacle texture first
    if (assets.hasTexture("obstacle_0")) {
        int idx = std::rand() % 3;
        char texName[16];
        std::snprintf(texName, sizeof(texName), "obstacle_%d", idx);
        sprite.textureName = texName;
        if (assets.hasTexture(texName)) {
            sprite.setTexture(assets.getTexture(texName));
        }
        // Scale 230x217 PNG to reasonable obstacle size (~70px tall)
        sprite.sprite.setScale(0.3f, 0.3f);
        sf::Vector2u sz = sprite.texture->getSize();
        sprite.origin = sf::Vector2f(sz.x / 2.0f, static_cast<float>(sz.y) * 0.9f);
        sprite.applyOrigin();
    } else if (assets.hasTexture(baseKey)) {
        // Fallback to procedural texture
        sprite.textureName = baseKey;
        sprite.setTexture(assets.getTexture(baseKey));
    }
}

std::unique_ptr<Entity> ObstacleFactory::createChimney(AssetManager& assets, float x) {
    auto entity = std::make_unique<Entity>(obstacleIdCounter_++);
    entity->addComponent<TransformComponent>(x, Constants::GROUND_Y);
    auto& sprite = entity->addComponent<SpriteComponent>("chimney");
    sprite.zOrder = 5;
    applyObstacleSprite(assets, sprite, "chimney");
    auto& coll = entity->addComponent<CollisionComponent>(
        sf::FloatRect(0, 0, 50, 60), "obstacle");
    coll.localOffset = sf::Vector2f(-25.0f, -60.0f);
    coll.isStatic = true;
    return entity;
}

std::unique_ptr<Entity> ObstacleFactory::createACUnit(AssetManager& assets, float x) {
    auto entity = std::make_unique<Entity>(obstacleIdCounter_++);
    entity->addComponent<TransformComponent>(x, Constants::GROUND_Y);
    auto& sprite = entity->addComponent<SpriteComponent>("ac_unit");
    sprite.zOrder = 5;
    applyObstacleSprite(assets, sprite, "ac_unit");
    auto& coll = entity->addComponent<CollisionComponent>(
        sf::FloatRect(0, 0, 50, 40), "obstacle");
    coll.localOffset = sf::Vector2f(-25.0f, -40.0f);
    coll.isStatic = true;
    return entity;
}

std::unique_ptr<Entity> ObstacleFactory::createSatelliteDish(AssetManager& assets, float x) {
    auto entity = std::make_unique<Entity>(obstacleIdCounter_++);
    entity->addComponent<TransformComponent>(x, Constants::GROUND_Y);
    auto& sprite = entity->addComponent<SpriteComponent>("satellite");
    sprite.zOrder = 5;
    applyObstacleSprite(assets, sprite, "satellite");
    auto& coll = entity->addComponent<CollisionComponent>(
        sf::FloatRect(0, 0, 50, 60), "obstacle");
    coll.localOffset = sf::Vector2f(-25.0f, -60.0f);
    coll.isStatic = true;
    return entity;
}

std::unique_ptr<Entity> ObstacleFactory::createClothesline(AssetManager& assets, float x) {
    auto entity = std::make_unique<Entity>(obstacleIdCounter_++);
    entity->addComponent<TransformComponent>(x, Constants::GROUND_Y - 120);
    auto& sprite = entity->addComponent<SpriteComponent>("clothesline");
    sprite.zOrder = 3;
    applyObstacleSprite(assets, sprite, "clothesline");
    // Slide-able obstacle (low profile - must slide under it)
    auto& coll = entity->addComponent<CollisionComponent>(
        sf::FloatRect(0, 0, 70, 30), "obstacle");
    coll.localOffset = sf::Vector2f(-35.0f, -15.0f);
    coll.isStatic = true;
    return entity;
}

std::unique_ptr<Entity> ObstacleFactory::createSign(AssetManager& assets, float x) {
    auto entity = std::make_unique<Entity>(obstacleIdCounter_++);
    entity->addComponent<TransformComponent>(x, Constants::GROUND_Y - 130);
    auto& sprite = entity->addComponent<SpriteComponent>("sign");
    sprite.zOrder = 3;
    applyObstacleSprite(assets, sprite, "sign");
    auto& coll = entity->addComponent<CollisionComponent>(
        sf::FloatRect(0, 0, 50, 50), "obstacle");
    coll.localOffset = sf::Vector2f(-25.0f, -50.0f);
    coll.isStatic = true;
    return entity;
}

std::unique_ptr<Entity> ObstacleFactory::createPigeon(AssetManager& assets, float x) {
    auto entity = std::make_unique<Entity>(obstacleIdCounter_++);
    float y = Constants::GROUND_Y - 80 - (std::rand() % 50);
    entity->addComponent<TransformComponent>(x, y);

    // Use procedural pigeon textures (2-frame wing flap animation)
    std::string p0 = "pigeon_0";
    std::string p1 = "pigeon_1";
    auto& sprite = entity->addComponent<SpriteComponent>(p0);
    sprite.zOrder = 4;
    if (assets.hasTexture(p0)) {
        sprite.setTexture(assets.getTexture(p0));
    }
    sprite.origin = sf::Vector2f(15.0f, 10.0f);
    sprite.applyOrigin();

    // 2-frame wing flap animation using procedural pigeon textures
    auto& anim = entity->addComponent<AnimationComponent>();
    anim.useSeparateTextures = true;
    std::vector<AnimationFrame> flyFrames;
    flyFrames.emplace_back(sf::IntRect(0, 0, 30, 20), 0.15f, p0);
    flyFrames.emplace_back(sf::IntRect(0, 0, 30, 20), 0.15f, p1);
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

    // Use estrella_ninja PNGs with 2-frame animation
    std::string coinTex = assets.hasTexture("coin_0") ? "coin_0" : "shuriken";
    auto& sprite = entity->addComponent<SpriteComponent>(coinTex);
    if (assets.hasTexture(coinTex)) {
        sprite.setTexture(assets.getTexture(coinTex));
    }
    sprite.zOrder = 6;

    if (assets.hasTexture("coin_0")) {
        // Scale ~200px estrella_ninja to ~40px
        float scale = 0.2f;
        sprite.sprite.setScale(scale, scale);
        sf::Vector2u sz = sprite.texture->getSize();
        sprite.origin = sf::Vector2f(sz.x / 2.0f, sz.y / 2.0f);
        sprite.applyOrigin();

        // 2-frame spinning animation
        auto& anim = entity->addComponent<AnimationComponent>();
        anim.useSeparateTextures = true;
        std::vector<AnimationFrame> spinFrames;
        spinFrames.emplace_back(sf::IntRect(0, 0, static_cast<int>(sz.x), static_cast<int>(sz.y)), 0.12f, "coin_0");
        spinFrames.emplace_back(sf::IntRect(0, 0, static_cast<int>(sz.x), static_cast<int>(sz.y)), 0.12f, "coin_1");
        anim.addAnimation("spin", spinFrames);
        anim.play("spin");
    } else {
        sprite.origin = sf::Vector2f(12.0f, 12.0f);
        sprite.applyOrigin();
    }

    auto& coll = entity->addComponent<CollisionComponent>(
        sf::FloatRect(0, 0, 30, 30), "coin");
    coll.localOffset = sf::Vector2f(-15.0f, -15.0f);
    coll.isTrigger = true;
    coll.isStatic = true;
    return entity;
}

std::unique_ptr<Entity> ObstacleFactory::createPowerUp(AssetManager& assets, Constants::PowerUpType type, float x, float y) {
    auto entity = std::make_unique<Entity>(obstacleIdCounter_++);
    entity->addComponent<TransformComponent>(x, y);

    // Use tesoro_00.png treasure for powerups
    std::string puTex = assets.hasTexture("treasure") ? "treasure" : "shuriken";
    auto& sprite = entity->addComponent<SpriteComponent>(puTex);
    if (assets.hasTexture(puTex)) {
        sprite.setTexture(assets.getTexture(puTex));
    }
    sprite.zOrder = 6;

    if (assets.hasTexture("treasure")) {
        float scale = 0.3f;
        sprite.sprite.setScale(scale, scale);
        sprite.setColor(sf::Color(255, 255, 100));
        sf::Vector2u sz = sprite.texture->getSize();
        sprite.origin = sf::Vector2f(sz.x / 2.0f, sz.y / 2.0f);
        sprite.applyOrigin();
    } else {
        sprite.setColor(sf::Color(255, 255, 100));
        sprite.origin = sf::Vector2f(12.0f, 12.0f);
        sprite.applyOrigin();
    }

    auto& coll = entity->addComponent<CollisionComponent>(
        sf::FloatRect(0, 0, 50, 50), "powerup");
    coll.localOffset = sf::Vector2f(-25.0f, -25.0f);
    coll.isTrigger = true;
    coll.isStatic = true;
    return entity;
}
