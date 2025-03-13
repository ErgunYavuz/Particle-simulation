#include "simulation.h"
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <mutex>


namespace sim {
    Simulation::Simulation(int width, int height, int numParticles)
        : window(sf::VideoMode(width, height), "Particle Simulation"),
          windActive(false),
          windStrength(2000.0f),
          dt(1.0f / 60.0f),
          substeps(6),
          numThreads(std::thread::hardware_concurrency()) {

        // Setup fps limit and counter
        window.setFramerateLimit(60);

        //load fonts
        if (!font.loadFromFile("Roboto-VariableFont_wdth,wght.ttf")) {
            throw std::runtime_error("Failed to load font");
        }

        // Setup FPS counter
        fpsText.setFont(font);
        fpsText.setCharacterSize(20);
        fpsText.setFillColor(sf::Color::White);
        fpsText.setPosition(10, 10);

        // Setup Wind Button
        windButton.setSize(sf::Vector2f(150, 40));
        windButton.setPosition(width - 160, 10);
        windButton.setFillColor(sf::Color::Red);

        windButtonText.setFont(font);
        windButtonText.setCharacterSize(18);
        windButtonText.setFillColor(sf::Color::White);
        windButtonText.setString("Wind: OFF");
        windButtonText.setPosition(width - 140, 18);

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
        // Left wall
        if (p.position.x < radius) {
            p.position.x = radius;
            vel.x *= -restitution;
            p.setVelocity(vel);
        }
        // Right wall
        if (p.position.x > window.getSize().x - radius) {
            p.position.x = window.getSize().x - radius;
            vel.x *= -restitution;
            p.setVelocity(vel);
        }
        // Top wall
        if (p.position.y < radius) {
            p.position.y = radius;
            vel.y *= -restitution;
            p.setVelocity(vel);
        }
        // Bottom wall
        if (p.position.y + radius >= window.getSize().y) {
            p.position.y = window.getSize().y - radius;
            vel.y = -std::abs(vel.y) * restitution;
            vel.x *= 0.99f;  // Apply friction
            p.setVelocity(vel);
        }
    }

    void Simulation::resolveParticleCollision(prtcl::Particle& p1, prtcl::Particle& p2) {
        sf::Vector2f diff = p2.position - p1.position;
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        const float radius = p1.radius;  // Assuming both particles have same radius

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

    void Simulation::countFPS() {
        float frameTime = clock.restart().asSeconds();
        if (fpsClock.getElapsedTime().asMilliseconds() >= 500) {
            float fps = 1.f / frameTime;
            std::ostringstream fpsStream;
            fpsStream << "FPS: " << static_cast<int>(fps);
            fpsText.setString(fpsStream.str());
            fpsClock.restart();
        }
    }

    //todo : implement threadpool
    void Simulation::update(float dt) {
        float scaledDt = dt / substeps;

        for (int step = 0; step < substeps; step++) {
            for (auto& p : particles) {
                //p.acceleration = {0.0f, 1000.f};
                if (windActive) {
                    p.acceleration.y -= windStrength;
                }
            }
            // Phase 1: Update positions and resolve walls (parallel)
            {
                std::vector<std::thread> threads;
                int chunkSize = particles.size() / numThreads;
                for (int t = 0; t < numThreads; t++) {
                    int start = t * chunkSize;
                    int end = (t == numThreads - 1) ? particles.size() : start + chunkSize;
                    threads.emplace_back([this, start, end, scaledDt]() {
                        for (int i = start; i < end; i++) {
                            particles[i].update(scaledDt);
                            resolveWallCollisions(particles[i]);
                        }
                    });
                }
                for (auto& thread : threads) {
                    thread.join();
                }
            }

            // Build the grid
            grid.clear();
            cellSize = 2 * particles[0].radius;
            for (int i = 0; i < particles.size(); i++) {
                auto& p = particles[i];
                int cellX = static_cast<int>(p.position.x / cellSize);
                int cellY = static_cast<int>(p.position.y / cellSize);
                grid[CellKey(cellX, cellY)].push_back(i);
            }

            // Phase 2: Resolve collisions using the grid (parallel)
            {
                std::vector<std::thread> threads;
                int chunkSize = particles.size() / numThreads;
                for (int t = 0; t < numThreads; t++) {
                    int start = t * chunkSize;
                    int end = (t == numThreads - 1) ? particles.size() : start + chunkSize;
                    threads.emplace_back([this, start, end]() {
                        for (int i = start; i < end; i++) {
                            auto& p = particles[i];
                            int cellX = static_cast<int>(p.position.x / cellSize);
                            int cellY = static_cast<int>(p.position.y / cellSize);

                            // Check neighboring cells
                            for (int dx = -1; dx <= 1; dx++) {
                                for (int dy = -1; dy <= 1; dy++) {
                                    CellKey key(cellX + dx, cellY + dy);
                                    auto it = grid.find(key);
                                    if (it != grid.end()) {
                                        for (int j : it->second) {
                                            if (j > i) {
                                                resolveParticleCollision(particles[i], particles[j]);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    });
                }
                for (auto& thread : threads) {
                    thread.join();
                }
            }
        }
    }

    void Simulation::render() {
        window.clear();
        // Draw UI elements
        window.draw(fpsText);
        windButton.setFillColor(windActive ? sf::Color::Green : sf::Color::Red);
        window.draw(windButton);
        window.draw(windButtonText);
        // Draw particles
        for (auto& particle : particles) {
            particle.draw(window);
        }
        window.display();
    }

    void Simulation::run() {

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                // should not be there but otherwise interactions become inconsistent todo: move in handler
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    if (windButton.getGlobalBounds().contains(mousePos)) {
                        windActive = !windActive;
                        windButtonText.setString(windActive ? "Wind: ON" : "Wind: OFF");
                    }
                }
            }

            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                if (!windButton.getGlobalBounds().contains(mousePos)) {
                    mousePull(mousePos);
                }
            }

            if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                mousePush(mousePos);
            }
            countFPS();
            update(dt);
            render();
        }
    }
}  // namespace sim