#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

struct ViewScores {
    size_t up = 0;
    size_t down = 0;
    size_t left = 0;
    size_t right = 0;
};

struct Context {
    size_t width;
    size_t height;
    std::vector<uint8_t> tree_grid;
    std::vector<ViewScores> view_scores;
};

[[maybe_unused]]
void print_grid(const Context& context) {
    for (size_t y = 0; y < context.height; ++y) {
        for (size_t x = 0; x < context.width; ++x) {
            size_t tree_height = static_cast<size_t>(context.tree_grid[y * context.width + x]);
            auto view_scores = context.view_scores[y * context.width + x];
            std::cout << tree_height << "("
                << view_scores.up << ","
                << view_scores.down << ","
                << view_scores.left << ","
                << view_scores.right << ") ";
        }
        std::cout << std::endl;
    }
}

uint8_t get_tree_height(const Context& context, size_t x, size_t y) {
    return context.tree_grid[y * context.width + x];
}

size_t get_up_view(const Context& context, uint8_t my_tree_height, size_t x, size_t y) {
    if (y == 0) {
        return 0;
    }

    size_t distance = 0;
    for (size_t i = y; i > 0; --i) {
        // Note: Reverse iterator i ranges from y=>1, not y-1=>0!
        ++distance;
        if (get_tree_height(context, x, i - 1) >= my_tree_height) {
            break;
        }
    }
    return distance;
}

size_t get_down_view(const Context& context, uint8_t my_tree_height, size_t x, size_t y) {
    if (y == context.height - 1) {
        return 0;
    }

    size_t distance = 0;
    for (size_t i = y + 1; i < context.height; ++i) {
        ++distance;
        if (get_tree_height(context, x, i) >= my_tree_height) {
            break;
        }
    }
    return distance;
}

size_t get_left_view(const Context& context, uint8_t my_tree_height, size_t x, size_t y) {
    if (x == 0) {
        return 0;
    }

    size_t distance = 0;
    for (size_t i = x; i > 0; --i) {
        // Note: Reverse iterator i ranges from y=>1, not y-1=>0!
        ++distance;
        if (get_tree_height(context, i - 1, y) >= my_tree_height) {
            break;
        }
    }
    return distance;
}

size_t get_right_view(const Context& context, uint8_t my_tree_height, size_t x, size_t y) {
    if (x == context.width - 1) {
        return 0;
    }

    size_t distance = 0;
    for (size_t i = x + 1; i < context.width; ++i) {
        ++distance;
        if (get_tree_height(context, i, y) >= my_tree_height) {
            break;
        }
    }
    return distance;
}

void compute_view_distance(Context& context) {
    for (size_t y = 0; y < context.height; ++y) {
        for (size_t x = 0; x < context.width; ++x) {
            size_t tree_height = context.tree_grid[y * context.width + x];
            // View scores vector will not change size at this point.
            auto view_score = &context.view_scores[y * context.width + x];
            view_score->up = get_up_view(context, tree_height, x, y);
            view_score->down = get_down_view(context, tree_height, x, y);
            view_score->left = get_left_view(context, tree_height, x, y);
            view_score->right = get_right_view(context, tree_height, x, y);
        }
    }
}

size_t compute_scores(Context& context) {
    size_t highest_score = 0;
    for (auto it = context.view_scores.begin(); it != context.view_scores.end(); ++it) {
        size_t score = it->up * it->down * it->left * it->right;
        if (score > highest_score) {
            highest_score = score;
        }
    }
    return highest_score;
}

void read_grid_input(Context& context, std::istream& input) {
    std::string line;
    while (std::getline(input, line)) {
        ++context.height;
        if (context.width == 0) {
            context.width = line.size();
        }

        for (size_t i = 0; i < line.size(); ++i) {
            auto tree_height = std::stoi(std::string{line[i]});
            assert(tree_height >= 0 && tree_height <= 9);
            context.tree_grid.push_back(static_cast<uint8_t>(tree_height));
        }
    }

    // Zero-initialize view scores.
    context.view_scores.reserve(context.tree_grid.size());
    for (size_t i = 0; i < context.tree_grid.size(); ++i) {
        context.view_scores.emplace_back();
    }
}

int main() {
    Context context{};
    read_grid_input(context, std::cin);
    compute_view_distance(context);
    // print_grid(context);
    size_t score = compute_scores(context);
    std::cout << "Highest scenic score is: " << score << std::endl;
    return 0;
}