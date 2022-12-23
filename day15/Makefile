# Advent of Code Makefile

CXX			:= clang
CXXFLAGS	:= -O3 -Wall -pedantic -std=c++20
INCLUDES    := -I.
LIBS		:= -lstdc++

all: pressure

pressure: pressure.o
	@$(CXX) $^ $(LIBS) -o $@

%.o: %.cpp
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $^ -o $@

.PHONY: clean
clean:
	-@rm -f pressure *.o

.PHONY: test
test: pressure
	@./pressure < example.txt
