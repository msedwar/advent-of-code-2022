#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

static std::vector<std::vector<uint8_t>> tree_grid;
static std::vector<std::vector<bool>> visible;

[[maybe_unused]]
void print_grid() {
    for (size_t y = 0; y < tree_grid.size(); ++y) {
        for (size_t x = 0; x < tree_grid[0].size(); ++x) {
            std::cout << static_cast<uint32_t>(tree_grid[y][x]);
            std::cout << (visible[y][x] ? " " : "*");
            std::cout << " ";
        }
        std::cout << std::endl;
    }
}

bool is_tree_visible_up(size_t my_tree_height, size_t width, size_t height, size_t x, size_t y) {
    for (size_t i = y; i > 0; --i) {
        // Note: Reverse iterator i ranges from y=>1, not y-1=>0!

        if (tree_grid[i - 1][x] >= my_tree_height) {
            return false;
        }
    }
    return true;
}

bool is_tree_visible_down(size_t my_tree_height, size_t width, size_t height, size_t x, size_t y) {
    for (size_t i = y + 1; i < height; ++i) {
        if (tree_grid[i][x] >= my_tree_height) {
            return false;
        }
    }
    return true;
}

bool is_tree_visible_left(size_t my_tree_height, size_t width, size_t height, size_t x, size_t y) {
    for (size_t i = x; i > 0; --i) {
        // Note: Reverse iterator i ranges from x=>1, not x-1=>0!
        if (tree_grid[y][i - 1] >= my_tree_height) {
            return false;
        }
    }
    return true;
}

bool is_tree_visible_right(size_t my_tree_height, size_t width, size_t height, size_t x, size_t y) {
    for (size_t i = x + 1; i < width; ++i) {
        if (tree_grid[y][i] >= my_tree_height) {
            return false;
        }
    }
    return true;
}

bool is_tree_visible(size_t width, size_t height, size_t x, size_t y) {
    size_t my_tree_height = tree_grid[y][x];
    bool is_visible = false;
    is_visible |= is_tree_visible_up(my_tree_height, width, height, x, y);
    is_visible |= is_tree_visible_down(my_tree_height, width, height, x, y);
    is_visible |= is_tree_visible_left(my_tree_height, width, height, x, y);
    is_visible |= is_tree_visible_right(my_tree_height, width, height, x, y);
    return is_visible;
}

void check_all_visible() {
    size_t height = tree_grid.size();
    assert(height >= 2);
    size_t width = tree_grid[0].size();
    assert(width >= 2);
    
    // Trees on border will always be visible.
    for (size_t x = 0; x < width; ++x) {
        visible[0][x] = true;
        visible[height - 1][x] = true;
    }
    // Skip first and last rows, checked already.
    for (size_t y = 1; y < height - 1; ++y) {
        visible[y][0] = true;
        visible[y][width - 1] = true;
    }

    for (size_t y = 1; y < height - 1; ++y) {
        for (size_t x = 1; x < width - 1; ++x) {
            visible[y][x] = is_tree_visible(width, height, x, y);
        }
    }
}

void read_grid_input() {
    std::string line;
    while (std::getline(std::cin, line)) {
        std::vector<uint8_t> row;
        for (size_t i = 0; i < line.size(); ++i) {
            auto height = std::stoi(std::string{line[i]});
            assert(height >= 0 && height <= 9);
            row.push_back(static_cast<uint8_t>(height));
        }

        // Cheap vector move.
        tree_grid.push_back(std::move(row));
    }

    // Initialize visible table.
    visible.reserve(tree_grid.size());
    for (size_t y = 0; y < tree_grid.size(); ++y) {
        std::vector<bool> row;
        row.reserve(tree_grid[0].size());
        for (size_t x = 0; x < tree_grid[0].size(); ++x) {
            row.push_back(false);
        }

        // Cheap vector move.
        visible.push_back(std::move(row));
    }
}

size_t count_visible() {
    size_t num_visible = 0;
    for (auto row_it = visible.begin(); row_it != visible.end(); ++row_it) {
        auto row = *row_it;
        for (auto col_it = row.begin(); col_it != row.end(); ++col_it) {
            if (*col_it) {
                ++num_visible;
            }
        }
    }
    return num_visible;
}

int main() {
    read_grid_input();
    check_all_visible();
    // print_grid();
    size_t visible = count_visible();
    std::cout << visible << " trees are visible." << std::endl;
    return 0;
}
