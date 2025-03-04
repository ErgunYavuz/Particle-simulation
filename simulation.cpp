#include "Simulation.h"
#include <cmath>
#include <sstream>
#include <stdexcept>

namespace sim {
    bool windActive = false;
    float windStrength = 2000.0f;
    float dt = 1.0/60.0f;
    const int SUBSTEPS = 2;

    sf::RectangleShape windButton;
    sf::Text windButtonText;

    Simulation::Simulation(int WIDTH, int HEIGHT, int NUM_PARTICLES) : window(sf::VideoMode(WIDTH, HEIGHT), "Particle Simulation"), fps(0) {
        //setup fps limit and counter
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

        //obstacles.emplace_back(300, -10, 800, 150);
        //obstacles.emplace_back(WIDTH/2, HEIGHT, 900, 650);
        for (int i = 0; i < NUM_PARTICLES; i++) {
            float x = rand() % (WIDTH - 2 * (int) prtcl::RADIUS) + prtcl::RADIUS;
            float y = rand() % (HEIGHT - 2 * (int) prtcl::RADIUS) + prtcl::RADIUS;
            particles.emplace_back(x, y, HEIGHT, WIDTH);
        }
    }

    void Simulation::resolveParticleCollision(prtcl::Particle &p1, prtcl::Particle &p2) {
        sf::Vector2f diff = p2.position - p1.position;
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

        if (dist < 2 * prtcl::RADIUS) {  // Overlapping check
            sf::Vector2f normal = diff / dist;
            float overlap = (2 * prtcl::RADIUS - dist) * 0.5f;

            // Push particles apart
            p1.position -= normal * overlap;
            p2.position += normal * overlap;

            sf::Vector2f vel1 = p1.getVelocity(dt);
            sf::Vector2f vel2 = p2.getVelocity(dt);
            sf::Vector2f relativeVelocity = vel2 - vel1;
            float velocityAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;
            if (velocityAlongNormal > 0) return;
            float impulse = -(1 + p1.RESTITUTION) * velocityAlongNormal / 2.0f;
            vel1 -= impulse * normal;
            vel2 += impulse * normal;
            p1.setVelocity(vel1, dt);
            p2.setVelocity(vel2, dt);
        }
    }

    void Simulation::resolveObstacleCollision(prtcl::Particle &p) {
        for (auto &obstacle : obstacles) {
            sf::Vector2f collisionNormal;
            if (obstacle.checkCollision(p.position, prtcl::RADIUS, collisionNormal)) {
                sf::Vector2f vel = p.getVelocity(dt);

                float dot = vel.x * collisionNormal.x + vel.y * collisionNormal.y;
                sf::Vector2f reflection = vel - 2 * dot * collisionNormal * p.RESTITUTION;

                p.position += collisionNormal * .5f;

                p.setVelocity(reflection, dt);
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
            for (int i = 0; i < particles.size(); i++) {
                particles[i].acceleration = {0.0f, prtcl::GRAVITY};
                if (windActive) {
                    particles[i].acceleration.y -= windStrength;
                }
                particles[i].update(scaledDt);
                resolveObstacleCollision(particles[i]);
                for (int j = i + 1; j < particles.size(); j++) {
                    resolveParticleCollision(particles[i], particles[j]);
                }
            }
        }
    }

    void Simulation::render() {
        window.clear();
        window.draw(fpsText);
        if (windActive) {
            windButton.setFillColor(sf::Color::Green);
        } else {
            windButton.setFillColor(sf::Color::Red);
        }
        window.draw(windButton);
        window.draw(windButtonText);

        for (auto &o : obstacles) {
            o.draw(window);
        }

        for (auto &p : particles) {
            p.draw(window);
        }
        window.display();
    }

    void Simulation::run() {
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
                // Handle button click
                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left &&
                    windButton.getGlobalBounds().contains( sf::Vector2f(event.mouseButton.x, event.mouseButton.y))) {
                        windActive = !windActive;
                        windButtonText.setString(windActive ? "Wind: ON" : "Wind: OFF");
                }
            }
            countFPS();
            for (int i = 0; i < 1; i++) {
                update(dt);
            }
            render();
        }
    }
}