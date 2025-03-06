#pragma once
#include <SFML/Graphics.hpp>

namespace prtcl {
    const float RADIUS = 10.0f;
    const float GRAVITY = 0.0f;

    class Particle {
    public:
        sf::Vector2f oldPosition;
        const float RESTITUTION = 0.8f;
        sf::Vector2f position;
        sf::Vector2f acceleration;
        sf::CircleShape shape;

        Particle(float x, float y);
        void update(float dt);
        void setVelocity(const sf::Vector2f &vel);
        sf::Vector2f getVelocity() const;
        void accelerate(sf::Vector2f a);
        void draw(sf::RenderWindow &window);
    };
}