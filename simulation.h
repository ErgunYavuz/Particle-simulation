#pragma once

#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <vector>
#include "particle.h"


namespace sim {
    class Simulation {
        sf::RenderWindow window;
        std::vector<prtcl::Particle> particles;

        // Clock and FPS tracking
        sf::Clock clock;
        sf::Clock fpsClock;
        sf::Font font;
        sf::Text fpsText;

        // UI elements
        sf::RectangleShape windButton;
        sf::Text windButtonText;

        // Simulation parameters
        bool windActive;
        float windStrength;
        float dt;
        int substeps;
        int numThreads;

        struct CellKey {
            int x, y;
            CellKey(int x_, int y_) : x(x_), y(y_) {}
            bool operator==(const CellKey& other) const {
                return x == other.x && y == other.y;
            }
        };

        struct CellKeyHash {
            size_t operator()(const CellKey& k) const {
                return std::hash<int>()(k.x) ^ (std::hash<int>()(k.y) << 1);
            }
        };

        std::unordered_map<CellKey, std::vector<int>, CellKeyHash> grid;
        float cellSize;

        // Private methods
        void mousePull(sf::Vector2f pos);
        void mousePush(sf::Vector2f pos);
        void countFPS();
        void update(float dt);
        void render();
        void resolveWallCollisions(prtcl::Particle& p);
        void resolveParticleCollision(prtcl::Particle& p1, prtcl::Particle& p2);
        void resolveObstacleCollision(prtcl::Particle& p);
        void handleEvents();

    public:
        Simulation(int width, int height, int numParticles);
        void run();
    };
}