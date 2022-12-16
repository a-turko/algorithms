#ifndef AVL_TREE_HPP
#define AVL_TREE_HPP

#include <cstdio>
#include <utility>
#include <list>
#include <vector>
#include <cassert>
using namespace std;

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

    private:
    list<int> NTEdges;
    
    int get_num_nontree_edges();
    bool is_on_level();
};

#endif