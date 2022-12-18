#ifndef ET_TREES_HPP
#define ET_TREES_HPP

#include <unordered_map>
#include <vector>
#include <cstdint>
#include <set>
#include "avl_tree.hpp"
using namespace std;

/**
 * Euler Tour Tree Forest data structure for storing a forest
 * over n vertices.
 * 
 * Each tree is represented as an Euler tour stored on an AVL
 * tree (avl_tree.hpp). An Euler tour is formed by two EdgeNodes
 * for every tree edge (one in each direction) and a single
 * VertexNode for each vertex. Since an Euler tour visits a
 * vertex the same number of times as its degree, there may be
 * many possible positions for a VertexNode in the Euler tour.
 * For example, the following a two Euler tours are equivalent:
 * (0,1) ~ (1) ~ (1, 4) ~ (4) ~ (4,1) ~ (1,0)
 * (0,1) ~ (1, 4) ~ (4) ~ (4,1) ~ (1) ~ (1,0)
 */

class ETTForest {
    public:

    /* Initializes the data structure for a n-vertex graph */
    ETTForest(int _n);

    ~ETTForest();

    /**
     * Stores a nontree edge (a,b)
     * a and b already need to be in the same connected component
     * Complexity: O(log n)
     */
    void insert_nontree_edge(int a, int b);

    /** 
     * Adds a tree edge (a,b) and marks it as being on the
     * level equal to the level of the forest (on_level == true)
     * or on a level above it (on_level == false)
     * 
     * a and b need to be in different connected components
     * Compexity: O(log n)
     */
    void insert_tree_edge(int a, int b, bool on_level);

    /**
     * Lists all tree edges on the level of the forest
     * in the connected component containing a and
     * marks them as being on a level above the forest
     * 
     * Complexity: O(k log n) where k is the number of tree edges
     * being promoted
    */
    vector <pair <int, int> > promote_tree_edges(int a);

    /**
     * Removes a tree edge (a,b) from the forest
     * Complexity: O(log n)
     */
    void remove_tree_edge(int a, int b);

    /**
     * Removes a nontree edge (a,b) from the forest
     * Complexity: O(log n)
     */
    void remove_nontree_edge(int a, int b);

    /**
     * Checks whether vertices a and b are connected
     * Complexity: O(log n)
     */
    bool connected(int a, int b);

    bool is_tree_edge(int a, int b);

    /**
     * Pops any nontree edge stored in the connected component
     * of a and returns it in edge parameter
     * If no such edge exists, returns false, otherwise true
     * Complexity: O(log n)
     */
    bool pop_nontree_edge(int a, pair <int, int> &edge);

    /**
     * Returns the number of vertices in the connected component
     * containing the vertex a
     * Complexity: O(log n)
     */
    int size(int a);

    /**
     * Checks whether the invariants of the data structure hold
     * Complexity: O(n log n + m), where m is the number of edges
     * currently in the forest
    */
    bool correct();

    void print();

    private:
    unordered_map <int64_t, EdgeNode*> TEdgeHooks;
    unordered_map <int64_t, list<int>::iterator> NTEdgeHooks;
    vector <VertexNode> Vertices;
};

#endif