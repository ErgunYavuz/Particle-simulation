#ifndef SIMULATION_H
#define SIMULATION_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "particle.h"
#include "threadpool.h"
#include "uniformGrid.h"

namespace sim {
    class Simulation {
        int width, height;
        int substeps;
        std::vector<prtcl::Particle> particles;
        int threadCount;
        std::vector<std::pair<int, int> > workDivisions;
        ThreadPool threadPool;
        UniformGrid grid;

    public:
        Simulation(int width, int height, int numParticles, int substeps, float dt);

        void update(float dt);

        void mousePull(sf::Vector2f pos);

        void mousePush(sf::Vector2f pos);

        std::vector<prtcl::Particle> &getParticle();

    private:
        void resolveWallCollisions(prtcl::Particle &p);

        void resolveParticleCollision(prtcl::Particle &p1, prtcl::Particle &p2);

        void processCollisions();

        void processGridRange(int startIdx, int endIdx);
    };
}

#endif
