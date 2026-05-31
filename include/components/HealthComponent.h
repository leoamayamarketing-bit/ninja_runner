#ifndef COMPONENTS_HEALTH_H
#define COMPONENTS_HEALTH_H

#include "../ecs/Entity.h"

struct HealthComponent : public Component {
    int currentHealth;
    int maxHealth;
    bool isInvulnerable = false;
    float invulnerabilityTimer = 0.0f;
    float invulnerabilityDuration = 1.5f;

    explicit HealthComponent(int maxHP = 3)
        : currentHealth(maxHP), maxHealth(maxHP) {}

    void takeDamage(int amount = 1) {
        if (!isInvulnerable) {
            currentHealth -= amount;
            isInvulnerable = true;
            invulnerabilityTimer = invulnerabilityDuration;
        }
    }

    void update(float dt) {
        if (isInvulnerable) {
            invulnerabilityTimer -= dt;
            if (invulnerabilityTimer <= 0.0f) {
                isInvulnerable = false;
                invulnerabilityTimer = 0.0f;
            }
        }
    }
};

#endif
