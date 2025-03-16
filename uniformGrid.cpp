#include <functional>
#include "headers/particle.h"

class UniformGrid {
    int cellSize;
    int gridWidth, gridHeight;
    std::vector<std::vector<prtcl::Particle*>> cells;

public:
    UniformGrid(){}

    UniformGrid(int width, int height, int cellSize)
        : cellSize(cellSize),
          gridWidth(width / cellSize + 1),
          gridHeight(height / cellSize + 1) {
        cells.resize(gridWidth * gridHeight);

    }

    void clear() {
        for (auto& cell : cells) {
            cell.clear();
        }
    }

    int getCellIndex(const sf::Vector2f& position) const {
        int cellX = static_cast<int>(position.x / cellSize);
        int cellY = static_cast<int>(position.y / cellSize);

        cellX = std::max(0, std::min(cellX, gridWidth - 1));
        cellY = std::max(0, std::min(cellY, gridHeight - 1));

        return cellY * gridWidth + cellX;
    }

    void insert(prtcl::Particle* particle) {
        int cellIndex = getCellIndex(particle->position);
        cells[cellIndex].push_back(particle);
    }

    // we are checking only 4 adjacent cells to avoid redundant checks
    void processCollisions(std::function<void(prtcl::Particle*, prtcl::Particle*)> collisionFunc) {
        // check collisions within the same cell
        for (auto& cell : cells) {
            for (size_t i = 0; i < cell.size(); i++) {
                for (size_t j = i + 1; j < cell.size(); j++) {
                    collisionFunc(cell[i], cell[j]);
                }
            }
        }

        // check with adjacent cells
        for (int y = 0; y < gridHeight; y++) {
            for (int x = 0; x < gridWidth; x++) {
                int cellIndex = y * gridWidth + x;
                std::vector<prtcl::Particle*>& currentCell = cells[cellIndex];

                // Check right neighbor
                if (x < gridWidth - 1) {
                    int rightIndex = y * gridWidth + (x + 1);
                    checkCellPair(currentCell, cells[rightIndex], collisionFunc);
                }

                // Check bottom neighbor
                if (y < gridHeight - 1) {
                    int bottomIndex = (y + 1) * gridWidth + x;
                    checkCellPair(currentCell, cells[bottomIndex], collisionFunc);
                }

                // Check bottom-right neighbor
                if (x < gridWidth - 1 && y < gridHeight - 1) {
                    int bottomRightIndex = (y + 1) * gridWidth + (x + 1);
                    checkCellPair(currentCell, cells[bottomRightIndex], collisionFunc);
                }

                // Check bottom-left neighbor (only if not at left edge)
                if (x > 0 && y < gridHeight - 1) {
                    int bottomLeftIndex = (y + 1) * gridWidth + (x - 1);
                    checkCellPair(currentCell, cells[bottomLeftIndex], collisionFunc);
                }
            }
        }
    }

    void checkCellPair(std::vector<prtcl::Particle*>& cell1,
                       std::vector<prtcl::Particle*>& cell2,
                       std::function<void(prtcl::Particle*, prtcl::Particle*)> collisionFunc) {
        for (auto p1 : cell1) {
            for (auto p2 : cell2) {
                collisionFunc(p1, p2);
            }
        }
    }

    // void draw(sf::RenderWindow& window) {
    //     // Draw grid lines
    //     sf::VertexArray gridLines(sf::Lines);
    //
    //     // Vertical lines
    //     for (int x = 0; x <= gridWidth; x++) {
    //         float posX = x * cellSize;
    //         gridLines.append(sf::Vertex(sf::Vector2f(posX, 0), sf::Color(100, 100, 100, 80)));
    //         gridLines.append(sf::Vertex(sf::Vector2f(posX, gridHeight * cellSize), sf::Color(100, 100, 100, 80)));
    //     }
    //
    //     // Horizontal lines
    //     for (int y = 0; y <= gridHeight; y++) {
    //         float posY = y * cellSize;
    //         gridLines.append(sf::Vertex(sf::Vector2f(0, posY), sf::Color(100, 100, 100, 80)));
    //         gridLines.append(sf::Vertex(sf::Vector2f(gridWidth * cellSize, posY), sf::Color(100, 100, 100, 80)));
    //     }
    //
    //     window.draw(gridLines);
    //
    //     // Highlight cells with particles
    //     for (int y = 0; y < gridHeight; y++) {
    //         for (int x = 0; x < gridWidth; x++) {
    //             int cellIndex = y * gridWidth + x;
    //             if (!cells[cellIndex].empty()) {
    //                 sf::RectangleShape cellRect;
    //                 cellRect.setPosition(x * cellSize, y * cellSize);
    //                 cellRect.setSize(sf::Vector2f(cellSize, cellSize));
    //
    //                 // Color based on particle count
    //                 int count = cells[cellIndex].size();
    //                 int intensity = std::min(255, 50 + count * 25);
    //                 cellRect.setFillColor(sf::Color(0, intensity, intensity, 100));
    //
    //                 window.draw(cellRect);
    //             }
    //         }
    //     }
    // }
};
