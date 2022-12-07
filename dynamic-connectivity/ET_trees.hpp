using namespace std;

virtual class AVLNode {
    public:
    unsigned int size;

    AVLNode();

    void split();
    void remove();
    void reverse();
    static AVLNode *merge(AVLNode *left, AVLNode *right);
    AVLNode *root();
    bool pop_nontree_edge(pair <int, int> &edge);

    private:
    AVLNode *left, *right, *parent;
    bool reverse;
    unsigned int height;
    unsigned int nontree_cnt;

    void local_update();
    void update_nontree_cnt(int dx);
    void check_reverse_from_root();
    void check_reverse();
    virtual void recount_nontree_cnt() {};
}

class EdgeNode : private AVLNode {
    public:
        int from, to;
}

class VertexNode: private AVLNode {
    public:
        int idx;

    bool pop_nontree_edge(pair <int, int> &edge);
    list<int>::iterator push_nontree_edge(int to);
    void erase_nontree_edge(list<int>::iterator it);

    private:
        list<int> NTEdges;
    
    void recount_nontree_cnt();
}

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

    private:
    unordered_map <pair <int, int>, list<EdgeNode*>::iterator> TEdgeHooks;
    unordered_map <pair <int, int>, list<int>::iterator> NTEdgeHooks;
    vector <VertexNode> Vertices;

    void recount_nontree_cnt();
};