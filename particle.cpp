#include <cmath>
#include <random>
#include "particle.h"

#include <iostream>


namespace prtcl {
    Particle::Particle(float x, float y, float vx, float vy) {
        position = {x, y};
        velocity = {vx, vy};
        shape.setRadius(RADIUS);
        shape.setOrigin(RADIUS, RADIUS);
        shape.setPosition(position);
        shape.setFillColor(sf::Color::Green);
    }

    void Particle::update() {
        //update position
        position += velocity;
        // Boundary collision
        if (position.x < 10) { position.x = 10; velocity.x *= -RESTITUTION;}
        if (position.x > 790) { position.x = 790; position.x *= -RESTITUTION;}
        if (position.y < 10) { position.y = 10; velocity.y *= -RESTITUTION;}
        // Ground collision
        if (position.y + shape.getRadius() >= 600) {
            position.y = 600 - shape.getRadius();
            velocity.y = -std::abs(velocity.y ) * RESTITUTION;
            velocity.x *= RESTITUTION;
        }
        shape.setPosition(position);
    }
};


