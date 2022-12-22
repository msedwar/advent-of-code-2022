#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>

class Crt {
  public:
    Crt() : regX(1), cycle(1), signalSum(0) {}

    void addX(int64_t v) {
        doCycle();
        doCycle();
        regX += v;
    }

    void noop() {
        doCycle();
    }

    int64_t getSignalSum() {
        return signalSum;
    }

  private:
    void doCycle() {
        if (cycle >= 20 && (cycle - 20) % 40 == 0) {
            signalSum += regX * cycle;
        }

        cycle++;
    }

    int64_t regX;
    size_t cycle;
    int64_t signalSum;
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

    std::cout << "Sum of signal strengths: " << crt.getSignalSum() << std::endl;

    return 0;
}
