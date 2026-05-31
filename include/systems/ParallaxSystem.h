#ifndef SYSTEMS_PARALLAX_H
#define SYSTEMS_PARALLAX_H

#include "../ecs/Entity.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <vector>

struct ParallaxLayer {
    sf::Sprite sprite;
    sf::Sprite sprite2;
    float speedFactor;
    float yOffset;
};

class ParallaxSystem {
public:
    void addLayer(sf::Texture& texture, float speedFactor, float yOffset = 0.0f);
    void update(float deltaTime, float scrollSpeed);
    void render(sf::RenderWindow& window);
    void setScrollDirection(float dir) { scrollDirection_ = dir; }

private:
    std::vector<ParallaxLayer> layers_;
    float scrollDirection_ = 1.0f;
};

#endif
