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

[[maybe_unused]]
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
    return isListValid(left.list, right.list) == Comparison::VALID;
}

Element createDistressPacket(size_t distress_integer) {
    Element num;
    num.tag = ElementType::INTEGER;
    num.integer = distress_integer;

    Element inner;
    inner.tag = ElementType::LIST;
    inner.list.push_back(num);

    Element root;
    root.tag = ElementType::LIST;
    root.list.push_back(inner);

    return root;
}

bool isDividerPacket(const Element& packet, const Element& divider) {
    assert(packet.tag == ElementType::LIST);
    assert(divider.tag == ElementType::LIST);
    return isListValid(divider.list, packet.list) == Comparison::EQUAL;
}

int main() {
    auto distress2 = createDistressPacket(2);
    auto distress6 = createDistressPacket(6);

    std::vector<Element> packets;
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            continue;
        }
        packets.push_back(parsePacket(line));
    }
    packets.push_back(distress2);
    packets.push_back(distress6);

    // Get divider packet indices.
    std::sort(packets.begin(), packets.end(), isPairValid);

    size_t key = 0;
    for (size_t i = 0; i < packets.size(); ++i) {
        if (isDividerPacket(packets[i], distress2)) {
            key = (i + 1);
        } else if (isDividerPacket(packets[i], distress6)) {
            key *= (i + 1);
            break;
        }
    }

    std::cout << std::endl;
    std::cout << "Decoder key: " << key << std::endl;
    return 0;
}
