#ifndef COMPONENTS_COLLISION_H
#define COMPONENTS_COLLISION_H

#include "../ecs/Entity.h"
#include <SFML/Graphics/Rect.hpp>
#include <string>

struct CollisionComponent : public Component {
    sf::FloatRect bounds;
    bool isTrigger = false;
    bool isStatic = false;
    std::string tag;

    int damageAmount = 0;
    int collisionCallbackId = -1;
    sf::Vector2f localOffset{0.0f, 0.0f};

    explicit CollisionComponent(const sf::FloatRect& rect = {}, const std::string& t = "")
        : bounds(rect), tag(t) {}

    void setBounds(const sf::FloatRect& rect) { bounds = rect; }
    void updatePosition(float x, float y) {
        bounds.left = x + localOffset.x;
        bounds.top = y + localOffset.y;
    }
};

#endif
