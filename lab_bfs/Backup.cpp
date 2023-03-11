#include "bfs.h"
#include <bits/stdc++.h>
#include <cstdlib>
#include <ctime>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

int get_neighbors(const Grid* grid, Point p, Point neighb[]) {
    int d_row[] = {1, 0, -1, 0};
    int d_col[] = {0, -1, 0, 1};
    int number_of_neighbours = 0;

    for (int i = 0; i < 4; i++) {
        int new_x = p.row + d_row[i];
        int new_y = p.col + d_col[i];

        if (!grid->mat[new_x][new_y]) {

            neighb[number_of_neighbours++] = {new_x, new_y};
        }
    }

    return number_of_neighbours;
}

void grid_to_graph(const Grid* grid, Graph* graph) {
    // we need to keep the nodes in a matrix, so we can easily refer to a
    // position in the grid
    Node* nodes[MAX_ROWS][MAX_COLS];
    int i, j, k;
    Point neighb[4];

    // compute how many nodes we have and allocate each node
    graph->nrNodes = 0;
    for (i = 0; i < grid->rows; ++i) {
        for (j = 0; j < grid->cols; ++j) {
            if (grid->mat[i][j] == 0) {
                nodes[i][j] = (Node*)malloc(sizeof(Node));
                memset(nodes[i][j], 0,
                       sizeof(Node)); // initialize all fields with 0/NULL
                nodes[i][j]->position.row = i;
                nodes[i][j]->position.col = j;
                ++graph->nrNodes;
            } else {
                nodes[i][j] = NULL;
            }
        }
    }
    graph->v = (Node**)malloc(graph->nrNodes * sizeof(Node*));
    k = 0;
    for (i = 0; i < grid->rows; ++i) {
        for (j = 0; j < grid->cols; ++j) {
            if (nodes[i][j] != NULL) {
                graph->v[k++] = nodes[i][j];
            }
        }
    }

    // compute the adjacency list for each node
    for (i = 0; i < graph->nrNodes; ++i) {
        graph->v[i]->adjSize =
            get_neighbors(grid, graph->v[i]->position, neighb);
        if (graph->v[i]->adjSize != 0) {
            graph->v[i]->adj =
                (Node**)malloc(graph->v[i]->adjSize * sizeof(Node*));
            k = 0;
            for (j = 0; j < graph->v[i]->adjSize; ++j) {
                if (neighb[j].row >= 0 && neighb[j].row < grid->rows &&
                    neighb[j].col >= 0 && neighb[j].col < grid->cols &&
                    grid->mat[neighb[j].row][neighb[j].col] == 0) {
                    graph->v[i]->adj[k++] = nodes[neighb[j].row][neighb[j].col];
                }
            }
            if (k < graph->v[i]->adjSize) {
                // get_neighbors returned some invalid neighbors
                graph->v[i]->adjSize = k;
                graph->v[i]->adj =
                    (Node**)realloc(graph->v[i]->adj, k * sizeof(Node*));
            }
        }
    }
}

void free_graph(Graph* graph) {
    if (graph->v != NULL) {
        for (int i = 0; i < graph->nrNodes; ++i) {
            if (graph->v[i] != NULL) {
                if (graph->v[i]->adj != NULL) {
                    free(graph->v[i]->adj);
                    graph->v[i]->adj = NULL;
                }
                graph->v[i]->adjSize = 0;
                free(graph->v[i]);
                graph->v[i] = NULL;
            }
        }
        free(graph->v);
        graph->v = NULL;
    }
    graph->nrNodes = 0;
}

void bfs(Graph* graph, Node* s, Operation* op) {

    for (int i = 0; i < graph->nrNodes; i++) {
        if (op)
            op->count();
        if (graph->v[i] != s) {
            if (op)
                op->count(3);
            graph->v[i]->color = COLOR_WHITE;
            graph->v[i]->dist = INT_MAX;
            graph->v[i]->parent = NULL;
        }
    }

    if (op)
        op->count(3);
    s->color = COLOR_GRAY;
    s->dist = 0;
    s->parent = NULL;

    std::queue<Node*> q;

    if (op)
        op->count();
    q.push(s);

    while (!q.empty()) {
        if (op)
            op->count(2);

        Node* u = q.front();
        q.pop();

        for (int i = 0; i < u->adjSize; i++) {
            if (op)
                op->count();
            if (u->adj[i]->color == COLOR_WHITE) {
                if (op)
                    op->count(4);
                u->adj[i]->color = COLOR_GRAY;
                u->adj[i]->dist++;
                u->adj[i]->parent = u;
                q.push(u->adj[i]);
            }
        }
        if (op)
            op->count();
        u->color = COLOR_BLACK;
    }
    // TODO: implement the BFS algorithm on the graph, starting from the node s
    // at the end of the algorithm, every node reachable from s should have the
    // color BLACK for all the visited nodes, the minimum distance from s (dist)
    // and the parent in the BFS tree should be set for counting the number of
    // operations, the optional op parameter is received since op can be NULL
    // (when we are calling the bfs for display purposes), you should check it
    // before counting: if(op != NULL) op->count();
}

void pp(int p[], int size, Point repr[], int target = -1, int level = 0) {
    for (int i = 0; i < size; i++) {
        if (p[i] == target) {
            std::cout << std::string(level * 4, ' ') << '(' << repr[i].row
                      << ", " << repr[i].col << ')' << '\n';
            pp(p, size, repr, i, level + 1);
        }
    }
}

void print_bfs_tree(Graph* graph) {
    // first, we will represent the BFS tree as a parent array
    int n = 0;          // the number of nodes
    int* p = NULL;      // the parent array
    Point* repr = NULL; // the representation for each element in p

    // some of the nodes in graph->v may not have been reached by BFS
    // p and repr will contain only the reachable nodes
    int* transf = (int*)malloc(graph->nrNodes * sizeof(int));
    for (int i = 0; i < graph->nrNodes; ++i) {
        if (graph->v[i]->color == COLOR_BLACK) {
            transf[i] = n;
            ++n;
        } else {
            transf[i] = -1;
        }
    }
    if (n == 0) {
        // no BFS tree
        free(transf);
        return;
    }

    int err = 0;
    p = (int*)malloc(n * sizeof(int));
    repr = (Point*)malloc(n * sizeof(Node));
    for (int i = 0; i < graph->nrNodes && !err; ++i) {
        if (graph->v[i]->color == COLOR_BLACK) {
            if (transf[i] < 0 || transf[i] >= n) {
                err = 1;
            } else {
                repr[transf[i]] = graph->v[i]->position;
                if (graph->v[i]->parent == NULL) {
                    p[transf[i]] = -1;
                } else {
                    err = 1;
                    for (int j = 0; j < graph->nrNodes; ++j) {
                        if (graph->v[i]->parent == graph->v[j]) {
                            if (transf[j] >= 0 && transf[j] < n) {
                                p[transf[i]] = transf[j];
                                err = 0;
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
    free(transf);
    transf = NULL;

    if (!err) {
        pp(p, n, repr);
    }

    if (p != NULL) {
        free(p);
        p = NULL;
    }
    if (repr != NULL) {
        free(repr);
        repr = NULL;
    }
}

int shortest_path(Graph* graph, Node* start, Node* end, Node* path[]) {
    // bfs()    t-> s
    // go from target to source through parent
    //
    // TODO: compute the shortest path between the nodes start and end in the
    // given graph the nodes from the path, should be filled, in order, in the
    // array path the number of nodes filled in the path array should be
    // returned if end is not reachable from start, return -1 note: the size of
    // the array path is guaranteed to be at least 1000
    return -1; // length
}

void link(Node* u, Node* v) {
    if (!u->adjSize) {
        u->adj = (Node**)malloc(sizeof(Node*));
    } else {
        u->adj = (Node**)realloc(u->adj, (u->adjSize + 1) * sizeof(Node*));
    }

    u->adj[u->adjSize++] = v;
}

void performance() {
    Profiler p("bfs");

    // vary the number of edges
    for (int n = 1000; n <= 4500; n += 100) {
        Operation op = p.createOperation("bfs-edges", n);
        Graph graph;
        graph.nrNodes = 100;
        // initialize the nodes of the graph
        graph.v = (Node**)malloc(graph.nrNodes * sizeof(Node*));
        for (int i = 0; i < graph.nrNodes; ++i) {
            graph.v[i] = (Node*)malloc(sizeof(Node));
            memset(graph.v[i], 0, sizeof(Node));
        }

        for (int i = 1; i < graph.nrNodes; i++) {
            link(graph.v[i - 1], graph.v[i]);
            link(graph.v[i], graph.v[i - 1]);
        }

        for (int i = graph.nrNodes; i < n; i++) {
            int u;
            int v;
            do {
                u = rand() % graph.nrNodes;
                v = rand() % graph.nrNodes;
            } while (u == v);

            link(graph.v[u], graph.v[v]);
            link(graph.v[u], graph.v[v]);
        }

        // TODO: generate n random edges
        // make sure the generated graph is connected

        bfs(&graph, graph.v[0], &op);
        free_graph(&graph);
    }

    // vary the number of vertices
    for (int n = 100; n <= 200; n += 10) {
        Operation op = p.createOperation("bfs-vertices", n);
        Graph graph;
        graph.nrNodes = n;
        // initialize the nodes of the graph
        graph.v = (Node**)malloc(graph.nrNodes * sizeof(Node*));
        for (int i = 0; i < graph.nrNodes; ++i) {
            graph.v[i] = (Node*)malloc(sizeof(Node));
            memset(graph.v[i], 0, sizeof(Node));
        }

        int n_ = 4500;

        for (int i = 1; i < graph.nrNodes; i++, n_--) {
            link(graph.v[i - 1], graph.v[i]);
            link(graph.v[i], graph.v[i - 1]);
        }

        for (int i = 0; i < n_; i++) {
            int u;
            int v;
            do {
                u = rand() % graph.nrNodes;
                v = rand() % graph.nrNodes;
            } while (u == v);

            link(graph.v[u], graph.v[v]);
            link(graph.v[u], graph.v[v]);
        }

        // TODO: generate 4500 random edges
        // make sure the generated graph is connected

        bfs(&graph, graph.v[0], &op);
        free_graph(&graph);
    }

    p.showReport();
}
