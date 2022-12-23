#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

constexpr int64_t MAX_COORD = 4000000;

// Quick and dirty pair hashing function.
struct HashPair {
    size_t operator()(const std::pair<int64_t, int64_t>& pair) const {
        return pair.first ^ pair.second;
    }
};

struct Sensor {
    Sensor(int64_t x, int64_t y, size_t range) : x(x), y(y), range(range) {}

    int64_t x;
    int64_t y;
    size_t range;
};

struct Bounds {
    int64_t minX;
    int64_t maxX;
    int64_t minY;
    int64_t maxY;
};

size_t manhattan_distance(int64_t ax, int64_t ay, int64_t bx, int64_t by) {
    return static_cast<size_t>(abs(ax - bx) + abs(ay - by));
}

void parse_sensors(
        std::istream& input,
        std::vector<Sensor>& sensors,
        std::unordered_set<std::pair<int64_t, int64_t>, HashPair>& beacons
) {
    std::string line;
    size_t begin;
    size_t end;
    while (std::getline(input, line)) {
        begin = line.find("x=") + 2;
        end = line.find(",", begin);
        assert(begin < end);
        int64_t sx = std::stoi(line.substr(begin, end - begin));

        begin = line.find("y=", end) + 2;
        end = line.find(":", begin);
        assert(begin < end);
        int64_t sy = std::stoi(line.substr(begin, end - begin));

        begin = line.find("x=", end) + 2;
        end = line.find(",", begin);
        assert(begin < end);
        int64_t bx = std::stoi(line.substr(begin, end - begin));

        begin = line.find("y=", end) + 2;
        assert(begin < line.size());
        int64_t by = std::stoi(line.substr(begin));

        sensors.emplace_back(sx, sy, manhattan_distance(sx, sy, bx, by));
        auto beacon = std::make_pair(bx, by);
        if (beacons.find(beacon) == beacons.end()) {
            beacons.insert(beacon);
        }
    }
}

void get_max_bounds(
        std::vector<Sensor>& sensors,
        Bounds& bounds
) {
    for (const auto& sensor : sensors) {
        if (sensor.x > bounds.maxX) {
            bounds.maxX = sensor.x;
        }
        if (sensor.y > bounds.maxY) {
            bounds.maxY = sensor.y;
        }
    }
}

int64_t tuning_freq(const Bounds& bounds, const std::vector<Sensor>& sensors) {
    bool found;
    for (int64_t y = bounds.minY; y < bounds.maxY; ++y) {
        for (int64_t x = bounds.minX; x < bounds.maxX; ++x) {
            found = true;
            for (const auto& sensor : sensors) {
                if (manhattan_distance(x, y, sensor.x, sensor.y) <= sensor.range) {
                    size_t y_dist = static_cast<size_t>(abs(sensor.y - y));
                    int64_t x_range = sensor.range - y_dist;
                    x = sensor.x + x_range;
                    found = false;
                    break;
                }
            }
            if (found) {
                return (x * MAX_COORD) + y;
            }
        }
    }
    return 0;
}

int main() {
    std::vector<Sensor> sensors;
    std::unordered_set<std::pair<int64_t, int64_t>, HashPair> beacons;
    parse_sensors(std::cin, sensors, beacons);

    Bounds bounds{0, 0, 0, 0};
    get_max_bounds(sensors, bounds);
    bounds.maxX = std::min(bounds.maxX, MAX_COORD);
    bounds.maxY = std::min(bounds.maxY, MAX_COORD);

    std::cout << tuning_freq(bounds, sensors) << std::endl;
    return 0;
}
