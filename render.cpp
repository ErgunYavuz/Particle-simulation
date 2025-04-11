#include "headers/render.h"
#include "headers/simulation.h"
#include "headers/particle.h"
#include <sstream>

namespace render {
    Renderer::Renderer(sf::RenderWindow &window, sim::Simulation &sim)
       : window(window), sim(sim) {

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
        //sim.tree.draw(window);
        for (auto& p : sim.getParticle()) {
            window.draw(p.shape);
        }
        window.draw(fpsText);
        window.display();
    }

    void Renderer::countFPS() {

        float frameTime = mainClock.restart().asSeconds();
        float ms = frameTime * 1000.0f;
        float fps = 1.f / frameTime;
        std::ostringstream fpsStream;
        fpsText.setString(std::to_string(fps) +"fps, "+ std::to_string(ms) + "ms, " + std::to_string(sim.getParticle().size()) + " particles");
        fpsClock.restart();
        // if (fpsClock.getElapsedTime().asMilliseconds() >= 500) {
        //     float ms = frameTime * 1000.0f;
        //     float fps = 1.f / frameTime;
        //     std::ostringstream fpsStream;
        //     fpsText.setString(std::to_string(fps) +"fps, "+ std::to_string(ms) + "ms, " + std::to_string(sim.getParticle().size()) + " particles");
        //     fpsClock.restart();
        // }
    }
}