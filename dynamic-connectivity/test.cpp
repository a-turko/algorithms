#include <cstdlib>
#include <cassert>
#include <cstdio>
#include <set>
#include <queue>
#include "dc.hpp"

#define test_debug(...) {}
//#define test_debug(...) fprintf(stderr, __VA_ARGS__), fflush(stderr)

using namespace std;

class NaiveConnectivity {
    public:
    NaiveConnectivity(int n) {
        Edges.resize(n);
    }

    void insert(int a, int b) {
        Edges[a][b]++;
        Edges[b][a]++;
    }

    void remove(int a, int b) {
        if ((--Edges[a][b]) == 0)
            Edges[a].erase(b);

        if ((--Edges[b][a]) == 0)
            Edges[b].erase(a);
    }

    bool connected(int a, int b) {
        set <int> Visited;
        queue <int> Q;
        Q.push(a);

        while (!Q.empty()) {
            int v = Q.front();
            Q.pop();
            if (v == b)
                return true;
            
            Visited.insert(v);
            for (auto it: Edges[v]) {
                if (Visited.find(it.first) == Visited.end())
                    Q.push(it.first);
            }
        }
        return false;
    }

    private:
    vector < map <int, int> > Edges;
};

vector <pair <char, pair <int, int> > > gen_test(int n, int q, int seed) {
    srand(seed);
    vector <pair <char, pair <int, int> > > test;
    map <pair <int, int>, int> Edges;
    set <pair <int, pair <int, int> > > edge_set;
    map <pair <int, int>, int> rank_map;
    
    int cnt = 0;
    int logn = 1;
    while ((1<<logn) < n) logn++;

    for (int i = 0; i < q; i++) {
        int a = rand() % n;
        int b = rand() % n;
        if (a == b)
            continue;
        if (a > b)
            swap(a,b);
        
        char t = rand() % 3;

        if (t == 0) {
            
            // make sure there are not too many edges
            if (Edges[{a,b}] == 0 and cnt > n) {
                auto e = *edge_set.begin();
                a = e.second.first;
                b = e.second.second;
            }

            if (Edges[{a,b}] > 0) {
                
                if (--Edges[{a,b}] == 0) {
                    edge_set.erase({rank_map[{a,b}], {a,b}});
                    rank_map.erase({a,b});
                }
                cnt--;
                t = 'R';
            } else {
                t = 2;
            }
        }

        if (t == 1) {
            t = 'I';
            if (Edges[{a,b}]++ == 0) {
                int rank = rand();
                edge_set.insert({rank, {a,b}});
                rank_map[{a,b}] = rank;
            }
            cnt++;
        }

        if (t == 2) {
            t = 'Q';
        }
        
        test.push_back({t, {a, b}});
    }
    return test;
}

bool check_correctness(int n, vector <pair <char, pair <int, int> > > test) {
    NaiveConnectivity NC(n);
    DynamicConnectivity DC(n);
    int idx = 0;

    for (auto p: test) {
        idx++;
        if (p.first == 'I') {
            NC.insert(p.second.first, p.second.second);
            DC.insert(p.second.first, p.second.second);
        }

        if (p.first == 'R') {
            NC.remove(p.second.first, p.second.second);
            DC.remove(p.second.first, p.second.second);
        }

        if (p.first == 'Q') {
            bool nc = NC.connected(p.second.first, p.second.second);
            bool dc = DC.connected(p.second.first, p.second.second);

            if (!nc and dc) {
                test_debug ("Error (od %d): %d %d are not connected (false positive)\n", idx, p.second.first, p.second.second);
                return false;
            }
            
            if (nc and !dc) {
                test_debug ("Error (op %d): %d %d are connected (false negative)\n", idx, p.second.first, p.second.second);
                return false;
            }
        }
    }

    return true;
}

bool verify_execution(int n, vector <pair <char, pair <int, int> > > test) {
    DynamicConnectivity DC(n);
    int idx = 0;

    if (!DC.correct())
        return false;

    for (auto p: test) {
        idx++;
        test_debug("\nExecuting %c %d %d (id %d)\n", p.first, p.second.first, p.second.second, idx);
        
        if (p.first == 'I')
            DC.insert(p.second.first, p.second.second);

        if (p.first == 'R')
            DC.remove(p.second.first, p.second.second);

        if (p.first == 'Q')
            DC.connected(p.second.first, p.second.second);
        
        if (!DC.correct())
            return false;
    }
    return true;
}

int main()
{
    assert(verify_execution(3, {{'I', {0, 1}}, {'I', {1, 2}}, {'Q', {0, 2}}}));
    
    assert(check_correctness(3, {{'I', {0, 1}}, {'I', {1, 2}}, {'Q', {0, 2}}}));
    assert(verify_execution(3, {{'I', {0, 1}}, {'I', {1, 2}}, {'Q', {0, 2}}}));

    for (int i = 0; i < 100; i++) {
        printf ("Test %d\n", i);
        if (!verify_execution(500, gen_test(500, 10000, i))) {
            test_debug ("Failed on test %d", i);
            assert(0);
        }

        assert(check_correctness(500, gen_test(500, 10000, i)));
    }

    assert(check_correctness(100, gen_test(100, 1000, -1)));
    assert(check_correctness(100, gen_test(100, 1000, -3)));
    assert(check_correctness(100, gen_test(100, 1000, -5)));
}