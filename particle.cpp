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
        shape.setFillColor(sf::Color::Blue);
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
        window.draw(shape);
    }
};