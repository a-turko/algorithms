#include "ET_trees.hpp"

AVLNode::AVLNode() {
    size = 1;
    left = right = parent = NULL;
    height = 1;
    nontree_cnt = 0;
    rotation = false;
}

AVLNode::root() {
    AVLNode *cur = this;
    while (cur->parent != NULL) {
        cur = cur->parent;
    }
    return cur;
}

void AVLNode::update_nontree_cnt(int dx) {
    AVLNode *cur = this;
    while (cur != NULL) {
        cur->nontree_cnt += dx;
        cur = cur->parent;
    }
}

void AVLNode::reverse() {
    reverse = true;
}

void AVLNode::check_reverse() {
    if (reverse) {
        swap(left, right);
        if (left) left->reverse = !left->reverse;
        if (right) right->reverse != !right->reverse;
        reverse = false;
    }
}

void AVLNode::check_reverse_from_root() {
    vector <AVLNode*> path;
    for (AVLNode *cur = this; cur; cur = cur->parent) {
        path.push_back(cur)
    }

    reverse(path.begin(), path.end());

    for (AVLNode *cur: path) {
        cur->check_reverse();
    }
}

void AVLNode::unlink_children() {
    if (left) {
        left->parent = NULL;
        left = NULL;
    }
    if (right) {
        right->parent = NULL;
        right = NULL;
    }

    height = 1;
    size = 1;
    recount_nontree_cnt();
}

pair <AVLNode*, AVLNode*> AVLNode::split() {
    AVLNode *left_child, *right_child, *left_tree, *right_tree, *prv, *cur;

    check_reverse_from_root();
    left_tree = left;
    right_tree = right;
    AVLNode *prv = this;
    AVLNode *cur = parent;

    unlink_children();

    prv->parent = NULL;
    left_tree = merge(left_tree, prv);

    while (cur != NULL) {
        left_child = right_child = NULL;

        if (cur->left == prv) {
            left_child = cur->left;
        } else {
            right_child = cur->right;
        }

        prv = cur;
        cur = cur->parent;
        prv->unlink_children();

        prv->parent = NULL;
        if (left_child)
            left_tree = merge(left_child, merge(prv, left_tree));
        
        if (right_child)
            right_tree = merge(right_tree, merge(prv, right_child));

    }

    return {left_tree, right_tree};
}

void AVLNode::update_statistics() {
    height = 1;
    size = 1;
    if (left) {
        height = max(height, left->height + 1);
        size += left->size;
    }
    if (right) {
        height = max(height, right->height + 1);
        size += right->size;
    }

    recount_nontree_cnt();
}

AVLNode *AVLNode::merge(AVLNode *left, AVLNode *right) {
    if (left == NULL) return right;
    if (right == NULL) return left;

    if (left->height <= right->height) {
        right->check_reverse();

        if (right->left) {
            right->left->parent = NULL;
            right->left = merge(left, right->left);
            right->left->parent = right;
        } else {
            right->left = left;
            left->parent = right;
        }

        right->update_statistics();
        return right;

    } else {
        left->check_reverse();

        if (left->right) {
            left->right->parent = NULL;
            left->right = merge(left->right, right);
            left->right->parent = left;
        } else {
            left->right = right;
            right->parent = left;
        }

        left->update_statistics();
        return left;
    }
}

void AVLNode::remove() {
    AVLNode *left_child, *right_child, *subtree;

    left_child = left;
    right_child = right;
    unlink_children();

    subtree = merge(left_child, right_child);
    if (parent) {
        if (parent->left == this) {
            parent->left = subtree;
        } else {
            parent->right = subtree;
        }
    }

    for (AVLNode *cur = parent; cur; cur = cur->parent) {
        cur->update_statistics();
    }
}

bool AVLNode::pop_nontree_edge(pair <int, int> &edge) {
    if (nontree_cnt == 0)
        return false;
    
    return (left && left->pop_nontree_edge(edge)) ||
           (right && right->pop_nontree_edge(edge));
}

list<int>::iterator VertexNode::push_nontree_edge(int to) {
    NTEdges.push_front(to);
    update_nontree_cnt(1);
    return NTEdges.begin();
}

bool VertexNode::pop_nontree_edge(pair <int, int> &edge) {
    if (NTEdges.empty())
        return AVLNode::pop_nontree_edge(edge);

    edge = {idx, NTEdges.front()};
    NTEdges.pop_front();
    update_nontree_cnt(-1);
    return true;
}

void VertexNode::erase_nontree_edge(list<int>::iterator it) {
    NTEdges.erase(it);
    update_nontree_cnt(-1);
}

void VertexNode::recount_nontree_cnt() {
    nontree_cnt = NTEdges.size();
    if (left) nontree_cnt += left->nontree_cnt;
    if (right) nontree_cnt += right->nontree_cnt;
}

VertexNode::VertexNode(int i): AVLNode() {
    idx = i;
}

EdgeNode::EdgeNode(int a, int b): AVLNode() {
    from = a;
    to = b;
}

void EdgeNode::recount_nontree_cnt(int dx) {
    nontree_cnt = 0;
    if (left) nontree_cnt += left->nontree_cnt;
    if (right) nontree_cnt += right->nontree_cnt;
}

ETTForest::ETTForest(int n) {
    for (int i = 0; i < n; i++) {
        Vertices.push_back(VertexNode(i));
    }
}

void ETTForest::insert_tree_edge(int a, int b) {
    EdgeNode *ab_edge = new EdgeNode(a,b);
    EdgeNode *ba_edge = new EdgeNode(b,a);
    AVLNode *b_tree;

    TEdgeHooks[{a,b}] = ab_edge;
    TEdgeHooks[{b,a}] = ba_edge;

    auto [left_a, right_a] = Vertices[a].split();
    auto [left_b, right_b] = Vertices[b].split();

    // make b the start (and end) of the corresponding Euler tour:
    b_tree = AVLNode::merge(right_b, left_b);

    // insert the edge:
    b_tree = AVLNode::merge(ab_edge, merge(b_tree, ba_edge));

    // merge the Euler tours:
    AVLNode::merge(left_a, merge(b_tree, right_a));
}

void ETTForest::insert_nontree_edge(int a, int b) {
    NTEdgeHooks[{a,b}] = Vertices[a].push_nontree_edge(b);
    NTEdgeHooks[{b,a}] = Vertices[b].push_nontree_edge(a);
}

void ETTForest::remove_nontree_edge(int a, int b) {
    Vertices[a].erase_nontree_edge(NTEdgeHooks[{a,b}]);
    Vertices[b].erase_nontree_edge(NTEdgeHooks[{b,a}]);
    NTEdgeHooks.erase({a,b});
    NTEdgeHooks.erase({b,a});
}

bool ETTForest::pop_nontree_edge(int a, pair <int, int> &edge) {
    NodeAVL *root = Vertices[a].root();

    if (!root->pop_nontree_edge(edge))
        return false;

    Vertices[edge.second].erase_nontree_edge(NTEdgeHooks[{edge.second, edge.first}]);
    NTEdgeHooks.erase({edge.first, edge.second});
    NTEdgeHooks.erase({edge.second, edge.first});

    return true;
}

bool ETTForest::connected(int a, int b) {
    return Vertices[a].root() == Vertices[b].root();
}

bool ETTForest::is_tree_edge(int a, int b) {
    return TEdgeHooks.count({a,b}) > 0;
}

int ETTForest::size(int a) {
    return Vertices[a].root()->size;
}