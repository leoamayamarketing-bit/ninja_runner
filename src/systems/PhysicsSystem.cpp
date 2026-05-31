#include "../../include/systems/PhysicsSystem.h"
#include "../../include/components/TransformComponent.h"
#include "../../include/components/PhysicsComponent.h"
#include "../../include/components/PlayerComponent.h"

void PhysicsSystem::update(float deltaTime, std::vector<Entity*>& entities) {
    for (auto* entity : entities) {
        if (!entity || !entity->isActive()) continue;
        auto* trans = entity->getComponent<TransformComponent>();
        auto* phys = entity->getComponent<PhysicsComponent>();
        if (!trans || !phys) continue;

        // Apply gravity
        if (phys->usesGravity) {
            auto* player = entity->getComponent<PlayerComponent>();
            // Variable gravity for better jump feel
            float gravMult = 1.0f;
            if (player) {
                if (phys->velocity.y < 0) gravMult = Constants::GRAVITY_UP_FACTOR;
                else gravMult = Constants::GRAVITY_DOWN_FACTOR;
            }
            phys->velocity.y += gravity_ * deltaTime * gravMult;
        }

        // Apply acceleration
        phys->velocity += phys->acceleration * deltaTime;

        // Update position
        trans->position += phys->velocity * deltaTime;

        // Ground collision
        if (trans->position.y >= groundY_) {
            trans->position.y = groundY_;
            phys->velocity.y = 0.0f;
            if (!phys->isGrounded) {
                phys->isGrounded = true;
                phys->jumpsAvailable = 2; // Reset double jump
            }
        } else {
            phys->isGrounded = false;
        }

        // Clamp to screen bounds
        if (trans->position.x < -200.0f) trans->position.x = -200.0f;
    }
}
