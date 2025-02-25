#pragma once
#include <SFML/Graphics.hpp>

class LineObstacle {
    sf::RectangleShape line;
    sf::Vector2f startPoint, endPoint;
    sf::Vector2f normal;

public:
    LineObstacle(float x1, float y1, float x2, float y2);

    void draw(sf::RenderWindow &window);
    bool checkCollision(sf::Vector2f &position, float radius, sf::Vector2f &collisionNormal);
};
