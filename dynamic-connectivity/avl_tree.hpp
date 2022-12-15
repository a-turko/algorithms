#include <cstdio>
#include <utility>
#include <list>
#include <vector>
using namespace std;

#define debug(...) fprintf(stderr, __VA_ARGS__), fflush(stderr)

class AVLNode {
    public:
    // number of vertex nodes in the subtree
    unsigned int size;

    AVLNode();
    virtual ~AVLNode() {};

    pair <AVLNode*, AVLNode*> split();
    static AVLNode *merge(AVLNode *left, AVLNode *middle, AVLNode *right);
    static AVLNode *build_avl(vector <AVLNode*>::iterator begin, vector<AVLNode*>::iterator end);
    AVLNode *root();
    bool pop_nontree_edge(pair <int, int> &edge);
    bool promote_tree_edge(pair <int, int> &edge);
    bool find_replacement(int a, int level, pair <int, int> &replacement);
    unsigned int get_nontree_cnt();
    bool correct_tree(AVLNode *correct_parent);
    void erase();
    static void deconstruct_tree(AVLNode *node, vector <AVLNode*> &node_list);

    private:
    int height;

    void balance();

    int get_num_nontree_edges();
    bool is_on_level();
    void replace_child(AVLNode *old_child, AVLNode *new_child);

    protected:
    unsigned int nontree_cnt;
    unsigned int on_level_cnt;
    void update_nontree_cnt(int dx);
    void update_on_level_cnt(int dx);
    void update_statistics();
    void unlink_children();
    AVLNode *rotate_left();
    AVLNode *rotate_right();
    AVLNode *left, *right, *parent;
};

class DummyNode : public AVLNode {};

class EdgeNode : public AVLNode {
    public:
    int from, to;
    
    EdgeNode(int _from, int _to, bool _on_level);
    bool promote_tree_edge(pair <int, int> &edge);

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

    private:
    list<int> NTEdges;
    
    void recount_nontree_cnt();
    int get_num_nontree_edges();
    bool is_on_level();
};
