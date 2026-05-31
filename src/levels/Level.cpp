#include "../../include/levels/Level.h"
#include "../../include/components/TransformComponent.h"

void Level::cleanupOffscreen(GameState& state) {
    for (auto& entity : state.entities) {
        if (!entity || !entity->isActive()) continue;
        auto* trans = entity->getComponent<TransformComponent>();
        if (trans && trans->position.x < -300.0f) {
            entity->setActive(false);
        }
    }
}
