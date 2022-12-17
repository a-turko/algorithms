#ifndef AVL_TREE_HPP
#define AVL_TREE_HPP

#include <cstdio>
#include <utility>
#include <list>
#include <vector>
#include <cassert>
using namespace std;

#define debug(...) {} //fprintf(stderr, __VA_ARGS__)

class AVLNode {
    public:
    // number of vertex nodes in the subtree
    int size;

    AVLNode();
    virtual ~AVLNode() {};

    pair <AVLNode*, AVLNode*> split();
    static AVLNode *merge(AVLNode *left, AVLNode *middle, AVLNode *right);
    AVLNode *root();
    virtual bool pop_nontree_edge(pair <int, int> &edge);
    virtual bool promote_tree_edge(pair <int, int> &edge);
    bool correct_tree(AVLNode *correct_parent);
    void unlink();

    void print_tree(int indent = 0) {
        
        if (left)
            left->print_tree(indent + 3);
        print_node(indent);
        if (right)
            right->print_tree(indent + 3);
    }

    virtual void print_node(int indent = 0) {
        char spaces[20] = "                   ";
        spaces[min(19, indent)] = 0;
        debug("%s0x%llx: left = 0x%llx, right = 0x%llx, parent = 0x%llx,\n \
                %sheight = %d, nontree_cnt = %d, on_level_cnt = %d, size = %d\n",
                spaces, (long long) this, (long long) left, (long long) right,
                (long long) parent, spaces, height, nontree_cnt, on_level_cnt, size);
    }

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

    void print_node(int indent = 0) {
        char spaces[20] = "                   ";
        spaces[min(19, indent)] = 0;
        debug ("%sEdgeNode: (%d %d)\n", spaces, from, to);
        AVLNode::print_node(indent);
    }
    
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

    void print_node(int indent = 0) {
        char spaces[20] = "                   ";
        spaces[min(19, indent)] = 0;
        debug ("%sVertexNode: %d: ", spaces, idx);
        for (int x: NTEdges) debug(" %d", x);
        debug("\n");
        AVLNode::print_node(indent);
    }


    private:
    list<int> NTEdges;
    
    int get_num_nontree_edges();
    bool is_on_level();
};

#endif