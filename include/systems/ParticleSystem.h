#ifndef SYSTEMS_PARTICLE_H
#define SYSTEMS_PARTICLE_H

#include "../ecs/Entity.h"
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <vector>

struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color color;
    float lifetime;
    float maxLifetime;
    float size;
};

class ParticleSystem {
public:
    ParticleSystem();
    void emit(sf::Vector2f position, int count, sf::Color color, float speed,
              float lifetime, float size);
    void updateParticles(float deltaTime);
    void render(sf::RenderWindow& window);
    void setGravity(float g) { gravity_ = g; }

private:
    std::vector<Particle> particles_;
    float gravity_ = 100.0f;
};

#endif
