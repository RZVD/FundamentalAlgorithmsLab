#include "Profiler.h"
#include <algorithm>
#include <bits/stdc++.h>
#include <iostream>
#include <stack>

using namespace std;

enum Color {
    WHITE = 0,
    GRAY,
    BlACK
};

struct Node {
    vector<Node*> adj;

    int key;
    Node* parent = NULL;
    Color color = WHITE;
    int d = 0;
    int f = 0;
    int index = 0;
    int lowlink = 0;
    int comp = 0;
    bool on_stack = false;
};

struct Graph {
    vector<Node*> vertices;
};

void dfs_visit(Graph g, Node* u, int& time, Operation* op) {
    u->d = ++time;
    u->color = GRAY;

    if (op)
        op->count();

    for (auto v : u->adj) {
        if (op)
            op->count();
        if (v->color == WHITE) {
            if (op)
                op->count();
            v->parent = u;
            dfs_visit(g, v, time, op);
        }
    }

    if(op) op->count();
    u->color = BlACK;
    u->f = ++time;
}

void dfs(Graph g, Operation* op) {
    for (auto u : g.vertices) {
        u->color = WHITE;
        u->parent = NULL;
    }
    int time = 0;
    for (auto u : g.vertices) {
        if (u->color == WHITE) {
            dfs_visit(g, u, time, op);
        }
    }
}

void topsort_visit(Graph g, Node* u, int& time, stack<Node*>& order) {
    u->d = ++time;
    u->color = GRAY;
    for (auto v : u->adj) {
        if (v->color == WHITE) {
            v->parent = u;
            topsort_visit(g, v, time, order);
        }
    }
    u->color = BlACK;
    u->f = ++time;
    order.push(u);
}

stack<Node*> topsort(Graph g) {

    stack<Node*> order;

    for (auto u : g.vertices) {
        u->color = WHITE;
        u->parent = NULL;
    }
    int time = 0;
    for (auto u : g.vertices) {
        if (u->color == WHITE) {
            topsort_visit(g, u, time, order);
        }
    }
    return order;
}

void strong_connect(Graph g, Node* u, int& index, stack<Node*>& S,
                    int& nr_components) {

    u->index = u->lowlink = index++;
    S.push(u);
    u->on_stack = true;

    for (Node* v : u->adj) {
        if (v->index == -1) {
            strong_connect(g, v, index, S, nr_components);
            u->lowlink = min(u->lowlink, v->lowlink);
        } else if (v->on_stack) {
            u->lowlink = min(u->lowlink, v->index);
        }
    }
    if (u->lowlink == u->index) {
        nr_components++;
        Node* v;
        do {
            v = S.top();
            S.pop();

            v->on_stack = false;
            v->comp = nr_components;

        } while (v != u);
    }
}

void tarjan(Graph g) {
    int index = 0;
    stack<Node*> S;
    int nr_components = 0;

    for (Node* u : g.vertices) {
        u->index = u->lowlink = -1;
        u->on_stack = false;
        u->comp = 0;
    }

    for (Node* u : g.vertices) {
        if (u->index == -1)
            strong_connect(g, u, index, S, nr_components);
    }
}

void print_dfs_tree(Node* v, int level = 0) {
    cout << string(4 * level, ' ') << v->key << '\n';
    for (Node* u : v->adj) {
        if (u->parent == v) {
            print_dfs_tree(u, level + 1);
        }
    }
}

void dfs_demo() {
    Node* zero = new Node;
    Node* one = new Node;
    Node* two = new Node;
    Node* three = new Node;
    Node* four = new Node;

    zero->adj.push_back(one);
    zero->key = 0;
    zero->adj.push_back(two);
    zero->adj.push_back(three);

    one->adj.push_back(zero);
    one->key = 1;
    one->adj.push_back(two);

    two->adj.push_back(zero);
    two->key = 2;
    two->adj.push_back(one);
    two->adj.push_back(four);

    three->adj.push_back(zero);
    three->key = 3;

    four->adj.push_back(two);
    four->key = 4;

    Graph g = Graph{{zero, one, two, three, four}};

    dfs(g, NULL);
    print_dfs_tree(g.vertices[0]);
}
void topsort_demo() {
    Node* one = new Node;
    Node* two = new Node;
    Node* three = new Node;
    Node* four = new Node;
    Node* five = new Node;

    one->key = 1;
    two->key = 2;
    three->key = 3;
    four->key = 4;
    five->key = 5;

    one->adj.push_back(two);
    one->adj.push_back(three);

    two->adj.push_back(four);
    two->adj.push_back(five);

    three->adj.push_back(four);
    four->adj.push_back(five);

    Graph g = {{one, two, three, four, five}};

    stack<Node*> order = topsort(g);

    while (!order.empty()) {
        cout << order.top()->key << ' ';
        order.pop();
    }
}
void tarjan_demo() {
    Node* one = new Node;
    Node* two = new Node;
    Node* three = new Node;
    Node* four = new Node;
    Node* five = new Node;
    Node* six = new Node;
    Node* seven = new Node;
    Node* eight = new Node;

    one->key = 1;
    two->key = 2;
    three->key = 3;
    four->key = 4;
    five->key = 5;
    six->key = 6;
    seven->key = 7;
    eight->key = 8;

    one->adj.push_back(three);
    two->adj.push_back(one);
    three->adj.push_back(two);
    three->adj.push_back(four);

    four->adj.push_back(five);

    five->adj.push_back(six);
    six->adj.push_back(seven);
    seven->adj.push_back(eight);
    eight->adj.push_back(five);

    Graph g = Graph{{one, two, three, four, five, six, seven, eight}};

    tarjan(g);

    set<int> s;

    for (auto v : g.vertices) {
        if (s.find(v->comp) == s.end()) {
            s.insert(v->comp);
            print_dfs_tree(v);
        }
    }
}

void demo() {
    dfs_demo();
    puts("\n\n");
    topsort_demo();
    puts("\n\n");
    tarjan_demo();
}
void perf() {
    Profiler p("bfs");

    bool adj[201][201] = {{false}};

    // vary the number of edges
    for (int n = 1000; n <= 4500; n += 100) {
        Operation op = p.createOperation("bfs-edges", n);
        const int NUMBER_OF_NODES = 100;
        Graph graph;

        for (int i = 0; i < NUMBER_OF_NODES; i++) {
            graph.vertices.push_back(new Node);
        }

        for (int i = 1; i < NUMBER_OF_NODES; i++) {
            graph.vertices[i - 1]->adj.push_back(graph.vertices[i]);
            adj[i - 1][i] = true;
        }

        for (int i = NUMBER_OF_NODES; i < n; i++) {
            int u;
            int v;
            do {
                u = rand() % NUMBER_OF_NODES;
                v = rand() % NUMBER_OF_NODES;
            } while (u == v && adj[u][v]);

            graph.vertices[u]->adj.push_back(graph.vertices[v]);
            adj[u][v] = true;
        }
        dfs(graph, &op);
    }

    for (int n = 100; n <= 200; n += 10) {
        Operation op = p.createOperation("bfs-vertices", n);
        Graph graph;
        const int NUMBER_OF_NODES = n;

        int n_ = 4500;

        for (int i = 0; i < NUMBER_OF_NODES; i++) {
            graph.vertices.push_back(new Node);
        }

        for (int i = 1; i < NUMBER_OF_NODES; i++, n_--) {
            graph.vertices[i - 1]->adj.push_back(graph.vertices[i]);
            adj[i - 1][i] = true;
        }

        for (int i = 0; i < n_; i++) {
            int u;
            int v;
            do {
                u = rand() % NUMBER_OF_NODES;
                v = rand() % NUMBER_OF_NODES;
            } while (u == v && adj[u][v]);

            graph.vertices[u]->adj.push_back(graph.vertices[v]);
            adj[u][v] = true;
        }
        dfs(graph, &op);
    }
    p.showReport();
}

int main() {
    demo();
    // perf();
    return 0;
}
