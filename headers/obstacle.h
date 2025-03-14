#pragma once
#include <SFML/Graphics.hpp>

class obstacle {
    sf::RectangleShape line;
    sf::Vector2f startPoint, endPoint;
    sf::Vector2f normal;
    sf::Vector2f direction;
    float length;


    sf::Vector2f normalizedDirection;

public:
    obstacle(float x1, float y1, float x2, float y2);

    void draw(sf::RenderWindow &window);

    bool resolveCollision(sf::Vector2f &position, sf::Vector2f &oldPosition, float radius, float restitution);

    bool checkCollision(sf::Vector2f &position, float radius, sf::Vector2f &collisionNormal);

    sf::Vector2f getClosestPoint(const sf::Vector2f& point) const;
};