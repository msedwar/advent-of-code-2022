#include <cassert>
#include <cstdint>
#include <iostream>
#include <unordered_set>
#include <vector>

static const auto hash_int64 = std::hash<int64_t>{};

// Position in a Cartesian grid.
// X increases left to right, Y increases bottom to top.
struct Position {
    Position(int32_t x, int32_t y) : x(x), y(y) {}

    int32_t x;
    int32_t y;

    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

// Position struct hash function.
struct HashPosition {
    size_t operator()(const Position& pos) const {
        int64_t val = static_cast<int64_t>(pos.x) << 32 | static_cast<int64_t>(pos.y);
        return hash_int64(val);
    }
};

// Direction in a Cartesian grid.
enum class Direction : uint32_t {
    UP,     // Increase Y.
    DOWN,   // Decrease Y.
    RIGHT,  // Increase X.
    LEFT,   // Decrease X.
};

class RopeSim {
  public:
    RopeSim(size_t num_segments) {
        assert(num_segments >= 2);
        this->segments.reserve(num_segments);
        for (size_t i = 0; i < num_segments; ++i) {
            this->segments.emplace_back(0, 0);
        }
        this->visited.insert(getTail());
    }

    void moveHead(Direction dir) {
        switch (dir) {
            case Direction::UP:
                getHead().y++;
                break;
            case Direction::DOWN:
                getHead().y--;
                break;
            case Direction::RIGHT:
                getHead().x++;
                break;
            case Direction::LEFT:
                getHead().x--;
                break;
        }

        // Consequently move rest of rope.
        simulateStep();
    }

    size_t getNumPositionsVisited() {
        return visited.size();
    }

  private:
    Position& getHead() {
        return segments[0];
    }

    Position& getTail() {
        return segments[segments.size() - 1];
    }

    bool areTouching(const Position& head, const Position& tail) {
        return abs(head.x - tail.x) <= 1 && abs(head.y - tail.y) <= 1;
    }

    void moveSegment(size_t index) {
        assert(index > 0 && index < segments.size());
        Position& head = segments[index - 1];
        Position& tail = segments[index];

        // Do nothing if head and tail are touching.
        if (areTouching(head, tail)) {
            return;
        }

        if (head.x == tail.x) {                 // Same column.
            assert(abs(head.y - tail.y) == 2);

            if (head.y > tail.y) {
                tail.y++;
            } else {
                tail.y--;
            }

        } else if (head.y == tail.y) {          // Same row.
            assert(abs(head.x - tail.x) == 2);

            if (head.x > tail.x) {
                tail.x++;
            } else {
                tail.x--;
            }

        } else if (head.x > tail.x) {           // Diagonal right.
            tail.x++;

            if (head.y > tail.y) {
                tail.y++;
            } else {
                tail.y--;
            }

        } else {                                // Diagonal left.
            tail.x--;

            if (head.y > tail.y) {
                tail.y++;
            } else {
                tail.y--;
            }
        }

        // Update tail position.
        assert(areTouching(head, tail));
    }

    void simulateStep() {
        for (size_t i = 1; i < segments.size(); ++i) {
            moveSegment(i);
        }

        Position& tail = getTail();
        if (visited.find(tail) == visited.end()) {
            visited.insert(tail);
        }
    }

    std::unordered_set<Position, HashPosition> visited;
    std::vector<Position> segments;
};

Direction parse_direction(char dir) {
    switch (dir) {
        case 'U':
            return Direction::UP;
        case 'D':
            return Direction::DOWN;
        case 'R':
            return Direction::RIGHT;
        case 'L':
            return Direction::LEFT;
    }
    std::cerr << "Invalid direction char: " << dir << std::endl;
    throw std::runtime_error{"Invalid direction char"};
}

void parse_movements(std::istream& input, RopeSim& sim) {
    std::string line;
    while (std::getline(input, line)) {
        assert(line.size() >= 3);
        Direction dir = parse_direction(line[0]);
        auto steps_str = line.substr(2);
        int32_t steps = std::stoi(steps_str);
        assert(steps > 0);
        for (uint32_t i = 0; i < static_cast<uint32_t>(steps); ++i) {
            sim.moveHead(dir);
        }
    }
}

int main() {
    RopeSim sim{10};
    parse_movements(std::cin, sim);

    std::cout << "Tail visited positions: " << sim.getNumPositionsVisited() << std::endl;
    return 0;
}
