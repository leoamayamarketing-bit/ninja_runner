#include "../../include/systems/ParallaxSystem.h"
#include <SFML/Graphics/RenderWindow.hpp>

void ParallaxSystem::addLayer(sf::Texture& texture, float speedFactor, float yOffset) {
    ParallaxLayer layer;
    layer.sprite.setTexture(texture);
    layer.sprite.setTextureRect(sf::IntRect(0, 0, texture.getSize().x, texture.getSize().y));
    layer.sprite2 = layer.sprite;
    layer.sprite2.setPosition(texture.getSize().x, 0);
    layer.speedFactor = speedFactor;
    layer.yOffset = yOffset;
    layers_.push_back(layer);
}

void ParallaxSystem::update(float deltaTime, float scrollSpeed) {
    float scrollAmount = scrollSpeed * deltaTime * scrollDirection_;
    for (auto& layer : layers_) {
        layer.sprite.move(-scrollAmount * layer.speedFactor, 0);
        layer.sprite2.move(-scrollAmount * layer.speedFactor, 0);

        float texW = layer.sprite.getTexture()->getSize().x;
        if (layer.sprite.getPosition().x + texW <= 0) {
            layer.sprite.setPosition(layer.sprite2.getPosition().x + texW, layer.yOffset);
        }
        if (layer.sprite2.getPosition().x + texW <= 0) {
            layer.sprite2.setPosition(layer.sprite.getPosition().x + texW, layer.yOffset);
        }
    }
}

void ParallaxSystem::render(sf::RenderWindow& window) {
    for (auto& layer : layers_) {
        layer.sprite.setPosition(layer.sprite.getPosition().x, layer.yOffset);
        layer.sprite2.setPosition(layer.sprite2.getPosition().x, layer.yOffset);
        window.draw(layer.sprite);
        window.draw(layer.sprite2);
    }
}
