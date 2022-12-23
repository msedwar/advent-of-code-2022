#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

enum class Tile : uint8_t {
    AIR,
    ROCK,
    SAND_SOURCE,
    SAND,
};

struct Coordinate {
    size_t x;
    size_t y;
};

class Cave {
  public:
    Cave(size_t width, size_t height) :
        width(width),
        height(0),
        max_width(501),
        min_width(499)
    {
        assert(width > 500);
        assert(height > 1);

        addRows(height);
        map[500] = Tile::SAND_SOURCE;
    }

    void drawLine(Coordinate start, Coordinate end) {
        if (start.x == end.x) {             // Vertical.
            if (start.y > end.y) {
                std::swap(start, end);
            }

            if (start.x < min_width) {
                min_width = start.x;
            }
            if (start.x > max_width) {
                max_width = start.x;
            }
            if (end.y >= height) {
                addRows(end.y - height + 2);
            }

            for (size_t i = start.y; i <= end.y; ++i) {
                map[i * width + start.x] = Tile::ROCK;
            }

        } else if (start.y == end.y) {      // Horizontal.
            if (start.x > end.x) {
                std::swap(start, end);
            }

            if (start.x < min_width) {
                min_width = start.x;
            }
            if (end.x > max_width) {
                max_width = end.x;
            }
            if (start.y >= height) {
                addRows(start.y - height + 2);
            }

            for (size_t i = start.x; i <= end.x; ++i) {
                map[start.y * width + i] = Tile::ROCK;
            }

        } else {
            throw std::runtime_error("Cannot draw diagonal lines");
        }
    }

    void print(std::ostream& out) const {
        for (size_t y = 0; y < height; ++y) {
            for (size_t x = min_width - 1; x <= max_width + 1; ++x) {
                switch (map[y * width + x]) {
                    case Tile::AIR:
                        out << ".";
                        break;
                    case Tile::ROCK:
                        out << "#";
                        break;
                    case Tile::SAND_SOURCE:
                        out << "+";
                        break;
                    case Tile::SAND:
                        out << "o";
                        break;
                }
            }
            out << std::endl;
        }
    }

    bool spawnSand() {
        Coordinate position = Coordinate{500, 0};
        if (getTileAt(position.x, position.y) != Tile::SAND_SOURCE) {
            return false;
        }

        while (position.y + 1 < height) {
            if (isTileEmpty(position.x, position.y + 1)) {
                position.y++;
            } else if (isTileEmpty(position.x - 1, position.y + 1)) {
                position.x--;
                position.y++;

                if (position.x < min_width) {
                    min_width = position.x;
                }
            } else if (isTileEmpty(position.x + 1, position.y + 1)) {
                position.x++;
                position.y++;

                if (position.x > max_width) {
                    max_width = position.x;
                }
            } else {
                map[position.y * width + position.x] = Tile::SAND;
                return true;
            }
        }
        map[position.y * width + position.x] = Tile::SAND;
        return true;
    }

private:
    bool isTileEmpty(size_t x, size_t y) const {
        switch (getTileAt(x, y)) {
            case Tile::AIR:
                return true;
            case Tile::ROCK:
                return false;
            case Tile::SAND:
                return false;
            case Tile::SAND_SOURCE:
                return true;
        }
    }

    Tile getTileAt(size_t x, size_t y) const {
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);
        return map[y * width + x];
    }

    void addRows(size_t count) {
        height += count;
        map.reserve(width * height);
        map.insert(map.end(), count * width, Tile::AIR);
        assert(map.size() == width * height);
    }

    size_t width;
    size_t height;

    size_t max_width;
    size_t min_width;

    std::vector<Tile> map;
};

Coordinate parseCoordinate(const std::string& input) {
    size_t index = input.find(',');
    assert(index != std::string::npos);
    size_t x = static_cast<size_t>(std::stoi(input.substr(0, index)));
    size_t y = static_cast<size_t>(std::stoi(input.substr(index + 1)));
    return Coordinate{x, y};
}

std::vector<Coordinate> parseLine(const std::string& line) {
    std::vector<Coordinate> points;
    size_t begin = 0;
    size_t end = line.find(' ');
    while (end != std::string::npos) {
        points.push_back(parseCoordinate(line.substr(begin, end - begin)));
        begin = end + 4;
        end = line.find(' ', begin);
    }
    points.push_back(parseCoordinate(line.substr(begin)));
    return points;
}

int main() {
    Cave cave(1024, 3);
    std::string line;
    while (std::getline(std::cin, line)) {
        auto points = parseLine(line);
        assert(points.size() >= 2);
        Coordinate start = points[0];
        for (size_t i = 1; i < points.size(); ++i) {
            cave.drawLine(start, points[i]);
            start = points[i];
        }
    }

    size_t counter = 0;
    while (cave.spawnSand()) {
        ++counter;
    }

    cave.print(std::cout);

    std::cout << std::endl;
    std::cout << "Sand at rest: " << counter << std::endl;
    return 0;
}
