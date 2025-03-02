#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "LineObstacle.h"
#include "Particle.h"

namespace sim{
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
        static void resolveParticleCollision(prtcl::Particle &p1, prtcl::Particle &p2);
        void resolveObstacleCollision(prtcl::Particle &p);

    public:
        Simulation(int WIDTH, int HEIGHT, int NUM_PARTICLES);
        void run();
    };
}

