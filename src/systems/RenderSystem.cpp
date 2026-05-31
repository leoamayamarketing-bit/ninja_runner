#include "../../include/systems/RenderSystem.h"
#include "../../include/components/TransformComponent.h"
#include "../../include/components/SpriteComponent.h"
#include "../../include/components/AnimationComponent.h"
#include "../../include/core/AssetManager.h"
#include <algorithm>

RenderSystem::RenderSystem(sf::RenderWindow& window)
    : window_(window), view_(window.getDefaultView()) {}

void RenderSystem::update(float deltaTime, std::vector<Entity*>& entities) {
    // Sort by zOrder
    std::sort(entities.begin(), entities.end(), [](Entity* a, Entity* b) {
        if (!a || !b) return false;
        auto* sa = a->getComponent<SpriteComponent>();
        auto* sb = b->getComponent<SpriteComponent>();
        int za = sa ? sa->zOrder : 0;
        int zb = sb ? sb->zOrder : 0;
        return za < zb;
    });

    for (auto* entity : entities) {
        if (!entity || !entity->isActive()) continue;
        auto* sprite = entity->getComponent<SpriteComponent>();
        auto* trans = entity->getComponent<TransformComponent>();
        auto* anim = entity->getComponent<AnimationComponent>();

        if (!sprite || !sprite->visible || !trans) continue;

        // Update sprite texture from animation if needed
        if (anim && sprite->texture) {
            sf::IntRect frameRect = anim->getCurrentFrame();
            if (frameRect.width > 0) {
                sprite->sprite.setTextureRect(frameRect);
            }
            // Flip sprite based on animation
            if (anim->flipped) {
                sprite->sprite.setScale(-std::abs(trans->scale.x), trans->scale.y);
            } else {
                sprite->sprite.setScale(trans->scale.x, trans->scale.y);
            }
        }

        sprite->sprite.setPosition(trans->position);
        sprite->sprite.setRotation(trans->rotation);
        window_.draw(sprite->sprite);
    }
}

void RenderSystem::setView(const sf::View& view) {
    view_ = view;
    window_.setView(view_);
}

void RenderSystem::setAssetManager(AssetManager* mgr) {
    assetManager_ = mgr;
}
