# Advent of Code Makefile

CXX			:= clang
CXXFLAGS	:= -O3 -Wall -pedantic -std=c++20
INCLUDES    := -I.
LIBS		:= -lstdc++

all: tree_cover scenic_score

tree_cover: tree_cover.o
	@$(CXX) $^ $(LIBS) -o $@

scenic_score: scenic_score.o
	@$(CXX) $^ $(LIBS) -o $@

%.o: %.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $^ -o $@

.PHONY: clean
clean:
	-@rm -f tree_cover scenic_score *.o

.PHONY: test
test: tree_cover
	@./tree_cover < example.txt

.PHONY: test2
test2: scenic_score
	@./scenic_score < example.txt
