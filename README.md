# Particle simulation

This is a simple particle simulation for learning and fun.

![Alt Text](ressources/Particle_simulation.gif)_

This project requires sfml dll files in the resources directory to be next to the executable produced.
Fonts are needed to show performance metric counters.

**Particle Physics**:

- The physics is based on verlet integration
- Particles move dynamically based on their velocity and applied external forces  (e.g., gravity,
  collisions...).

**Some features**:

- Rendering is done using sfml libraries
- Spatial partitioning : Using a uniform grid partitioning to speed up collision processing
- multi threading : Using a threadpool with grid regions assigned to threads to split workload

