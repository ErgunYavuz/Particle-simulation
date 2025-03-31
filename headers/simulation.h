#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "particle.h"

namespace sim {
    class Simulation {
    public:
        Simulation(int width, int height, int numParticles, int substeps, float dt);
        void update(float dt);
        void mousePull(sf::Vector2f pos);
        void mousePush(sf::Vector2f pos);

        // Access to particle system
        prtcl::ParticleSystem& getParticleSystem() { return particles; }

    private:
        int width, height;
        int substeps;
        prtcl::ParticleSystem particles;

        void resolveWallCollisions(int particleIndex);
        void resolveParticleCollision(int p1Index, int p2Index);
    };
}
