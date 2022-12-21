# Advent of Code Makefile

CXX			:= clang
CXXFLAGS	:= -O3 -Wall -pedantic -std=c++20
INCLUDES    := -I.
LIBS		:= -lstdc++

all: rope_sim

rope_sim: rope_sim.o
	@$(CXX) $^ $(LIBS) -o $@

%.o: %.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $^ -o $@

.PHONY: clean
clean:
	-@rm -f rope_sim *.o

.PHONY: test
test: rope_sim
	@./rope_sim < example.txt
