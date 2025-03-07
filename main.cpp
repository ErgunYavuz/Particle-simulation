#include "simulation.h"

int main() {
    const int WIDTH = 1920;
    const int HEIGHT = 1080;
    const int NUM_PARTICLES = 2000;

    sim::Simulation simulation(WIDTH, HEIGHT, NUM_PARTICLES);
    simulation.run();
    return 0;
}
