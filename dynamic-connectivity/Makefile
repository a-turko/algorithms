CXXFLAGS= -std=c++17 -Wall -Wextra -pedantic -g
CXX= g++
SANITIZE= -fsanitize=address -fsanitize=undefined -static-libasan
OPTIMIZE= -O3

objects=et_trees.o et_trees.san.o avl_tree.san.o avl_tree.o dc.o dc.san.o test.o test.san.o
executables=test-opt test-san cf_a cf_e
submissions=cf_a_standalone.cpp cf_e_standalone.cpp
dc_sources=dc.hpp dc.cpp et_trees.hpp et_trees.cpp avl_tree.hpp avl_tree.cpp

default: all

all: $(executables)

%.o: %.cpp %.hpp
	$(CXX) -c $(CXXFLAGS) $(OPTIMIZE) $< -o $@

%.san.o: %.cpp %.hpp
	$(CXX) -c $(CXXFLAGS) $(SANITIZE) $< -o $@

test.o: test.cpp
	$(CXX) -c $(CXXFLAGS) $(OPTIMIZE) $< -o $@

test.san.o: test.cpp
	$(CXX) -c $(CXXFLAGS) $(SANITIZE) $< -o $@

test-san: test.san.o dc.san.o et_trees.san.o avl_tree.san.o
	$(CXX) $(CXXFLAGS) $(SANITIZE) $^ -o $@

test-opt: test.o dc.o et_trees.o avl_tree.o
	$(CXX) $(CXXFLAGS) $(OPTIMIZE) $^ -o $@

cf_a: cf_a.cpp
	python3 paste_includes.py cf_a.cpp > cf_a_standalone.cpp
	$(CXX) $(CXXFLAGS) $(SANITIZE) cf_a_standalone.cpp -o cf_a

cf_e: cf_e.cpp
	python3 paste_includes.py cf_e.cpp > cf_e_standalone.cpp
	$(CXX) $(CXXFLAGS) $(SANITIZE) cf_e_standalone.cpp -o cf_e

pdf:
	enscript -C -2r -Ecpp -fCourier8 -o - $(dc_sources) | ps2pdf - dc.pdf

clean:
	rm -f $(objects) $(executables) $(submissions)
