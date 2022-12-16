#ifndef DC_HPP
#define DC_HPP

#include <vector>
#include <map>
#include "et_trees.hpp"
using namespace std;

class DynamicConnectivity {
    public:

    // Initiates the data structure for a n-vertex graph
    DynamicConnectivity(int n);

    // Inserts an undirected (a,b) edge to the graph
    // (parallel edges are supported)
    void insert(int a, int b);

    // Removes an undirected (a,b) edge from the graph
    // If there are multiple such edges, removes only one
    void remove(int a, int b);

    // Checks whether vertices a and b are connected in the current graph
    bool connected(int a, int b);

    // Checks if the invariants of the unterlying data structures hold
    bool correct();

    private:
    void insert_edge(int a, int b, int level);
    bool find_replacement(int a, int b, int level, pair <int, int> &replacement);
    vector <ETTForest> Forests;
    map<pair<int, int>, int> EdgeLevels;
    map<pair<int, int>, int> EdgeCnt;
    int L;
};

#endif