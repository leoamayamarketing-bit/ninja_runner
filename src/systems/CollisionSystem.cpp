#include "../../include/systems/CollisionSystem.h"
#include "../../include/components/TransformComponent.h"
#include "../../include/components/CollisionComponent.h"

void CollisionSystem::update(float deltaTime, std::vector<Entity*>& entities) {
    // Update collision box positions
    for (auto* entity : entities) {
        if (!entity || !entity->isActive()) continue;
        auto* trans = entity->getComponent<TransformComponent>();
        auto* coll = entity->getComponent<CollisionComponent>();
        if (trans && coll) {
            coll->updatePosition(trans->position.x, trans->position.y);
        }
    }

    // Check collisions
    for (size_t i = 0; i < entities.size(); i++) {
        if (!entities[i] || !entities[i]->isActive()) continue;
        auto* collA = entities[i]->getComponent<CollisionComponent>();
        if (!collA) continue;

        for (size_t j = i + 1; j < entities.size(); j++) {
            if (!entities[j] || !entities[j]->isActive()) continue;
            auto* collB = entities[j]->getComponent<CollisionComponent>();
            if (!collB) continue;

            if (checkAABB(collA->bounds, collB->bounds)) {
                for (auto& rule : collisionRules_) {
                    if ((rule.tagA == collA->tag && rule.tagB == collB->tag) ||
                        (rule.tagA == collB->tag && rule.tagB == collA->tag)) {
                        rule.callback(entities[i], entities[j]);
                    }
                }
            }
        }
    }
}

void CollisionSystem::onCollision(const std::string& tagA, const std::string& tagB,
                                   CollisionCallback callback) {
    collisionRules_.push_back({tagA, tagB, callback});
}

bool CollisionSystem::checkAABB(const sf::FloatRect& a, const sf::FloatRect& b) const {
    return a.intersects(b);
}
