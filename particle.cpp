#include "particle.h"
#include <cmath>
#include <iostream>
#include <vector>

namespace prtcl {
    constexpr float MIN_SPEED = 0.0f;
    constexpr float MAX_SPEED = 10.0f;

    Particle::Particle(float x, float y) {
        position = {x, y};
        oldPosition = position;
        acceleration = {0.0f, 0.0f};
        shape.setRadius(radius);
        shape.setPosition(position);
    }

    void Particle::update(float dt) {
        // Verlet integration
        sf::Vector2f temp = position;
        position = 2.0f * position - oldPosition + acceleration * (dt * dt);
        oldPosition = temp;

        // sf::Vector2f velocity = getVelocity();
        // float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
        // if (speed > 20.f) {
        //     // Scale down velocity to max speed
        //     velocity = velocity * (20.f / speed);
        //     setVelocity(velocity);
        // }
        acceleration ={0.f, 500.f};
        shape.setPosition(position);
        setColor();
    }

    sf::Vector2f Particle::getVelocity() const {
        return position - oldPosition;
    }

    void Particle::setVelocity(const sf::Vector2f& vel) {
        oldPosition = position - vel;
    }

    void Particle::accelerate(const sf::Vector2f& force) {
        acceleration += force;
    }

    sf::Vector2f Particle::getPosition() const {
        return position;
    }

    void Particle::setColor() {
        sf::Vector2f vel = getVelocity();
        float speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);

        // Normalize speed to [0, 1] range
        float normalizedSpeed = std::clamp((speed - MIN_SPEED) / (MAX_SPEED - MIN_SPEED), 0.0f, 1.0f);

        // Color gradient from cold (blue) to hot (red)
        const std::vector colors = {
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

        //fixes particles going out of the window and crashing the sim due to segfault
        if (position.x > 0 && position.y > 0 && position.x < 1920 && position.y < 1080) {
            sf::Color color;
            color.r = static_cast<sf::Uint8>(colors[index1].r + t * (colors[index2].r - colors[index1].r));
            color.g = static_cast<sf::Uint8>(colors[index1].g + t * (colors[index2].g - colors[index1].g));
            color.b = static_cast<sf::Uint8>(colors[index1].b + t * (colors[index2].b - colors[index1].b));
            shape.setFillColor(color);
        }else {
            shape.setFillColor(sf::Color::White);
            //std::cout << "out of window" << std::endl;
        }
    }

}  // namespace prtcl