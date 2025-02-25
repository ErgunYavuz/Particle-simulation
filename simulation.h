#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "LineObstacle.h"
#include "Particle.h"

namespace sim{
    const int WIDTH = 1600;
    const int HEIGHT = 800;
    const int NUM_PARTICLES = 1000;
    const float SPEED = 2.0f;

    class Simulation {
        sf::RenderWindow window;
        std::vector<prtcl::Particle> particles;
        std::vector<LineObstacle> obstacles;
        sf::Clock clock;
        sf::Clock fpsClock;
        sf::Font font;
        sf::Text fpsText;
        float fps;

        void resolveWallCollisions(prtcl::Particle &p);
        void resolveParticleCollision(prtcl::Particle &p1, prtcl::Particle &p2);
        void resolveObstacleCollision(prtcl::Particle &p);

    public:
        Simulation();
        void run();
    };
}

