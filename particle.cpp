#include <cmath>
#include <random>
#include "particle.h"

#include <iostream>

namespace prtcl {

    Particle::Particle(float x, float y){
        position = {x, y};
        oldPosition = position;
        acceleration = {0.0f, 0.0f};
        shape.setRadius(RADIUS);
        shape.setPosition(position);
    }

    void Particle::update(float dt) {
        sf::Vector2f temp = position;
        position = 2.0f * position - oldPosition + acceleration * (dt * dt);
        oldPosition = temp;
        //applyConstraints();
        shape.setPosition(position);
    }

    sf::Vector2f Particle::getVelocity() const {
        return position - oldPosition;
    }

    void Particle::setVelocity(const sf::Vector2f& vel) {
        oldPosition = position - vel;
    }
    void Particle::accelerate(sf::Vector2f a) {
        //acceleration.y -= GRAVITY;
        acceleration += a;
    }

    void Particle::draw(sf::RenderWindow &window) {
        sf::Vector2f vel = getVelocity();
        float speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);

        const float MIN_SPEED = 0.0f;
        const float MAX_SPEED = 5.0f;

        // Normalize the speed to a range of [0, 1]
        float normalizedSpeed = std::max(0.0f, std::min(1.0f, (speed - MIN_SPEED) / (MAX_SPEED - MIN_SPEED)));

        // Define a color gradient (cold to hot)
        std::vector<sf::Color> colors = {
            sf::Color(0, 0, 255),    // Blue (cold)
            sf::Color(0, 255, 255),  // Cyan
            sf::Color(0, 255, 0),    // Green
            sf::Color(255, 255, 0),  // Yellow
            sf::Color(255, 0, 0)     // Red (hot)
        };

        // Map the normalized speed to the color gradient
        float colorIndex = normalizedSpeed * (colors.size() - 1);
        int index1 = static_cast<int>(colorIndex); // Lower index
        int index2 = std::min(index1 + 1, static_cast<int>(colors.size() - 1)); // Upper index
        float t = colorIndex - index1; // Interpolation factor

        // Interpolate between the two nearest colors
        sf::Color color;
        color.r = static_cast<sf::Uint8>(colors[index1].r + t * (colors[index2].r - colors[index1].r));
        color.g = static_cast<sf::Uint8>(colors[index1].g + t * (colors[index2].g - colors[index1].g));
        color.b = static_cast<sf::Uint8>(colors[index1].b + t * (colors[index2].b - colors[index1].b));
        shape.setFillColor(color);
        window.draw(shape);
    }
};