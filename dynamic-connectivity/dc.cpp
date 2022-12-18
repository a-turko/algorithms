#include <bits/stdc++.h>
#include "dc.hpp"

static pair <int, int> ordered_pair(int a, int b) {
    return {min(a,b), max(a,b)};
}

DynamicConnectivity::DynamicConnectivity(int n) {
    L = 0;
    while (n > (1<<L)) L++;

    Forests.resize(L+1, ETTForest(n));
}

/**
 * Inserts a (possibly duplicate) edge to the graph.
 * 
 * Complexity: O(log n)
 * 
 * Extra amortization cost: O(log^2 n), because a new edge is inserted
*/
void DynamicConnectivity::insert(int a, int b) {

    if (EdgeCnt[ordered_pair(a,b)]++ == 0)
        insert_edge(a, b, 0);

}

/**
 * Inserts an unique (a,b) edge to the graph with the given level
 * 
 * If a and b are not connected, it's added to the forest.
 * Otherwise it is added to the list of nontree edges to be
 * accessed later when we'll be looking for replacement edges.
 * 
 * Complexity: O((level+1) * log n) -- O(log^2 n)
*/
void DynamicConnectivity::insert_edge(int a, int b, int level) {

    EdgeLevels[ordered_pair(a,b)] = level;

    if (!Forests[level].connected(a,b)) {

        for (int l = 0; l <= level; l++)
            Forests[l].insert_tree_edge(a, b, (level == l));

    } else {
        Forests[level].insert_nontree_edge(a,b);
    }
}

/**
 * Checks whether vertices a and b are connected in the current graph
 * Complexity: O(log n)
*/
bool DynamicConnectivity::connected(int a, int b) {
    return Forests[0].connected(a, b);
}

/**
 * Looks for a replacement of edge (a,b) among nontree edges on the specified level,
 * Returns true on success and false on failure.
 * 
 * Let A and B be the trees containing a and b (w.l.o.g let A_l be the smaller one).
 * In order to reconnect A and B it is enough to browse through all nontree edges with
 * one endpoint in A. The other endpoint will either be in A or in B (the (a,b) edge
 * has just been removed).
 * Since, by our invariant, A_i and B_i together have at most n / 2^l vertices,
 * A_i has at most n / 2^{l+1} of them. Thus, we can promote its tree edges to the 
 * next level. This way, each nontree edge with both endpoints in A can also be
 * promoted to the next level (it will be a nontree edge there, too).
 * This allows us to account for the O(log n) operations needed to process it.
 * As soon as we encounter a nontree edge with one endpoint in A and the other in B,
 * we stop the procedure.
 * 
 * Complexity: O(log n) per each edge whose level grows by one (amortized) plus O(log n)
 * (not amortized)
 * 
*/
bool DynamicConnectivity::find_replacement(int a, int b, int level, pair <int, int> &replacement) {
    
    if (Forests[level].size(a) > Forests[level].size(b))
        swap(a, b);
    
    // Promote tree edges from A to the next level:
    auto promoted = Forests[level].promote_tree_edges(a);
    for (auto e: promoted) {
        EdgeLevels[ordered_pair(e.first, e.second)] = level+1;
        Forests[level+1].insert_tree_edge(e.first, e.second, true);
    }

    // Browse through all nontree edges with one endpoint in A:
    pair <int, int> edge;
    while (Forests[level].pop_nontree_edge(a, edge)) {

        if (Forests[level].connected(edge.second, b)) {
            replacement = edge;
            return true;

        } else {
            Forests[level+1].insert_nontree_edge(edge.first, edge.second);
            EdgeLevels[ordered_pair(edge.first, edge.second)] = level+1;
        }
    }

    return false;
}

/**
 * Removes an (a,b) edge from the graph and looks for its replacement
 * 
 * If it's a nontree edge, removing it does not impact connectivity.
 * Otherwise, after removing it, a and b are in different trees at all levels.
 * However, levels where (a,b) used to exist can contain edges connecting
 * those trees.
 * 
 * Complexity: O(log^2 n) (not amortized) and O(log n) per each edge
 * whose level grows by one (amortized)
 */ 
void DynamicConnectivity::remove(int a, int b) {

    // Check if the edge exists
    if (EdgeCnt[ordered_pair(a,b)] == 0)
        return;
    
    if (--EdgeCnt[ordered_pair(a,b)] > 0)
        return;

    // Remove the edge:
    int level = EdgeLevels[ordered_pair(a,b)];
    EdgeLevels.erase(ordered_pair(a,b));

    if (!Forests[level].is_tree_edge(a, b)) {
        Forests[level].remove_nontree_edge(a, b);
        return;

    } else {
        
        for (int l = 0; l <= level; l++) {
            Forests[l].remove_tree_edge(a,b);
        }
    }

    // Look for a replacement of the removed edge:
    pair <int, int> replacement;
    while (level >= 0 and !find_replacement(a, b, level, replacement))
        level--;

    if (level >= 0)
        insert_edge(replacement.first, replacement.second, level);

}

void DynamicConnectivity::print() {
    #ifdef DBG
    for (int i = 0; i <= L; i++) {
        debug ("--------------Level %d--------------------\n", i);
        Forests[i].print();
    }
    #endif
}

bool DynamicConnectivity::correct() {
    for (int l = 0; l <= L; l++) {
        if (!Forests[l].correct()) {
            return false;
        }
    }
    return true;
}