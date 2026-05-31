#include "../../include/systems/PlayerControlSystem.h"
#include "../../include/components/TransformComponent.h"
#include "../../include/components/PhysicsComponent.h"
#include "../../include/components/PlayerComponent.h"
#include "../../include/components/AnimationComponent.h"
#include "../../include/components/CollisionComponent.h"
#include "../../include/core/Constants.h"
#include "../../include/core/AudioManager.h"
#include "../../include/core/AssetManager.h"

PlayerControlSystem::PlayerControlSystem(InputManager& input, GameState& state,
                                         AudioManager* audio)
    : input_(input), state_(state), audio_(audio) {}

void PlayerControlSystem::update(float deltaTime, std::vector<Entity*>& entities) {
    for (auto* entity : entities) {
        if (!entity || !entity->isActive()) continue;
        auto* player = entity->getComponent<PlayerComponent>();
        auto* phys = entity->getComponent<PhysicsComponent>();
        auto* trans = entity->getComponent<TransformComponent>();
        auto* anim = entity->getComponent<AnimationComponent>();
        auto* coll = entity->getComponent<CollisionComponent>();

        if (!player || !phys || !trans || !anim) continue;

        // Coyote time
        if (phys->isGrounded) {
            coyoteTimer_ = Constants::COYOTE_TIME;
        } else {
            coyoteTimer_ -= deltaTime;
        }

        // Jump buffer
        bool jumpPressed = input_.isJumpPressed();
        if (jumpPressed && !wasJumpPressed_) {
            jumpBufferTimer_ = Constants::JUMP_BUFFER_TIME;
        }
        wasJumpPressed_ = jumpPressed;
        jumpBufferTimer_ -= deltaTime;

        // Jump
        if (jumpBufferTimer_ > 0.0f && phys->jumpsAvailable > 0) {
            float jumpVel = (phys->jumpsAvailable == 2) ? player->jumpVelocity : player->doubleJumpVelocity;
            phys->velocity.y = jumpVel;
            phys->jumpsAvailable--;
            jumpBufferTimer_ = 0.0f;
            coyoteTimer_ = 0.0f;
            phys->isGrounded = false;

            if (audio_) {
                if (phys->jumpsAvailable == 0) audio_->playDoubleJump();
                else audio_->playJump();
            }
        }

        // Slide (crouch)
        bool slideHeld = input_.isSlidePressed();
        if (slideHeld && phys->isGrounded && !phys->isSliding) {
            phys->isSliding = true;
            phys->velocity.y = 200.0f; // Quick drop into slide
            if (audio_) audio_->playSlide();

            // Adjust collision box for slide
            if (coll) {
                coll->bounds.height = 30.0f;
                coll->localOffset.y = -30.0f;
            }
        } else if (!slideHeld && phys->isSliding) {
            phys->isSliding = false;
            // Restore collision box
            if (coll) {
                coll->bounds.height = 55.0f;
                coll->localOffset.y = -55.0f;
            }
        }

        // Update animation based on state
        if (phys->isSliding) {
            anim->play("slide");
            anim->speed = 1.0f;
        } else if (!phys->isGrounded) {
            anim->play("jump");
            anim->speed = 1.0f;
        } else {
            anim->play("run");
            // Speed up animation as game speeds up
            anim->speed = std::min(state_.currentSpeed / Constants::BASE_SPEED, 2.0f);
        }

        // Move player with game speed (auto-run)
        // Player stays at fixed X; the world scrolls
        if (phys->isGrounded && !phys->isSliding) {
            trans->position.x = 100.0f;
        }

        // Track grounded state for landing effects
        wasGrounded_ = phys->isGrounded;
    }
}
