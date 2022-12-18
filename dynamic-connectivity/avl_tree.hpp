#ifndef AVL_TREE_HPP
#define AVL_TREE_HPP

#include <cstdio>
#include <utility>
#include <list>
#include <vector>
#include <cassert>
using namespace std;

// A switch for managing debug output in the whole DC implementation
// #define DBG
#ifdef DBG  
    #define debug(...) fprintf(stderr, __VA_ARGS__), fflush(stderr)
#else
    #define debug(...) {}
#endif

/**
 * AVL Binary Search Tree (https://en.wikipedia.org/wiki/AVL_tree)
 */

class AVLNode {
    public:
    /* number of vertex nodes in the subtree of the current node */
    int size;

    AVLNode();
    virtual ~AVLNode() {};

    /**
     * Splits the tree into two parts and returns pointers to their roots
     * The first part contains all nodes to the left of the current node
     * (inclusive), the second part contains all nodes to the right of it
     * (exclusive).
     * 
     * Complexity: O(log n)
     */
    pair <AVLNode*, AVLNode*> split();

    /**
     * Merges two trees (left and right) into one (the middle node is optional)
     * and returns the root of the resulting tree.
     * The order of nodes is as follows: left(unchanged), middle, right(unchanged).
     * 
     * Complexity if middle is NULL: O(max(h_l, h_r))
     * Complexity if middle is not NULL: O(abs(h_l - h_r))
     * Here h_l and h_r are the heights of the left and right subtrees.
    */
    static AVLNode *merge(AVLNode *left, AVLNode *middle, AVLNode *right);

    /**
     * Returns the root of the tree represting the connected component of the node.
     * Complexity: O(log n)
     */
    AVLNode *root();

    /**
     * Removes the current node from the BST it belongs to
     * Complexity: O(log n)
    */
    void unlink();

    /**
     * Pops a nontree edge from the subtree of the current node
     * and returns it in the edge parameter.
     * Returns true on success and false on failure.
     * Complexity: O(log n)
     */
    virtual bool pop_nontree_edge(pair <int, int> &edge);

    /**
     * Pops a tree edge marked as on_level from the subtree of the current node
     * and returns it in the edge parameter.
     * Returns true on success and false on failure.
     * Complexity: O(log n)
     */
    virtual bool promote_tree_edge(pair <int, int> &edge);

    /**
     * Checks if the invariants hold in the subtree of the current node
     * Complexity: linear in the size of the tree
    */
    bool correct_tree(AVLNode *correct_parent);

    void print_tree(int indent = 0);
    virtual void print_node(int indent = 0);

    private:
    AVLNode *balance();
    AVLNode *rotate_left();
    AVLNode *rotate_right();
    void unlink_children();

    void replace_child(AVLNode *old_child, AVLNode *new_child);

    protected:
    int nontree_cnt, on_level_cnt, height;
    AVLNode *left, *right, *parent;

    virtual bool is_on_level();
    virtual int get_num_nontree_edges();
    void update_nontree_cnt(int dx);
    void update_on_level_cnt(int dx);
    void update_statistics();
};

class EdgeNode : public AVLNode {
    public:
    int from, to;
    
    EdgeNode(int _from, int _to, bool _on_level);
    bool promote_tree_edge(pair <int, int> &edge);
    bool pop_nontree_edge(pair <int, int> &edge);

    void print_node(int indent = 0);
    
    private:
        int get_num_nontree_edges();
        bool is_on_level();
        bool on_level;
};

class VertexNode: public AVLNode {
    public:
    int idx;

    VertexNode(int idx);
    bool pop_nontree_edge(pair <int, int> &edge);
    list<int>::iterator push_nontree_edge(int to);
    void erase_nontree_edge(list<int>::iterator it);
    bool promote_tree_edge(pair <int, int> &edge);

    void print_node(int indent = 0);


    private:
    list <int> NTEdges;
    
    int get_num_nontree_edges();
    bool is_on_level();
};

#endif