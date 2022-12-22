#include <cassert>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>

class Crt {
  public:
    Crt() : regX(1), cycle(1) {}

    void addX(int64_t v) {
        doCycle();
        doCycle();
        regX += v;
    }

    void noop() {
        doCycle();
    }

    const std::string getDisplay() {
        return display.str();
    }

  private:
    size_t getDrawColumn() {
        return (cycle - 1) % 40;
    }

    bool isSpriteLit() {
        int64_t column = getDrawColumn();
        int64_t spriteL = regX - 1;
        int64_t spriteR = regX + 1;
        return column >= spriteL && column <= spriteR;
    }

    void doCycle() {
        display << (isSpriteLit() ? "#" : ".");

        if (getDrawColumn() == 39) {
            display << std::endl;
        }

        cycle++;
    }

    std::stringstream display;
    int64_t regX;
    size_t cycle;
};

void parseInstr(Crt& crt, const std::string& line) {
    assert(line.size() >= 4);
    auto command = line.substr(0, 4);

    if (command == "noop") {
        crt.noop();
    } else if (command == "addx") {
        int64_t v = std::stoi(line.substr(5));
        crt.addX(v);
    } else {
        throw std::runtime_error("Invalid command");
    }
}

int main() {
    Crt crt;
    std::string line;
    while (std::getline(std::cin, line)) {
        parseInstr(crt, line);
    }

    std::cout << crt.getDisplay();

    return 0;
}
