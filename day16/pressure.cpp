#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using valve_name_t = uint32_t;

struct Valve {
    explicit Valve() : name(0), flow_rate(0) {}

    valve_name_t name;
    size_t flow_rate;
    std::unordered_set<valve_name_t> tunnels;
};

const char* name_to_string(const valve_name_t* name) {
    return reinterpret_cast<const char*>(name);
}

void string_to_name(valve_name_t* valve_name, const std::string& name) {
    assert(name.size() == 2);
    strncpy(reinterpret_cast<char*>(valve_name), name.c_str(), 4);
}

void parse_edges(const std::string& in, Valve& v) {
    assert(!in.empty());

    valve_name_t name;
    size_t begin = 0;
    size_t end = in.find(',');
    while (end != std::string::npos) {
        string_to_name(&name, in.substr(begin, end - begin));
        v.tunnels.insert(name);

        begin = end + 2;
        end = in.find(',', begin);
    }
    string_to_name(&name, in.substr(begin));
    v.tunnels.insert(name);
}

void parse_vertices(std::istream& in, std::unordered_map<valve_name_t, Valve>& valves) {
    std::string line;
    size_t begin;
    size_t end;
    valve_name_t valve_name;
    while (std::getline(in, line)) {
        begin = line.find(' ') + 1;
        end = line.find(' ', begin);
        std::string name = line.substr(begin, end - begin);
        string_to_name(&valve_name, name);

        assert(valves.find(valve_name) == valves.end());
        valves.emplace(std::piecewise_construct, std::tuple<valve_name_t>(valve_name), std::tuple<>());
        Valve& v = valves[valve_name];
        v.name = valve_name;

        begin = line.find('=') + 1;
        end = line.find(';', begin);
        v.flow_rate = static_cast<size_t>(std::stoi(line.substr(begin, end - begin)));

        begin = line.find("valves");
        if (begin == std::string::npos) {
            begin = line.find("valve");
        }
        assert(begin != std::string::npos);
        begin = line.find(' ', begin) + 1;
        parse_edges(line.substr(begin), v);
    }
}

size_t lower_bound(
        const std::unordered_map<valve_name_t, Valve>& valves,
        const std::unordered_set<valve_name_t>& already_opened,
        valve_name_t starting,
        size_t minutes_left
) {
    size_t pressure_released = 0;
    valve_name_t current = starting;

    std::unordered_set<valve_name_t> opened;
    for (valve_name_t name : already_opened) {
        opened.insert(name);
    }

    for (int64_t i = minutes_left; i > 1; --i) {
        const Valve& current_valve = valves.find(current)->second;

        if (current_valve.flow_rate > 0) {
            opened.insert(current_valve.name);
            pressure_released += i * current_valve.flow_rate;
        }

        bool found = false;
        size_t best_next_score = 0;
        valve_name_t best_next_name;
        for (valve_name_t next : current_valve.tunnels) {
            if (opened.find(next) != opened.end()) {
                continue;
            }
            size_t next_flow_rate = valves.find(next)->second.flow_rate;
            if (!found || next_flow_rate > best_next_score) {
                best_next_name = next;
                best_next_score = next_flow_rate;
                found = true;
            }
        }
        if (!found) {
            break;
        }

        // Go to next node.
        current = best_next_name;
    }
    
    return pressure_released;
}

size_t upper_bound(
        const std::unordered_map<valve_name_t, Valve>& valves,
        const std::unordered_set<valve_name_t>& already_opened,
        size_t minutes_left
) {
    std::vector<size_t> scores;
    for (const auto& pair : valves) {
        if (already_opened.find(pair.first) != already_opened.end()) {
            continue;
        }
        scores.push_back(pair.second.flow_rate);
    }
    std::sort(scores.begin(), scores.end(), std::greater<size_t>());

    size_t pressure_released = 0;
    for (int64_t i = 0; i < std::min(scores.size(), minutes_left / 2); ++i) {
        if (scores[i] == 0) {
            break;
        }
        pressure_released += scores[i] * (minutes_left - i * 2);
    }
    return pressure_released;
}

size_t search(
    const std::unordered_map<valve_name_t, Valve>& valves,
    const std::unordered_set<valve_name_t>& opened,
    valve_name_t current,
    size_t minutes_left,
    size_t lbound
) {
    if (minutes_left <= 1) {
        return 0;
    } else if (upper_bound(valves, opened, minutes_left) < lbound) {
        return 0;
    }
    const Valve& current_valve = valves.find(current)->second;

    size_t best_score = lbound;

    for (valve_name_t next : current_valve.tunnels) {
        size_t closed_score = search(valves, opened, next, minutes_left - 1, lbound);
        if (closed_score > best_score) {
            best_score = closed_score;
        }
    }

    // Valve is unopened and potentially worth opening.
    if (current_valve.flow_rate > 0 && opened.find(current) == opened.end()) {
        std::unordered_set<valve_name_t> new_opened;
        for (valve_name_t name : opened) {
            new_opened.insert(name);
        }
        new_opened.insert(current);
        size_t opening_score = (minutes_left - 1) * current_valve.flow_rate;
        size_t new_lbound = lower_bound(valves, new_opened, current, minutes_left - 2);

        for (valve_name_t next : current_valve.tunnels) {
            size_t score = opening_score + search(valves, new_opened, next, minutes_left - 2, new_lbound);
            if (score > best_score) {
                best_score = score;
            }
        }
    }

    return best_score;
}

size_t best_path(const std::unordered_map<valve_name_t, Valve>& valves) {
    valve_name_t starting_valve;
    string_to_name(&starting_valve, "AA");

    std::unordered_set<valve_name_t> opened;
    size_t lbound = lower_bound(valves, opened, starting_valve, 5);
    return search(valves, opened, starting_valve, 6, lbound);
}

int main() {
    std::unordered_map<valve_name_t, Valve> valves;
    parse_vertices(std::cin, valves);
    size_t pressure_released = best_path(valves);

    std::cout << "Released pressure: " << pressure_released << std::endl;
    return 0;
}
