#include <cassert>
#include "avl_tree.hpp"

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

void AVLNode::update_on_level_cnt(int dx) {
    for (AVLNode *cur = this; cur != NULL; cur = cur->parent) {
        cur->on_level_cnt += dx;
    }
}

void AVLNode::update_nontree_cnt(int dx) {
   for (AVLNode *cur = this; cur != NULL; cur = cur->parent) {
        cur->nontree_cnt += dx;
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
    on_level_cnt = is_on_level() ? 1 : 0;
    if (left) {
        height = max(height, left->height + 1);
        size += left->size;
        on_level_cnt += left->on_level_cnt;
    }
    if (right) {
        height = max(height, right->height + 1);
        size += right->size;
        on_level_cnt += right->on_level_cnt;
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

void AVLNode::unlink() {
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

bool AVLNode::promote_tree_edge(pair <int, int> &edge) {
    if (nontree_cnt == 0)
        return false;
    
    return (left && left->promote_tree_edge(edge)) ||
           (right && right->promote_tree_edge(edge));
}

unsigned int AVLNode::get_nontree_cnt() {
    return nontree_cnt;
}

AVLNode* AVLNode::rotate_right() {
    assert(left != NULL and left->left != NULL and left->right != NULL);

    AVLNode *left_child = left;

    this->parent = left_child;
    left_child->right = this;

    this->left = left_child->right;
    left_child->right->parent = this;
    
    this->update_statistics();
    left_child->update_statistics();

    return left_child;
}

AVLNode* AVLNode::rotate_left() {
    assert(right != NULL and right->left != NULL and right->right != NULL);

    AVLNode *right_child = right;

    this->parent = right_child;
    right_child->left = this;

    this->right = right_child->left;
    right_child->left->parent = this;
    
    this->update_statistics();
    right_child->update_statistics();

    return right_child;
}

void AVLNode::balance() {

    unsigned int hl = left ? left->height : 0;
    unsigned int hr = right ? right->height : 0;
    // root and parent node of the subtree that is being balanced
    AVLNode *root, *parent_node = parent;

    if (hl == hr + 2) {
        unsigned int hll = left->left ? left->left->height : 0;
        unsigned int hlr = left->right ? left->right->height : 0;

        if (hll >= hlr) {
            root = this->rotate_right();
        } else {
            left = left->rotate_left();
            root = this->rotate_right();
        }
    } else if (hr == hl + 2) {
        unsigned int hrr = right->right ? right->right->height : 0;
        unsigned int hrl = right->left ? right->left->height : 0;

        if (hrr >= hrl) {
            root = this->rotate_left();
        } else {
            right = right->rotate_right();
            root = this->rotate_left();
        }
    }

    if (parent_node) {
        if (parent_node->left == this) {
            parent_node->left = root;
        } else {
            parent_node->right = root;
        }
    }

    if (height != max(hl, hr) + 1 and parent_node) {
        parent_node->balance();
    }
}

bool AVLNode::correct_tree(AVLNode *correct_parent) {
    unsigned int correct_height = 1;
    unsigned int correct_size = 1;
    unsigned int correct_nontree_cnt = get_num_nontree_edges();
    unsigned int correct_on_level_cnt = is_on_level() ? 1 : 0;

    if (parent != correct_parent) {
        debug ("parent does not match\n");
        return false;
    }

    if (left) {
        if (left->correct_tree(this))
            return false;
        
        correct_height = max(correct_height, left->height + 1);
        correct_size += left->size;
        correct_nontree_cnt += left->nontree_cnt;
        correct_on_level_cnt += left->on_level_cnt;

        if (left->height < height - 2) {
            debug ("height invariant violated\n");
            return false;
        }

    } else {
        if (height > 2) {
            debug ("height invariant violated\n");
            return false;
        }
    }
    
    if (right) {
        if (right->correct_tree(this))
            return false;
        
        correct_height = max(correct_height, right->height + 1);
        correct_size += right->size;
        correct_nontree_cnt += right->nontree_cnt;
        correct_on_level_cnt += right->on_level_cnt;

        if (right->height < height - 2) {
            debug ("height invariant violated\n");
            return false;
        }

    } else {
        if (height > 2) {
            debug ("height invariant violated\n");
            return false;
        }
    }

    if (correct_height != height) {
        debug ("height does not match\n");
        return false;
    }
    
    if (correct_size != size) {
        debug ("size does not match\n");
        return false;
    }

    if (correct_nontree_cnt != nontree_cnt) {
        debug ("nontree_cnt does not match\n");
        return false;
    }
    
    if (correct_on_level_cnt != on_level_cnt) {
        debug ("on_level_cnt does not match\n");
        return false;
    }
    
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

bool VertexNode::is_on_level() {
    return false;
}

EdgeNode::EdgeNode(int a, int b, bool _on_level): AVLNode() {
    from = a;
    to = b;
    on_level = _on_level;
    on_level_cnt = on_level ? 1 : 0;
}

void EdgeNode::recount_nontree_cnt() {
    nontree_cnt = 0;
    if (left) nontree_cnt += left->get_nontree_cnt();
    if (right) nontree_cnt += right->get_nontree_cnt();
}

bool EdgeNode::promote_tree_edge(pair <int, int> &edge) {
    if (on_level) {
        edge = {from, to};
        on_level = false;
        update_on_level_cnt(-1);
        return true;
    }
    
    return AVLNode::promote_tree_edge(edge);
}


int EdgeNode::get_num_nontree_edges() {
    return 0;
}

bool EdgeNode::is_on_level() {
    return on_level;
}
