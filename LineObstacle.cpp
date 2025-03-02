#include "LineObstacle.h"
#include <cmath>

LineObstacle::LineObstacle(float x1, float y1, float x2, float y2) {
    startPoint = {x1, y1};
    endPoint = {x2, y2};

    sf::Vector2f direction = endPoint - startPoint;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    line.setSize({length, 10.f});
    line.setOrigin(0, 2.5f);
    line.setPosition(startPoint);
    line.setRotation(std::atan2(direction.y, direction.x) * 180.f / 3.14159f);
    line.setFillColor(sf::Color::White);

    normal = sf::Vector2f(-direction.y / length, direction.x / length);
}

void LineObstacle::draw(sf::RenderWindow &window) {
    window.draw(line);
}

bool LineObstacle::checkCollision(sf::Vector2f &position, float radius, sf::Vector2f &collisionNormal) {
    sf::Vector2f lineVector = endPoint - startPoint;
    sf::Vector2f pointVector = position - startPoint;

    float lineLength = std::sqrt(lineVector.x * lineVector.x + lineVector.y * lineVector.y);
    sf::Vector2f lineDirection = lineVector / lineLength;

    // Projection of pointVector onto the line
    float projection = (pointVector.x * lineDirection.x + pointVector.y * lineDirection.y);

    // Clamp the projection to the line segment
    if (projection < 0) projection = 0;
    if (projection > lineLength) projection = lineLength;

    // Calculate the closest point on the line
    sf::Vector2f closestPoint = startPoint + lineDirection * projection;

    // Calculate distance and normal
    sf::Vector2f distanceVec = position - closestPoint;
    float distance = std::sqrt(distanceVec.x * distanceVec.x + distanceVec.y * distanceVec.y);

    // Check if the particle collides
    if (distance < radius + 4) {
        // Dynamically compute collision normal by normalizing the distance vector
        collisionNormal = distanceVec / distance;
        return true;
    }

    return false;
}

