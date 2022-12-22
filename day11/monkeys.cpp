#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

// Number of simulation rounds.
constexpr size_t NUM_ROUNDS = 10000;

// List item delimiter.
constexpr auto LIST_DELIM = ", ";
constexpr size_t LIST_DELIM_SIZE = 2;

enum class OperationType {
    ADD_IMM,
    ADD_OLD,
    MUL_IMM,
    MUL_OLD,
};

struct Operation {
    OperationType ty;
    int64_t imm;

    int64_t operator()(int64_t val) const {
        switch (ty) {
            case OperationType::ADD_IMM:
                return val + imm;
            case OperationType::ADD_OLD:
                return val + val;
            case OperationType::MUL_IMM:
                return val * imm;
            case OperationType::MUL_OLD:
                return val * val;
        }
    }
};

class Monkey {
  public:
    Monkey(
        const std::string& name,
        std::vector<size_t>&& starting_items,
        const Operation& op,
        int64_t test,
        size_t true_cond,
        size_t false_cond
    ) :
        name(name),
        items(starting_items),
        op(op),
        test(test),
        true_cond(true_cond),
        false_cond(false_cond),
        lcd(1),
        inspection_count(0)
    {}

    const std::string& getName() const {
        return name;
    }

    int64_t getTestCondition() const {
        return test;
    }

    size_t getInspectionCount() const {
        return inspection_count;
    }

    void setLCD(int64_t lcd) {
        this->lcd = lcd;
    }

    void receiveThrownItem(size_t worry_level) {
        items.push_back(worry_level);
    }

    void inspectItems(std::vector<Monkey>& monkies) {
        // Monkey will not throw items to itself.
        for (size_t worry_level : items) {
            inspectItem(monkies, worry_level);
        }
        items.clear();
    }

  private:
    std::string name;
    std::vector<size_t> items;
    Operation op;
    int64_t test;
    size_t true_cond;
    size_t false_cond;

    int64_t lcd;
    size_t inspection_count;

    void inspectItem(std::vector<Monkey>& monkies, size_t worry_level) {
        inspection_count++;
        worry_level = op(worry_level);
        worry_level %= lcd;

        if (worry_level % test == 0) {
            monkies[true_cond].receiveThrownItem(worry_level);
        } else {
            monkies[false_cond].receiveThrownItem(worry_level);
        }
    }
};

Operation parseOperation(const std::string& op_str) {
    assert(op_str.size() >= 3);

    Operation op;
    bool add = (op_str[0] == '+');
    auto imm_str = op_str.substr(2);

    if (imm_str == "old") {
        op.ty = (add ? OperationType::ADD_OLD : OperationType::MUL_OLD);
    } else {
        op.ty = (add ? OperationType::ADD_IMM : OperationType::MUL_IMM);
        op.imm = std::stoi(imm_str);
    }

    return op;
}

std::vector<size_t> parseItems(const std::string& items_str) {
    std::vector<size_t> items;

    size_t start = 0;
    size_t end = items_str.find(LIST_DELIM);

    if (end == std::string::npos) {
        items.push_back(static_cast<size_t>(std::stoi(items_str)));
    } else {
        while (end != std::string::npos)
        {
            // Add parsed item.
            items.push_back(static_cast<size_t>(std::stoi(
                items_str.substr(start, end - start))));

            // Find next.
            start = end + LIST_DELIM_SIZE;
            end = items_str.find(LIST_DELIM, start);
        }
        
        // Add final item in list.
        items.push_back(static_cast<size_t>(std::stoi(
                items_str.substr(start, end))));
    }

    return items;
}

Monkey parseMonkey(const std::string& name, std::array<std::string, 5>& input) {
    // Extract rules.
    auto starting_items_str = input[0].substr(18);
    auto operation_str = input[1].substr(23);
    auto test_val_str = input[2].substr(21);
    auto true_cond_str = input[3].substr(29);
    auto false_cond_str = input[4].substr(30);

    // Parse rules.
    auto starting_items = parseItems(starting_items_str);
    auto op = parseOperation(operation_str);
    int64_t test = std::stoi(test_val_str);
    size_t true_cond = static_cast<size_t>(std::stoi(true_cond_str));
    size_t false_cond = static_cast<size_t>(std::stoi(false_cond_str));

    Monkey monkey(name, std::move(starting_items), op, test, true_cond, false_cond);
    return monkey;
}

void doRound(std::vector<Monkey>& monkies) {
    for (auto& monkey : monkies) {
        monkey.inspectItems(monkies);
    }
}

void printInspectionCounts(size_t round, const std::vector<Monkey>& monkies) {
    std::cout << "== Round " << round << " ==" << std::endl;
    for (const auto& monkey : monkies) {
        std::cout << monkey.getName()
            << " inspected items " << monkey.getInspectionCount()
            << " times." << std::endl;
    }
    std::cout << std::endl;
}

int main() {
    std::vector<Monkey> monkies;

    std::string line;
    while (std::getline(std::cin, line)) {
        // Skip blank lines.
        if (line.size() == 0) {
            continue;
        }
        assert(line.size() >= 8);

        auto monkey_name = line.substr(0, line.size() - 1);
        std::array<std::string, 5> input;
        for (size_t i = 0; i < 5; ++i) {
            std::getline(std::cin, input[i]);
        }
        monkies.push_back(parseMonkey(monkey_name, input));
    }

    // Must have at least 2 monkies to calculate monkey business.
    assert(monkies.size() >= 2);

    // Compute LCD for all monkies.
    int64_t lcd = 1;
    for (const auto& monkey : monkies) {
        lcd *= monkey.getTestCondition();
    }

    // Set LCD for all monkies.
    for (auto& monkey : monkies) {
        monkey.setLCD(lcd);
    }

    for (size_t round = 1; round <= NUM_ROUNDS; ++round) {
        doRound(monkies);
        if (round == 1 || round == 20 || round == 1000 || round == 10000) {
            printInspectionCounts(round, monkies);
        }
    }

    // Print results.
    std::vector<size_t> inspection_counts;
    inspection_counts.reserve(monkies.size());
    for (const auto& monkey : monkies) {
        inspection_counts.push_back(monkey.getInspectionCount());
    }

    // Sort inspection counts, high to low.
    std::sort(inspection_counts.begin(),
        inspection_counts.end(),
        std::greater<size_t>());

    size_t monkey_business = inspection_counts[0] * inspection_counts[1];

    std::cout << "Monkey business: " << monkey_business << std::endl;

    return 0;
}
