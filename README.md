# Particle simulation

This is a simple particle simulation featuring particle, walls and obstacle collisions with wind interaction 

![Alt Text](Ressources/Particle%20simulation.gif)_

This project requires sfml dll files in the resources directory next to the executable produced.

## Key Concepts in the Code
1. **Particle Physics**: Particles move dynamically based on their velocity and external forces (e.g., gravity, wind).
    - Collisions are detected with walls, particles, or obstacles, and velocities are adjusted accordingly.

2. **Collision Detection**:
    - Checks distances and projections for determining collisions.
    - Adjusts positions using normal vectors for simulated restitution.

3. **Rendering**:
    - Uses **SFML**'s rendering engine to draw particles and obstacles in real-time.
