# Advent of Code Makefile

CXX			:= clang
CXXFLAGS	:= -O3 -Wall -pedantic -std=c++20
INCLUDES    := -I.
LIBS		:= -lstdc++

all: monkeys

monkeys: monkeys.o
	@$(CXX) $^ $(LIBS) -o $@

%.o: %.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $^ -o $@

.PHONY: clean
clean:
	-@rm -f monkeys *.o

.PHONY: test
test: monkeys
	@./monkeys < example.txt
