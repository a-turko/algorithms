#include <bits/stdc++.h>
#include "dc.hpp"
using namespace std;

pair <int, int> DynamicConnectivity::ordered_pair(int a, int b) {
    return {min(a,b), max(a,b)};
}

DynamicConnectivity::DynamicConnectivity(int n) {
    L = 0;
    while (n > (1<<L)) L++;

    Forests.resize(L+1, ETTForest(n));
}

void DynamicConnectivity::insert(int a, int b) {

    if (EdgeCnt[ordered_pair(a,b)]++ == 0)
        insert_edge(a, b, 0);

}

void DynamicConnectivity::insert_edge(int a, int b, int level) {

    EdgeLevels[ordered_pair(a,b)] = level;

    if (Forests[level].connected(a,b)) {
        Forests[level].insert_tree_edge(a, b);
    } else {
        Forests[level].insert_nontree_edge(a,b);
    }
}

bool DynamicConnectivity::connected(int a, int b) {
    return Forests[0].connected(a, b);
}

void DynamicConnectivity::remove(int a, int b) {

    // Check if the edge exists
    if (EdgeCnt[ordered_pair(a,b)] == 0)
        return;
    
    if (--EdgeCnt[ordered_pair(a,b)] > 0)
        return;

    // Remove the edge:
    int level = EdgeLevels[ordered_pair(a,b)];

    if (!Forests[level].is_tree_edge(a, b)) {
        Forests[level].remove_nontree_edge(a, b);
        return;
    }

    for (int l = 0; l <= level; l++) {
        Forests[l].remove_tree_edge(a, b);
    }

    // Look for a replacement of the removed edge:
    for (int l = level; l >= 0; l--) {
        int sa = Forests[l].size(a);
        int sb = Forests[l].size(b);

        if (sa > sb)
            swap(a, b);

        pair <int, int> edge;
        while (Forests[l].pop_nontree_edge(a, edge)) {
            
            if (Forests[l].connected(edge.first, edge.second)) {
                
                insert_edge(a,b,l+1);
                EdgeLevels[ordered_pair(a,b)] = l+1;

            } else {
                
                // We have found a replacement edge!
                Forests[l].insert_tree_edge(a,b);

                return;
            }
        }
    }
}

bool DynamicConnectivity::correct() {
    for (int l = 0; l <= L; l++) {
        if (!Forests[l].correct())
            return false;
    }
    return true;
}