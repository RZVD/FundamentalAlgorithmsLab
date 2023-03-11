#include "Profiler.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;

Profiler profiler = Profiler("");

struct DisjointSet {
    int key;
    int rank = 1;
    DisjointSet* parent = NULL;

    //~DisjointSet() {
    //    delete this;
    //}
};

DisjointSet* make_set(DisjointSet* x) {
    x->parent = x;
    return x;
}

struct Edge {
    int u;
    int v;
    int weight;

    void print() {
        printf("<%d-%d> ", this->u, this->v);
    }

    //~Edge() {
    //    delete this;
    //}
};

struct Graph {
    vector<int> vertices;
    vector<Edge*> edges;

    //~Graph() {
    //    delete this;
    //}
};

struct Node {
    int key;
    vector<Node*> children;
};

DisjointSet* find_set(DisjointSet* x, Operation& op) {
    op.count();
    if (x->parent == x)
        return x;

    op.count();
    x->parent = find_set(x->parent, op);
}

Graph* generate_random_graph(int number_of_nodes) {
    int E = 4 * number_of_nodes;
    int* nodes = new int[number_of_nodes];
    FillRandomArray(nodes, number_of_nodes, 1, number_of_nodes, true,
                    ASCENDING);

    vector<int> vertices(nodes, nodes + number_of_nodes);
    vector<Edge*> edges;

    delete[] nodes;
    while (E--) {
        int u;
        int v;
        do {
            u = vertices[rand() % (number_of_nodes - 1)];
            v = vertices[rand() % (number_of_nodes - 1)];
        } while (u == v);

        edges.push_back(new Edge{u, v, 1 + rand() % 500});
    }

    return new Graph{vertices, edges};
}

void union_(DisjointSet* x, DisjointSet* y, Operation& op) {
    auto a = find_set(x, op);
    auto b = find_set(y, op);

    if (a->key == b->key) {
        op.count();
        return;
    } else if (a->rank < b->rank) {
        b->parent = a;
    } else {
        a->parent = b;
    }
    op.count();
}

void demo_sets() {
    Operation _ = profiler.createOperation("unused", 0);
    DisjointSet** arr = new DisjointSet*[10];
    for (int i = 1; i <= 10; i++) {
        arr[i - 1] = make_set(new DisjointSet{i});
    }

    for (int i = 1; i < 10; i++) {
        union_(arr[i - 1], arr[i], _);
    }

    cout << find_set(arr[4], _)->key << '\n';
    delete[] arr;
}

bool comp(Edge* a, Edge* b) {
    return a->weight > b->weight;
}

vector<Edge*> kruskal(Graph* g) {
    Operation op = profiler.createOperation("disj_set_op", g->vertices.size());

    DisjointSet** sets = new DisjointSet*[g->vertices.size()];

    vector<Edge*> mst;
    for (int i = 0; i < g->vertices.size(); i++) {
        sets[i] = make_set(new DisjointSet{g->vertices[i]});
    }

    sort(g->edges.begin(), g->edges.end(), comp);

    for (auto edge : g->edges) {
        if (find_set(sets[edge->u], op)->key !=
            find_set(sets[edge->v], op)->key) {
            mst.push_back(edge);
            union_(sets[edge->u], sets[edge->v], op);
        }
        if (mst.size() == g->vertices.size() - 1)
            break;
    }
    delete[] sets;
    return mst;
}

void demo_graph() {
    auto mst = kruskal(generate_random_graph(10));

    for (auto e : mst) {
        e->print();
    }
    cout << '\n';
}

void demo_hard_coded(int number_of_nodes = 9) {
    int nodes[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    vector<int> vertices(nodes, nodes + number_of_nodes);
    vector<Edge*> edges;

    edges.push_back(new Edge{1, 2, 4});  // ab
    edges.push_back(new Edge{2, 3, 8});  // bc
    edges.push_back(new Edge{1, 8, 8});  // ah
    edges.push_back(new Edge{2, 8, 11}); // bh
    edges.push_back(new Edge{3, 4, 7});  // cd
    edges.push_back(new Edge{4, 5, 9});  // de
    edges.push_back(new Edge{5, 6, 10}); // ef
    edges.push_back(new Edge{4, 6, 14}); // df
    edges.push_back(new Edge{6, 3, 4});  // fc
    edges.push_back(new Edge{6, 7, 2});  // fg
    edges.push_back(new Edge{7, 8, 1});  // gh
    edges.push_back(new Edge{7, 9, 6});  // gi
    // edges.push_back(new Edge{8, 9, 7});  // hi
    // edges.push_back(new Edge{9, 3, 2});  // ic

    Graph* g = new Graph{vertices, edges};

    auto mst = kruskal(g);

    for (auto e : mst) {
        e->print();
    }
    cout << '\n';
}

void demo() {
    // demo_sets();
    demo_hard_coded();
    // demo_graph();
}

void perf() {
    srand(time(NULL));
    const int NUMBER_OF_NODES = 10000;
    const int STEP = 100;

    for (int n = 100; n <= NUMBER_OF_NODES; n += STEP) {
        vector<Edge*> mst = kruskal(generate_random_graph(n));
    }
    profiler.showReport();
}

int main() {
    demo_hard_coded();
    // perf();
    return 0;
}
