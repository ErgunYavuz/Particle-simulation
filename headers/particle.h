#pragma once
#include <SFML/Graphics.hpp>

namespace prtcl {
    class Particle {
    public:
        const float radius = 5.0f;

        const float restitution = 0.8f;
        sf::Vector2f oldPosition;
        sf::Vector2f position;
        sf::Vector2f acceleration;
        sf::CircleShape shape;

        Particle(float x, float y);

        void update(float dt);

        void setVelocity(const sf::Vector2f &vel);

        sf::Vector2f getVelocity() const;

        void accelerate(const sf::Vector2f &force);

        sf::Vector2f getPosition() const;

        void setColor();

        void draw(sf::RenderWindow &window);
    };
}
