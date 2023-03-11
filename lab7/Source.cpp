#include "Profiler.h"
#include <cstdlib>
#include <iostream>
#include <stdio.h>

using namespace std;

Profiler profiler = Profiler("Averege");

struct Node {
    int key;
    unsigned int size;
    Node* left;
    Node* right;

    // TODO AVL
    // int height;
    // int balance_factor;
};

void print_tree_inorder(Node* root, bool is_root = true) {
    if (root) {
        print_tree_inorder(root->left, false);
        printf("%d %d\t", root->key, root->size);
        print_tree_inorder(root->right, false);
    }
    if (is_root)
        puts("");
}

void pretty_print(Node* root, int level = 0) {
    if (!root)
        return;
    pretty_print(root->right, level + 1);
    char s[100];
    sprintf(s, "(%d, %d)", root->key, root->size);
    cout << string(8 * level, ' ') << s << '\n';
    pretty_print(root->left, level + 1);
}

Node* build_tree_helper(int l, int r) {
    Node* root;
    int m = (l + r) / 2;
    unsigned int size = r - l + 1;

    root = new Node{m, size};

    if (l <= m - 1)
        root->left = build_tree_helper(l, m - 1);
    if (m + 1 <= r)
        root->right = build_tree_helper(m + 1, r);

    return root;
}

Node* build_tree(int n) {
    return build_tree_helper(1, n);
}

Node* OS_select(Node* root, int i, Operation& op) {
    if (!root)
        return NULL;

    int rank = (root->left) ? root->left->size + 1 : 1;
    op.count();

    if (i == rank) {
        op.count();
        return root;
    } else if (i < rank) {
        op.count();
        return OS_select(root->left, i, op);
    }

    return OS_select(root->right, i - rank, op);
}

Node* find_tree_min(Node* x) {
    while (x->left) {
        x = x->left;
    }
    return x;
}

Node* OS_delete(Node* root, int z, Operation& op) {
    if (!root)
        return NULL;

    op.count();
    root->size--;

    if (z < root->key) {
        root->left = OS_delete(root->left, z, op);
    } else if (z > root->key) {
        root->right = OS_delete(root->right, z, op);
    }

    else {
        if (!root->left && !root->right) {
            delete root;
            root = NULL;
        } else if (!root->left) {
            Node* aux = root;
            root = root->right;
            delete aux;
        } else if (!root->right) {
            Node* aux = root;
            root = root->left;
            delete aux;
        } else {
            Node* aux = find_tree_min(root->right);
            root->key = aux->key;
            root->right = OS_delete(root->right, aux->key, op);
        }
    }

    return root;
}

void perf() {
    srand(time(NULL));
    const int NUMBER_OF_TESTS = 5;
    int v[100000];
    for (int n = 100; n <= 100000; n += 100) {
        Node* root = build_tree(n);
        FillRandomArray(v, n, 1, n, true, UNSORTED);
        for (int j = 0; j < n; j++) {
            int r = v[j];
            Operation op = profiler.createOperation("Op", n);
            OS_select(root, r, op);
            root = OS_delete(root, r, op);
        }
        pretty_print(root);
    }

    profiler.divideValues("Op", NUMBER_OF_TESTS);
}
void demo() {
    Operation _op = profiler.createOperation("Unused", 11);

    Node* r = build_tree(15);

    pretty_print(r);

    OS_select(r, 12, _op);
    r = OS_delete(r, 12, _op);

    OS_select(r, 4, _op);
    r = OS_delete(r, 4, _op);

    OS_select(r, 7, _op);
    r = OS_delete(r, 7, _op);

    puts("\n\n");
    pretty_print(r);
}

int main() {

    demo();
    // perf();
    // profiler.showReport();

    return 0;
}
