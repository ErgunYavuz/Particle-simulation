#include <cmath>
#include <random>
#include "particle.h"

#include <iostream>


namespace prtcl {
    Particle::Particle(float x, float y, float vx, float vy, int height, int width): height(height), width(width) {
        position = {x, y};
        velocity = {vx, vy};
        shape.setRadius(RADIUS);
        shape.setOrigin(RADIUS, RADIUS);
        shape.setPosition(position);
        shape.setFillColor(sf::Color::Blue);
    }

    void Particle::update() {
        //update position
        position += velocity;
        position.y += GRAVITY;
        // Boundary collision
        if (position.x < 10) { position.x = 10; velocity.x *= -RESTITUTION;}
        if (position.x > width + shape.getRadius()) { position.x *= -shape.getRadius();}
        if (position.y < 10) { position.y = 10; velocity.y *= -RESTITUTION;}
        // Ground collision
        if (position.y + shape.getRadius() >= height) {
            position.y = height - shape.getRadius();
            velocity.y = -std::abs(velocity.y ) * RESTITUTION;
            velocity.x *= RESTITUTION;
        }
        shape.setPosition(position);
    }

    // In Particle::update()
void Particle::update(float dt) {
    // Apply forces and update velocity
    velocity.y += GRAVITY * dt;

    // Update position using velocity and time step
    position += velocity * dt;

    // Boundary collision checks (unchanged)
    if (position.x < 10) { position.x = 10; velocity.x *= -RESTITUTION;}
    if (position.x > width - shape.getRadius()) { position.x = width - shape.getRadius(); velocity.x *= -RESTITUTION;}
    if (position.y < 10) { position.y = 10; velocity.y *= -RESTITUTION;}
    if (position.y + shape.getRadius() >= height) {
        position.y = height - shape.getRadius();
        velocity.y = -std::abs(velocity.y ) * RESTITUTION;
        velocity.x *= RESTITUTION;
    }

    // Update shape position
    shape.setPosition(position);
}

};



