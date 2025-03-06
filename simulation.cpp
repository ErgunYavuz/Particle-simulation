#include "Simulation.h"
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>
#include <iostream>

namespace sim {
    bool windActive = false;
    float windStrength = 2000.0f;
    float dt = 1.0 / 60.0f;
    const int SUBSTEPS = 4;
    const int NUM_THREADS = std::thread::hardware_concurrency(); // Number of available threads

    sf::RectangleShape windButton;
    sf::Text windButtonText;

    Simulation::Simulation(int WIDTH, int HEIGHT, int NUM_PARTICLES) : window(sf::VideoMode(WIDTH, HEIGHT), "Particle Simulation"), fps(0) {
        //std::cout << NUM_THREADS << std::endl;
        // Setup fps limit and counter
        window.setFramerateLimit(60);
        if (!font.loadFromFile("Roboto-VariableFont_wdth,wght.ttf")) {
            throw std::runtime_error("Failed to load font");
        }
        fpsText.setFont(font);
        fpsText.setCharacterSize(20);
        fpsText.setFillColor(sf::Color::White);
        fpsText.setPosition(10, 10);

        // Setup Wind Button
        windButton.setSize(sf::Vector2f(150, 40));
        windButton.setPosition(WIDTH - 160, 10);
        windButton.setFillColor(sf::Color::Red);
        windButtonText.setFont(font);
        windButtonText.setCharacterSize(18);
        windButtonText.setFillColor(sf::Color::White);
        windButtonText.setString("Wind: OFF");
        windButtonText.setPosition(WIDTH - 140, 18);

        for (int i = 0; i < NUM_PARTICLES; i++) {
            float x = rand() % (WIDTH - 2 * (int)prtcl::RADIUS) + prtcl::RADIUS;
            float y = rand() % (HEIGHT - 2 * (int)prtcl::RADIUS) + prtcl::RADIUS;
            particles.emplace_back(x, y);
        }
    }

    void Simulation::mousePull(sf::Vector2f pos) {
        for (prtcl::Particle& obj : particles) {
            sf::Vector2f dir = pos - obj.position;
            float dist = sqrt(dir.x * dir.x + dir.y * dir.y);
            obj.accelerate(dir);
            //obj.update(dt);
        }
    }

    void Simulation::mousePush(sf::Vector2f pos) {
        for (prtcl::Particle& obj : particles) {
            sf::Vector2f dir = pos - obj.position;
            float dist = sqrt(dir.x * dir.x + dir.y * dir.y);
            obj.accelerate(-dir);
            //obj.update(dt);
        }
    }

    void Simulation::resolveWallCollisions(prtcl::Particle& p) {
        sf::Vector2f vel = p.getVelocity();
        if (p.position.x < 10) {
            p.position.x = 10;
            vel.x *= -p.RESTITUTION;
            p.setVelocity(vel);
        }

        if (p.position.x > window.getSize().x - p.shape.getRadius()) {
            p.position.x = window.getSize().x - p.shape.getRadius();
            vel.x *= -p.RESTITUTION;
            p.setVelocity(vel);
        }

        if (p.position.y < 10) {
            p.position.y = 10;
            vel.y *= -p.RESTITUTION;
            p.setVelocity(vel);
        }

        if (p.position.y + p.shape.getRadius() >= window.getSize().y) {
            p.position.y = window.getSize().y - p.shape.getRadius();
            vel.y = -std::abs(vel.y) * p.RESTITUTION;
            vel.x *= 0.99f;
            p.setVelocity(vel);
        }
    }

    void Simulation::resolveParticleCollision(prtcl::Particle& p1, prtcl::Particle& p2) {
        sf::Vector2f diff = p2.position - p1.position;
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

        if (dist < 2 * prtcl::RADIUS) {  // Overlapping check
            sf::Vector2f normal = diff / dist;
            float overlap = (2 * prtcl::RADIUS - dist) * 0.5f;

            // Push particles apart
            p1.position -= normal * overlap;
            p2.position += normal * overlap;

            sf::Vector2f vel1 = p1.getVelocity();
            sf::Vector2f vel2 = p2.getVelocity();
            sf::Vector2f relativeVelocity = vel2 - vel1;
            float velocityAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;
            if (velocityAlongNormal > 0) return;
            float impulse = -(1 + p1.RESTITUTION) * velocityAlongNormal / 2.0f;
            vel1 -= impulse * normal;
            vel2 += impulse * normal;
            p1.setVelocity(vel1);
            p2.setVelocity(vel2);
        }
    }

    void Simulation::resolveObstacleCollision(prtcl::Particle& p) {
        for (auto& obstacle : obstacles) {
            sf::Vector2f collisionNormal;
            if (obstacle.checkCollision(p.position, prtcl::RADIUS, collisionNormal)) {
                sf::Vector2f vel = p.getVelocity();

                float dot = vel.x * collisionNormal.x + vel.y * collisionNormal.y;
                sf::Vector2f reflection = vel - 2 * dot * collisionNormal * p.RESTITUTION;

                p.position += collisionNormal * .5f;

                p.setVelocity(reflection);
            }
        }
    }

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

    void Simulation::update(float dt) {
        float scaledDt = dt / SUBSTEPS;
        for (int step = 0; step < SUBSTEPS; step++) {
            std::vector<std::thread> threads;
            int chunkSize = particles.size() / NUM_THREADS;

            // Parallelize particle updates
            for (int t = 0; t < NUM_THREADS; t++) {
                int start = t * chunkSize;
                int end = (t == NUM_THREADS - 1) ? particles.size() : start + chunkSize;
                threads.emplace_back([this, start, end, scaledDt]() {
                    for (int i = start; i < end; i++) {
                        particles[i].acceleration = { 0.0f, prtcl::GRAVITY };
                        if (windActive) {
                            particles[i].acceleration.y -= windStrength;
                        }
                        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                            mousePull(sf::Vector2f(sf::Mouse::getPosition()));
                        }
                        if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                            mousePush(sf::Vector2f(sf::Mouse::getPosition()));
                        }
                        particles[i].update(scaledDt);
                        resolveWallCollisions(particles[i]);
                        resolveObstacleCollision(particles[i]);
                        for (int j = i + 1; j < particles.size(); j++) {
                            resolveParticleCollision(particles[i], particles[j]);
                        }
                    }
                });
            }

            for (auto& thread : threads) {
                thread.join();
            }
        }
    }

    void Simulation::render() {
        window.clear();
        window.draw(fpsText);
        if (windActive) {
            windButton.setFillColor(sf::Color::Green);
        }
        else {
            windButton.setFillColor(sf::Color::Red);
        }
        window.draw(windButton);
        window.draw(windButtonText);

        for (auto& o : obstacles) {
            o.draw(window);
        }

        for (auto& p : particles) {
            p.draw(window);
        }

        //Visualize pull force
        // sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        // for (prtcl::Particle& p : particles) {
        //     sf::Vertex line[] = {
        //         sf::Vertex(p.position, sf::Color::Red),
        //         sf::Vertex(mousePos, sf::Color::Red)
        //     };
        //     window.draw(line, 2, sf::Lines);
        // }

        window.display();
    }

    void Simulation::run() {
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
                //Handle button click
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left) &&
                    windButton.getGlobalBounds().contains(sf::Vector2f(event.mouseButton.x, event.mouseButton.y))) {
                    windActive = !windActive;
                    windButtonText.setString(windActive ? "Wind: ON" : "Wind: OFF");
                }

            }

            countFPS();
            update(dt);
            render();
        }
    }
}
