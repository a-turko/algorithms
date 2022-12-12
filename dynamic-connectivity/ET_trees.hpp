#include <unordered_map>
#include <map>
#include <list>
#include <vector>
#include <cstdint>
using namespace std;

class AVLNode {
    public:
    unsigned int size;

    AVLNode();
    virtual ~AVLNode() {};

    pair <AVLNode*, AVLNode*> split();
    void remove();
    static AVLNode *merge(AVLNode *left, AVLNode *right);
    AVLNode *root();
    bool pop_nontree_edge(pair <int, int> &edge);
    unsigned int get_nontree_cnt();
    virtual int get_num_nontree_edges() {return 0;}
    bool correct_tree(AVLNode *parent);

    private:
    unsigned int height;

    void update_statistics();
    void unlink_children();
    virtual void recount_nontree_cnt() {};

    protected:
    unsigned int nontree_cnt;
    void update_nontree_cnt(int dx);
    AVLNode *left, *right, *parent;
};

class EdgeNode : public AVLNode {
    public:
    int from, to;
    
    EdgeNode(int from, int to);

    private:
        void recount_nontree_cnt();
        int get_num_nontree_edges();
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
};

class ETTForest {
    public:

    ETTForest(int n);

    void insert_nontree_edge(int a, int b);
    void insert_tree_edge(int a, int b);
    void remove_tree_edge(int a, int b);
    void remove_nontree_edge(int a, int b);
    bool connected(int a, int b);
    bool is_tree_edge(int a, int b);
    bool pop_nontree_edge(int a, pair <int, int> &edge);
    int size(int a);
    bool correct();

    private:
    unordered_map <int64_t, EdgeNode*> TEdgeHooks;
    unordered_map <int64_t, list<int>::iterator> NTEdgeHooks;
    vector <VertexNode> Vertices;

    static int64_t edge_id(int a, int b);
};