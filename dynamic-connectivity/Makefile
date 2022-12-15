CXXFLAGS= -std=c++17 -Wall -Wextra -pedantic -g
CXX= g++
SANITIZE= -fsanitize=address -fsanitize=undefined -static-libasan
OPTIMIZE= -O3

objects=et_trees.o et_trees.san.o avl_tree.san.o avl_tree.o dc.o dc.san.o test.o test.san.o
executables=test
submissions=cf_a_standalone.cpp

default: test

all: $(executables)

%.o: %.cpp %.hpp
	$(CXX) -c $(CXXFLAGS) $(OPTIMIZE) $< -o $@

%.san.o: %.cpp %.hpp
	$(CXX) -c $(CXXFLAGS) $(SANITIZE) $< -o $@

test.o: test.cpp
	$(CXX) -c $(CXXFLAGS) $(OPTIMIZE) $< -o $@

test.san.o: test.cpp
	$(CXX) -c $(CXXFLAGS) $(SANITIZE) $< -o $@

test: test.san.o dc.san.o et_trees.san.o avl_tree.san.o
	$(CXX) $(CXXFLAGS) $(SANITIZE) $^ -o $@

cf_a: cf_a.cpp
	$(CXX) $(CXXFLAGS) -E $< -o cf_a_standalone.cpp
	$(CXX) $(CXXFLAGS) $(SANITIZE) cf_a_standalone.cpp -o cf_a
clean:
	rm -f $(objects) $(executables) $(submissions)
