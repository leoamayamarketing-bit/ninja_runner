#ifndef ENTITIES_OBSTACLEFACTORY_H
#define ENTITIES_OBSTACLEFACTORY_H

#include "../ecs/Entity.h"
#include "../core/Constants.h"
#include "../core/AssetManager.h"
#include <memory>

class ObstacleFactory {
public:
    static std::unique_ptr<Entity> createChimney(AssetManager& assets, float x);
    static std::unique_ptr<Entity> createACUnit(AssetManager& assets, float x);
    static std::unique_ptr<Entity> createSatelliteDish(AssetManager& assets, float x);
    static std::unique_ptr<Entity> createClothesline(AssetManager& assets, float x);
    static std::unique_ptr<Entity> createSign(AssetManager& assets, float x);
    static std::unique_ptr<Entity> createPigeon(AssetManager& assets, float x);
    static std::unique_ptr<Entity> createCoin(AssetManager& assets, float x, float y);
    static std::unique_ptr<Entity> createPowerUp(AssetManager& assets, Constants::PowerUpType type, float x, float y);
};

#endif
