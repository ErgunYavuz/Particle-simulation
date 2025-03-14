#include "headers/obstacle.h"
#include <cmath>
#include <iostream>

obstacle::obstacle(float x1, float y1, float x2, float y2) {
    startPoint = {x1, y1};
    endPoint = {x2, y2};
    direction = endPoint - startPoint;
    length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    normalizedDirection = direction / length;
    line.setSize({length, 10.f});
    line.setOrigin(0, 5.f);
    line.setPosition(startPoint);
    line.setRotation(std::atan2(direction.y, direction.x) * 180.f / 3.14159f);
    line.setFillColor(sf::Color::White);
    normal = sf::Vector2f(-normalizedDirection.y, normalizedDirection.x);
}

sf::Vector2f obstacle::getClosestPoint(const sf::Vector2f& point) const {
    sf::Vector2f pointVector = point - startPoint;
    float projection = pointVector.x * normalizedDirection.x + pointVector.y * normalizedDirection.y;
    projection = std::max(0.0f, std::min(projection, length));
    return startPoint + normalizedDirection * projection;
}

bool obstacle::checkCollision(sf::Vector2f &position, float radius, sf::Vector2f &collisionNormal) {
    sf::Vector2f closestPoint = getClosestPoint(position);

    sf::Vector2f distanceVec = position - closestPoint;
    float distanceSq = distanceVec.x * distanceVec.x + distanceVec.y * distanceVec.y;
    float radiusSq = (radius + 5.0f) * (radius + 5.0f);

    if (distanceSq < radiusSq) {
        float distance = std::sqrt(distanceSq);
        collisionNormal = distanceVec / distance;
        return true;
    }

    return false;
}

void obstacle::draw(sf::RenderWindow &window) {
    window.draw(line);
}