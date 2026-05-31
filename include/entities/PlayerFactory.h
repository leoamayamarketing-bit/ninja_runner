#ifndef ENTITIES_PLAYERFACTORY_H
#define ENTITIES_PLAYERFACTORY_H

#include "../ecs/Entity.h"
#include "../core/Constants.h"
#include "../core/AssetManager.h"
#include <memory>

class PlayerFactory {
public:
    static std::unique_ptr<Entity> createPlayer(Constants::NinjaType type, AssetManager& assets);
};

#endif
