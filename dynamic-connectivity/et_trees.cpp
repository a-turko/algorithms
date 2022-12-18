#include <cstdint>
#include <set>
#include "et_trees.hpp"

static int64_t edge_id(int a, int b) {
    return (int64_t(a) << 32) | b;
}

ETTForest::ETTForest(int n) {
    for (int i = 0; i < n; i++) {
        Vertices.push_back(VertexNode(i));
    }
}

ETTForest::~ETTForest() {
    set <AVLNode *> processed;
    vector <AVLNode *> unused;

    for (auto edge: TEdgeHooks) {        
        delete edge.second;
    }
}

void ETTForest::insert_tree_edge(int a, int b, bool on_level) {
    // it's enough to mark only one of the copies as on the level
    EdgeNode *ab_edge = new EdgeNode(a,b, on_level);
    EdgeNode *ba_edge = new EdgeNode(b,a, false);

    TEdgeHooks[edge_id(a,b)] = ab_edge;
    TEdgeHooks[edge_id(b,a)] = ba_edge;

    // split the Euler tour to the part up to a and after a
    auto [left_a, right_a] = Vertices[a].split();
    // do the same for b
    auto [left_b, right_b] = Vertices[b].split();    

    // create the new Euler tour placing the new edge
    // between vertices a and b or edges with them as endpoints
    AVLNode::merge(AVLNode::merge(left_a, ab_edge, right_b),
                   NULL, 
                   AVLNode::merge(left_b, ba_edge, right_a));

}

void ETTForest::remove_tree_edge(int a, int b) {
    EdgeNode *ab_edge = TEdgeHooks[edge_id(a,b)];
    EdgeNode *ba_edge = TEdgeHooks[edge_id(b,a)];
    TEdgeHooks.erase(edge_id(a,b));
    TEdgeHooks.erase(edge_id(b,a));

    auto [l, r] = ab_edge->split();
    if (l == ba_edge->root()) {

        // the tour routed in lr starts after (b,a) and finishes
        // at (a,b), so it describes the new connected component
        // of the vertex a
        auto [ll, lr] = ba_edge->split();

        // the rest of the tour describes b's connected component
        AVLNode::merge(ll, NULL, r);

    } else {
        assert(r == ba_edge->root());

        // the tour routed in rl starts after (a,b) and finishes
        // at (b,a), so it describes the new connected component
        // of the vertex b
        auto [rl, rr] = ba_edge->split();

        // the rest of the tour describes a's connected component
        AVLNode::merge(l, NULL, rr);
    }

    // remove both copies of this edge from the Euler tours
    ab_edge->unlink();
    ba_edge->unlink();
    delete ab_edge;
    delete ba_edge;
}

void ETTForest::insert_nontree_edge(int a, int b) {
    NTEdgeHooks[edge_id(a,b)] = Vertices[a].push_nontree_edge(b);
    NTEdgeHooks[edge_id(b,a)] = Vertices[b].push_nontree_edge(a);
}

vector <pair <int, int> > ETTForest::promote_tree_edges(int a) {
    vector <pair <int, int> > edges;
    pair <int, int> edge;
    AVLNode *root = Vertices[a].root();

    while (root->promote_tree_edge(edge)) {
        edges.push_back(edge);
    }
    return edges;
}

void ETTForest::remove_nontree_edge(int a, int b) {
    Vertices[a].erase_nontree_edge(NTEdgeHooks[edge_id(a,b)]);
    Vertices[b].erase_nontree_edge(NTEdgeHooks[edge_id(b,a)]);
    NTEdgeHooks.erase(edge_id(a,b));
    NTEdgeHooks.erase(edge_id(b,a));
}

bool ETTForest::pop_nontree_edge(int a, pair <int, int> &edge) {
    AVLNode *root = Vertices[a].root();

    if (!root->pop_nontree_edge(edge))
        return false;

    Vertices[edge.second].erase_nontree_edge(
        NTEdgeHooks[edge_id(edge.second, edge.first)]);
    NTEdgeHooks.erase(edge_id(edge.first, edge.second));
    NTEdgeHooks.erase(edge_id(edge.second, edge.first));

    return true;
}

bool ETTForest::connected(int a, int b) {
    return Vertices[a].root() == Vertices[b].root();
}

bool ETTForest::is_tree_edge(int a, int b) {
    return TEdgeHooks.count(edge_id(a,b)) > 0;
}

int ETTForest::size(int a) {
    return Vertices[a].root()->size;
}

void ETTForest::print() {
#ifdef DBG
    debug ("Print the forest:\n");
    set <AVLNode*> processed;
    for (auto &v: Vertices) {
        auto root = v.root();
        if (processed.find(root) == processed.end()) {
            processed.insert(root);
            root->print_tree();
            debug("\n");
        }
    }
#endif
}

bool ETTForest::correct() {
    set<AVLNode *> processed;
    for (VertexNode &node: Vertices) {
        AVLNode *root = node.root();

        if (processed.count(root) > 0)
            continue;

        processed.insert(root);
        
        if (!root->correct_tree(nullptr)) {
            return false;
        }
    }
    return true;
}