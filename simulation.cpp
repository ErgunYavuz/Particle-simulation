#include <cmath>
#include "headers/simulation.h"

namespace sim {
    float subDt;
    int cellSize = 12;
    UniformGrid grid;


    Simulation::Simulation(int width, int height, int numParticles, int substeps, float dt)
        : width(width),
          height(height),
          substeps(substeps) {
        particles.reserve(numParticles);

        grid = UniformGrid(width, height, cellSize);

        // Predefined positions (example positions - replace with your own)
        std::vector<sf::Vector2f> predefinedPositions;

        // If you want a grid pattern instead of manual positions:
        int gridSize = std::ceil(std::sqrt(numParticles));
        float spacing = 10.0f;
        for (int i = 0; i < gridSize; i++) {
            for (int j = 0; j < gridSize && predefinedPositions.size() < numParticles; j++) {
                predefinedPositions.emplace_back(
                    1.0f + i * spacing,
                    1.0f + j * spacing
                );
            }
        }

        for (auto coords: predefinedPositions) {
            particles.emplace_back(coords.x, coords.y);
        }
        subDt = dt / substeps;
    }


    void Simulation::mousePull(sf::Vector2f pos) {
        const float PULL_RADIUS_SQ = 100.0f * 100.0f;
        for (auto &obj: particles) {
            sf::Vector2f diff = pos - obj.position;
            float distSq = diff.x * diff.x + diff.y * diff.y;
            if (distSq > PULL_RADIUS_SQ) continue;
            obj.accelerate(diff * 1000.f);
        }
    }

    void Simulation::mousePush(sf::Vector2f pos) {
        const float PULL_RADIUS_SQ = 100.0f * 100.0f;
        for (auto &obj: particles) {
            sf::Vector2f diff = pos - obj.position;
            float distSq = diff.x * diff.x + diff.y * diff.y;
            if (distSq > PULL_RADIUS_SQ) continue;
            obj.accelerate(-diff * 1000.f);
        }
    }

    void Simulation::resolveWallCollisions(prtcl::Particle &p) {
        sf::Vector2f vel = p.getVelocity();
        const float radius = p.radius;
        const float restitution = p.restitution;
        const int padding = 10;
        // Left wall
        if (p.position.x < radius) {
            p.position.x = radius;
            vel.x *= -restitution;
            p.setVelocity(vel);
        }
        // Right wall
        if (p.position.x > width - radius - padding) {
            p.position.x = width - radius - padding;
            vel.x *= -restitution;
            p.setVelocity(vel);
        }
        // Top wall
        if (p.position.y < radius + padding) {
            p.position.y = radius + padding;
            vel.y *= -restitution;
            p.setVelocity(vel);
        }
        // Bottom wall
        if (p.position.y > height - radius - padding) {
            p.position.y = height - radius - padding;
            vel.y = -std::abs(vel.y) * restitution;
            vel.x *= 0.99f; // Apply friction
            p.setVelocity(vel);
        }
    }

    void Simulation::resolveParticleCollision(prtcl::Particle &p1, prtcl::Particle &p2) {
        sf::Vector2f diff = p2.position - p1.position;
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        // float dx = std::abs(diff.x), dy = std::abs(diff.y);
        // float dist = 0.96f * std::max(dx, dy) + 0.4f * std::min(dx, dy);  // ~5% error
        const float radius = p1.radius;

        if (dist < 2 * radius) {
            // Particles are overlapping
            // Calculate normal vector
            sf::Vector2f normal = (dist > 0) ? diff / dist : sf::Vector2f(1.0f, 0.0f);
            float overlap = (2 * radius - dist) * 0.5f;

            // Push particles apart
            p1.position -= normal * overlap;
            p2.position += normal * overlap;

            // Calculate impulse for collision response
            sf::Vector2f vel1 = p1.getVelocity();
            sf::Vector2f vel2 = p2.getVelocity();
            sf::Vector2f relativeVelocity = vel2 - vel1;
            float velocityAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;

            // Only apply impulse if objects are moving toward each other
            if (velocityAlongNormal > 0) return;

            float restitution = p1.restitution; // Assuming both particles have same restitution
            float impulse = -(1 + restitution) * velocityAlongNormal / 2.0f;

            vel1 -= impulse * normal;
            vel2 += impulse * normal;

            p1.setVelocity(vel1);
            p2.setVelocity(vel2);
        }
    }

    void Simulation::processCollisions() {
        for (auto &cell: grid.cells) {
            for (size_t i = 0; i < cell.size(); i++) {
                for (size_t j = i + 1; j < cell.size(); j++) {
                    resolveParticleCollision(*cell[i], *cell[j]);
                }
            }
        }
        // check with adjacent cells
        for (int y = 0; y < grid.gridHeight; y++) {
            for (int x = 0; x < grid.gridWidth; x++) {
                int cellIndex = y * grid.gridWidth + x;
                std::vector<prtcl::Particle *> &currentCell = grid.cells[cellIndex];

                // Check right neighbor
                if (x < grid.gridWidth - 1) {
                    int rightIndex = y * grid.gridWidth + (x + 1);
                    for (auto p1: currentCell) {
                        for (auto p2: grid.cells[rightIndex]) {
                            resolveParticleCollision(*p1, *p2);
                        }
                    }
                }

                // Check bottom neighbor
                if (y < grid.gridHeight - 1) {
                    int bottomIndex = (y + 1) * grid.gridWidth + x;
                    for (auto p1: currentCell) {
                        for (auto p2: grid.cells[bottomIndex]) {
                            resolveParticleCollision(*p1, *p2);
                        }
                    }
                }

                // Check bottom-right neighbor
                if (x < grid.gridWidth - 1 && y < grid.gridHeight - 1) {
                    int bottomRightIndex = (y + 1) * grid.gridWidth + (x + 1);
                    for (auto p1: currentCell) {
                        for (auto p2: grid.cells[bottomRightIndex]) {
                            resolveParticleCollision(*p1, *p2);
                        }
                    }
                }

                // Check bottom-left neighbor (only if not at left edge)
                if (x > 0 && y < grid.gridHeight - 1) {
                    int bottomLeftIndex = (y + 1) * grid.gridWidth + (x - 1);
                    for (auto p1: currentCell) {
                        for (auto p2: grid.cells[bottomLeftIndex]) {
                            resolveParticleCollision(*p1, *p2);
                        }
                    }
                }
            }
        }
    }

    //using uniform grids
    void Simulation::update(float dt) {
        for (int step = 0; step < substeps; step++) {
            for (auto &p: particles) {
                p.update(subDt);
                resolveWallCollisions(p);
            }

            grid.clear();
            for (auto &p: particles) {
                grid.insert(&p);
            }

            processCollisions();
        }
    }


    // using quadtree
    // void Simulation::update(float dt) {
    //     float subDt = dt / substeps;
    //     for (int step = 0; step < substeps; step++) {
    //         // Clear and rebuild tree each step
    //         tree.clear();
    //         for (auto& p : particles) {
    //             tree.insert(&p);
    //         }
    //
    //         for (int i = 0; i < particles.size(); i++) {
    //             particles[i].update(subDt);
    //             resolveWallCollisions(particles[i]);
    //
    //             // Check only nearby particles
    //             float queryRadius = particles[i].radius * 2;
    //             sf::FloatRect queryArea(
    //                 particles[i].position.x - queryRadius,
    //                 particles[i].position.y - queryRadius,
    //                 queryRadius * 2,
    //                 queryRadius * 2
    //             );
    //
    //             std::vector<prtcl::Particle*> nearby;
    //             tree.getParticlesInRange(queryArea, nearby);
    //
    //             for (auto& other : nearby) {
    //                 if (other != &particles[i]) {
    //                     resolveParticleCollision(particles[i], *other);
    //                 }
    //             }
    //         }
    //     }
    // }

    // O(N²) double for loop over all particles
    // void Simulation::update(float dt) {
    //     float subDt = dt / substeps;
    //     for (int step = 0; step < substeps; step++) {
    //         for (int i = 0; i < particles.size(); i++) {
    //             particles[i].update(subDt);
    //             resolveWallCollisions(particles[i]);
    //             for (int j = i + 1; j < particles.size(); j++) {
    //                 resolveParticleCollision(particles[i], particles[j]);
    //             }
    //         }
    //     }
    // }

    std::vector<prtcl::Particle> &Simulation::getParticle() {
        return particles;
    }


    // void Simulation::resolveObstacleCollision(prtcl::Particle& p) {
    //     for (auto& obstacle : obstacles) {
    //         sf::Vector2f collisionNormal;
    //         if (obstacle.checkCollision(p.position, p.getRadius(), collisionNormal)) {
    //             sf::Vector2f vel = p.getVelocity();
    //             float restitution = p.getRestitution();
    //
    //             float dot = vel.x * collisionNormal.x + vel.y * collisionNormal.y;
    //             sf::Vector2f reflection = vel - 2 * dot * collisionNormal * restitution;
    //
    //             p.position += collisionNormal * 0.5f;
    //             p.setVelocity(reflection);
    //         }
    //     }
    // }
}
