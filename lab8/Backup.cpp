#include "Profiler.h"
#include <cstdlib>
#include <iostream>
#include <stdio.h>

using namespace std;

Profiler profiler = Profiler("Rec vs Iter");

struct Node {
    int val;
    Node* left;
    Node* right;
    Node* parent;
};

enum Orders {
    PREORDER,
    INORDER,
    POSTORDER
};

enum Direction {
    LEFT = 1,
    RIGHT,
    UP,
};

void rec(Node* root, Operation& op, Orders order = INORDER, bool perf = true,
         bool is_root = true) {
    if (!root)
        return;

    if (order == PREORDER) {
        if (!perf) {
            cout << root->val << ' ';
        }
        op.count();
    }
    rec(root->left, op, order, true, false);
    if (order == INORDER) {
        if (!perf) {
            cout << root->val << ' ';
        }
        op.count();
    }
    rec(root->right, op, order, true, false);
    if (order == POSTORDER) {
        if (!perf) {
            cout << root->val << ' ';
        }
        op.count();
    }

    if (is_root && !perf)
        cout << '\n';
}

void iter(Node* root, Operation& op, bool perf = true, Orders ord = INORDER) {
    int dir = LEFT;
    while (root->parent || dir != UP) {
        if (dir == LEFT) {
            if (ord == PREORDER) {
                if (!perf)
                    cout << root->val << ' ';
                op.count();
            }
            if (root->left)
                root = root->left;
            else {
                dir = RIGHT;
            }
        } else if (dir == RIGHT) {
            if (ord == INORDER) {
                if (!perf)
                    cout << root->val << ' ';
                op.count();
            }
            if (root->right) {
                root = root->right;
                dir = LEFT;
            } else
                dir = UP;
        } else if (dir == UP) {
            if (ord == POSTORDER) {
                if (!perf)
                    cout << root->val << ' ';
                op.count();
            }
            if (root == root->parent->left) {
                dir = RIGHT;
            }
            root = root->parent;
        }
    }

    if (ord == POSTORDER) {

        if (!perf)
            cout << root->val;
        op.count();
    }

    if (!perf)
        cout << '\n';
}

Node* build_tree_helper(int l, int r) {
    Node* root;
    int m = (l + r) / 2;

    root = new Node{m};

    if (l <= m - 1) {
        root->left = build_tree_helper(l, m - 1);
        root->left->parent = root;
    }
    if (m + 1 <= r) {
        root->right = build_tree_helper(m + 1, r);
        root->right->parent = root;
    }

    return root;
}

Node* build_tree(int n) {
    return build_tree_helper(1, n);
}

void clear_root(Node* root) {
    if (!root)
        return;

    clear_root(root->left);
    clear_root(root->right);

    delete root;
}

void perf_tree() {
    for (int i = 100; i <= 10000; i += 100) {
        Operation op_rec = profiler.createOperation("rec_print", i);
        Operation op_iter = profiler.createOperation("iter_print", i);

        Node* root = build_tree(i);

        rec(root, op_rec);
        iter(root, op_iter);

        clear_root(root);
    }

    profiler.createGroup("rec vs iter", "rec_print", "iter_print");

    profiler.reset("QuickSort Hybridization");
}

void insertion_sort(int* arr, int size, Operation& op, bool counting) {

    int buff;
    for (int i = 1; i < size; i++) {
        int j = i - 1;
        if (counting)
            op.count();
        buff = arr[i];

        while (j >= 0 && buff < arr[j]) {
            if (counting) {
                op.count();
                op.count();
            }
            arr[j + 1] = arr[j];
            j--;
        }

        if (j >= 0 && counting)
            op.count();

        if (j + 1 != i) {
            if (counting)
                op.count();
            arr[j + 1] = buff;
        }
    }
}

int partition(int* arr, int l, int r, Operation& op, bool counting) {

    if (counting)
        op.count();
    int x = arr[r];
    int i = l - 1;

    for (int j = l; j < r; j++) {
        if (counting)
            op.count();
        if (arr[j] <= x) {
            i++;
            if (counting)
                op.count(3);
            swap(arr[i], arr[j]);
        }
    }
    if (counting)
        op.count(3);
    swap(arr[i + 1], arr[r]);
    return i + 1;
}

void quick_sort(int* arr, int l, int r, Operation* op, bool counting) {
    if (l < r) {
        int p = partition(arr, l, r, *op, counting);
        quick_sort(arr, l, p - 1, op, counting);
        quick_sort(arr, p + 1, r, op, counting);
    }
}
void hybridized_quick_sort(int* arr, int l, int r, Operation* op, bool counting,
                           int treshold = 20) {
    if (l >= r)
        return;

    int size = r - l + 1;

    if (size <= treshold) {
        insertion_sort(arr + l, size, *op, counting);
    } else {
        int p = partition(arr, l, r, *op, counting);
        hybridized_quick_sort(arr, l, p - 1, op, counting);
        hybridized_quick_sort(arr, p + 1, r, op, counting);
    }
}

void quick_sort_helper(int* arr, int size, bool hybridized, bool counting) {
    const int NUMBER_OF_TESTS = 3000;
    if (counting) {
        if (hybridized) {
            Operation op = profiler.createOperation("Hyb", size);
            hybridized_quick_sort(arr, 0, size - 1, &op, counting);
        } else {
            Operation op = profiler.createOperation("Classic", size);
            quick_sort(arr, 0, size - 1, &op, counting);
        }
    } else {
        if (hybridized) {
            profiler.startTimer("Hyb_time", size);
            for (int i = 0; i < NUMBER_OF_TESTS; i++) {
                hybridized_quick_sort(arr, 0, size - 1, NULL, counting);
            }
            profiler.stopTimer("Hyb_time", size);
        } else {
            profiler.startTimer("Classic_time", size);
            for (int i = 0; i < NUMBER_OF_TESTS; i++) {
                quick_sort(arr, 0, size - 1, NULL, counting);
            }
            profiler.stopTimer("Classic_time", size);
        }
    }
}

void perf_hyb() {

    int v[10000], u[10000];

    const int NUMBER_OF_TESTS = 100;
    for (int j = 0; j < NUMBER_OF_TESTS; j++) {
        for (int i = 100; i <= 10000; i += 100) {
            FillRandomArray(v, i);
            CopyArray(u, v, i);

            quick_sort_helper(u, i, true, true);

            FillRandomArray(v, i);
            CopyArray(u, v, i);

            quick_sort_helper(u, i, false, true);
        }
    }
    profiler.divideValues("Hyb", NUMBER_OF_TESTS);
    profiler.divideValues("Classic", NUMBER_OF_TESTS);

    profiler.createGroup("OP classic vs Hyb", "Hyb", "Classic");
    profiler.reset("time");
}

void perf_time() {
    int v[10000], u[10000];

    for (int i = 100; i <= 10000; i += 100) {
        FillRandomArray(v, i);
        CopyArray(u, v, i);

        quick_sort_helper(u, i, true, false);

        FillRandomArray(v, i);
        CopyArray(u, v, i);

        quick_sort_helper(u, i, false, false);
    }

    profiler.createGroup("OP classic vs Hyb", "Hyb_time", "Classic_time");
    profiler.reset("treshold");
}

void perf_treshhold() {
    int v[10000], u[10000];

    FillRandomArray(v, 10000);

    for (int i = 1; i <= 100; i++) {
        CopyArray(u, v, 10000);
        Operation op = profiler.createOperation("treshold op", i);

        hybridized_quick_sort(u, 0, 9999, &op, true, i);
    }

    profiler.showReport();
}

void perf() {
    // perf_tree();
    // perf_hyb();
    // perf_time();
    perf_treshhold();
}

int main() {
    perf();
}
