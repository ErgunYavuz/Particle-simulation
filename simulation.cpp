#include <cmath>
#include <future>
#include "headers/simulation.h"

namespace sim {
    int cellSize = 12;

    Simulation::Simulation(int width, int height, int numParticles, int substeps, float dt)
        : width(width),
          height(height),
          substeps(substeps),
          grid(UniformGrid(width, height, cellSize)),
          threadPool(std::thread::hardware_concurrency()) {
        particles.reserve(numParticles);

        threadCount = std::thread::hardware_concurrency();

        // Initialize work divisions for threads
        workDivisions.resize(threadCount);

        // Spawn particles in grid pattern
        std::vector<sf::Vector2f> predefinedPositions;
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
    }

    void Simulation::mousePull(sf::Vector2f pos) {
        const float PULL_RADIUS_SQ = 100.0f * 100.0f * 10;
        for (auto &obj: particles) {
            sf::Vector2f diff = pos - obj.position;
            float distSq = diff.x * diff.x + diff.y * diff.y;
            if (distSq > PULL_RADIUS_SQ) continue;
            obj.accelerate(diff * 100.f);
        }
    }

    void Simulation::mousePush(sf::Vector2f pos) {
        const float PULL_RADIUS_SQ = 100.0f * 100.0f;
        for (auto &obj: particles) {
            sf::Vector2f diff = pos - obj.position;
            float distSq = diff.x * diff.x + diff.y * diff.y;
            if (distSq > PULL_RADIUS_SQ) continue;
            obj.accelerate(-diff * 10000.f);
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
        const float radius = p1.radius;

        // Particles are overlapping
        if (dist < 2 * radius) {
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

    void Simulation::processGridRange(int startIdx, int endIdx) {
        for (int idx = startIdx; idx < endIdx; idx++) {
            // Get y and x from linear index
            int y = idx / grid.gridWidth;
            int x = idx % grid.gridWidth;

            int cellIndex = y * grid.gridWidth + x;
            std::vector<prtcl::Particle *> &currentCell = grid.cells[cellIndex];

            // Process current cell
            for (size_t i = 0; i < currentCell.size(); i++) {
                for (size_t j = i + 1; j < currentCell.size(); j++) {
                    resolveParticleCollision(*currentCell[i], *currentCell[j]);
                }
            }

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

            // Check bottom-left neighbor
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

    void Simulation::processCollisions() {
        int totalCells = grid.gridWidth * grid.gridHeight;
        int cellsPerThread = totalCells / threadCount;
        int remainingCells = totalCells % threadCount;

        std::vector<std::future<void> > futures;

        // Distribute cells among threads
        int currentCell = 0;
        for (int i = 0; i < threadCount; i++) {
            int cellsForThisThread = cellsPerThread + (i < remainingCells ? 1 : 0);
            int startIdx = currentCell;
            int endIdx = startIdx + cellsForThisThread;

            // Submit task to thread pool
            futures.push_back(
                threadPool.enqueue([this, startIdx, endIdx]() {
                    this->processGridRange(startIdx, endIdx);
                })
            );

            currentCell = endIdx;
        }

        for (auto &future: futures) {
            future.wait();
        }
    }

    void Simulation::update(float dt) {
        for (int step = 0; step < substeps; step++) {
            for (auto &p: particles) {
                p.update(dt / substeps);
                resolveWallCollisions(p);
            }

            grid.clear();
            for (auto &p: particles) {
                grid.insert(&p);
            }

            processCollisions();
        }
    }

    std::vector<prtcl::Particle> &Simulation::getParticle() {
        return particles;
    }
}
