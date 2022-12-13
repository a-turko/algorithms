CXXFLAGS= -std=c++17 -Wall -Wextra -pedantic -g
CXX= g++
SANITIZE= -fsanitize=address -fsanitize=undefined -static-libasan
OPTIMIZE= -O3

objects=ET_trees.o ET_trees.san.o dc.o dc.san.o test.o test.san.o
executables=test

default: all

all: $(executables)

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(OPTIMIZE) $< -o $@

%.san.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(SANITIZE) $< -o $@

test: test.san.o dc.san.o ET_trees.san.o
	$(CXX) $(CXXFLAGS) $(SANITIZE) test.san.o dc.san.o ET_trees.san.o -o $@

clean:
	rm -f $(objects) $(executables)