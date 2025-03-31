#include <cmath>
#include "headers/simulation.h"
#include "uniformGrid.cpp"

namespace sim {
    float subDt;
    int cellSize;

    Simulation::Simulation(int width, int height, int numParticles, int substeps, float dt)
        : width(width),
          height(height),
          substeps(substeps),
          particles(numParticles) {

        for (int i = 0; i < numParticles; i++) {
            float x = rand() % (width);
            float y = rand() % (height);
            particles.addParticle(x, y);
        }

        subDt = dt/substeps;
        cellSize = particles.radius * 4;
    }

    void Simulation::mousePull(sf::Vector2f pos) {
        const float PULL_RADIUS_SQ = 100.0f * 100.0f;
        for (int i = 0; i < particles.count(); i++) {
            sf::Vector2f diff = pos - particles.positions[i];
            float distSq = diff.x*diff.x + diff.y*diff.y;
            if (distSq > PULL_RADIUS_SQ) continue;
            particles.accelerate(i, diff * 1000.f);
        }
    }

    void Simulation::mousePush(sf::Vector2f pos) {
        const float PULL_RADIUS_SQ = 100.0f * 100.0f;
        for (int i = 0; i < particles.count(); i++) {
            sf::Vector2f diff = pos - particles.positions[i];
            float distSq = diff.x*diff.x + diff.y*diff.y;
            if (distSq > PULL_RADIUS_SQ) continue;
            particles.accelerate(i, -diff * 1000.f);
        }
    }

    void Simulation::resolveWallCollisions(int index) {
        sf::Vector2f vel = particles.getVelocity(index);
        const float radius = particles.radius;
        const float restitution = particles.restitution;
        const int padding = 10;

        // Left wall
        if (particles.positions[index].x < radius) {
            particles.positions[index].x = radius;
            vel.x *= -restitution;
            particles.setVelocity(index, vel);
        }
        // Right wall
        if (particles.positions[index].x > width - radius - padding) {
            particles.positions[index].x = width - radius - padding;
            vel.x *= -restitution;
            particles.setVelocity(index, vel);
        }
        // Top wall
        if (particles.positions[index].y < radius + padding) {
            particles.positions[index].y = radius + padding;
            vel.y *= -restitution;
            particles.setVelocity(index, vel);
        }
        // Bottom wall
        if (particles.positions[index].y > height - radius - padding) {
            particles.positions[index].y = height - radius - padding;
            vel.y = -std::abs(vel.y) * restitution;
            vel.x *= 0.99f;  // Apply friction
            particles.setVelocity(index, vel);
        }
    }

    void Simulation::resolveParticleCollision(int idx1, int idx2) {
        sf::Vector2f diff = particles.positions[idx2] - particles.positions[idx1];
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        const float radius = particles.radius;

        if (dist < 2 * radius) {  // Particles are overlapping
            // Calculate normal vector
            sf::Vector2f normal = (dist > 0) ? diff / dist : sf::Vector2f(1.0f, 0.0f);
            float overlap = (2 * radius - dist) * 0.5f;

            // Push particles apart
            particles.positions[idx1] -= normal * overlap;
            particles.positions[idx2] += normal * overlap;

            // Calculate impulse for collision response
            sf::Vector2f vel1 = particles.getVelocity(idx1);
            sf::Vector2f vel2 = particles.getVelocity(idx2);
            sf::Vector2f relativeVelocity = vel2 - vel1;
            float velocityAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;

            // Only apply impulse if objects are moving toward each other
            if (velocityAlongNormal > 0) return;

            float restitution = particles.restitution;
            float impulse = -(1 + restitution) * velocityAlongNormal / 2.0f;

            vel1 -= impulse * normal;
            vel2 += impulse * normal;

            particles.setVelocity(idx1, vel1);
            particles.setVelocity(idx2, vel2);
        }
    }

    void Simulation::update(float dt) {
        UniformGrid grid(width, height, cellSize);

        for (int step = 0; step < substeps; step++) {
            particles.update(subDt);

            for (int i = 0; i < particles.count(); i++) {
                resolveWallCollisions(i);
            }

            grid.clear();
            for (int i = 0; i < particles.count(); i++) {
                grid.insert(i, particles.positions[i]);
            }

            grid.processCollisions([this](int idx1, int idx2) {
                resolveParticleCollision(idx1, idx2);
            });
        }
    }
}