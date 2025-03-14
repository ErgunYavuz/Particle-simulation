#include "headers/render.h"
#include "headers/simulation.h"
#include "headers/particle.h"
#include <sstream>

namespace render {
    Renderer::Renderer(sf::RenderWindow &window, sim::Simulation &sim)
       : window(window), sim(sim) {

        // Load fonts
        if (!font.loadFromFile("Roboto-VariableFont_wdth,wght.ttf")) {
            throw std::runtime_error("Failed to load font");
        }
        // Setup FPS counter
        fpsText.setFont(font);
        fpsText.setCharacterSize(20);
        fpsText.setFillColor(sf::Color::White);
        fpsText.setPosition(10, 10);
    }

    void Renderer::render() {
        countFPS();
        window.clear();
        window.draw(fpsText);
        for (auto& p : sim.getParticle()) {
            window.draw(p.shape);
        }
        window.display();
    }

    void Renderer::countFPS() {
        float frameTime = mainClock.restart().asSeconds();
        if (fpsClock.getElapsedTime().asMilliseconds() >= 500) {
            float fps = 1.f / frameTime;
            std::ostringstream fpsStream;
            fpsStream << "FPS: " << static_cast<int>(fps);
            fpsText.setString(fpsStream.str());
            fpsClock.restart();
        }
    }
}