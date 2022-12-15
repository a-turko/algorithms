#include <cstdint>
#include <set>
#include "et_trees.hpp"

static int64_t edge_id(int a, int b) {
    return (int64_t(a) << 32) | b;
}

ETTForest::ETTForest(int _n) {
    n = _n;
    dummy_node_credits = 0;
    logn = 0;
    while ((1<<logn) < n) logn++;

    for (int i = 0; i < n; i++) {
        Vertices.push_back(VertexNode(i));
    }
}

ETTForest::~ETTForest() {
    set <AVLNode *> processed;
    vector <AVLNode *> unused;

    for (auto &vtex: Vertices) {
        AVLNode *root = vtex.root();
        if (processed.count(root) == 0) {
            processed.insert(root);

            AVLNode::deconstruct_tree(root, unused);
        }
    }

    for (auto edge: TEdgeHooks) {        
        delete edge.second;
    }
}

void ETTForest::insert_tree_edge(int a, int b, bool on_level) {
    EdgeNode *ab_edge = new EdgeNode(a,b, on_level);
    EdgeNode *ba_edge = new EdgeNode(b,a, on_level);

    TEdgeHooks[edge_id(a,b)] = ab_edge;
    TEdgeHooks[edge_id(b,a)] = ba_edge;

    auto [left_a, right_a] = Vertices[a].split();
    auto [left_b, right_b] = Vertices[b].split();

    //TODO: describe the order of the euler tour
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
        auto [ll, lr] = ba_edge->split();
        AVLNode::merge(ll, NULL, r);

    } else {
        auto [rl, rr] = ba_edge->split();
        AVLNode::merge(l, NULL, rr);
    }

    ab_edge->erase();
    ba_edge->erase();
    delete ab_edge;
    delete ba_edge;
}

void ETTForest::rebuild_trees() {
    set <AVLNode*> roots;
    for (AVLNode &vertex: Vertices)
        roots.insert(vertex.root());
    
    for (AVLNode *root: roots) {
        vector <AVLNode *> nodes;
        AVLNode::deconstruct_tree(root, nodes);
        AVLNode::build_avl(nodes.begin(), nodes.end());
    }

    // all dummy nodes have been freed
    dummy_node_credits = 0;
}

void ETTForest::insert_nontree_edge(int a, int b) {
    NTEdgeHooks[edge_id(a,b)] = Vertices[a].push_nontree_edge(b);
    NTEdgeHooks[edge_id(b,a)] = Vertices[b].push_nontree_edge(a);
}

vector <pair <int, int> > ETTForest::promote_tree_edges(int a) {
    vector <pair <int, int> > edges;
    pair <int, int> edge;

    while (Vertices[a].promote_tree_edge(edge)) {
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

    Vertices[edge.second].erase_nontree_edge(NTEdgeHooks[edge_id(edge.second, edge.first)]);
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

// TODO: process any root only once
bool ETTForest::correct() {
    for (VertexNode &node: Vertices) {
        AVLNode *root = node.root();
        debug ("Test vertex %d:\n", node.idx);

        if (!root->correct_tree(nullptr))
            return false;
    }
    return true;
}