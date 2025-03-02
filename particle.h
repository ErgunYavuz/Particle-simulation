#pragma once
#include <SFML/Graphics.hpp>

namespace prtcl {
    const float RADIUS = 10.0f;
    const float GRAVITY = 1.0f;

    class Particle {
    public:
        int const height, width;
        sf::Vector2f oldPosition;
        const float RESTITUTION = 0.5f;
        sf::Vector2f position;
        sf::Vector2f acceleration;
        sf::CircleShape shape;
        Particle(float x, float y, int height, int width);

        void update(float dt);
        void applyConstraints(float dt);
        void setVelocity(const sf::Vector2f& vel, float dt);

        void draw(sf::RenderWindow &window);

        sf::Vector2f getVelocity(float dt) const;
    };
}