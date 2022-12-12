#include "ET_trees.hpp"

AVLNode::AVLNode() {
    size = 1;
    left = right = parent = NULL;
    height = 1;
    nontree_cnt = 0;
}

AVLNode *AVLNode::root() {
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

    left_tree = left;
    right_tree = right;
    prv = this;
    cur = parent;

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

unsigned int AVLNode::get_nontree_cnt() {
    return nontree_cnt;
}

bool AVLNode::correct_tree(AVLNode *correct_parent) {
    unsigned int correct_height = 1;
    unsigned int correct_size = 1;
    unsigned int correct_nontree_cnt = get_num_nontree_edges();

    if (parent != correct_parent)
        return false;

    if (left) {
        if (left->correct_tree(this))
            return false;
        
        correct_height = max(correct_height, left->height + 1);
        correct_size += left->size;
        correct_nontree_cnt += left->get_nontree_cnt();

        if (left->height < height - 2)
            return false;

    } else {
        if (height > 2)
            return false;
    }
    
    if (right) {
        if (right->correct_tree(this))
            return false;
        
        correct_height = max(correct_height, right->height + 1);
        correct_size += right->size;
        correct_nontree_cnt += right->get_nontree_cnt();

        if (right->height < height - 2)
            return false;

    } else {
        if (height > 2)
            return false;
    }

    if (correct_height != height)
        return false;
    
    if (correct_size != size)
        return false;

    if (correct_nontree_cnt != nontree_cnt)
        return false;
    
    return true;
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
    if (left) nontree_cnt += left->get_nontree_cnt();
    if (right) nontree_cnt += right->get_nontree_cnt();
}

int VertexNode::get_num_nontree_edges() {
    return NTEdges.size();
}

VertexNode::VertexNode(int i): AVLNode() {
    idx = i;
}

EdgeNode::EdgeNode(int a, int b): AVLNode() {
    from = a;
    to = b;
}

void EdgeNode::recount_nontree_cnt() {
    nontree_cnt = 0;
    if (left) nontree_cnt += left->get_nontree_cnt();
    if (right) nontree_cnt += right->get_nontree_cnt();
}

int EdgeNode::get_num_nontree_edges() {
    return 0;
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

    TEdgeHooks[edge_id(a,b)] = ab_edge;
    TEdgeHooks[edge_id(b,a)] = ba_edge;

    auto [left_a, right_a] = Vertices[a].split();
    auto [left_b, right_b] = Vertices[b].split();

    // make b the start (and end) of the corresponding Euler tour:
    b_tree = AVLNode::merge(right_b, left_b);

    // insert the edge:
    b_tree = AVLNode::merge(ab_edge, AVLNode::merge(b_tree, ba_edge));

    // merge the Euler tours:
    AVLNode::merge(left_a, AVLNode::merge(b_tree, right_a));
}

void ETTForest::remove_tree_edge(int a, int b) {
    pair<AVLNode*, AVLNode*> subtrees;
    AVLNode *left, *right;
    EdgeNode *ab_edge = TEdgeHooks[edge_id(a,b)];
    EdgeNode *ba_edge = TEdgeHooks[edge_id(b,a)];
    TEdgeHooks.erase(edge_id(a,b));
    TEdgeHooks.erase(edge_id(b,a));

    subtrees = ab_edge->split();
    if (subtrees.first == ba_edge->root()) {
        right = subtrees.second;
        subtrees = ba_edge->split();
        left = subtrees.first;

    } else {
        left = subtrees.first;
        subtrees = ba_edge->split();
        right = subtrees.second;
    }

    AVLNode::merge(left, right);

    ab_edge->remove();
    ba_edge->remove();
    delete ab_edge;
    delete ba_edge;
}

void ETTForest::insert_nontree_edge(int a, int b) {
    NTEdgeHooks[edge_id(a,b)] = Vertices[a].push_nontree_edge(b);
    NTEdgeHooks[edge_id(b,a)] = Vertices[b].push_nontree_edge(a);
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

int64_t ETTForest::edge_id(int a, int b) {
    return int64_t(a) << 32 | b;
}

bool ETTForest::correct() {    
    for (AVLNode &node: Vertices) {
        AVLNode *root = node.root();

        if (!root->correct_tree(nullptr))
            return false;
    }
    return true;
}