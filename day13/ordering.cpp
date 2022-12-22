#include <cassert>
#include <iostream>
#include <stack>
#include <vector>

enum class ElementType {
    INTEGER,
    LIST,
};

struct Element {
    explicit Element() : parent(nullptr), tag(ElementType::INTEGER) {}

    Element* parent;
    ElementType tag;

    // These could be in a union, but avoiding complexity with non-trivial constructors.
    size_t integer;
    std::vector<Element> list;
};

enum class Comparison {
    EQUAL,
    INVALID,
    VALID,
};

[[maybe_unused]]
void printElement(const Element& element) {
    switch (element.tag) {
        case ElementType::INTEGER:
            std::cout << element.integer;
            break;
        case ElementType::LIST:
            std::cout << "[";
            for (size_t i = 0; i < element.list.size(); ++i) {
                printElement(element.list[i]);
                if (i < element.list.size() - 1) {
                    std::cout << ",";
                }
            }
            std::cout << "]";
            break;
    }
}

Element parsePacket(const std::string& input) {
    Element root;
    root.tag = ElementType::LIST;

    Element* current = &root;

    assert(input.size() >= 2 && input[0] == '[');
    size_t index = 1;
    while (index < input.size()) {
        assert(current->tag == ElementType::LIST);
        
        if (input[index] == '[') {
            current->list.emplace_back();
            Element& new_element = current->list.back();
            new_element.parent = current;
            new_element.tag = ElementType::LIST;
            current = &current->list.back();
            ++index;
        } else if (input[index] == ']') {
            assert(current == &root || current->parent != nullptr);
            current = current->parent;
            ++index;
        } else {
            auto end = std::min(input.find(",", index), input.find("]", index));
            assert(end != std::string::npos);

            if (index != end) {
                size_t integer = std::stoi(input.substr(index, end - index));
                current->list.emplace_back();
                Element& new_element = current->list.back();
                new_element.parent = current;
                new_element.tag = ElementType::INTEGER;
                new_element.integer = integer;
                index = end;
            }

            // Skip commas.
            if (end < input.size() && input[end] != ']') {
                ++index;
            }
        }
    }

    return root;
}

bool parsePair(std::istream& input, Element& left, Element& right) {
    std::string line;

    std::getline(input, line);
    if (line.empty()) {
        return false;
    }
    left = parsePacket(line);

    std::getline(input, line);
    if (line.empty()) {
        return false;
    }
    right = parsePacket(line);

    // Eat blank line.
    std::getline(input, line);
    return true;
}

Comparison isListValid(const std::vector<Element>& left_list, const std::vector<Element>& right_list) {
    for (size_t i = 0; i < left_list.size(); ++i) {
        // Left list should be same size as right, or shorter.
        if (i >= right_list.size()) {
            return Comparison::INVALID;
        }

        const Element& left = left_list[i];
        const Element& right = right_list[i];

        if (left.tag == ElementType::INTEGER && right.tag == ElementType::INTEGER) {
            if (left.integer == right.integer) {
                continue;
            }
            return left.integer < right.integer ? Comparison::VALID : Comparison::INVALID;
        } else if (left.tag == ElementType::LIST && right.tag == ElementType::LIST) {
            Comparison sublist_comp = isListValid(left.list, right.list);
            if (sublist_comp == Comparison::EQUAL) {
                continue;
            }
            return sublist_comp;
        } else {
            Comparison sublist_comp = Comparison::EQUAL;
            std::vector<Element> hoisted_list;

            if (left.tag == ElementType::INTEGER) {
                hoisted_list.push_back(left);
                sublist_comp = isListValid(hoisted_list, right.list);
            } else {
                hoisted_list.push_back(right);
                sublist_comp = isListValid(left.list, hoisted_list);
            }

            if (sublist_comp == Comparison::EQUAL) {
                continue;
            }
            return sublist_comp;
        }
    }
    return left_list.size() == right_list.size() ? Comparison::EQUAL : Comparison::VALID;
}

bool isPairValid(const Element& left, const Element& right) {
    assert(left.tag == ElementType::LIST);
    assert(right.tag == ElementType::LIST);
    auto result = isListValid(left.list, right.list);
    assert(result != Comparison::EQUAL);
    return result == Comparison::VALID;
}

int main() {
    size_t index = 1;
    size_t sum = 0;
    Element left;
    Element right;
    while (parsePair(std::cin, left, right)) {
        bool valid = isPairValid(left, right);
        std::cout << "Compare ";
        printElement(left);
        std::cout << " to ";
        printElement(right);
        std::cout << std::endl << " is " << (valid ? "VALID" : "INVALID") << std::endl;
        if (valid) {
            sum += index;
        }
        index++;
    }

    std::cout << std::endl;
    std::cout << "Sum of valid packet pair indices: " << sum << std::endl;
    return 0;
}
