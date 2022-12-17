#include <cstdio>
#include "dc.hpp"

#include "dc.cpp"
#include "avl_tree.cpp"
#include "et_trees.cpp"

/**
 * A solution to the "Connect and Disconnect" problem from codeforces
 * (https://codeforces.com/gym/100551/problem/A) to validate the
 * correctness and performance of the DynamicConnectivity class.
 * This solution gets a Memory Limit Exceeded verdict (the model
 * solution uses less because in the problem queries can be
 * processed offline)
*/

int main()
{
    freopen("connect.in", "r", stdin);
    freopen("connect.out", "w", stdout);

    int n, k;
    scanf ("%d %d", &n, &k);
    // number of connected components
    int n_cc = n;

    DynamicConnectivity DC(n);

    while(k--) {
        char type;
        int a, b;
        scanf (" %c", &type);
        if (type != '?') {
            scanf ("%d %d", &a, &b);
            a--;
            b--;
        }
        switch(type) {
        case '?':
            printf ("%d\n", n_cc);
            break;

        case '+':
            if (!DC.connected(a,b)) {
                n_cc--;
            }
            DC.insert(a,b);
            break;
        
        case '-':
            DC.remove(a,b);
            if (!DC.connected(a,b)) {
                n_cc++;
            }
            break;
        }
    }
}