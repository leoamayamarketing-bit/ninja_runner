#include "../../include/systems/ParticleSystem.h"
#include <cmath>

ParticleSystem::ParticleSystem() {
    particles_.reserve(200);
}

void ParticleSystem::emit(sf::Vector2f position, int count, sf::Color color,
                           float speed, float lifetime, float size) {
    for (int i = 0; i < count; i++) {
        Particle p;
        p.position = position;
        float angle = (std::rand() % 360) * 3.14159f / 180.0f;
        float spd = (std::rand() % 100) / 100.0f * speed;
        p.velocity = sf::Vector2f(std::cos(angle) * spd, std::sin(angle) * spd);
        p.color = color;
        p.lifetime = lifetime;
        p.maxLifetime = lifetime;
        p.size = size;
        particles_.push_back(p);
    }
}

void ParticleSystem::updateParticles(float deltaTime) {
    for (size_t i = 0; i < particles_.size();) {
        auto& p = particles_[i];
        p.lifetime -= deltaTime;
        if (p.lifetime <= 0.0f) {
            particles_.erase(particles_.begin() + i);
            continue;
        }
        p.velocity.y += gravity_ * deltaTime;
        p.position += p.velocity * deltaTime;
        float lifeRatio = p.lifetime / p.maxLifetime;
        p.color.a = static_cast<sf::Uint8>(lifeRatio * 255);
        i++;
    }
}

void ParticleSystem::render(sf::RenderWindow& window) {
    sf::VertexArray vertices(sf::Quads, particles_.size() * 4);
    for (size_t i = 0; i < particles_.size(); i++) {
        auto& p = particles_[i];
        float hs = p.size * 0.5f;
        vertices[i * 4 + 0].position = sf::Vector2f(p.position.x - hs, p.position.y - hs);
        vertices[i * 4 + 1].position = sf::Vector2f(p.position.x + hs, p.position.y - hs);
        vertices[i * 4 + 2].position = sf::Vector2f(p.position.x + hs, p.position.y + hs);
        vertices[i * 4 + 3].position = sf::Vector2f(p.position.x - hs, p.position.y + hs);
        vertices[i * 4 + 0].color = p.color;
        vertices[i * 4 + 1].color = p.color;
        vertices[i * 4 + 2].color = p.color;
        vertices[i * 4 + 3].color = p.color;
    }
    window.draw(vertices);
}
