#include "../../include/systems/AnimationSystem.h"
#include "../../include/components/AnimationComponent.h"
#include "../../include/components/SpriteComponent.h"
#include "../../include/core/AssetManager.h"

AnimationSystem::AnimationSystem(AssetManager* assets)
    : assetManager_(assets) {}

void AnimationSystem::update(float deltaTime, std::vector<Entity*>& entities) {
    for (auto* entity : entities) {
        if (!entity || !entity->isActive()) continue;
        auto* anim = entity->getComponent<AnimationComponent>();
        auto* sprite = entity->getComponent<SpriteComponent>();
        if (!anim || !sprite) continue;

        anim->update(deltaTime);

        // Update sprite texture for per-frame textures
        if (anim->useSeparateTextures && assetManager_) {
            std::string texName = anim->getCurrentTextureName();
            if (!texName.empty() && assetManager_->hasTexture(texName)) {
                sprite->setTexture(assetManager_->getTexture(texName));
            }
        }
    }
}
