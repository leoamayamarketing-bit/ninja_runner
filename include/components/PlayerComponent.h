#ifndef COMPONENTS_PLAYER_H
#define COMPONENTS_PLAYER_H

#include "../ecs/Entity.h"
#include "../core/Constants.h"

struct PlayerComponent : public Component {
    Constants::NinjaType ninjaType;
    float jumpVelocity;
    float doubleJumpVelocity;
    float dashMultiplier;
    float maxHealth;
    float slideHeight;

    bool isSliding = false;
    bool hasShield = false;
    bool hasMagnet = false;
    int currentHealth = 3;
    int maxLives = 3;

    explicit PlayerComponent(Constants::NinjaType type = Constants::NinjaType::SHADOW)
        : ninjaType(type) {
        switch (type) {
            case Constants::NinjaType::SHADOW:
                jumpVelocity = Constants::JUMP_VELOCITY;
                doubleJumpVelocity = Constants::DOUBLE_JUMP_VELOCITY;
                dashMultiplier = Constants::DASH_SPEED_MULTIPLIER;
                maxHealth = 3.0f;
                slideHeight = Constants::SLIDE_HEIGHT;
                break;
            case Constants::NinjaType::BLAZE:
                jumpVelocity = -700.0f;
                doubleJumpVelocity = -580.0f;
                dashMultiplier = 2.5f;
                maxHealth = 2.0f;
                slideHeight = 0.35f;
                break;
            case Constants::NinjaType::STORM:
                jumpVelocity = -600.0f;
                doubleJumpVelocity = -500.0f;
                dashMultiplier = 1.5f;
                maxHealth = 5.0f;
                slideHeight = 0.45f;
                break;
        }
    }
};

#endif
