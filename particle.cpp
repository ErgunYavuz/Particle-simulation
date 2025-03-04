#include <cmath>
#include <random>
#include "particle.h"

#include <iostream>

namespace prtcl {

    Particle::Particle(float x, float y, int height, int width): height(height), width(width) {
        position = {x, y};
        oldPosition = position;
        acceleration = {0.0f, GRAVITY};
        shape.setRadius(RADIUS);
        shape.setOrigin(RADIUS, RADIUS);
        shape.setPosition(position);
        //shape.setFillColor(sf::Color::Blue);
    }

    void Particle::update(float dt) {
        sf::Vector2f temp = position;
        position = 2.0f * position - oldPosition + acceleration * (dt * dt);
        oldPosition = temp;
        applyConstraints(dt);
        shape.setPosition(position);
    }

    void Particle::applyConstraints(float dt) {
        sf::Vector2f vel = getVelocity(dt);
        if (position.x < 10) {
            position.x = 10;
            vel.x *= -RESTITUTION;
            setVelocity(vel, dt);
        }

        if (position.x > width - shape.getRadius()) {
            position.x = width - shape.getRadius();
            vel.x *= -RESTITUTION;
            setVelocity(vel, dt);
        }

        if (position.y < 10) {
            position.y = 10;
            vel.y *= -RESTITUTION;
            setVelocity(vel, dt);
        }

        if (position.y + shape.getRadius() >= height) {
            position.y = height - shape.getRadius();
            vel.y = -std::abs(vel.y) * RESTITUTION;
            vel.x *= 0.99f;
            setVelocity(vel, dt);
        }
    }

    sf::Vector2f Particle::getVelocity(float dt) const {
        // Calculate velocity from positions
        return (position - oldPosition) / dt;
    }

    void Particle::setVelocity(const sf::Vector2f& vel, float dt) {
        // Update old position to reflect the new velocity
        oldPosition = position - vel * dt;
    }

    void Particle::draw(sf::RenderWindow &window) {
        // Calculate velocity magnitude (speed)
        sf::Vector2f vel = getVelocity(1.0f); // Using 1.0 as dt for simplicity
        float speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);

        // Define min and max speeds for color mapping
        const float MIN_SPEED = 0.0f;
        const float MAX_SPEED = 10.0f; // Adjust based on your simulation

        // Clamp and normalize speed between 0 and 1
        float normalizedSpeed = std::max(0.0f, std::min(1.0f, (speed - MIN_SPEED) / (MAX_SPEED - MIN_SPEED)));

        // Interpolate between blue (0,0,255) and red (255,0,0)
        int red = static_cast<int>(normalizedSpeed * 255);
        int blue = static_cast<int>((1.0f - normalizedSpeed) * 255);

        // Set the color
        shape.setFillColor(sf::Color(red, 0, blue));

        window.draw(shape);
    }
};