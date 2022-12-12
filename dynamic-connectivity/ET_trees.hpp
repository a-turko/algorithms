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
    bool promote_tree_edge(pair <int, int> &edge);
    bool find_replacement(int a, int level, pair <int, int> &replacement);
    unsigned int get_nontree_cnt();
    bool correct_tree(AVLNode *parent);

    private:
    unsigned int height;

    void update_statistics();
    void unlink_children();
    virtual void recount_nontree_cnt() {};
    virtual int get_num_nontree_edges() {return 0;}
    virtual bool is_on_level() {return false;}

    protected:
    unsigned int nontree_cnt;
    unsigned int on_level_cnt;
    void update_nontree_cnt(int dx);
    void update_on_level_cnt(int dx);
    AVLNode *left, *right, *parent;
};

class EdgeNode : public AVLNode {
    public:
    int from, to;
    
    EdgeNode(int a, int b, bool on_level);
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

class ETTForest {
    public:

    ETTForest(int n);

    void insert_nontree_edge(int a, int b);
    void insert_tree_edge(int a, int b, bool on_level);
    vector <pair <int, int> > promote_tree_edges(int a);
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
};