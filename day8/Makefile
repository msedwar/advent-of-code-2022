# Advent of Code Makefile

CXX			:= clang
CXXFLAGS	:= -O3 -Wall -pedantic -std=c++20
INCLUDES    := -I.
LIBS		:= -lstdc++

all: tree_cover

tree_cover: tree_cover.o
	@$(CXX) $^ $(LIBS) -o $@

%.o: %.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $^ -o $@

.PHONY: clean
clean:
	@rm tree_cover
	@rm *.o

.PHONY: test
test: tree_cover
	@./tree_cover < example.txt
