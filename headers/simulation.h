#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "particle.h"


namespace sim {
    class Simulation {
        int width, height, substeps;
        std::vector<prtcl::Particle> particles;

        // Private methods
        void resolveWallCollisions(prtcl::Particle& p);
        void resolveParticleCollision(prtcl::Particle& p1, prtcl::Particle& p2);
        void resolveObstacleCollision(prtcl::Particle& p);

    public:
        Simulation(int width, int height, int numParticles);

        void mousePull(sf::Vector2f pos);
        void mousePush(sf::Vector2f pos);

        void update(float dt);

        std::vector<prtcl::Particle> &getParticle();

    };
}