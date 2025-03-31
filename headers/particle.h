#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

namespace prtcl {
    constexpr float DEFAULT_RADIUS = 4.0f;
    constexpr float DEFAULT_RESTITUTION = 0.85f;

    // SoA container for all particles
    class ParticleSystem {
    public:
        ParticleSystem(int capacity);

        // Add a new particle
        void addParticle(float x, float y);

        // Update all particles
        void update(float dt);

        // Access functions for individual particles
        sf::Vector2f getVelocity(int index) const;
        void setVelocity(int index, const sf::Vector2f& vel);
        void accelerate(int index, const sf::Vector2f& force);
        void updateColor(int index);

        // Arrays for particle properties
        std::vector<sf::Vector2f> positions;
        std::vector<sf::Vector2f> oldPositions;
        std::vector<sf::Vector2f> accelerations;
        std::vector<sf::CircleShape> shapes;
        std::vector<bool> active;  // Active flag for particles

        // Constants
        float radius = DEFAULT_RADIUS;
        float restitution = DEFAULT_RESTITUTION;

        // Number of particles
        int count() const { return positions.size(); }
    };
}
