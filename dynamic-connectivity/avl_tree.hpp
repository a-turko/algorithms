#ifndef AVL_TREE_HPP
#define AVL_TREE_HPP

#include <cstdio>
#include <utility>
#include <list>
#include <vector>
#include <cassert>
using namespace std;

#define debug(...) {} //fprintf(stderr, __VA_ARGS__), fflush(stderr)

class AVLNode {
    public:
    // number of vertex nodes in the subtree
    unsigned int size;

    AVLNode();
    virtual ~AVLNode() {};

    pair <AVLNode*, AVLNode*> split();
    static AVLNode *merge(AVLNode *left, AVLNode *middle, AVLNode *right);
    AVLNode *root();
    virtual bool pop_nontree_edge(pair <int, int> &edge);
    virtual bool promote_tree_edge(pair <int, int> &edge);
    bool find_replacement(int a, int level, pair <int, int> &replacement);
    unsigned int get_nontree_cnt();
    bool correct_tree(AVLNode *correct_parent);
    void erase();

    virtual void print_node(int indent = 0) {
        static char spaces[] = "          ";
        debug ("%sNode 0x%llx: size = %d, height = %d, nontree_cnt = %d, on_level_cnt = %d,\n %sparent = 0x%llx, left = 0x%llx, right = 0x%llx\n",
        &spaces[max(0,10 - 2*indent)], (long long) this, size, height, nontree_cnt, on_level_cnt,
        &spaces[max(0,10 - 2*indent)], (long long) parent, (long long) left, (long long) right);
    }

    void print_tree(int indent = 0) {
        return;
        if (left) left->print_tree(indent+1);
        print_node(indent);
        if (right) right->print_tree(indent+1);
    }


    private:
    int height;

    AVLNode *balance();
    
    void replace_child(AVLNode *old_child, AVLNode *new_child);

    protected:
    unsigned int nontree_cnt;
    unsigned int on_level_cnt;
    virtual bool is_on_level();
    virtual int get_num_nontree_edges();
    void update_nontree_cnt(int dx);
    void update_on_level_cnt(int dx);
    void update_statistics();
    void unlink_children();
    AVLNode *rotate_left();
    AVLNode *rotate_right();
    AVLNode *left, *right, *parent;
};

class EdgeNode : public AVLNode {
    public:
    int from, to;
    
    EdgeNode(int _from, int _to, bool _on_level);
    bool promote_tree_edge(pair <int, int> &edge);
    bool pop_nontree_edge(pair <int, int> &edge);

    void print_node(int indent) {
        static char spaces[] = "          ";
        debug("%sEdge (%d %d)\n", &spaces[max(0, 10 - 2*indent)], from, to);
        AVLNode::print_node(indent);
    }

    private:
        void recount_nontree_cnt();
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
        static char spaces[] = "          ";
        debug("%sVertex %d\n", &spaces[max(0, 10 - 2*indent)], idx);
        AVLNode::print_node(indent);
    };

    private:
    list<int> NTEdges;
    
    void recount_nontree_cnt();
    int get_num_nontree_edges();
    bool is_on_level();
};

#endif