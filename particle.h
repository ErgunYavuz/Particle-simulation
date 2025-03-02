#pragma once
#include <SFML/Graphics.hpp>

namespace prtcl {
    const float RADIUS = 10.0f;
    const float GRAVITY = .10f;

    class Particle {
        int const height, width;
    public:
        const float RESTITUTION = 0.75f;
        sf::Vector2f position;
        sf::Vector2f velocity;
        sf::CircleShape shape;

        Particle(float x, float y, float vx, float vy, int height, int width);

        void update();

        Particle(float x, float y, int height, int width);

        void update(float dt);
    };
}
