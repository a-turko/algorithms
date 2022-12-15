#include <cassert>
#include <cstdlib>
#include "avl_tree.hpp"

AVLNode::AVLNode() {
    left = right = parent = NULL;
    height = 1;
    size = 0;
    nontree_cnt = 0;
    on_level_cnt = 0;
}

VertexNode::VertexNode(int i) {
    idx = i;
    size = 1;
}

EdgeNode::EdgeNode(int _from, int _to, bool _on_level) {
    from = _from;
    to = _to;
    on_level = _on_level;
    on_level_cnt = _on_level ? 1 : 0;
}

// BST operations:

void AVLNode::unlink_children() {
    if (left) {
        left->parent = NULL;
        left = NULL;
    }
    if (right) {
        right->parent = NULL;
        right = NULL;
    }

    update_statistics();
}

AVLNode *AVLNode::root() {
    int cnt = 5;
    AVLNode *cur = this;
    while (cur->parent != NULL and cnt--) {
        debug ("0x%llx\n", (long long)cur);
        cur->print_node();
        cur = cur->parent;
    }
    assert(cnt > 0); //TODO: remove
    return cur;
}

//TODO: debug -- this isnot working
pair <AVLNode*, AVLNode*> AVLNode::split() {
    AVLNode *left_child, *right_child, *left_tree, *right_tree, *prv, *cur;

    left_tree = left;
    right_tree = right;
    prv = this;
    cur = parent;

    prv->unlink_children();
    prv->parent = NULL;

    left_tree = merge(left_tree, NULL, prv);

    while (cur != NULL) {
        left_child = right_child = NULL;

        if (cur->right == prv) {
            left_child = cur->left;
        } else {
            right_child = cur->right;
        }

        prv = cur;
        cur = cur->parent;
        prv->unlink_children();
        prv->parent = NULL;

        if (left_child)
            left_tree = merge(left_child, prv, left_tree);
        
        if (right_child)
            right_tree = merge(right_tree, prv, right_child);

    }

    return {left_tree, right_tree};
}

void AVLNode::replace_child(AVLNode *old_child, AVLNode *new_child) {
    if (left == old_child) {
        left = new_child;

    } else if (right == old_child) {
        right = new_child;

    } else {
        assert(0);
    }
}


AVLNode *AVLNode::merge(AVLNode *left, AVLNode *middle, AVLNode *right) {

    if (left == NULL) {
        if (middle == NULL) {
            return right;
        } else {
            return merge(middle, NULL, right);
        }
    }
    if (right == NULL) {
        if (middle == NULL) {
            return left;
        } else {
            return merge(left, NULL, middle);
        }
    }

    if (middle and abs(left->height - right->height) <= 1) {

        middle->left = left;
        middle->right = right;
        left->parent = middle;
        right->parent = middle;

        middle->update_statistics();
        return middle;
    }
    

    if (left->height <= right->height) {

        if (right->left)
            right->left->parent = NULL;
        
        right->left = merge(left, middle, right->left);
        right->left->parent = right;

        right->update_statistics();
        
        return right->balance();

    } else { // left->height > right->height

        if (left->right)
            left->right->parent = NULL;
        
        left->right = merge(left->right, middle, right);
        left->right->parent = left;

        left->update_statistics();
        return left->balance();
    }
}

void AVLNode::erase() {

    // make sure that this node has at most one child
    if (right) {
        AVLNode *replacement = right;
        while (replacement->left)
            replacement = replacement->left;

        swap(this->parent, replacement->parent);
        swap(this->left, replacement->left);
        swap(this->right, replacement->right);
        

        this->parent->replace_child(replacement, this);
        if (replacement->parent)
            replacement->parent->replace_child(this, replacement);

        if (replacement->left)
            replacement->left->parent = replacement;
        if (this->left)
            this->left->parent = this;
        
        if (replacement->right)
            replacement->right->parent = replacement;
        if (this->right)
            this->right->parent = this;        
    }

    // replace this node with its subtree
    if (left) {
        if (parent)
            parent->replace_child(this, left);
        
        left->parent = parent;

    } else {
        if (parent)
            parent->replace_child(this, right);
        if (right)
            right->parent = parent;
    }

    // rebalance the tree
    for (AVLNode *cur = parent; cur; cur = cur->parent) {
        cur->update_statistics();
        cur->balance();
    }
}

AVLNode* AVLNode::rotate_right() {
    assert(left != NULL);

    AVLNode *l_node = left, *lr_node = left->right;

    this->parent = l_node;
    l_node->right = this;

    this->left = lr_node;
    if (lr_node)
        lr_node->parent = this;
    
    this->update_statistics();
    l_node->update_statistics();

    return l_node;
}

AVLNode* AVLNode::rotate_left() {
    assert(right != NULL);

    AVLNode *r_node = right, *rl_node = right->left;

    this->parent = r_node;
    r_node->left = this;

    this->right = rl_node;
    if (rl_node)
        rl_node->parent = this;
    
    this->update_statistics();
    r_node->update_statistics();

    return r_node;
}

AVLNode* AVLNode::balance() {

    int hl = left ? left->height : 0;
    int hr = right ? right->height : 0;
    // root and parent node of the subtree that is being balanced
    AVLNode *root, *parent_node = parent;

    assert(abs(hl - hr) <= 2);

    if (hl == hr + 2) {
        int hll = left->left ? left->left->height : 0;
        int hlr = left->right ? left->right->height : 0;

        if (hll >= hlr) {
            root = this->rotate_right();
        } else {
            left = left->rotate_left();
            left->parent = this;
            root = this->rotate_right();
        }
    } else if (hr == hl + 2) {
        int hrr = right->right ? right->right->height : 0;
        int hrl = right->left ? right->left->height : 0;

        if (hrr >= hrl) {
            root = this->rotate_left();
        } else {
            right = right->rotate_right();
            right->parent = this;
            root = this->rotate_left();
        }
    } else {
        // no rebalancing needed at this level
        root = this;
        this->update_statistics();
    }

    root->parent = parent_node;
    if (parent_node)
        parent_node->replace_child(this, root);
    
    return root;
}

// Getters for subclass specific fields

bool EdgeNode::is_on_level() {
    return on_level;
}

bool AVLNode::is_on_level() {
    return false;
}

bool VertexNode::is_on_level() {
    return AVLNode::is_on_level();
}

int AVLNode::get_num_nontree_edges() {
    return 0;
}

int VertexNode::get_num_nontree_edges() {
    return NTEdges.size();
}

int EdgeNode::get_num_nontree_edges() {
    return AVLNode::get_num_nontree_edges();
}

// Manage data stored in the nodes

void AVLNode::update_statistics() {
    height = 1;
    size = dynamic_cast<VertexNode*>(this) ? 1 : 0;
    on_level_cnt = is_on_level() ? 1 : 0;
    nontree_cnt = get_num_nontree_edges();

    if (left) {
        height = max(height, left->height + 1);
        size += left->size;
        on_level_cnt += left->on_level_cnt;
        nontree_cnt += left->nontree_cnt;
    }
    if (right) {
        height = max(height, right->height + 1);
        size += right->size;
        on_level_cnt += right->on_level_cnt;
        nontree_cnt += right->nontree_cnt;
    }
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

list<int>::iterator VertexNode::push_nontree_edge(int to) {
    NTEdges.push_front(to);
    update_nontree_cnt(1);
    return NTEdges.begin();
}

bool AVLNode::pop_nontree_edge(pair <int, int> &edge) {
    if (nontree_cnt == 0)
        return false;
    
    return (left && left->pop_nontree_edge(edge)) ||
           (right && right->pop_nontree_edge(edge));
}

bool VertexNode::pop_nontree_edge(pair <int, int> &edge) {
    if (NTEdges.empty())
        return AVLNode::pop_nontree_edge(edge);

    edge = {idx, NTEdges.front()};
    NTEdges.pop_front();
    update_nontree_cnt(-1);
    return true;
}

bool EdgeNode::pop_nontree_edge(pair <int, int> &edge) {
    return AVLNode::pop_nontree_edge(edge);
}

void VertexNode::erase_nontree_edge(list<int>::iterator it) {
    NTEdges.erase(it);
    update_nontree_cnt(-1);
}

bool AVLNode::promote_tree_edge(pair <int, int> &edge) {
    if (on_level_cnt == 0)
        return false;
    
    return (left && left->promote_tree_edge(edge)) ||
           (right && right->promote_tree_edge(edge));
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

bool VertexNode::promote_tree_edge(pair <int, int> &edge) {
    return AVLNode::promote_tree_edge(edge);
}


bool AVLNode::correct_tree(AVLNode *correct_parent) {
    int correct_height = 1;
    unsigned int correct_size = dynamic_cast<VertexNode*>(this) ? 1 : 0;
    unsigned int correct_nontree_cnt = get_num_nontree_edges();
    unsigned int correct_on_level_cnt = is_on_level() ? 1 : 0;

    debug ("correct tree from 0x%llx\n", (long long)this);
    if (parent != correct_parent) {
        debug ("parent does not match\n");
        return false;
    }

    if (left) {
        if (!left->correct_tree(this))
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
        if (!right->correct_tree(this))
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
