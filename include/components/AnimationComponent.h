#ifndef COMPONENTS_ANIMATION_H
#define COMPONENTS_ANIMATION_H

#include "../ecs/Entity.h"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>

struct AnimationFrame {
    sf::IntRect rect;
    float duration = 0.1f;
    std::string textureName;

    AnimationFrame() = default;
    AnimationFrame(const sf::IntRect& r, float d, const std::string& texName = "")
        : rect(r), duration(d), textureName(texName) {}
};

struct AnimationComponent : public Component {
    std::unordered_map<std::string, std::vector<AnimationFrame>> animations;
    std::vector<AnimationFrame>* currentFrames = nullptr;
    int currentFrame = 0;
    float timer = 0.0f;
    bool playing = true;
    bool looping = true;
    float speed = 1.0f;
    std::string currentAnimation;
    bool flipped = false;
    bool useSeparateTextures = false;

    void addAnimation(const std::string& name, const std::vector<AnimationFrame>& frames) {
        animations[name] = frames;
        if (!currentFrames) play(name);
    }

    void play(const std::string& name) {
        if (currentAnimation == name && currentFrames) return;
        auto it = animations.find(name);
        if (it != animations.end()) {
            currentAnimation = name;
            currentFrames = &it->second;
            currentFrame = 0;
            timer = 0.0f;
            playing = true;
        }
    }

    bool hasAnimation(const std::string& name) const {
        return animations.find(name) != animations.end();
    }

    void update(float dt) {
        if (!playing || !currentFrames || currentFrames->empty()) return;
        timer += dt * speed;
        if (timer >= (*currentFrames)[currentFrame].duration) {
            timer = 0.0f;
            currentFrame++;
            if (currentFrame >= static_cast<int>(currentFrames->size())) {
                if (looping) currentFrame = 0;
                else {
                    currentFrame = static_cast<int>(currentFrames->size()) - 1;
                    playing = false;
                }
            }
        }
    }

    sf::IntRect getCurrentFrame() const {
        if (!currentFrames || currentFrames->empty()) return {};
        if (currentFrame >= static_cast<int>(currentFrames->size())) return {};
        return (*currentFrames)[currentFrame].rect;
    }

    std::string getCurrentTextureName() const {
        if (!currentFrames || currentFrames->empty()) return "";
        if (currentFrame >= static_cast<int>(currentFrames->size())) return "";
        return (*currentFrames)[currentFrame].textureName;
    }

    void reset() {
        currentFrame = 0;
        timer = 0.0f;
        playing = true;
    }
};

#endif
