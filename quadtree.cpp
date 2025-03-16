#include <vector>
#include <SFML/Graphics.hpp>
#include "headers/particle.h"

class Quadtree {
    static const int MAX_CAPACITY = 8;
    static const int MAX_DEPTH = 8;

    int depth;
    sf::FloatRect bounds;
    std::vector<prtcl::Particle*> particles;
    bool isSubdivided;

    Quadtree* northWest;
    Quadtree* northEast;
    Quadtree* southWest;
    Quadtree* southEast;

    void subdivide() {
        float x = bounds.left;
        float y = bounds.top;
        float w = bounds.width / 2.0f;
        float h = bounds.height / 2.0f;

        northWest = new Quadtree(depth + 1, sf::FloatRect(x, y, w, h));
        northEast = new Quadtree(depth + 1, sf::FloatRect(x + w, y, w, h));
        southWest = new Quadtree(depth + 1, sf::FloatRect(x, y + h, w, h));
        southEast = new Quadtree(depth + 1, sf::FloatRect(x + w, y + h, w, h));

        isSubdivided = true;
    }

    void redistributeParticles() {
        for (auto it = particles.begin(); it != particles.end(); ) {
            bool moved = false;
            if (northWest->insert(*it)) moved = true;
            else if (northEast->insert(*it)) moved = true;
            else if (southWest->insert(*it)) moved = true;
            else if (southEast->insert(*it)) moved = true;

            if (moved) {
                it = particles.erase(it);
            } else {
                ++it;
            }
        }
    }

public:
    Quadtree(int depth, const sf::FloatRect& bounds)
        : depth(depth), bounds(bounds), isSubdivided(false),
          northWest(nullptr), northEast(nullptr),
          southWest(nullptr), southEast(nullptr) {}

    ~Quadtree() {
        clear();
    }

    bool insert(prtcl::Particle* particle) {
        if (!bounds.contains(particle->position)) {
            return false;
        }

        if (particles.size() < MAX_CAPACITY || depth >= MAX_DEPTH) {
            particles.push_back(particle);
            return true;
        }

        if (!isSubdivided) {
            subdivide();
            redistributeParticles();
        }

        if (northWest->insert(particle)) return true;
        if (northEast->insert(particle)) return true;
        if (southWest->insert(particle)) return true;
        if (southEast->insert(particle)) return true;

        return false;
    }

    void getParticlesInRange(const sf::FloatRect& range, std::vector<prtcl::Particle*>& found) {
        if (!bounds.intersects(range)) {
            return;
        }

        for (auto p : particles) {
            if (range.contains(p->position)) {
                found.push_back(p);
            }
        }

        if (isSubdivided) {
            northWest->getParticlesInRange(range, found);
            northEast->getParticlesInRange(range, found);
            southWest->getParticlesInRange(range, found);
            southEast->getParticlesInRange(range, found);
        }
    }

    void clear() {
        particles.clear();

        if (isSubdivided) {
            delete northWest;
            delete northEast;
            delete southWest;
            delete southEast;

            isSubdivided = false;
            northWest = nullptr;
            northEast = nullptr;
            southWest = nullptr;
            southEast = nullptr;
        }
    }

    void draw(sf::RenderWindow& window) {
        sf::RectangleShape rect;
        rect.setPosition(bounds.left, bounds.top);
        rect.setSize(sf::Vector2f(bounds.width, bounds.height));
        rect.setFillColor(sf::Color::Transparent);
        rect.setOutlineColor(sf::Color(255, 255, 255, 50));
        rect.setOutlineThickness(1.0f);
        window.draw(rect);

        if (isSubdivided) {
            northWest->draw(window);
            northEast->draw(window);
            southWest->draw(window);
            southEast->draw(window);
        }
    }
};