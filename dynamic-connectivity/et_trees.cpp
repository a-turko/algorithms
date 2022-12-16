#include <cstdint>
#include <set>
#include "et_trees.hpp"

#define show_tree(x) {debug ("%s: %p\n", #x, (void *)x);if (x) x->print_tree(); else debug("(empty)\n");}

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

    for (auto edge: TEdgeHooks) {        
        delete edge.second;
    }
}

#define show_tree(x) {debug ("\n");if (x) x->print_tree(); else debug("(empty)\n");}

void ETTForest::insert_tree_edge(int a, int b, bool on_level) {
    EdgeNode *ab_edge = new EdgeNode(a,b, on_level);
    EdgeNode *ba_edge = new EdgeNode(b,a, on_level);

    TEdgeHooks[edge_id(a,b)] = ab_edge;
    TEdgeHooks[edge_id(b,a)] = ba_edge;

    auto [left_a, right_a] = Vertices[a].split();
    auto [left_b, right_b] = Vertices[b].split();

    debug ("Insert tree edge (%d %d)\n", a, b);
    show_tree(left_a);
    show_tree(right_a);
    show_tree(left_b);
    show_tree(right_b);
    

    //TODO: describe the order of the euler tour
    AVLNode::merge(AVLNode::merge(left_a, ab_edge, right_b),
                   NULL, 
                   AVLNode::merge(left_b, ba_edge, right_a));

}

void ETTForest::remove_tree_edge(int a, int b) {
    debug ("Remove tree edge (%d %d)\n", a, b);

    AVLNode *outer, *inner;
    EdgeNode *ab_edge = TEdgeHooks[edge_id(a,b)];
    EdgeNode *ba_edge = TEdgeHooks[edge_id(b,a)];
    TEdgeHooks.erase(edge_id(a,b));
    TEdgeHooks.erase(edge_id(b,a));

    debug ("Initially:\n");
    show_tree(ab_edge->root());

    auto [l, r] = ab_edge->split();

    show_tree(l);
    show_tree(r);

    if (l == ba_edge->root()) {
        auto [ll, lr] = ba_edge->split();

        outer = AVLNode::merge(ll, NULL, r);
        inner = lr;

    } else {
        assert(r == ba_edge->root());

        auto [rl, rr] = ba_edge->split();
        outer = AVLNode::merge(l, NULL, rr);
        inner = rl;
    }

    debug ("After split:\n");
    show_tree(outer);
    show_tree(inner);

    ab_edge->erase();
    ba_edge->erase();
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
    debug ("%d vertices\n", (int)Vertices.size());
    set<AVLNode *> processed;
    for (VertexNode &node: Vertices) {

        debug ("Test vertex %d: \n", node.idx);

        AVLNode *root = node.root();

        if (processed.count(root) > 0) {
            debug ("Already processed\n");
            continue;
        }
        processed.insert(root);
        
        debug ("\nRoot: %p\n", (void*)root);
        root->print_tree();
        if (!root->correct_tree(nullptr)) {
            return false;
        }
    }
    return true;
}