#include <bits/stdc++.h>
#include "ET_trees.hpp"
using namespace std;

class DynamicConnectivity {
    public:
    DynamicConnectivity(int n);
    void insert(int a, int b, int eid);
    void remove(int b, int b, int eid);
    bool connected(int a, int b);

    private:
    void insert_edge(int a, int b, int level);
    pair <int, int> edge(int a, int b);
    vector <ETTForest> Forests;
    map<pair<int, int>, int> EdgeLevels;
    map<pair<int, int>, int> EdgeCnt;
    int L;
};

pair <int, int> DynamicConnectivity::edge(int a, int b) {
    return {min(a,b), max(a,b)};
}

void DynamicConnectivity::DynamicConnectivity(int n) {
    L = 0;
    while (n > (1<<L)) L++;

    Forests.resize(L+1, ETTForest(n));
}

void DynamicConnectivity::insert(int a, int b, int level) {

    if (EdgeCnt[edge(a,b)]++ == 0)
        insert_edge(a, b, level);

}

void DynamicConnectivity::insert_edge(int a, int b, int level) {

    EdgeLevels[edge(a,b)] = level;

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

    if (--EdgeCnt[edgge(a,b)] > 0)
        return;

    // Remove the edge:
    int level = EdgeLevels[edge(a,b)];

    if (!Forest[level].is_tree_edge(a, b)) {
        Forests[level].remove_nontree_edge(a, b);
        return;
    }

    for (int l = 0; l <= level; l++) {
        Forests[l].remove_tree_edge(a, b);
    }

    // Look for a replacementof the removed edge:

    for (int l = level; l >= 0; l--) {
        int sa = Forests[l].size(a);
        int sb = Forests[l].size(b);

        if (sa > sb)
            swap(a, b);

        pair <int, int> edge;
        while (Forest[l].pop_nontree_edge(a, edge)) {
            
            if (Forest[l].connected(edge.first, edge.second)) {
                
                insert_edge(a,b,l+1)
                EdgeLevels[edge(a,b)] = l+1;

            } else {
                
                // We have found a replacement edge!
                Forest[l].insert_tree_edge(a,b);

                return;
            }
        }
    }
}


int main()
{

}