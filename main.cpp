#include <omp.h>
#include <thread>

#include "headers/simulation.h"
#include "headers/render.h"

int main() {
    const int WIDTH = 999;
    const int HEIGHT = 999;
    const int NUM_PARTICLES = 5000;
    const float STEPTIME = 1.f/60.f;
    const int SUBSTEPS = 8;
    const int FRAMERATE = 60;

    sf::ContextSettings settings;
    settings.antialiasingLevel = 1;
    sf::RenderWindow window = sf::RenderWindow(sf::VideoMode(WIDTH, HEIGHT), "Particle simulation", sf::Style::Default, settings);
    window.setFramerateLimit(FRAMERATE);

    sim::Simulation sim = sim::Simulation(WIDTH, HEIGHT, NUM_PARTICLES, SUBSTEPS, STEPTIME);
    // int i = 0;
    // while (i < 300) {
    //     sim.update(STEPTIME);
    //     i++;
    // }

    render::Renderer r = render::Renderer(window, sim);

     while (window.isOpen()){
         sf::Event event;
         while (window.pollEvent(event)){
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