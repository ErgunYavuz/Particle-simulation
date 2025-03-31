#include <functional>
#include <SFML/Graphics.hpp>

class UniformGrid {
    int cellSize;
    int gridWidth, gridHeight;
    std::vector<std::vector<int>> cells;  // Now storing indices instead of pointers

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

    void insert(int particleIndex, const sf::Vector2f& position) {
        int cellIndex = getCellIndex(position);
        cells[cellIndex].push_back(particleIndex);
    }

    void processCollisions(std::function<void(int, int)> collisionFunc) {
        // Check collisions within the same cell
        for (auto& cell : cells) {
            for (size_t i = 0; i < cell.size(); i++) {
                for (size_t j = i + 1; j < cell.size(); j++) {
                    collisionFunc(cell[i], cell[j]);
                }
            }
        }

        // Check with adjacent cells
        for (int y = 0; y < gridHeight; y++) {
            for (int x = 0; x < gridWidth; x++) {
                int cellIndex = y * gridWidth + x;
                std::vector<int>& currentCell = cells[cellIndex];

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

    void checkCellPair(std::vector<int>& cell1,
                       std::vector<int>& cell2,
                       std::function<void(int, int)> collisionFunc) {
        for (auto p1 : cell1) {
            for (auto p2 : cell2) {
                collisionFunc(p1, p2);
            }
        }
    }
};
