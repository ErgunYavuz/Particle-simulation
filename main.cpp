#include "simulation.h"

int main() {
    const int WIDTH = 2400;
    const int HEIGHT = 800;
    const int NUM_PARTICLES = 1000;

    sim::Simulation simulation(WIDTH, HEIGHT, NUM_PARTICLES);
    simulation.run();
    return 0;
}
