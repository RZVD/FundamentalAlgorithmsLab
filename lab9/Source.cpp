#include "Profiler.h"
#include <bits/stdc++.h>
#include <iostream>

using namespace std;

Profiler profiler = Profiler("Kruskal set op");

struct DisjointSet {
    int key;
    int rank = 1;
    DisjointSet* parent = NULL;

    ~DisjointSet() {
        delete this->parent;
        delete this;
    }
};

struct Edge {
    int u;
    int v;
    int weight;

    void print() {
        printf("<%c-%c> ", this->u + 'a' - 1, this->v + 'a' - 1);
    }

    ~Edge() {
        delete this;
    }
};

struct Graph {
    vector<int> vertices;
    vector<Edge*> edges;

    ~Graph() {
        delete this;
    }
};

struct Node {
    int key;
    vector<Node*> children;
};

void print_mst(vector<Edge*> mst) {

    for (Edge* e : mst) {
        e->print();
    }
    cout << '\n';
}
DisjointSet* find_set(DisjointSet* x, Operation& op) {
    op.count();
    if (!x->parent)
        return x;

    op.count();
    x->parent = find_set(x->parent, op);
}

// DisjointSet* find_set(DisjointSet* x, Operation& op) {
//     return (!x->parent) ? x : (x->parent = find_set(x->parent, op));
// }

Graph* generate_random_graph(int number_of_nodes) {
    int E = 4 * number_of_nodes;
    int* nodes = new int[number_of_nodes];

    for (int i = 0; i < number_of_nodes; i++)
        nodes[i] = i + 1;

    vector<int> vertices(nodes, nodes + number_of_nodes);
    vector<Edge*> edges;

    delete[] nodes;
    set<pair<int, int>> added_edges;

    // Connectivity
    for (int i = 1; i < vertices.size(); i++) {
        pair<int, int> e = {vertices[i - 1], vertices[i]};
        added_edges.insert(e);
        edges.push_back(new Edge{vertices[i - 1], vertices[i], rand() % 500});
        E--;
    }

    while (E--) {
        int u;
        int v;
        do {
            u = vertices[rand() % (vertices.size() - 1)];
            v = vertices[rand() % (vertices.size() - 1)];
        } while (u == v &&
                 (added_edges.count({u, v}) || added_edges.count({v, u})));

        edges.push_back(new Edge{u, v, 1 + rand() % 500});
        added_edges.insert({u, v});
    }

    return new Graph{vertices, edges};
}

void union_(DisjointSet* x, DisjointSet* y, Operation& op) {
    DisjointSet* a = find_set(x, op);
    DisjointSet* b = find_set(y, op);

    if (a == b) {
        op.count();
        return;
    }
    if (a->rank < b->rank) {
        b->parent = a;
    } else {
        a->parent = b;
        if (a->rank == b->rank) {
            y->rank++;
        }
    }
    op.count();
}

bool comp(Edge* a, Edge* b) {
    return a->weight < b->weight;
}
void demo_sets() {
    Operation _ = profiler.createOperation("unused", 0);
    DisjointSet** arr = new DisjointSet*[10];
    for (int i = 1; i <= 10; i++) {
        arr[i - 1] = new DisjointSet{i};
    }

    for (int i = 1; i < 10; i++) {
        union_(arr[i - 1], arr[i], _);
    }

    cout << find_set(arr[4], _)->key << '\n';
    delete[] arr;
}

Graph* kruskal(Graph* g) {
    Operation op = profiler.createOperation("disj_set_op", g->vertices.size());

    vector<DisjointSet*> sets(g->vertices.size());

    vector<Edge*> mst;
    for (int i = 0; i < g->vertices.size(); i++) {
        sets[i] = new DisjointSet{g->vertices[i]};
    }

    sort(g->edges.begin(), g->edges.end(), comp);

    for (Edge* edge : g->edges) {
        if (find_set(sets[edge->u - 1], op) !=
            find_set(sets[edge->v - 1], op)) {

            mst.push_back(edge);
            union_(sets[edge->u - 1], sets[edge->v - 1], op);
        }
        if (mst.size() == g->vertices.size() - 1)
            break;
    }

    return new Graph{
        g->vertices,
        mst,
    };
}

void demo_graph() {
    Graph* mst = kruskal(generate_random_graph(10));
    print_mst(mst->edges);
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
    edges.push_back(new Edge{8, 9, 7});  // hi
    edges.push_back(new Edge{9, 3, 2});  // ic

    Graph* g = new Graph{vertices, edges};

    Graph* mst = kruskal(g);

    print_mst(mst->edges);
}

void demo() {
    demo_sets();
    demo_hard_coded();
    puts("\n\n");
    demo_graph();
}

void perf() {
    srand(time(NULL));
    const int NUMBER_OF_NODES = 10000;
    const int STEP = 100;

    for (int n = 100; n <= NUMBER_OF_NODES; n += STEP) {
        Graph* g = generate_random_graph(n);
        Graph* mst = kruskal(g);
    }
    profiler.showReport();
}

int main() {
    demo();
    // perf();
    return 0;
}
