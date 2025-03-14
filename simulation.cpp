#include "headers/simulation.h"
#include <cmath>

namespace sim {
    Simulation::Simulation(int width, int height, int numParticles)
        : width(width),
          height(height),
          substeps(6){

        // Create particles
        particles.reserve(numParticles);
        for (int i = 0; i < numParticles; i++) {
            float x = rand() % (width - 1000);
            float y = rand() % (height - 1000);
            particles.emplace_back(x, y);
        }
    }

    void Simulation::mousePull(sf::Vector2f pos) {
        for (prtcl::Particle& obj : particles) {
            sf::Vector2f dir = pos - obj.position;
            obj.accelerate(dir * 10.f);
        }
    }

    void Simulation::mousePush(sf::Vector2f pos) {
        for (prtcl::Particle& obj : particles) {
            sf::Vector2f dir = pos - obj.position;
            obj.accelerate(-dir*2.f);
        }
    }

    void Simulation::resolveWallCollisions(prtcl::Particle& p) {
        sf::Vector2f vel = p.getVelocity();
        const float radius = p.radius;
        const float restitution = p.restitution;
        const int padding = 10;
        // Left wall
        if (p.position.x < radius ) {
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
            vel.x *= 0.99f;  // Apply friction
            p.setVelocity(vel);
        }
    }

    void Simulation::resolveParticleCollision(prtcl::Particle& p1, prtcl::Particle& p2) {
        sf::Vector2f diff = p2.position - p1.position;
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        const float radius = p1.radius;

        if (dist < 2 * radius) {  // Particles are overlapping
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

            float restitution = p1.restitution;  // Assuming both particles have same restitution
            float impulse = -(1 + restitution) * velocityAlongNormal / 2.0f;

            vel1 -= impulse * normal;
            vel2 += impulse * normal;

            p1.setVelocity(vel1);
            p2.setVelocity(vel2);
        }
    }

    // void Simulation::resolveObstacleCollision(prtcl::Particle& p) {
    //     for (auto& obstacle : obstacles) {
    //         sf::Vector2f collisionNormal;
    //         if (obstacle.checkCollision(p.position, p.getRadius(), collisionNormal)) {
    //             sf::Vector2f vel = p.getVelocity();
    //             float restitution = p.getRestitution();
    //
    //             // Calculate reflection vector
    //             float dot = vel.x * collisionNormal.x + vel.y * collisionNormal.y;
    //             sf::Vector2f reflection = vel - 2 * dot * collisionNormal * restitution;
    //
    //             // Move particle slightly away from obstacle to prevent sticking
    //             p.position += collisionNormal * 0.5f;
    //             p.setVelocity(reflection);
    //         }
    //     }
    // }

    void Simulation::update(float dt) {
        float subDt = dt / substeps;
        for (int step = 0; step < substeps; step++) {
            for (int i = 0; i < particles.size(); i++) {
                particles[i].update(subDt);
                resolveWallCollisions(particles[i]);
                for (int j = i + 1; j < particles.size(); j++) {
                    resolveParticleCollision(particles[i], particles[j]);
                }
            }
        }
    }

    std::vector<prtcl::Particle> &Simulation::getParticle() {
        return particles;
    }
}  // namespace sim