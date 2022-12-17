#pragma GCC optimize("O3")
#include <cstdio>

#include "dc.cpp"
#include "et_trees.cpp"
#include "avl_tree.cpp"

const int K = 1e5;
int Res[K];

static inline bool in(int x, vector <int> &v) {
    for (int y: v) {
        if (x == y) {
            return true;
        }
    }
    return false;
}

int main(int argc, char **argv)
{
    if (argc > 1) {
        freopen(argv[1], "r", stdin);
    } else {
        freopen("disconnected.in", "r", stdin);
        freopen("disconnected.out", "w", stdout);
    }

    int n, m;
    scanf ("%d %d", &n, &m);
    DynamicConnectivity DC(n);
    vector <pair <int, int> > edges(m);

    int n_cc = n;

    for (auto &e: edges) {
        scanf ("%d %d", &e.first, &e.second);
        e.first--;
        e.second--;
        debug ("(%d %d)", e.first, e.second);
        if (!DC.connected(e.first,e.second)) {
            n_cc--;
        }
        DC.insert(e.first,e.second);
    }

    int k;
    scanf ("%d", &k);
    vector <vector <int> > Q(k);
    int id = 0;
    for (auto &v: Q) {
        int c;
        scanf ("%d", &c);
        v.resize(c);

        for (int &i: v) {
            scanf ("%d", &i);
            i--;
        }
        sort(v.begin(), v.end());
        v.push_back(K + id++);
    }

    Q.push_back(vector<int>());
    sort(Q.begin(), Q.end());

    for (int i = 1; i <= k; i++) {
        int q = Q[i].back() - K;
        Q[i].pop_back();

        for (int e: Q[i-1]) if (!in(e, Q[i])) {
            int a = edges[e].first;
            int b = edges[e].second;
            if (!DC.connected(a,b)) {
                n_cc--;
            }
            DC.insert(a,b);
        }

        for (int e: Q[i]) if (!in(e, Q[i-1])) {
            int a = edges[e].first;
            int b = edges[e].second;
            DC.remove(a,b);
            if (!DC.connected(a,b)) {
                n_cc++;
            }
        }

        Res[q] = n_cc;
    }

    for (int i = 0; i < k; i++) {
        if (Res[i] == 1) printf ("Connected\n");
        else printf ("Disconnected\n");
    }
}