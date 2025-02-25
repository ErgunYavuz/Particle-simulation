#pragma once
#include <SFML/Graphics.hpp>

namespace prtcl {
    const float RADIUS = 10.0f;
    //const float GRAVITY = 0.1f;
    const float RESTITUTION = 0.8f;

    class Particle {
        int const height, width;
    public:
        sf::Vector2f position;
        sf::Vector2f velocity;
        sf::CircleShape shape;

        Particle(float x, float y, float vx, float vy);

        Particle(float x, float y, float vx, float vy, int height, int width);

        void update();
    };
}
