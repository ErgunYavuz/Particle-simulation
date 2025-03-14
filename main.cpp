#include "headers/simulation.h"
#include "headers/render.h"

int main() {
    const int WIDTH = 800;
    const int HEIGHT = 800;
    const int NUM_PARTICLES = 500;
    const float STEPTIME = 1.f/60.f;
    const int SUBSTEPS = 8;
    const int FRAMRATE = 60;

    sf::RenderWindow window = sf::RenderWindow(sf::VideoMode(WIDTH, HEIGHT), "Particle simulation");
    window.setFramerateLimit(FRAMRATE);

    sim::Simulation sim = sim::Simulation(WIDTH, HEIGHT, NUM_PARTICLES, SUBSTEPS);
    render::Renderer r = render::Renderer(window, sim);

    while (window.isOpen()){
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            sim.mousePull(mousePos);

        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            sim.mousePush(mousePos);
        }

        sim.update(STEPTIME);
        r.render();

    }
    return 0;
}


