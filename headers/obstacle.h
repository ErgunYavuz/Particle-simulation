#pragma once
#include <SFML/Graphics.hpp>

class obstacle {
    sf::RectangleShape line;
    sf::Vector2f startPoint, endPoint;
    sf::Vector2f normal;
    sf::Vector2f direction; // Normalized direction vector
    float length;  // Length of the line segment

    // Precomputed values for faster collision detection
    sf::Vector2f normalizedDirection;

public:
    obstacle(float x1, float y1, float x2, float y2);

    void draw(sf::RenderWindow &window);

    // Check and resolve collision in one operation for Verlet integration
    bool resolveCollision(sf::Vector2f &position, sf::Vector2f &oldPosition, float radius, float restitution);

    // Original collision check (keeping for backwards compatibility)
    bool checkCollision(sf::Vector2f &position, float radius, sf::Vector2f &collisionNormal);

    // Calculate the closest point on line segment to a given point
    sf::Vector2f getClosestPoint(const sf::Vector2f& point) const;
};