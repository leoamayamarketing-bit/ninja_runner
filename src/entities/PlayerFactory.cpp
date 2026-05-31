#include "../../include/entities/PlayerFactory.h"
#include "../../include/components/TransformComponent.h"
#include "../../include/components/PhysicsComponent.h"
#include "../../include/components/CollisionComponent.h"
#include "../../include/components/SpriteComponent.h"
#include "../../include/components/AnimationComponent.h"
#include "../../include/components/PlayerComponent.h"
#include "../../include/components/HealthComponent.h"
#include <cstdio>

static std::string getTypeSuffix(Constants::NinjaType type) {
    switch (type) {
        case Constants::NinjaType::SHADOW: return "_shadow";
        case Constants::NinjaType::BLAZE:  return "_blaze";
        case Constants::NinjaType::STORM:  return "_storm";
    }
    return "_shadow";
}

std::unique_ptr<Entity> PlayerFactory::createPlayer(Constants::NinjaType type, AssetManager& assets) {
    auto player = std::make_unique<Entity>(1);

    player->addComponent<TransformComponent>(100.0f, Constants::GROUND_Y);
    player->addComponent<PhysicsComponent>();
    player->addComponent<HealthComponent>(3);

    std::string suffix = getTypeSuffix(type);
    std::string run0 = "ninja_run_0" + suffix;

    // Sprite
    auto& sprite = player->addComponent<SpriteComponent>(run0);
    if (assets.hasTexture(run0)) {
        sprite.setTexture(assets.getTexture(run0));
    }
    sprite.zOrder = 10;
    sprite.origin = sf::Vector2f(25.0f, 30.0f);
    sprite.applyOrigin();

    // Animation - use separate textures for each frame
    auto& anim = player->addComponent<AnimationComponent>();
    anim.useSeparateTextures = true;

    // Run animation (8 frames)
    std::vector<AnimationFrame> runFrames;
    for (int i = 0; i < 8; i++) {
        char name[32];
        std::snprintf(name, sizeof(name), "ninja_run_%d%s", i, suffix.c_str());
        runFrames.emplace_back(sf::IntRect(0, 0, 50, 60), 0.07f, name);
    }
    anim.addAnimation("run", runFrames);

    // Jump animation
    std::vector<AnimationFrame> jumpFrames;
    std::string jumpName = "ninja_jump" + suffix;
    jumpFrames.emplace_back(sf::IntRect(0, 0, 50, 60), 0.15f, jumpName);
    anim.addAnimation("jump", jumpFrames);

    // Slide animation
    std::vector<AnimationFrame> slideFrames;
    std::string slideName = "ninja_slide" + suffix;
    slideFrames.emplace_back(sf::IntRect(0, 0, 50, 40), 0.1f, slideName);
    anim.addAnimation("slide", slideFrames);

    anim.play("run");

    // Collision
    auto& coll = player->addComponent<CollisionComponent>(
        sf::FloatRect(0, 0, 40, 55), "player");
    coll.localOffset = sf::Vector2f(-20.0f, -55.0f);
    coll.isTrigger = false;
    coll.isStatic = false;

    player->addComponent<PlayerComponent>(type);

    return player;
}
