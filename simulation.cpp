#include "Simulation.h"
#include <cmath>
#include <sstream>
#include <stdexcept>

namespace sim {
    bool windActive = false;  // Wind toggle
    float windStrength = 0.05f; // Adjustable wind force

    sf::RectangleShape windButton; // UI Button
    sf::Text windButtonText;

    Simulation::Simulation() : window(sf::VideoMode(WIDTH, HEIGHT), "Particle Simulation"), fps(0) {
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

        obstacles.emplace_back(300, -10, 800, 150); // Slanted line
        //obstacles.emplace_back(600, HEIGHT, 900, 650); //
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

        if (dist < 2 * prtcl::RADIUS) {  // Overlapping
            sf::Vector2f normal = diff / dist;
            float overlap = (2 * prtcl::RADIUS - dist) * 0.5f; // Half the overlap each

            // Push them apart smoothly
            p1.position -= normal * overlap;
            p2.position += normal * overlap;

            // Velocity response (elastic collision)
            sf::Vector2f relativeVelocity = p2.velocity - p1.velocity;
            float velocityAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;

            if (velocityAlongNormal > 0) return;

            float elasticity = 1.0f; // Perfectly elastic collision
            float impulse = -(1 + elasticity) * velocityAlongNormal / 2.0f;
            p1.velocity -= impulse * normal;
            p2.velocity += impulse * normal;
        }
    }

    void Simulation::resolveObstacleCollision(prtcl::Particle &p) {
        for (auto &obstacle : obstacles) {
            sf::Vector2f collisionNormal;
            if (obstacle.checkCollision(p.position, prtcl::RADIUS, collisionNormal)) {
                // Reflect particle velocity
                float dot = p.velocity.x * collisionNormal.x + p.velocity.y * collisionNormal.y;
                p.velocity -= 2 * dot * collisionNormal *p.RESTITUTION;
                p.position += collisionNormal * 2.f; // Push out slightly to prevent re-entering
            }
        }
    }

    void Simulation::run() {
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();

                // Handle button click
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                        if (windButton.getGlobalBounds().contains(mousePos)) {
                            windActive = !windActive;
                            windButtonText.setString(windActive ? "Wind: ON" : "Wind: OFF");
                        }
                    }
                }
            }

            float deltaTime = clock.restart().asSeconds();
            if (fpsClock.getElapsedTime().asMilliseconds() >= 500) {
                float fps = 1.f / deltaTime;
                std::ostringstream fpsStream;
                fpsStream << "FPS: " << static_cast<int>(fps);
                fpsText.setString(fpsStream.str());
                fpsClock.restart();
            }

            for (auto &p : particles) {
                if (windActive) {
                    p.velocity.x += windStrength; // Apply wind force
                }
                p.update();
                resolveObstacleCollision(p);
                //resolveWallCollisions(p);
            }

            for (size_t i = 0; i < particles.size(); i++) {
                for (size_t j = i + 1; j < particles.size(); j++) {
                    resolveParticleCollision(particles[i], particles[j]);
                }
            }

            window.clear();
            window.draw(fpsText);
            if (windActive) {
                windButton.setFillColor(sf::Color::Green);
            }else
                windButton.setFillColor(sf::Color::Red);
            window.draw(windButton);
            window.draw(windButtonText);

            for (auto &o : obstacles) {
                o.draw(window);
            }

            for (auto &p : particles)
                window.draw(p.shape);
            window.display();
        }
    }
}





