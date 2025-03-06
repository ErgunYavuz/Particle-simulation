#include "simulation.h"

/*todo : - change collision detection to use quadtree
 *       - threadpool + better parallelization
 *       - simulation thread + rendering thread
 *       - search memory Optimization
 *       - reduce draw calls
 *       - refactoring
 *       - CUDA?
 *
 */

int main() {
    const int WIDTH = 1920;
    const int HEIGHT = 1090;
    const int NUM_PARTICLES = 500;

    sim::Simulation simulation(WIDTH, HEIGHT, NUM_PARTICLES);
    simulation.run();
    return 0;
}
