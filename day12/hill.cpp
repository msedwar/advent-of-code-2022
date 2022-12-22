#include <cassert>
#include <climits>
#include <cstdint>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_set>
#include <vector>

static const auto hash_int32 = std::hash<int32_t>{};

struct Coordinate {
    Coordinate(int32_t x, int32_t y) : x(x), y(y) {}

    int32_t x;
    int32_t y;

    bool operator==(const Coordinate& other) const {
        return x == other.x && y == other.y;
    }
};

// Coordinate struct hash function.
struct HashCoordinate {
    size_t operator()(const Coordinate& pos) const {
        int64_t val = static_cast<int64_t>(pos.x) << 32 | static_cast<int64_t>(pos.y);
        return hash_int32(val);
    }
};

class Terrain {
  public:
    explicit Terrain() : width(0), height(0), start{0, 0}, end{0, 0} {}

    void addRow(const std::vector<uint8_t>& row) {
        if (width == 0) {
            width = row.size();
        }
        assert(width == row.size());
        height++;
        height_map.reserve(width * height);
        dist.reserve(width * height);
        for (auto h : row) {
            height_map.push_back(h);
            dist.push_back(UINT32_MAX);
        }
    }

    void setEndpoints(const Coordinate& start, const Coordinate& end) {
        this->start = start;
        this->end = end;
    }

    void markVisited(const Coordinate& coord, uint32_t distance) {
        assert(isValidCoord(coord));
        dist[coord.y * width + coord.x] = distance;
    }

    const Coordinate& getStart() const {
        return start;
    }

    const Coordinate& getEnd() const {
        return end;
    }

    bool isValidCoord(const Coordinate& coord) const {
        return coord.x >= 0 && coord.x < width
            && coord.y >= 0 && coord.y < height;
    }

    bool isVisited(const Coordinate& coord) const {
        assert(isValidCoord(coord));
        return dist[coord.y * width + coord.x] != UINT32_MAX;
    }

    size_t getHeight(const Coordinate& coord) const {
        assert(isValidCoord(coord));
        return static_cast<size_t>(height_map[coord.y * width + coord.x]);
    }

    uint32_t getDistance(const Coordinate& coord) const {
        assert(isValidCoord(coord));
        return dist[coord.y * width + coord.x];
    }

  private:
    size_t width;
    size_t height;
    std::vector<uint8_t> height_map;
    std::vector<uint32_t> dist;
    Coordinate start;
    Coordinate end;
};

Terrain parseTerrain(std::istream& input) {
    Terrain terrain;
    Coordinate start{-1, -1};
    Coordinate end{-1, -1};

    std::string line;
    size_t row = 0;
    while (std::getline(input, line)) {
        std::vector<uint8_t> row_heights;
        row_heights.reserve(line.size());
        for (size_t i = 0; i < line.size(); ++i) {
            if (line[i] == 'S') {
                start.x = static_cast<int32_t>(i);
                start.y = static_cast<int32_t>(row);
                row_heights.push_back(0);
            } else if (line[i] == 'E') {
                end.x = static_cast<int32_t>(i);
                end.y = static_cast<int32_t>(row);
                row_heights.push_back(25);
            } else {
                assert(line[i] >= 'a' && line[i] <= 'z');
                row_heights.push_back(static_cast<uint8_t>(line[i] - 'a'));
            }
        }
        terrain.addRow(row_heights);
        row++;
    }

    assert(terrain.isValidCoord(start));
    assert(terrain.isValidCoord(end));
    terrain.setEndpoints(start, end);
    return terrain;
}

void addNeighbor(
    Terrain& terrain,
    std::queue<Coordinate>& queue,
    const Coordinate& coord,
    size_t height,
    uint32_t distance
) {
    if (!terrain.isValidCoord(coord)
        || terrain.isVisited(coord)
        || terrain.getHeight(coord) > height + 1) {
        return;
    }

    terrain.markVisited(coord, distance + 1);
    queue.push(coord);
}

// A* path search would be faster, but BFS is simpler to implement.
size_t shortestPath(Terrain& terrain) {
    terrain.markVisited(terrain.getStart(), 0);

    std::queue<Coordinate> queue;
    queue.push(terrain.getStart());

    while (!queue.empty()) {
        Coordinate current = queue.front();
        queue.pop();
        if (current == terrain.getEnd()) {
            break;
        }

        auto height = terrain.getHeight(current);
        auto distance = terrain.getDistance(current);

        // Add neighbors.
        addNeighbor(terrain, queue, Coordinate{current.x, current.y + 1}, height, distance);
        addNeighbor(terrain, queue, Coordinate{current.x, current.y - 1}, height, distance);
        addNeighbor(terrain, queue, Coordinate{current.x + 1, current.y}, height, distance);
        addNeighbor(terrain, queue, Coordinate{current.x - 1, current.y}, height, distance);
    }

    uint32_t distance = terrain.getDistance(terrain.getEnd());
    assert(distance != UINT32_MAX);
    return static_cast<size_t>(distance);
}

int main() {
    Terrain terrain = parseTerrain(std::cin);
    auto steps = shortestPath(terrain);

    std::cout << "Shortest path is " << steps << " steps" << std::endl;
    return 0;
}
