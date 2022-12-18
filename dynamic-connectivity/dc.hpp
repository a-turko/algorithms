#ifndef DC_HPP
#define DC_HPP

#include <vector>
#include <map>

#include "et_trees.hpp"
using namespace std;

/**
 * Dynamic Connectivity data structure as designed by
 * Jacob Holm, Kristian de Lichtenberg and Mikkel Thorup
 * (https://u.cs.biu.ac.il/~rodittl/p723-holm.pdf -- section 3).
 * 
 * Brief description of the algorithm:
 * Let G be the graph we are maintaining.
 * Each edge of G is assigned a level l in [0, log n].
 * A newly added edge is assigned level 0, which can change
 * during the course of the algorithm.
 * The data structure keeps a family of log n forests:
 * F_0, F_1, ..., F_{log n}.
 * F_i is a maximal spanning forest of G such that all edges in
 * it have level at least i. Also, we maintain that F_{i+1} is
 * always a subgraph of F_i. Furthermore, we will maintain that
 * edges on level i form connected components of size
 * at most n / (2^i). Hence, F_{log n} won't contain any edges.
 * 
 * Each forest is implemented as an union of Euler Tour Trees
 * (et_trees.hpp).This data structure suports connectivity
 * testing, edge insertion and removal (all in O(log n) time).
 * Those allow us to answer the connectivity queries (F_0 is a
 * maximal spanning forest of G) and handle the edges belonging
 * to the forests.
 * 
 * Euler tour trees also allow us to store values in a given
 * vertex and then pop them from a given connected component
 * in F_i. This allows us to store the edges that are not in
 * the forest (nontree edges) and then access those incident
 * to a given connected component one by one (each in O(log n)
 * time). An edge of level i which does not belong to F_i is
 * stored in its endpoints in the Euler tour trees for F_i.
 * After a tree edge has been removed, this allows us to find
 * a replacement edge, which would reconnect the two resulting
 * connected components. Processing each candidate takes O(log n)
 * time. However, we will be able to promote each edge that did
 * not reconnect the connected components to the next level.
 * As each edge can be promoted at most log n times, the total
 * cost of looking for replacements (over the whole runtime) is
 * bounded by O(m * log^2 n) where m is the number of edge
 * insertions.
*/

class DynamicConnectivity {
    public:

    /* Initiates the data structure for a n-vertex graph */
    DynamicConnectivity(int n);

    /**
     * Inserts an undirected (a,b) edge to the graph
     * (parallel edges are supported)
     * Complexity (runtime): O(log n)
     * Complexity (cost accounting for amortization): O(log^2 n)
    */
    void insert(int a, int b);

    /** Removes an undirected (a,b) edge from the graph
     * If there are multiple such edges, removes only one
     * 
     * Complexity (runtime): only bounded by the number of
     * edges added so far, but the total cost of all calls is
     * amortized over the whole lifetime of the structure
     * 
     * Complexity (cost accounting for amortization): O(log n) 
     */
    void remove(int a, int b);

    /** 
     * Checks whether vertices a and b are connected in the graph
     * Complexity: O(log n)
     */
    bool connected(int a, int b);

    /**
     *  Checks if the invariants of the data structure hold
     * Complexity: O(n log n)
     */
    bool correct();

    /* With DBG defined, prints the data structure to stderr */
    void print();

    private:
    void insert_edge(int a, int b, int level);
    bool find_replacement(int a, int b, int level,
                          pair <int, int> &replacement);
    vector <ETTForest> Forests;
    map<pair<int, int>, int> EdgeLevels;
    map<pair<int, int>, int> EdgeCnt;
    int L;
};

#endif