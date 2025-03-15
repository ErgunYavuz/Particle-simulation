#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "particle.h"
#include "../quadtree.cpp"
#include "../uniformgrid.cpp"

namespace sim {
    class Simulation {
        int width, height, substeps;
        std::vector<prtcl::Particle> particles;

        void resolveWallCollisions(prtcl::Particle& p);
        void resolveParticleCollision(prtcl::Particle& p1, prtcl::Particle& p2);
        void resolveObstacleCollision(prtcl::Particle& p);

    public:
        Quadtree tree;
        //UniformGrid grid;
        Simulation(int width, int height, int numParticles, int substeps);

        Simulation(int width, int height, int numParticles, int substeps, sf::RenderWindow &window);

        Simulation(int width, int height, int numParticles, int substeps, float dt);

        void mousePull(sf::Vector2f pos);
        void mousePush(sf::Vector2f pos);

        void update(float dt);

        std::vector<prtcl::Particle> &getParticle();

    };
}