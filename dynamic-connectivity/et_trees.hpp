#include <unordered_map>
#include <vector>
#include <cstdint>
#include "avl_tree.hpp"
using namespace std;

class ETTForest {
    public:

    ETTForest(int n);
    ~ETTForest();

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