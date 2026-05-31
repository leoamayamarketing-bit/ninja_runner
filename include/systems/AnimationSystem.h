#ifndef SYSTEMS_ANIMATION_H
#define SYSTEMS_ANIMATION_H

#include "../ecs/Entity.h"

class AssetManager;

class AnimationSystem : public System {
public:
    explicit AnimationSystem(AssetManager* assets = nullptr);
    void update(float deltaTime, std::vector<Entity*>& entities) override;
    void setAssetManager(AssetManager* mgr) { assetManager_ = mgr; }

private:
    AssetManager* assetManager_ = nullptr;
};

#endif
