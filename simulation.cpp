// Simulation.cpp
#include "Simulation.h"
#include <cmath>
#include <sstream>
#include <stdexcept>

namespace sim {
    Simulation::Simulation() : window(sf::VideoMode(WIDTH, HEIGHT), "Particle Simulation"), fps(0) {
        for (int i = 0; i < NUM_PARTICLES; i++) {
            float x = rand() % (WIDTH - 2 * (int) prtcl::RADIUS) + prtcl::RADIUS;
            float y = rand() % (HEIGHT - 2 * (int) prtcl::RADIUS) + prtcl::RADIUS;
            float vx = SPEED * ((rand() % 200 - 100) / 100.0f);
            float vy = SPEED * ((rand() % 200 - 100) / 100.0f);
            particles.emplace_back(x, y, vx, vy, HEIGHT, WIDTH);
        }
    }
    void Simulation::resolveWallCollisions(prtcl::Particle &p) {
        if (p.position.x - prtcl::RADIUS < 0 || p.position.x + prtcl::RADIUS > WIDTH)
            p.velocity.x = -p.velocity.x;
        if (p.position.y - prtcl::RADIUS < 0 || p.position.y + prtcl::RADIUS > HEIGHT) {
            p.velocity.y = -p.velocity.y;
            p.position.y = std::min(p.position.y, HEIGHT - prtcl::RADIUS);
        }
    }

    void Simulation::resolveParticleCollision(prtcl::Particle &p1, prtcl::Particle &p2) {
        sf::Vector2f diff = p2.position - p1.position;
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        if (dist < 2 * prtcl::RADIUS) {
            sf::Vector2f normal = diff / dist;
            sf::Vector2f relativeVelocity = p2.velocity - p1.velocity;
            float velocityAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;

            if (velocityAlongNormal > 0) return;

            float elasticity = 1.0f;
            float impulse = -(1 + elasticity) * velocityAlongNormal / 2.0f;
            p1.velocity -= impulse * normal;
            p2.velocity += impulse * normal;
        }
    }

    void Simulation::run() {
        window.setFramerateLimit(60);
        if (!font.loadFromFile("Roboto-VariableFont_wdth,wght.ttf")) {
            throw std::runtime_error("Failed to load font");
        }
        fpsText.setFont(font);
        fpsText.setCharacterSize(20);
        fpsText.setFillColor(sf::Color::White);
        fpsText.setPosition(10, 10);

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            float deltaTime = clock.restart().asSeconds();
            if (fpsClock.getElapsedTime().asMilliseconds() >= 500) {
                fps = 1.f / deltaTime;
                std::ostringstream fpsStream;
                fpsStream << "FPS: " << static_cast<int>(fps);
                fpsText.setString(fpsStream.str());
                fpsClock.restart();
            }

            for (auto &p : particles) {
                p.update();
            }

            //brute force methode to resolve collsion O(n²)
            //TODO : implement grid collision detection algo
            for (size_t i = 0; i < particles.size(); i++) {
                for (size_t j = i + 1; j < particles.size(); j++) {
                    resolveParticleCollision(particles[i], particles[j]);
                }
            }

            window.clear();
            window.draw(fpsText);
            //One draw call per particle kills performance
            //TODO : implement instanced rendering
            for (auto &p : particles)
                window.draw(p.shape);
            window.display();
        }
    }
}





