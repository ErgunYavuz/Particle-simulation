#include "headers/particle.h"
#include <cmath>
#include <algorithm>

namespace prtcl {
    constexpr float MIN_SPEED = 0.0f;
    constexpr float MAX_SPEED = 7.0f;

    ParticleSystem::ParticleSystem(int capacity) {
        positions.reserve(capacity);
        oldPositions.reserve(capacity);
        accelerations.reserve(capacity);
        shapes.reserve(capacity);
        active.reserve(capacity);
    }

    void ParticleSystem::addParticle(float x, float y) {
        sf::Vector2f pos(x, y);

        positions.push_back(pos);
        oldPositions.push_back(pos);
        accelerations.push_back({0.0f, 1000.0f});

        sf::CircleShape shape;
        shape.setRadius(radius);
        shape.setPosition(pos);
        shapes.push_back(shape);

        active.push_back(true);

        // Update color of the newly added particle
        updateColor(positions.size() - 1);
    }

    void ParticleSystem::update(float dt) {
        for (int i = 0; i < count(); ++i) {
            if (!active[i]) continue;

            // Verlet integration
            sf::Vector2f temp = positions[i];
            positions[i] = 2.0f * positions[i] - oldPositions[i] + accelerations[i] * (dt * dt);
            oldPositions[i] = temp;

            accelerations[i] = {0.f, 0.f};
            shapes[i].setPosition(positions[i]);
            updateColor(i);
        }
    }

    sf::Vector2f ParticleSystem::getVelocity(int index) const {
        return positions[index] - oldPositions[index];
    }

    void ParticleSystem::setVelocity(int index, const sf::Vector2f& vel) {
        oldPositions[index] = positions[index] - vel;
    }

    void ParticleSystem::accelerate(int index, const sf::Vector2f& force) {
        accelerations[index] += force;
    }

    void ParticleSystem::updateColor(int index) {
        sf::Vector2f vel = getVelocity(index);
        float speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);

        // Normalize speed to [0, 1] range
        float normalizedSpeed = std::clamp((speed - MIN_SPEED) / (MAX_SPEED - MIN_SPEED), 0.0f, 1.0f);

        // Color gradient from cold (blue) to hot (red)
        const std::vector<sf::Color> colors = {
            sf::Color(0, 0, 255),    // Blue (cold)
            sf::Color(0, 255, 255),  // Cyan
            sf::Color(0, 255, 0),    // Green
            sf::Color(255, 255, 0),  // Yellow
            sf::Color(255, 0, 0)     // Red (hot)
        };

        // Interpolate between colors
        float colorIndex = normalizedSpeed * (colors.size() - 1);
        int index1 = static_cast<int>(colorIndex);
        int index2 = std::min(index1 + 1, static_cast<int>(colors.size() - 1));
        float t = colorIndex - index1;

        // Check if particle is within window bounds
        if (positions[index].x > 0 && positions[index].y > 0 && positions[index].x < 1920 && positions[index].y < 1080) {
            sf::Color color;
            color.r = static_cast<sf::Uint8>(colors[index1].r + t * (colors[index2].r - colors[index1].r));
            color.g = static_cast<sf::Uint8>(colors[index1].g + t * (colors[index2].g - colors[index1].g));
            color.b = static_cast<sf::Uint8>(colors[index1].b + t * (colors[index2].b - colors[index1].b));
            shapes[index].setFillColor(color);
        } else {
            shapes[index].setFillColor(sf::Color::White);
        }
    }
}