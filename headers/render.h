#pragma once
#include "simulation.h"

namespace render {
    class Renderer {

        sf::RenderWindow &window;
        sim::Simulation &sim;
        sf::Text fpsText;

        // Clock and FPS tracking
        sf::Clock fpsClock;
        sf::Font font;
        sf::Clock mainClock;

        void countFPS();
    public:
        Renderer(sf::RenderWindow &window, sim::Simulation &sim);
        void render();
    };
}
