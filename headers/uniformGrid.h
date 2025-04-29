#include <functional>
#include "particle.h"

class UniformGrid {
public:
    int cellSize;
    int gridWidth, gridHeight;
    std::vector<std::vector<prtcl::Particle *> > cells;

    UniformGrid() {
    }

    UniformGrid(int width, int height, int cellSize)
        : cellSize(cellSize),
          gridWidth(width / cellSize + 1),
          gridHeight(height / cellSize + 1) {
        cells.resize(gridWidth * gridHeight);
    }

    void clear() {
        for (auto &cell: cells) {
            cell.clear();
        }
    }

    int getCellIndex(const sf::Vector2f &position) const {
        int cellX = static_cast<int>(position.x / cellSize);
        int cellY = static_cast<int>(position.y / cellSize);

        cellX = std::max(0, std::min(cellX, gridWidth - 1));
        cellY = std::max(0, std::min(cellY, gridHeight - 1));

        return cellY * gridWidth + cellX;
    }

    void insert(prtcl::Particle *particle) {
        int cellIndex = getCellIndex(particle->position);
        cells[cellIndex].push_back(particle);
    }
};
